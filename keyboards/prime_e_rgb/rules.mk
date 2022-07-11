# MCU name
MCU = atmega32u4

# Bootloader selection
BOOTLOADER = atmel-dfu

BACKLIGHT_ENABLE = no
RGBLIGHT_ENABLE = yes

# reduce firmware size
ifeq ($(strip $(VIAL_ENABLE)), yes)
   QMK_SETTINGS = no
endif
