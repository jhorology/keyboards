#include <zephyr/shell/shell.h>

#include <hardware/flash.h>

#define FLASH_RDID_CMD 0x9f
#define FLASH_RDID_TOTAL_BYTES (3 + 1)

static int cmd_handler(const struct shell *sh, size_t argc, char **argv, void *data) {
  uint8_t txbuf[FLASH_RDID_TOTAL_BYTES] = {0};
  uint8_t rxbuf[FLASH_RDID_TOTAL_BYTES] = {0};
  txbuf[0] = FLASH_RDID_CMD;
  flash_do_cmd(txbuf, rxbuf, FLASH_RDID_TOTAL_BYTES);
  shell_fprintf(sh, SHELL_NORMAL, "jedec-id = [%02x %02x %02x];\n", rxbuf[1], rxbuf[2], rxbuf[3]);
  shell_fprintf(sh, SHELL_NORMAL, "Probably the capacity is %d bytes\n", 1 << rxbuf[3]);
  return 0;
}

SHELL_CMD_ARG_REGISTER(rp2flash, NULL, "show rp2 flash info", &cmd_handler, 1, 0);
