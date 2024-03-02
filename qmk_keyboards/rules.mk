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
SECURE_ENABLE = yes
TAP_DANCE_ENABLE = yes
DEFERRED_EXEC_ENABLE = yes

# activate apple extra usages
APPLE_EXTRA_KEY_ENABLE = no
ifeq ($(strip $(APPLE_EXTRA_KEY_ENABLE)), yes)
    OPT_DEFS += -DAPPLE_EXTRA_KEY_ENABLE
endif

# activate keycode field of keyrecord_t
ACTION_FOR_KEYCODE_ENABLE = yes
ifeq ($(strip $(ACTION_FOR_KEYCODE_ENABLE)), yes)
    OPT_DEFS += -DACTION_FOR_KEYCODE_ENABLE
endif

# common library
SRC += lib/my_keyboard_common.c
SRC += lib/custom_config.c
SRC += lib/apple_fn.c
SRC += lib/jis_util.c
SRC += lib/alternate_device_descriptor.c
SRC += lib/tap_dance.c
SRC += lib/via_custom_menus.c
SRC += lib/os_fingerprint.c
