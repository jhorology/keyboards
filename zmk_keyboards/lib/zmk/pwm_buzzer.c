/* Copyright (c) 2021 Megamind (megamind4089)
 * Copyright (c) 2024 The ZMK Contributors
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/errno.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/pwm.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/workqueue.h>
#include "zmk/pwm_buzzer.h"

#define BUZZER_NODE DT_ALIAS(buzzer)

#if !DT_NODE_HAS_STATUS(BUZZER_NODE, okay)
#  error "Unsupported board: buzzer devicetree alias is not defined"
#endif

#define PWM_BUZZER_BEEP_QUEUE_SIZE 16

static const struct pwm_dt_spec pwm = PWM_DT_SPEC_GET(BUZZER_NODE);
static enum beep_state { NOT_READY, IDLE, ON, OFF } state = NOT_READY;
struct k_mutex lock;

struct beep_data {
  uint32_t period;
  uint32_t pulse;
  k_timeout_t on_duration;
  k_timeout_t off_duration;
};

static char beep_msgq_buffer[sizeof(struct beep_data) * PWM_BUZZER_BEEP_QUEUE_SIZE];
static struct k_msgq beep_msgq;

static int beep_on(void);
static int beep_off(void);
static void pwm_buzzer_beep_handler(struct k_work* work);

K_WORK_DELAYABLE_DEFINE(pwm_buzzer_beep_work, pwm_buzzer_beep_handler);

static int beep_on() {
  struct beep_data beep;

  int err = k_msgq_peek(&beep_msgq, &beep);
  if (err == -ENOMSG) {
    state = IDLE;
    return 0;
  }

  err = pwm_set_dt(&pwm, beep.period, beep.pulse);
  if (err < 0) {
    LOG_ERR("FAILED TO SET PWM PERIOD AND PULSE: %d", err);
    goto err_exit;
  }

  err = k_work_schedule_for_queue(zmk_workqueue_lowprio_work_q(), &pwm_buzzer_beep_work,
                                  beep.on_duration);
  if (err < 0) {
    LOG_ERR("FAILED TO SCEHDULE BEEP WORK: %d", err);
    goto err_exit;
  }

  state = ON;
  return 0;

err_exit:
  (void)k_msgq_get(&beep_msgq, &beep, K_NO_WAIT);
  state = IDLE;
  return err;
}

static int beep_off() {
  struct beep_data beep;

  int err = k_msgq_get(&beep_msgq, &beep, K_NO_WAIT);
  if (err == -ENOMSG) {
    LOG_ERR("BEEP MESSAGE QUEUE IS EMPTY: %d", err);
    state = IDLE;
    return err;
  }

  err = pwm_set_dt(&pwm, 0, 0);
  if (err < 0) {
    LOG_ERR("FAILED TO SET PWM PERIOD AND PULSE: %d", err);
    state = IDLE;
    return err;
  }

  err = k_work_schedule_for_queue(zmk_workqueue_lowprio_work_q(), &pwm_buzzer_beep_work,
                                  beep.off_duration);
  if (err < 0) {
    LOG_ERR("FAILED TO SCEHDULE BEEP WORK: %d", err);
    state = IDLE;
    return err;
  }

  state = OFF;
  return 0;
}

static void pwm_buzzer_beep_handler(struct k_work* work) {
  int err;
  err = k_mutex_lock(&lock, K_FOREVER);
  if (err < 0) {
    LOG_ERR("FAILED TO LOCK MUTEX: %d", err);
    return;
  }
  switch (state) {
    case IDLE:
      (void)beep_on();
      break;
    case ON:
      (void)beep_off();
      break;
    case OFF:
      (void)beep_on();
      break;
    default:
      LOG_ERR("UNSUPPORTED BEEP STATELOCK MUTEX: %d", state);
  }
  err = k_mutex_unlock(&lock);
  if (err < 0) {
    LOG_ERR("FAILED TO UNLOCK MUTEX: %d", err);
  }
}

int zmk_pwm_buzzer_beep(uint32_t period, uint32_t pulse, k_timeout_t on_duration,
                        k_timeout_t off_duration) {
  int err;
  struct beep_data beep = {
    .period = period, .pulse = pulse, .on_duration = on_duration, .off_duration = off_duration};

  if (state == NOT_READY) {
    LOG_ERR("NOT INITIALIZED");
    return -EIO;
  }

  err = k_msgq_put(&beep_msgq, &beep, K_NO_WAIT);
  if (err < 0) {
    LOG_ERR("FAILED TO PUT BEEP TO QUEUE: %d", err);
    return err;
  }

  err = k_mutex_lock(&lock, K_FOREVER);
  if (err < 0) {
    LOG_ERR("FAILED TO LOCK MUTEX: %d", err);
    return err;
  }

  if (state == IDLE) {
    (void)beep_on();
  }

  err = k_mutex_unlock(&lock);
  if (err < 0) {
    LOG_ERR("FAILED TO UNLOCK MUTEX: %d", err);
    return err;
  }

  return 0;
}

static int pwm_buzzer_init(void) {
  int err = device_is_ready(pwm.dev);
  if (err < 0) {
    LOG_ERR("PWM DEVICE IS NOT READY: %d", err);
    return err;
  }

  err = pwm_set_dt(&pwm, 0, 0);
  if (err < 0) {
    LOG_ERR("FAILED TO SET PWM PERIOD AND PULSE: %d", err);
    state = IDLE;
    return err;
  }

  err = k_mutex_init(&lock);
  if (err < 0) {
    LOG_ERR("FAILED TO INITIALIZE MUTEX: %d", err);
    return err;
  }

  k_msgq_init(&beep_msgq, beep_msgq_buffer, sizeof(struct beep_data), PWM_BUZZER_BEEP_QUEUE_SIZE);

  state = IDLE;
  return 0;
}

SYS_INIT(pwm_buzzer_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
