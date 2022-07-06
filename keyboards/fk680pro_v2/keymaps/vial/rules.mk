VIA_ENABLE = yes
VIAL_ENABLE = yes

# reducing firmware size and RAM usage
# see https://get.vial.today/docs/firmware-size.html

QMK_SETTINGS = no

# -10 bytes
# MOUSEKEY_ENABLE = no

# -1080 bytes
# TAP_DANCE_ENABLE = no

#  -864 bytes
COMBO_ENABLE = no

#  -944 bytes
KEY_OVERRIDE_ENABLE = no

# RGB_MATRIX_ENABLE = no

# default 0x0800, 0x0400
USE_PROCESS_STACKSIZE = 0x0400
USE_EXCEPTIONS_STACKSIZE = 0x0200
