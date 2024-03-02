# Build Options
#   change yes to no to disable.
#
DIP_SWITCH_ENABLE = yes
RGB_MATRIX_ENABLE = yes

# debounce
DEBOUNCE_TYPE = sym_eager_pk

# os detection debug
OS_FINGERPRINT_DEBUG_ENABLE = yes
OPT_DEFS += -DOS_FINGERPRINT_DEBUG_ENABLE
SEND_STRING_ENABLE = yes

# gcc optimization
OPT = 2
