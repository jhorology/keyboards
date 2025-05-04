
#include <stdlib.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/shell/shell.h>
#include <zephyr/input/input.h>
#include <zephyr/input/input_analog_axis_settings.h>
#include <zephyr/input/input_analog_axis.h>
#include <zephyr/devicetree.h>
#include <zephyr/sys/util_macro.h>

#define SAMPLE_BUF_SIZE 63

struct sample_result {
  int16_t min;
  int16_t median;
  int16_t max;
};

struct channel_data {
  // config
  bool invert_output;
  int16_t out_min;
  int16_t out_max;
  uint16_t axis;

  // data
  int16_t raw_val;
  int32_t out_val;
};

struct device_data {
  const struct device *dev;
  size_t num_channels;
  struct channel_data (*ch_data)[];
};

#define _NODE_SEP(node) node,
#define _NODE(node) node
#define _LIST_LEN(...) UTIL_INC(NUM_VA_ARGS_LESS_1(__VA_ARGS__))

#define ANALOG_AXIS_NODE_LIST LIST_DROP_EMPTY(DT_FOREACH_STATUS_OKAY(analog_axis_ya, _NODE_SEP))
#define ANALOG_AXIS_CH_LIST(node) DT_FOREACH_CHILD_STATUS_OKAY_SEP(node, _NODE, (, ))

#define NUM_DEVICES _LIST_LEN(ANALOG_AXIS_NODE_LIST)

#define ANALOG_AXIS_DATA(node)                    \
  {.invert_output = DT_PROP(node, invert_output), \
   .out_min = (int16_t)DT_PROP(node, out_min),    \
   .out_max = (int16_t)DT_PROP(node, out_max),    \
   .axis = DT_PROP(node, zephyr_axis)}

#define ANALOG_AXIS_CH_ARRAY(n, node) \
  static struct channel_data ch_data_##n[] = {DT_FOREACH_CHILD_SEP(node, ANALOG_AXIS_DATA, (, ))};

FOR_EACH_IDX(ANALOG_AXIS_CH_ARRAY, (), ANALOG_AXIS_NODE_LIST)

#define ANALOG_AXIS_DEVICE(n, node)                      \
  {.dev = DEVICE_DT_GET(node),                           \
   .num_channels = _LIST_LEN(ANALOG_AXIS_CH_LIST(node)), \
   .ch_data = &ch_data_##n}

static struct device_data devs[NUM_DEVICES] = {
  FOR_EACH_IDX(ANALOG_AXIS_DEVICE, (, ), ANALOG_AXIS_NODE_LIST)};

static int16_t sample_buf[SAMPLE_BUF_SIZE];
static volatile size_t sample_cnt;
static int cal_dev = -1;
static int cal_ch = -1;

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

static void analog_axis_listener(const struct device *dev, int dev_index, struct input_event *evt) {
  struct device_data *data = &devs[dev_index];
  for (size_t i = 0; i < data->num_channels; i++) {
    struct channel_data *ch_data = &(*data->ch_data)[i];
    if (evt->code == ch_data->axis) {
      ch_data->out_val = evt->value;
      return;
    }
  }
}

static void analog_axis_raw_data_cb(const struct device *dev, int ch, int16_t raw) {
  struct device_data *data;
  struct channel_data *ch_data;
  int dev_index = -1;
  for (size_t i = 0; i < NUM_DEVICES; i++) {
    if (dev == devs[i].dev) {
      dev_index = i;
      data = &devs[i];
      break;
    }
  }
  if (dev_index == -1) {
    return;
  }

  if (ch >= 0 && ch < data->num_channels) {
    ch_data = &(*data->ch_data)[ch];
    ch_data->raw_val = raw;
  }

  if (cal_dev < 0 || cal_ch < 0) {
    return;
  }

  if (dev_index == cal_dev && ch == cal_ch && sample_cnt < SAMPLE_BUF_SIZE) {
    sample_buf[sample_cnt++] = raw;
  }
}

static int compare_sample(const void *a, const void *b) {
  int16_t aa = *(const int16_t *)a;
  int16_t bb = *(const int16_t *)b;

  return (aa > bb) - (aa < bb);
};

static struct sample_result get_sample_result(int dev_index, int ch) {
  struct sample_result res;

  // start
  cal_ch = ch;
  cal_dev = dev_index;
  sample_cnt = 0;
  analog_axis_set_raw_data_cb(devs[dev_index].dev, &analog_axis_raw_data_cb);

  while (sample_cnt < SAMPLE_BUF_SIZE) k_msleep(100);

  // stop
  analog_axis_set_raw_data_cb(devs[dev_index].dev, NULL);
  cal_ch = -1;
  cal_dev = -1;

  qsort(&sample_buf[0], SAMPLE_BUF_SIZE, 2, &compare_sample);

  res.min = sample_buf[0];
  res.median = sample_buf[SAMPLE_BUF_SIZE / 2];
  res.max = sample_buf[SAMPLE_BUF_SIZE - 1];
  return res;
}

static int check_device(const struct shell *sh, size_t dev_index) {
  if (dev_index < 0 || dev_index >= NUM_DEVICES) {
    shell_error(sh, "Error: Invalid device index\n");
    return -1;
  }
  if (!device_is_ready(devs[dev_index].dev)) {
    shell_error(sh, "Error: dev.%d is not ready.\n", dev_index);
    return -2;
  }
  return 0;
}

static int cmd_show(const struct shell *sh, size_t argc, char **argv) {
  static struct analog_axis_calibration cal_data;
  struct channel_data *ch_data;

  for (size_t i = 0; i < NUM_DEVICES; i++) {
    if (check_device(sh, i)) return -1;

    shell_print(sh, "dev.%d (%s) settings:", i, devs[i].dev->name);
    for (size_t ch = 0; ch < devs[i].num_channels; ch++) {
      ch_data = &(*devs[i].ch_data)[ch];
      analog_axis_calibration_get(devs[i].dev, ch, &cal_data);
      shell_info(sh,
                 "\tch.%d in_min: %d, in_max: %d, in_deadzone: %d, out_min: %d, out_max: %d, "
                 "invert_output: %s",
                 ch, cal_data.in_min, cal_data.in_max, cal_data.in_deadzone, ch_data->out_min,
                 ch_data->out_max, ch_data->invert_output ? "true" : "false");
    }
  }
  return 0;
}

static int cmd_cal(const struct shell *sh, size_t argc, char **argv) {
#define CAL_ZERO_NUM_TIMES 10
  struct device_data *data;
  struct channel_data *ch_data;
  struct analog_axis_calibration cal_data;
  struct sample_result res;
  int dev_index;
  int ch;
  int16_t out_min;
  int16_t out_max;
  bool invert_output;
  int sum = 0;
  int min;
  int max;
  int origin;
  int16_t deadzone;
  int pos_noise;
  int neg_noise;
  int pos_range;
  int neg_range;

  dev_index = atoi(argv[1]);
  if (check_device(sh, dev_index)) return -1;
  data = &devs[dev_index];

  ch = atoi(argv[2]);
  if (ch >= data->num_channels || ch < 0) {
    shell_error(sh, "Error: Invalid <ch> argument\n");
    return -1;
  }
  ch_data = &(*data->ch_data)[ch];
  out_min = ch_data->out_min;
  out_max = ch_data->out_max;
  invert_output = ch_data->invert_output;

  if (out_min >= out_max) {
    shell_error(sh, "Error: Invalid <out_min>, <out_max> properties\n");
    return -1;
  }

  shell_print(
    sh,
    "\nSet the ch.%d axis to zero(or origin) position and then don't touch while calibrating.\n"
    "Are you sure to start? [y/n]",
    ch);
  if (confirm(sh)) return -1;

  for (int i = 0; i < CAL_ZERO_NUM_TIMES; i++) {
    res = get_sample_result(dev_index, ch);
    if (i == 0 || res.min < min) min = res.min;
    if (i == 0 || res.max > max) max = res.max;
    sum += res.median;
    origin = sum / (i + 1);
    shell_print(sh, "%d/%d  min:%d, median:%d, max:%d -> min:%d, average:%d, max:%d", i + 1,
                CAL_ZERO_NUM_TIMES, res.min, res.median, res.max, min, origin, max);
  }
  pos_noise = max - origin;
  neg_noise = origin - min;

  // Accroding to input_analog_axis.c
  //
  //       not this   <--------deadzone------->
  //       *correct   <-deadzone->|<-deadzone->
  //   in_min  -------------------|----------------- in_max
  //                             mid
  //    deadzone applied to mid position, not zero position

  deadzone = MAX(pos_noise, neg_noise) * 3 / 2;  // noise x 1.5

  shell_print(sh,
              "Done.\n\nNext, move the ch.%d axis from end to end several times.\n"
              "Are you sure to start? [y/n]",
              ch);
  if (confirm(sh)) return -1;

  min = origin, max = origin;
  while (true) {
    res = get_sample_result(dev_index, ch);
    if (res.min < min) min = res.min;
    if (res.max > max) max = res.max;
    shell_info(sh, "min:%d, max:%d\tPress [S] to stop", min, max);
    if (stop(sh)) break;
  }
  min += pos_noise;
  max -= neg_noise;

  // adjust mid position
  if (invert_output) {
    pos_range = origin - min;
    neg_range = max - origin;
  } else {
    pos_range = max - origin;
    neg_range = origin - min;
  }

  if (pos_range > neg_range) {
    // positive offset
    max = origin + neg_range;
  } else {
    // negative offset
    min = origin - pos_range;
  }

  analog_axis_calibration_get(data->dev, ch, &cal_data);
  shell_info(sh, "Done.\n\nOld setting: dev.%d ch.%d in_min: %d, in_max: %d, in_deadzone: %d",
             dev_index, ch, cal_data.in_min, cal_data.in_max, cal_data.in_deadzone);
  shell_info(sh, "Calibrated setting: dev.%d ch.%d in_min: %d, in_max: %d, in_deadzone: %d",
             dev_index, ch, min, max, deadzone);

  shell_print(sh, "\nAre you sure to save new settting? [y/n]");
  if (confirm(sh)) return -1;
  cal_data.in_min = min;
  cal_data.in_max = max;
  cal_data.in_deadzone = deadzone;
  analog_axis_calibration_set(data->dev, ch, &cal_data);
  analog_axis_calibration_save(data->dev);

  return 0;
}

static int cmd_test(const struct shell *sh, size_t argc, char **argv) {
  struct device_data *data;
  struct channel_data *ch_data;
  int dev_index;

  dev_index = atoi(argv[1]);
  if (check_device(sh, dev_index)) return -1;
  data = &devs[dev_index];

  analog_axis_set_raw_data_cb(data->dev, &analog_axis_raw_data_cb);
  while (true) {
    for (size_t ch = 0; ch < data->num_channels; ch++) {
      ch_data = &(*data->ch_data)[ch];
      shell_fprintf(sh, SHELL_NORMAL,
                    ch == 0                         ? "in: [ %d, "
                    : ch < (data->num_channels - 1) ? "%d, "
                                                    : "%d ]\t->\t",
                    ch_data->raw_val);
    }

    for (size_t ch = 0; ch < data->num_channels; ch++) {
      ch_data = &(*data->ch_data)[ch];
      shell_fprintf(sh, SHELL_NORMAL,
                    ch == 0                         ? "out: [ %d, "
                    : ch < (data->num_channels - 1) ? "%d, "
                                                    : "%d ]\t\tPress [S] to stop.\n",
                    ch_data->out_val);
    }
    if (stop(sh)) break;
    k_msleep(200);
  }
  analog_axis_set_raw_data_cb(data->dev, NULL);
  return 0;
}

static int cmd_deadzone(const struct shell *sh, size_t argc, char **argv) {
  struct device_data *data;
  struct analog_axis_calibration cal_data;
  int dev_index;

  dev_index = atoi(argv[1]);
  if (check_device(sh, dev_index)) return -1;
  data = &devs[dev_index];

  int ch = atoi(argv[2]);
  if (ch >= data->num_channels || ch < 0) {
    shell_print(sh, "Error: Invalid <ch> argument\n");
    return -1;
  }

  analog_axis_calibration_get(data->dev, ch, &cal_data);
  shell_print(sh, "Old setting: ch.%d in_min: %d, in_max: %d, in_deadzone: %d", ch, cal_data.in_min,
              cal_data.in_max, cal_data.in_deadzone);

  cal_data.in_deadzone = atoi(argv[3]);

  analog_axis_calibration_set(data->dev, ch, &cal_data);
  analog_axis_calibration_save(data->dev);

  analog_axis_calibration_get(data->dev, ch, &cal_data);
  shell_print(sh, "New setting: ch.%d in_min: %d, in_max: %d, in_deadzone: %d", ch, cal_data.in_min,
              cal_data.in_max, cal_data.in_deadzone);

  return 0;
}

#define DEFINE_CALLBACK(i, node)                                  \
  static void analog_axis_listener_##i(struct input_event *evt) { \
    analog_axis_listener(DEVICE_DT_GET(node), i, evt);            \
  }                                                               \
                                                                  \
  INPUT_CALLBACK_DEFINE(DEVICE_DT_GET(node), analog_axis_listener_##i);

FOR_EACH_IDX(DEFINE_CALLBACK, (), ANALOG_AXIS_NODE_LIST)

SHELL_STATIC_SUBCMD_SET_CREATE(
  analog_axis_sub_cmds, SHELL_CMD_ARG(show, NULL, "Show current calibration data", cmd_show, 1, 0),
  SHELL_CMD_ARG(cal, NULL, "Calibration\tUsage: analog_axis cal <dev> <ch>", cmd_cal, 3, 0),
  SHELL_CMD_ARG(test, NULL, "Test analog_axis\tUsage: analog_axis test <dev>", cmd_test, 2, 0),
  SHELL_CMD_ARG(deadzone, NULL,
                "Set deadzone value\tUsage: analog_axis deadzone <dev> <ch> <value>", cmd_deadzone,
                4, 0),

  SHELL_SUBCMD_SET_END /* Array terminated. */
);
SHELL_CMD_ARG_REGISTER(analog_axis, &analog_axis_sub_cmds, "analog_axis commands", NULL, 2, 0);
