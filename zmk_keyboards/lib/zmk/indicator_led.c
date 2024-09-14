#include <zephyr/drivers/led.h>
#include <zephyr/logging/log.h>

#include <zmk/events/hid_indicators_changed.h>

#define NUM_LOCK_IDX 0
#define CAPS_LOCK_IDX 1
#define SCROLL_LOCK_IDX 2
#define COMPOSE_IDX 3
#define KANA_IDX 4

#define DT_HAS_INDICATOR_LED(id) DT_NODE_HAS_COMPAT(DT_PARENT(DT_NODELABEL(id##_led)), gpio_leds)

#define DT_HAS_ANY_INDICATOR_LED(...) (FOR_EACH(DT_HAS_INDICATOR_LED, (||), __VA_ARGS__))

#if DT_HAS_ANY_INDICATOR_LED(num_lock, caps_lock, scroll_lock, compose, kana)

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#  define LED_CHILD_IDX(id) DT_NODE_CHILD_IDX(DT_NODELABEL(id##_led))

#  define DECLARE_LED_DEVICE(id)                                                        \
    IF_ENABLED(DT_HAS_INDICATOR_LED(id), (static const struct device *const id##_leds = \
                                            DEVICE_DT_GET(DT_PARENT(DT_NODELABEL(id##_led)))));

DECLARE_LED_DEVICE(num_lock);
DECLARE_LED_DEVICE(caps_lock);
DECLARE_LED_DEVICE(scroll_lock);
DECLARE_LED_DEVICE(compose);
DECLARE_LED_DEVICE(kana);

#  define IMPLEMENT_INDICATOR_LED(idx, id)                                                    \
    IF_ENABLED(DT_HAS_INDICATOR_LED(id), (if (hid_indicators_changed & BIT(idx)) {            \
                 int err = ev->indicators & BIT(idx) ? led_on(id##_leds, LED_CHILD_IDX(id))   \
                                                     : led_off(id##_leds, LED_CHILD_IDX(id)); \
                 if (err) {                                                                   \
                   LOG_ERR("Failed to update indicator LED (%d) (err %d)", idx, err);         \
                 }                                                                            \
               }))

static int indicator_led_listener(const zmk_event_t *eh) {
  static zmk_hid_indicators_t prev_indicators = 0;
  const struct zmk_hid_indicators_changed *ev = as_zmk_hid_indicators_changed(eh);
  zmk_hid_indicators_t hid_indicators_changed = prev_indicators ^ ev->indicators;
  prev_indicators = ev->indicators;

  IMPLEMENT_INDICATOR_LED(NUM_LOCK_IDX, num_lock);
  IMPLEMENT_INDICATOR_LED(CAPS_LOCK_IDX, caps_lock);
  IMPLEMENT_INDICATOR_LED(SCROLL_LOCK_IDX, scroll_lock);
  IMPLEMENT_INDICATOR_LED(COMPOSE_IDX, compose);
  IMPLEMENT_INDICATOR_LED(KANA_IDX, kana);

  return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(indicator_led, indicator_led_listener);
ZMK_SUBSCRIPTION(indicator_led, zmk_hid_indicators_changed);

#endif
