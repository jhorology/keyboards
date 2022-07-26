# MCU name
MCU = atmega32u4

# Bootloader selection
BOOTLOADER = atmel-dfu

ENCODER_ENABLE = yes        # Enable Encoder

# reducing firmware size
ifeq ($(strip $(VIAL_ENABLE)), yes)
  QMK_SETTINGS = no
  # MOUSEKEY_ENABLE = no
  # TAP_DANCE_ENABLE = no
  # COMBO_ENABLE = no
  # KEY_OVERRIDE_ENABLE = no
  # RGB_MATRIX_ENABLE = no
endif
