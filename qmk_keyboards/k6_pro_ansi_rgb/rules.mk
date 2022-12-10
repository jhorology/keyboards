# MCU name
MCU = STM32L432

# Bootloader selection
BOOTLOADER = stm32-dfu

# Build Options
#   change yes to no to disable
#
# Do not enable SLEEP_LED_ENABLE. it uses the same timer as BACKLIGHT_ENABLE
SLEEP_LED_ENABLE = no       # Breathing sleep LED during USB suspend
# if this doesn't work, see here: https://github.com/tmk/tmk_keyboard/wiki/FAQ#nkro-doesnt-work
BLUETOOTH_ENABLE = no       # Enable Bluetooth with
KC_BLUETOOTH_ENABLE = yes       # Enable Bluetooth with
BLUETOOTH_DRIVER = custom
DIP_SWITCH_ENABLE = yes
RGB_MATRIX_ENABLE = yes
RGB_MATRIX_DRIVER = CKLED2001
EEPROM_DRIVER = wear_leveling
WEAR_LEVELING_DRIVER = embedded_flash

# radial controller
RADIAL_CONTROLLER_ENABLE = yes
SRC += lib/radial_controller.c

# Enter lower-power sleep mode when on the ChibiOS idle thread
OPT_DEFS += -DNO_USB_STARTUP_CHECK
OPT_DEFS += -DKC_BLUETOOTH_ENABLE

CUSTOM_MATRIX = lite
SRC += matrix.c

SRC += \
  bluetooth/bluetooth.c \
  bluetooth/report_buffer.c \
  bluetooth/ckbt51.c \
  bluetooth/indicator.c \
  bluetooth/bluetooth_main.c \
  bluetooth/transport.c \
  bluetooth/lpm.c \
  bluetooth/lpm_stm32l432.c \
  bluetooth/battery.c \
  bluetooth/bat_level_animation.c

KEYCHRON_BLUETOOTH_DIR := $(dir $(lastword $(MAKEFILE_LIST)))bluetooth
VPATH += $(KEYCHRON_BLUETOOTH_DIR)
