
#include <stdlib.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/shell/shell.h>
#include <zephyr/input/input.h>
#include <zephyr/input/input_analog_axis_settings.h>
#include <zephyr/input/input_analog_axis.h>

#define SAMPLE_BUF_SIZE 63

struct sample_result {
  int16_t min;
  int16_t median;
  int16_t max;
};

static int16_t sample_buf[SAMPLE_BUF_SIZE];
static volatile size_t sample_cnt;
static int cal_ch = -1;

static int16_t raw_val[CONFIG_INPUT_ANALOG_AXIS_SETTINGS_MAX_AXES];
static int32_t out_x;
static int32_t out_y;
static int32_t out_z;
static const struct device *const dev = DEVICE_DT_GET_ANY(analog_axis);

static int confirm(const struct shell *sh) {
  char c;
  size_t cnt;
  while (true) {
    sh->iface->api->read(sh->iface, &c, sizeof(c), &cnt);
    if (c == 'n' || c == 'N') {
      return -1;
    }
    if (c == 'y' || c == 'Y') {
      return 0;
    }
    k_msleep(300);
  }
  return 0;
}

static int stop(const struct shell *sh) {
  char c;
  size_t cnt;
  sh->iface->api->read(sh->iface, &c, sizeof(c), &cnt);
  if (c == 's' || c == 'S') {
    return -1;
  }
  return 0;
}

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
  if (ch >= 0 && ch < CONFIG_INPUT_ANALOG_AXIS_SETTINGS_MAX_AXES) {
    raw_val[ch] = raw;
  }
  if (ch == cal_ch && sample_cnt < SAMPLE_BUF_SIZE) {
    sample_buf[sample_cnt++] = raw;
  }
}

static int compare_sample(const void *a, const void *b) {
  int16_t aa = *(const int16_t *)a;
  int16_t bb = *(const int16_t *)b;

  return (aa > bb) - (aa < bb);
};

static struct sample_result get_sample_result(int ch) {
  struct sample_result res;

  analog_axis_set_raw_data_cb(dev, &analog_axis_raw_data_cb);
  cal_ch = ch;
  sample_cnt = 0;

  while (sample_cnt < SAMPLE_BUF_SIZE) k_msleep(100);

  cal_ch = -1;
  analog_axis_set_raw_data_cb(dev, NULL);

  qsort(&sample_buf[0], SAMPLE_BUF_SIZE, 2, &compare_sample);

  res.min = sample_buf[0];
  res.median = sample_buf[SAMPLE_BUF_SIZE / 2];
  res.max = sample_buf[SAMPLE_BUF_SIZE - 1];
  return res;
}

static int check_device(const struct shell *sh) {
  if (dev == NULL) {
    shell_error(sh, "Error: Could not find device\n");
    return -1;
  }

  if (!device_is_ready(dev)) {
    shell_error(sh, "Error: Device is not ready.\n");
    return -2;
  }
  return 0;
}

static int cmd_show(const struct shell *sh, size_t argc, char **argv) {
  static struct analog_axis_calibration cal_data;
  int num_axes;

  if (check_device(sh)) return -1;

  num_axes = analog_axis_num_axes(dev);
  shell_print(sh, "Current settings:");
  for (int ch = 0; ch < num_axes; ch++) {
    analog_axis_calibration_get(dev, ch, &cal_data);
    shell_info(sh, "ch.%d in_min: %d, in_max: %d, in_deadzone: %d", ch, cal_data.in_min,
               cal_data.in_max, cal_data.in_deadzone);
  }
  return 0;
}

static int cmd_cal(const struct shell *sh, size_t argc, char **argv) {
#define CAL_ZERO_NUM_TIMES 10
  struct analog_axis_calibration cal_data;
  struct sample_result res;
  int ch;
  int16_t out_min;
  int16_t out_max;
  int num_axes;
  int sum = 0;
  int min;
  int max;
  int origin;
  int16_t deadzone;
  int pos_noise;
  int neg_noise;
  int pos_range;
  int neg_range;

  if (check_device(sh)) return -1;

  ch = atoi(argv[1]);
  /* TODO get from DT  */
  out_min = atoi(argv[2]);
  out_max = atoi(argv[3]);

  num_axes = analog_axis_num_axes(dev);
  if (ch >= num_axes || ch < 0) {
    shell_error(sh, "Error: Invalid <ch> argument\n");
    return -1;
  }

  if (out_min >= out_max) {
    shell_error(sh, "Error: Invalid <out_min>, <out_max> arguments\n");
    return -1;
  }

  shell_print(
    sh,
    "\nSet the ch.%d axis to zero(or origin) position and then don't touch while calibrating.\n"
    "Are you sure to start? [y/n]",
    ch);
  if (confirm(sh)) return -1;

  for (int i = 0; i < CAL_ZERO_NUM_TIMES; i++) {
    res = get_sample_result(ch);
    if (i == 0 || res.min < min) min = res.min;
    if (i == 0 || res.max > max) max = res.max;
    sum += res.median;
    origin = sum / (i + 1);
    shell_print(sh, "%d/%d  min:%d, median:%d, max:%d -> min:%d, average:%d, max:%d", i + 1,
                CAL_ZERO_NUM_TIMES, res.min, res.median, res.max, min, origin, max);
  }
  deadzone = (max - min) * 3 / 2;  // moise width x 1.5
  pos_noise = max - origin;
  neg_noise = origin - min;

  shell_print(sh,
              "Done.\n\nNext, move the ch.%d axis from end to end several times.\n"
              "Are you sure to start? [y/n]",
              ch);
  if (confirm(sh)) return -1;

  min = origin, max = origin;
  while (true) {
    res = get_sample_result(ch);
    if (res.min < min) min = res.min;
    if (res.max > max) max = res.max;
    shell_info(sh, "min:%d, max:%d\tPress [S] to stop", min, max);
    if (stop(sh)) break;
  }
  min += pos_noise;
  max -= neg_noise;

  // adjust zero position
  if (out_min < 0 && out_max > 0) {
    pos_range = max - origin;
    neg_range = origin - min;

    if (pos_range * abs(out_min) > neg_range * out_max) {
      // offseted positive
      max = origin + neg_range * out_max / abs(out_min);
    } else {
      // offseted negative
      min = origin - pos_range * abs(out_min) / out_max;
    }
  }

  analog_axis_calibration_get(dev, ch, &cal_data);
  shell_info(sh, "Done.\n\nOld setting: ch.%d in_min: %d, in_max: %d, in_deadzone: %d", ch,
             cal_data.in_min, cal_data.in_max, cal_data.in_deadzone);
  shell_info(sh, "Calibrated setting: ch.%d in_min: %d, in_max: %d, in_deadzone: %d", ch, min, max,
             deadzone);

  shell_print(sh, "\nAre you sure to save new settting? [y/n]");
  if (confirm(sh)) return -1;
  cal_data.in_min = min;
  cal_data.in_max = max;
  cal_data.in_deadzone = deadzone;
  analog_axis_calibration_set(dev, ch, &cal_data);
  analog_axis_calibration_save(dev);

  return 0;
}

static int cmd_test(const struct shell *sh, size_t argc, char **argv) {
  if (check_device(sh)) return -1;

  analog_axis_set_raw_data_cb(dev, &analog_axis_raw_data_cb);
  while (true) {
    shell_print(sh, "[%d, %d, %d]\t->\tx:%d, y:%d, z:%d.\tPress [S] to stop.", raw_val[0],
                raw_val[1], raw_val[2], out_x, out_y, out_z);
    if (stop(sh)) break;
    k_msleep(300);
  }
  analog_axis_set_raw_data_cb(dev, NULL);
  return 0;
}

static int cmd_deadzone(const struct shell *sh, size_t argc, char **argv) {
  struct analog_axis_calibration cal_data;

  if (check_device(sh)) return -1;

  int num_axes = analog_axis_num_axes(dev);
  int ch = atoi(argv[1]);
  if (ch >= num_axes || ch < 0) {
    shell_print(sh, "Error: Invalid <ch> argument\n");
    return -1;
  }

  analog_axis_calibration_get(dev, ch, &cal_data);
  shell_print(sh, "Old setting: ch.%d in_min: %d, in_max: %d, in_deadzone: %d", ch, cal_data.in_min,
              cal_data.in_max, cal_data.in_deadzone);

  cal_data.in_deadzone = atoi(argv[2]);

  analog_axis_calibration_set(dev, ch, &cal_data);
  analog_axis_calibration_save(dev);

  analog_axis_calibration_get(dev, ch, &cal_data);
  shell_print(sh, "New setting: ch.%d in_min: %d, in_max: %d, in_deadzone: %d", ch, cal_data.in_min,
              cal_data.in_max, cal_data.in_deadzone);

  return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(
  analog_axis_sub_cmds, SHELL_CMD_ARG(show, NULL, "Show current calibration data", cmd_show, 1, 0),
  SHELL_CMD_ARG(cal, NULL, "Calibration\tUsage: analog_axis cal <ch> <out_min> <out_max>", cmd_cal,
                4, 0),
  SHELL_CMD_ARG(test, NULL, "Test analog_axis\tUsage: analog_axis test, cmd_test", cmd_test, 1, 0),
  SHELL_CMD_ARG(deadzone, NULL, "Set deadzone value\tUsage: analog_axis deadzone <ch> <value>",
                cmd_deadzone, 3, 0),

  SHELL_SUBCMD_SET_END /* Array terminated. */
);
SHELL_CMD_ARG_REGISTER(analog_axis, &analog_axis_sub_cmds, "analog_axis commands", NULL, 2, 0);
