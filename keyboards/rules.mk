# Build Options
#   change yes to no to disable
#
BOOTMAGIC_ENABLE = yes      # Enable Bootmagic Lite
MOUSEKEY_ENABLE = no        # Mouse keys
EXTRAKEY_ENABLE = yes       # Audio control and System control
CONSOLE_ENABLE = no         # Console for debug
COMMAND_ENABLE = no         # Commands for debug and configuration
AUDIO_ENABLE = no           # Audio output
LTO_ENABLE = yes            # Link time optimization
VIA_ENABLE = yes
NKRO_ENABLE = yes
APPLE_FN_ENABLE = yes
RADIAL_CONTROLLER_ENABLE = yes

ifeq ($(strip $(MCU)), atmega32u2)
  # for lack of available usb endpoints
  RADIAL_CONTROLLER_SHARED_EP = yes
else
  # separated endpoint is for except windows,
  # shared endpoint can't open from application
  RADIAL_CONTROLLER_SHARED_EP = no
endif

ifeq ($(strip $(VIAL_ENABLE)), no)
  TAP_DANCE_ENABLE = yes
  COMBO_ENABLE = yes
endif

SRC += lib/my_keyboard_common.c
SRC += lib/custom_config.c
SRC += lib/apple_fn.c
SRC += lib/jis_util.c
SRC += lib/alternate_device_descriptor.c
SRC += lib/radial_controller.c

ifeq ($(strip $(VIAL_ENABLE)), no)
  SRC += lib/tap_dance.c
endif
