#include <zephyr/shell/shell.h>

#if IS_ENABLED(CONFIG_PICOSDK_USE_FLASH)
#  include <hardware/flash.h>
#  define FLASH_RDID_CMD 0x9f
#  define FLASH_RDID_TOTAL_BYTES (3 + 1)
#endif

static int cmd_hello(const struct shell *sh, size_t argc, char **argv) {
  shell_fprintf(sh, SHELL_NORMAL, "hello!\n");
  return 0;
}

#if IS_ENABLED(CONFIG_PICOSDK_USE_FLASH)
// zephyr 3.2 Flash API dosen't support RP2040
// for research purposes only, maybe danger with running application
static int cmd_get_rp2_flash_info(const struct shell *sh, size_t argc, char **argv) {
  uint8_t txbuf[FLASH_RDID_TOTAL_BYTES] = {0};
  uint8_t rxbuf[FLASH_RDID_TOTAL_BYTES] = {0};
  txbuf[0] = FLASH_RDID_CMD;
  flash_do_cmd(txbuf, rxbuf, FLASH_RDID_TOTAL_BYTES);
  shell_fprintf(sh, SHELL_NORMAL, "jedec-id = [%02x %02x %02x];\n", rxbuf[1], rxbuf[2], rxbuf[3]);
  shell_fprintf(sh, SHELL_NORMAL, "Probably the capacity is %d bytes\n", 1 << rxbuf[3]);
  return 0;
}
#endif

SHELL_STATIC_SUBCMD_SET_CREATE(sub_info, SHELL_CMD_ARG(hello, NULL, "Hello", cmd_hello, 1, 0),
#if IS_ENABLED(CONFIG_PICOSDK_USE_FLASH)
                               SHELL_CMD_ARG(rp2flash, NULL, "Show flash chip info", cmd_get_rp2_flash_info, 1, 0),
#endif
                               SHELL_SUBCMD_SET_END /* Array terminated. */
);
SHELL_CMD_ARG_REGISTER(info, &sub_info, "INFO commands", NULL, 2, 0);
