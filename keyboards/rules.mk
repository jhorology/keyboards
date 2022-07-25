# Build Options
#   change yes to no to disable
#
BOOTMAGIC_ENABLE = yes      # Enable Bootmagic Lite
MOUSEKEY_ENABLE = yes       # Mouse keys
EXTRAKEY_ENABLE = yes       # Audio control and System control
CONSOLE_ENABLE = no         # Console for debug
COMMAND_ENABLE = no         # Commands for debug and configuration
AUDIO_ENABLE = no           # Audio output
LTO_ENABLE = yes            # Link time optimization
VIA_ENABLE = yes

ifeq ($(strip $(VIAL_ENABLE)), no)
  TAP_DANCE_ENABLE = yes
endif

ifeq ($(strip $(APPLE_FN_ENABLE)), yes)
  # see https://gist.github.com/fauxpark/010dcf5d6377c3a71ac98ce37414c6c4
  NKRO_ENABLE = no
endif
SRC += lib/my_keyboard_common.c
SRC += lib/layout_util.c
SRC += lib/alternate_device_descriptor.c
