ENCODER_ENABLE = yes        # Enable Encoder
ENCODER_MAP_ENABLE = yes

# debounce
# DEBOUNCE_TYPE = sym_eager_pk

# radial controller
RADIAL_CONTROLLER_ENABLE = yes
SRC += lib/radial_controller.c

# gcc optimization
OPT = s  # optimize for size
