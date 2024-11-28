
#include <stdlib.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/shell/shell.h>
#include <zephyr/input/input.h>
#include <zephyr/input/input_analog_axis_settings.h>
#include <zephyr/input/input_analog_axis.h>

static struct analog_axis_calibration cal_data;
static bool cal_start;
static int cal_ch = -1;
static int16_t raw_val;
static int16_t cal_min;
static int16_t cal_max;
static int32_t out_x;
static int32_t out_y;
static int32_t out_z;
static const struct device *const dev = DEVICE_DT_GET_ANY(analog_axis);

static void analog_axis_event_cb(struct input_event *evt) {
  switch (evt->code) {
    case INPUT_ABS_X:
      out_x = evt->value;
      break;
    case INPUT_ABS_Y:
      out_y = evt->value;
      break;
    case INPUT_ABS_Z:
      out_z = evt->value;
      break;
  }
}
INPUT_CALLBACK_DEFINE(dev, analog_axis_event_cb);

static void analog_axis_raw_data_cb(const struct device *dev, int ch, int16_t raw) {
  if (ch == cal_ch) {
    raw_val = raw;
    if (cal_start || raw_val < cal_min) {
      cal_min = raw_val;
    }
    if (cal_start || raw_val > cal_max) {
      cal_max = raw_val;
    }
  }
  cal_start = false;
}

static int check_device(const struct shell *sh) {
  if (dev == NULL) {
    shell_print(sh, "Error: Could not find device\n");
    return -1;
  }

  if (!device_is_ready(dev)) {
    shell_print(sh, "Error: Device is not ready.\n");
    return -1;
  }
  return 0;
}
static int cmd_show(const struct shell *sh, size_t argc, char **argv) {
  int err = check_device(sh);
  if (err < 0) {
    return err;
  }

  int num_axes = analog_axis_num_axes(dev);
  shell_print(sh, "Current settings:");
  for (int ch = 0; ch < num_axes; ch++) {
    struct analog_axis_calibration *cal = &cal_data;
    analog_axis_calibration_get(dev, ch, cal);
    shell_print(sh, "ch.%d in_min: %d, in_max: %d, in_deadzone: %d", ch, (int)cal->in_min,
                (int)cal->in_max, (int)cal->in_deadzone);
  }

  return 0;
}

static int cmd_cal(const struct shell *sh, size_t argc, char **argv) {
  int err = check_device(sh);
  if (err < 0) {
    return err;
  }
  int num_axes = analog_axis_num_axes(dev);
  int ch = atoi(argv[1]);
  if (ch >= num_axes || ch < 0) {
    shell_print(sh, "Error: Invalid <ch> argument\n");
    return -1;
  }

  struct analog_axis_calibration *cal = &cal_data;
  analog_axis_calibration_get(dev, ch, cal);

  cal_ch = ch;
  cal_start = true;
  analog_axis_set_raw_data_cb(dev, &analog_axis_raw_data_cb);
  out_x = 0;
  out_y = 0;
  out_z = 0;
  char c;
  size_t cnt;

  while (true) {
    shell_info(sh, "ch.%d raw value=%d, min=%d, max=%d, out=(x:%d, y:%d, z:%d)", ch, raw_val,
               cal_min, cal_max, out_x, out_y, out_z);

    sh->iface->api->read(sh->iface, &c, sizeof(c), &cnt);
    if (c == 'Q' || c == 'q') {
      break;
    }
    if (c == 'S' || c == 's') {
      shell_print(sh, "\nOld setting: ch.%d in_min: %d, in_max: %d, in_deadzone: %d", ch,
                  (int)cal->in_min, (int)cal->in_max, (int)cal->in_deadzone);
      cal->in_max = cal_max;
      cal->in_min = cal_min;
      analog_axis_calibration_set(dev, ch, cal);
      analog_axis_calibration_save(dev);

      analog_axis_calibration_get(dev, ch, cal);
      shell_print(sh, "New setting: ch.%d in_min: %d, in_max: %d, in_deadzone: %d", ch,
                  (int)cal->in_min, (int)cal->in_max, (int)cal->in_deadzone);
      break;
    }

    k_msleep(500);
  }
  analog_axis_set_raw_data_cb(dev, NULL);

  return 0;
}

static int cmd_center(const struct shell *sh, size_t argc, char **argv) {
  int err = check_device(sh);
  if (err < 0) {
    return err;
  }

  int num_axes = analog_axis_num_axes(dev);
  int ch = atoi(argv[1]);
  if (ch >= num_axes || ch < 0) {
    shell_print(sh, "Error: Invalid <ch> argument\n");
    return -1;
  }
  struct analog_axis_calibration *cal = &cal_data;
  analog_axis_calibration_get(dev, ch, cal);
  shell_print(sh, "Old setting: ch.%d in_min: %d, in_max: %d, in_deadzone: %d", ch,
              (int)cal->in_min, (int)cal->in_max, (int)cal->in_deadzone);

  int center = atoi(argv[2]);
  int d_max = cal->in_max - center;
  int d_min = center - cal->in_min;

  if (d_max > d_min) {
    cal->in_max = center + d_min;
  } else {
    cal->in_min = center - d_max;
  }
  analog_axis_calibration_set(dev, ch, cal);
  analog_axis_calibration_save(dev);

  analog_axis_calibration_get(dev, ch, cal);
  shell_print(sh, "New setting: ch.%d in_min: %d, in_max: %d, in_deadzone: %d", ch,
              (int)cal->in_min, (int)cal->in_max, (int)cal->in_deadzone);

  return 0;
}

static int cmd_deadzone(const struct shell *sh, size_t argc, char **argv) {
  int err = check_device(sh);
  if (err < 0) {
    return err;
  }

  int num_axes = analog_axis_num_axes(dev);
  int ch = atoi(argv[1]);
  if (ch >= num_axes || ch < 0) {
    shell_print(sh, "Error: Invalid <ch> argument\n");
    return -1;
  }
  struct analog_axis_calibration *cal = &cal_data;
  analog_axis_calibration_get(dev, ch, cal);
  shell_print(sh, "Old setting: ch.%d in_min: %d, in_max: %d, in_deadzone: %d", ch,
              (int)cal->in_min, (int)cal->in_max, (int)cal->in_deadzone);

  cal->in_deadzone = atoi(argv[2]);

  analog_axis_calibration_set(dev, ch, cal);
  analog_axis_calibration_save(dev);

  analog_axis_calibration_get(dev, ch, cal);
  shell_print(sh, "New setting: ch.%d in_min: %d, in_max: %d, in_deadzone: %d", ch,
              (int)cal->in_min, (int)cal->in_max, (int)cal->in_deadzone);

  return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(
  analog_axis_sub_cmds, SHELL_CMD_ARG(show, NULL, "show currnt calibration data", cmd_show, 1, 0),
  SHELL_CMD_ARG(cal, NULL,
                "usage: analog_axis cal <ch>  |  calibrate in_min and in_max, [qQ] to quit without "
                "save, [sS] to quit and save data",
                cmd_cal, 2, 0),
  SHELL_CMD_ARG(center, NULL, "usage: analog_axis center <ch> <center value>  |  adjust center",
                cmd_center, 3, 0),
  SHELL_CMD_ARG(deadzone, NULL, "usage: analog_axis deadzone <ch> <value>  |  set deadzone",
                cmd_deadzone, 3, 0),

  SHELL_SUBCMD_SET_END /* Array terminated. */
);
SHELL_CMD_ARG_REGISTER(analog_axis, &analog_axis_sub_cmds, "analog_axis commands", NULL, 2, 0);
