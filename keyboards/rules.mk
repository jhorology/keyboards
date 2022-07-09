# Build Options
#   change yes to no to disable
#
BOOTMAGIC_ENABLE = yes      # Enable Bootmagic Lite
MOUSEKEY_ENABLE = yes       # Mouse keys
EXTRAKEY_ENABLE = yes       # Audio control and System control
CONSOLE_ENABLE = no         # Console for debug
COMMAND_ENABLE = no         # Commands for debug and configuration
NKRO_ENABLE = no            # Enable N-Key Rollover
BACKLIGHT_ENABLE = no       # Enable keyboard backlight functionality
RGBLIGHT_ENABLE = no        # Enable keyboard RGB underglow
RGB_MATRIX_ENABLE = no      # Use RGB matrix
AUDIO_ENABLE = no           # Audio output
LTO_ENABLE = yes            # Link time optimization
# Apple Fn/Globe key patch
# see https://gist.github.com/fauxpark/010dcf5d6377c3a71ac98ce37414c6c4
NKRO_ENABLE = no

VIA_ENABLE = yes

ifeq ($(strip $(VIAL_ENABLE)), yes)
	QMK_SETTINGS := no
endif

ifeq ($(strip $(APPLE_FN_ENABLE)), yes)
	SRC += lib/apple_fn.c
endif
