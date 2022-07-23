# MCU name
MCU = atmega32u2

# Bootloader selection
BOOTLOADER = atmel-dfu

# reducing firmware size
ifeq ($(strip $(VIAL_ENABLE)), yes)
  # QMK_SETTINGS = no
  # TAP_DANCE_ENABLE = no
  COMBO_ENABLE = no
  KEY_OVERRIDE_ENABLE = no
endif
