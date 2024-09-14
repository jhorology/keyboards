#include <zephyr/drivers/led.h>

#include <zmk/events/hid_indicators_changed.h>

#define DT_HAS_INDICATOR_LED(id) DT_NODE_HAS_COMPAT(DT_PARENT(DT_NODELABEL(id##_led)), gpio_leds)

#define DT_HAS_ANY_INDICATOR_LED(...) (FOR_EACH(DT_HAS_INDICATOR_LED, (||), __VA_ARGS__))

#if DT_HAS_ANY_INDICATOR_LED(num_lock, caps_lock, scroll_lock, compose, kana)

#  define LED_CHILD_IDX(id) DT_NODE_CHILD_IDX(DT_NODELABEL(id##_led))

#  define DECLARE_LED_DEVICE(id)                                                        \
    IF_ENABLED(DT_HAS_INDICATOR_LED(id), (static const struct device *const id##_leds = \
                                            DEVICE_DT_GET(DT_PARENT(DT_NODELABEL(id##_led)))));

FOR_EACH(DECLARE_LED_DEVICE, (;), num_lock, caps_lock, scroll_lock, compose, kana);

#  define IMPLEMENT_INDICATOR_LED(idx, id)                                       \
    IF_ENABLED(DT_HAS_INDICATOR_LED(id),                                         \
               (if (hid_indicators_changed & BIT(idx)) ev->indicators & BIT(idx) \
                  ? led_on(id##_leds, LED_CHILD_IDX(id))                         \
                  : led_off(id##_leds, LED_CHILD_IDX(id))))

static int indicator_led_listener(const zmk_event_t *eh) {
  static zmk_hid_indicators_t prev_indicators = 0;
  const struct zmk_hid_indicators_changed *ev = as_zmk_hid_indicators_changed(eh);
  zmk_hid_indicators_t hid_indicators_changed = prev_indicators ^ ev->indicators;
  prev_indicators = ev->indicators;

  // FOR_EACH_IDX doesn't work if exist LOG code inside macro
  FOR_EACH_IDX(IMPLEMENT_INDICATOR_LED, (;), num_lock, caps_lock, scroll_lock, compose, kana);

  return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(indicator_led, indicator_led_listener);
ZMK_SUBSCRIPTION(indicator_led, zmk_hid_indicators_changed);

#endif
