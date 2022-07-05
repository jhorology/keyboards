VIA_ENABLE = yes
VIAL_ENABLE = yes
QMK_SETTINGS = no
# TODO it's diffficult to balance VIAL and RGB_MATRIX due to RAM usage.
# reducing firmware size and RAM usage
# see https://get.vial.today/docs/firmware-size.htmlhttps://get.vial.today/docs/firmware-size.html
# MOUSEKEY_ENABLE = no          # Mouse keys
# TAP_DANCE_ENABLE = no
COMBO_ENABLE = no
KEY_OVERRIDE_ENABLE = no
# RGB_MATRIX_ENABLE = no        # Use RGB matrix
USE_PROCESS_STACKSIZE = 0x0400
USE_EXCEPTIONS_STACKSIZE = 0x0200
