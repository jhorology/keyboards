# MCU name
MCU = STM32F103
MCU_LDSCRIPT = STM32F103xB_uf2
FIRMWARE_FORMAT = uf2
BOARD = STM32_F103_STM32DUINO

# Bootloader selection
BOOTLOADER = custom

# Build Options
#   comment out to disable the options.
#
RGB_MATRIX_ENABLE = yes        # Use RGB matrix
RGB_MATRIX_DRIVER = WS2812
WS2812_DRIVER = pwm

# radial controller
RADIAL_CONTROLLER_ENABLE = yes
# radial controller requires DEFERRED_EXEC_ENABLE
DEFERRED_EXEC_ENABLE = yes
SRC += lib/radial_controller.c

SRC += uf2_boot.c

# reducing RAM usage
ifeq ($(strip $(VIAL_ENABLE)), yes)
  # QMK_SETTINGS = no
  # -10 bytes
  # MOUSEKEY_ENABLE = no

  # -1080 bytes
  # TAP_DANCE_ENABLE = no

  #  -864 bytes
  # COMBO_ENABLE = no

  #  -944 bytes
  # KEY_OVERRIDE_ENABLE = no

  # RGB_MATRIX_ENABLE = no

  # default 0x0800, 0x0400
  # USE_PROCESS_STACKSIZE = 0x0400
  # USE_EXCEPTIONS_STACKSIZE = 0x0200
endif
