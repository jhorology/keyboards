
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/input/input.h>

#include <zmk/endpoints.h>
#include <zmk/hid.h>
#include <zmk/joystick.h>

#if UTIL_AND(DT_NODE_HAS_COMPAT(DT_NODELABEL(joystick), analog_axis), \
             DT_NODE_HAS_STATUS(DT_NODELABEL(joystick), okay))

static const struct device *const dev = DEVICE_DT_GET(DT_NODELABEL(joystick));

static int8_t x = 0;
static int8_t y = 0;
static enum joystick_mode mode = MOUSE;
static void joystick_input_cb(struct input_event *evt) {
  struct zmk_hid_joystick_report *report = zmk_hid_get_joystick_report();

  /*
   * INPUT_ABS_X == INPUT_REL_X == 0
   * INPUT_ABS_Y == INPUT_REL_Y == 0
   */
  switch (evt->code) {
    case INPUT_ABS_X:
      x = evt->value;
      break;
    case INPUT_ABS_Y:
      y = evt->value;
      break;
  }
  if (mode == JOYSTICK) {
    if (report->body.x != x || report->body.y != y) {
      zmk_hid_joystick_set(x, y, 0);
      zmk_endpoints_send_joystick_report();
    }
  }
}
INPUT_CALLBACK_DEFINE(dev, joystick_input_cb);

/* TODO  mouse & scroll */
void zmk_joystick_mode_set(enum joystick_mode mode) {}

#endif  // DT has joystick
