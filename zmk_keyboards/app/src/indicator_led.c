#include <zephyr/drivers/led.h>
#include <zephyr/logging/log.h>

#include <zmk/events/hid_indicators_changed.h>

#define NUM_LOCK_IDX 0
#define CAPS_LOCK_IDX 1
#define SCROLL_LOCK_IDX 2
#define COMPOSE_MASK_IDX 3
#define KANA_MASK_IDX 4

#define DT_HAS_INDICATOR_LED(id) \
  DT_NODE_HAS_COMPAT_STATUS(DT_PARENT(DT_NODELABEL(id##_led)), gpio_leds, okay)

#define DT_HAS_ANY_INDICATOR_LED(...) (FOR_EACH(DT_HAS_INDICATOR_LED, (||), __VA_ARGS__))

#if DT_HAS_ANY_INDICATOR_LED(num_lock, caps_lock, scroll_lock, compose, kana)

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#  define LED_CHILD_IDX(id) DT_NODE_CHILD_IDX(DT_NODELABEL(id##_led))

#  define LED_API_DEVICE(id) \
    static const struct device *const id##_leds = DEVICE_DT_GET(DT_PARENT(DT_NODELABEL(id##_led)))

#  if DT_HAS_INDICATOR_LED(num_lock)
LED_API_DEVICE(num_lock);
#  endif
#  if DT_HAS_INDICATOR_LED(caps_lock)
LED_API_DEVICE(caps_lock);
#  endif
#  if DT_HAS_INDICATOR_LED(scroll_lock)
LED_API_DEVICE(scroll_lock);
#  endif
#  if DT_HAS_INDICATOR_LED(compose)
LED_API_DEVICE(compose);
#  endif
#  if DT_HAS_INDICATOR_LED(kana)
LED_API_DEVICE(kana);
#  endif

#  define UPDATE_INDICATOR_LED(idx, id)                                            \
    if (hid_indicators_changed & BIT(idx)) {                                       \
      int err = ev->indicators & BIT(idx) ? led_on(id##_leds, LED_CHILD_IDX(id))   \
                                          : led_off(id##_leds, LED_CHILD_IDX(id)); \
      if (err) {                                                                   \
        LOG_ERR("Failed to update indicator LED (%d) (err %d)", idx, err);         \
      }                                                                            \
    }

static int indicator_led_listener(const zmk_event_t *eh) {
  static zmk_hid_indicators_t prev_indicators = 0;
  const struct zmk_hid_indicators_changed *ev = as_zmk_hid_indicators_changed(eh);
  zmk_hid_indicators_t hid_indicators_changed = prev_indicators ^ ev->indicators;
  prev_indicators = ev->indicators;

#  if DT_HAS_INDICATOR_LED(num_lock)
  UPDATE_INDICATOR_LED(NUM_LOCK_IDX, num_lock);
#  endif
#  if DT_HAS_INDICATOR_LED(caps_lock)
  UPDATE_INDICATOR_LED(CAPS_LOCK_IDX, caps_lock);
#  endif
#  if DT_HAS_INDICATOR_LED(scroll_lock)
  UPDATE_INDICATOR_LED(SCROLL_LOCK_IDX, scroll_lock);
#  endif
#  if DT_HAS_INDICATOR_LED(compose)
  UPDATE_INDICATOR_LED(COMPOSE_IDX, compose);
#  endif
#  if DT_HAS_INDICATOR_LED(kana)
  UPDATE_INDICATOR_LED(KANA_IDX, kana);
#  endif

  return ZMK_EV_EVENT_BUBBLE;
}

// TODO activity event
ZMK_LISTENER(indicator_led, indicator_led_listener);
ZMK_SUBSCRIPTION(indicator_led, zmk_hid_indicators_changed);

#endif
