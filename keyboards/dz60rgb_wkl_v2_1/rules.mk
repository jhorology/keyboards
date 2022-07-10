# MCU name
MCU = atmega32u4

# Bootloader selection
BOOTLOADER = lufa-ms
BOOTLOADER_SIZE = 6144

# Build Options
#   change yes to no to disable
#
RGB_MATRIX_ENABLE = yes        # Use RGB matrix
RGB_MATRIX_DRIVER = IS31FL3733

# reducing firmware size
ifeq ($(strip $(VIAL_ENABLE)), yes)
  # TAP_DANCE_ENABLE = no
  COMBO_ENABLE = no
  KEY_OVERRIDE_ENABLE = no
  # RGB_MATRIX_ENABLE = no
endif
