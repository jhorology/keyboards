
# TODO include only if any font is enabled
include(${ZEPHYR_CURRENT_MODULE_DIR}/cmake/lv_font_conv.cmake NO_POLICY_SCOPE)

#------------------------------------------------------------------------------
# micro5
# https://github.com/scfried/soft-type-micro.git
# OFL-1.1 license
#------------------------------------------------------------------------------
if(CONFIG_ZMK_FONT_MICRO5_10)
  generate_lv_font_for_target(
    app
    ${ZEPHYR_BASE}/../fonts/soft-type-micro/fonts//ttf/Micro5-Regular.ttf
    micro5_10                  # structure name
    1                          # bpp
    10                         # size
    --autohint-off             # should be off for pixel font
    --range 0x20-0x7f          # only ascii glyph
  )
endif()

#------------------------------------------------------------------------------
# tiny5
# https://github.com/Gissio/font_Tiny5
# OFL-1.1 license
#------------------------------------------------------------------------------
if(CONFIG_ZMK_FONT_TINY5_9)
  generate_lv_font_for_target(
    app
    ${ZEPHYR_BASE}/../fonts/font_Tiny5/fonts//ttf/Tiny5-SolidRegular.ttf
    tiny5_9                    # structure name
    1                          # bpp
    9                          # size
    # --autohint-off           # should be off for pixel font
    --range 0x20-0x7f          # only ascii glyph
  )
endif()

if(CONFIG_ZMK_FONT_TINY5_BOLD_9)
  generate_lv_font_for_target(
    app
    ${ZEPHYR_BASE}/../fonts/font_Tiny5/fonts//ttf/Tiny5-SolidBold.ttf
    tiny5_bold_9               # structure name
    1                          # bpp
    9                          # size
    # --autohint-off           # should be off for pixel font
    --range 0x20-0x7f          # only ascii glyph
  )
endif()

#------------------------------------------------------------------------------
# PixelMplus font
# https://github.com/itouhiro/PixelMplus
# M+ FONT lisense
#------------------------------------------------------------------------------
if(CONFIG_ZMK_FONT_PIXEL_MPLUS_10)
  # 5x10 mono
  generate_lv_font_for_target(
    app
    ${ZEPHYR_BASE}/../fonts/PixelMplus-20130602/PixelMplus10-Regular.ttf
    pixel_mplus_10  # structure name
    1                          # bpp
    10                         # size
    --autohint-off             # should be off for pixel font
    --range 0x20-0x7f          # only ascii glyph
    --range 0x30A0-0x30ff      # full-width Katakana
  )
endif()

if(CONFIG_ZMK_FONT_PIXEL_MPLUS_12)
  # 6x12 mono
  generate_lv_font_for_target(
    app
    ${ZEPHYR_BASE}/../fonts/PixelMplus-20130602/PixelMplus12-Regular.ttf
    pixel_mplus_12             # structure name
    1                          # bpp
    12                         # size
    --autohint-off             # should be off for pixel font
    --range 0x20-0x7f          # ascii
  )
endif()

if(CONFIG_ZMK_FONT_PIXEL_MPLUS_BOLD_10)
  # 5x10 mono
  generate_lv_font_for_target(
    app
    ${ZEPHYR_BASE}/../fonts/PixelMplus-20130602/PixelMplus10-Bold.ttf
    pixel_mplus_bold_10        # structure name
    1                          # bpp
    10                         # size
    --autohint-off             # should be off for pixel font
    --range 0x20-0x7f          # only ascii glyph
  )
endif()

if(CONFIG_ZMK_FONT_PIXEL_MPLUS_BOLD_12)
  # 6x12 mono
  generate_lv_font_for_target(
    app
    ${ZEPHYR_BASE}/../fonts/PixelMplus-20130602/PixelMplus12-Bold.ttf
    pixel_mplus_bold_12        # structure name
    1                          # bpp
    12                         # size
    --autohint-off             # should be off for pixel font
    --range 0x20-0x7f          # ascii
  )
endif()

#------------------------------------------------------------------------------
# teko
# https://github.com/googlefonts/teko
# OFL-1.1 license
# condensed, propotional
#------------------------------------------------------------------------------
if(CONFIG_ZMK_FONT_TEKO_BOLD_15)
  generate_lv_font_for_target(
    app
    ${ZEPHYR_BASE}/../fonts/teko/fonts/ttf/Teko-SemiBold.ttf
    teko_bold_15               # structure name
    1                          # bpp
    15                         # size
    # --autohint-off
    --range 0x20-0x7f          # only ascii glyph
  )
endif()

if(CONFIG_ZMK_FONT_TEKO_15)
  generate_lv_font_for_target(
    app
    ${ZEPHYR_BASE}/../fonts/teko/fonts/ttf/Teko-Regular.ttf
    teko_15                    # structure name
    1                          # bpp
    15                         # size
    # --autohint-off
    --range 0x20-0x7f          # only ascii glyph
  )
endif()

#------------------------------------------------------------------------------
# Cozetta 6x13 pixel font
# https://github.com/slavfox/Cozette
# MIT license
#
# TODO delete unnecessary icons
# TODO search pixel perfect icons in a little bigger size
#
# *use nerd-font to see correct icons
#------------------------------------------------------------------------------
if(CONFIG_ZMK_FONT_COZETTA_ICONS_13)
  generate_lv_font_for_target(
    app
    ${ZEPHYR_BASE}/../fonts/CozetteFonts/CozetteVector.ttf
    cozetta_icons_13           # structure name
    1                          # bpp
    13                         # size (cozzeta is 6x13 pixel font)
    --autohint-off             # should be off for pixel font

    # space
    --range 0x20

    # vertical bar |
    --range 0x7c

    # 0x023fb ⏻ nf-iec-power
    # 0x023fc ⏼ nf-iec-toggle_power
    --range 0x23fb-0x23fc

    # 0x0e615  nf-seti-config
    --range 0xe615

    # 0x0e725  nf-dev-git_branch
    # 0x0e726  nf-dev-git_pull_request
    # 0x0e727  nf-dev-git_merge
    # 0x0e728  nf-dev-git_compare
    --range 0xe725-0xe728

    # 0x0f00c  nf-fa-check
    # 0x0f00d  nf-fa-close nf-fa-times nf-fa-xmark
    --range 0xf00c-0xf00d

    # 0x0f03a  nf-fa-list
    --range 0xf03a

    # 0x0f040  nf-fa-pencil
    --range 0xf040

    # 0x0f048  nf-fa-backward_step nf-fa-step_backward
    # 0x0f049  nf-fa-backward_fast nf-fa-fast_backward
    # 0x0f04a  nf-fa-backward
    # 0x0f04b  nf-fa-play
    # 0x0f04c  nf-fa-pause
    # 0x0f04d  nf-fa-stop
    # 0x0f04e  nf-fa-forward
    --range 0xf048-0xf04e

    # 0x0f050  nf-fa-fast_forward nf-fa-forward_fast
    # 0x0f051  nf-fa-forward_step nf-fa-step_forward
    # 0x0f052  nf-fa-eject
    # 0x0f053  nf-fa-chevron_left
    # 0x0f054  nf-fa-chevron_right
    # 0x0f055  nf-fa-plus_circle
    # 0x0f056  nf-fa-minus_circle
    # 0x0f057  nf-fa-remove_sign nf-fa-times_circle
    # 0x0f058  nf-fa-check_circle nf-fa-ok_sign
    # 0x0f059  nf-fa-circle_question nf-fa-question_circle
    # 0x0f05a  nf-fa-circle_info nf-fa-info_circle
    --range 0xf050-0xf05a

    # 0x0f0e7  nf-fa-bolt nf-fa-flash
    --range 0xf0e7

    # 0x0f126  nf-fa-code_branch nf-fa-code_fork
    --range 0xf126

    # 0x0f179  nf-fa-apple
    # 0x0f17a  nf-fa-windows
    # 0x0f17b  nf-fa-android
    # 0x0f17c  nf-fa-linux
    --range 0xf179-0xf17c

    # 0x0f1eb  nf-fa-wifi
    --range 0xf1eb

    # 0x0f240  nf-fa-battery nf-fa-battery_4 nf-fa-battery_full
    # 0x0f241                nf-fa-battery_3 nf-fa-battery_three_quaters
    # 0x0f242                nf-fa-battery_2 nf-fa-battery_half
    # 0x0f243                nf-fa-battery_1 nf-fa-battery_quater
    # 0x0f244                nf-fa-battery_0 nf-fa-battery_empty
    --range 0xf240-0xf244

    # 0x0f293  nf-fa-bluetooth
    # 0x0f294  nf-fa-bluetooth_b
    --range 0xf293-0xf294

    #
    # material design icons
    #  *Coetta dosen't support nerd-font v3 code range
    #  *LVGL seems to support only 16 bit code range
    #  *icon = nerd-font v3
    # 0x0f578=>0xf0079 =>󰁹 nf-md-battery
    # 0x0f579=>0xf007a =>󰁺 nf-md-battery_10
    # 0x0f57a=>0xf007b =>󰁻 nf-md-battery_20
    # 0x0f57b=>0xf007c =>󰁼 nf-md-battery_30
    # 0x0f57c=>0xf007d =>󰁽 nf-md-battery_40
    # 0x0f57d=>0xf007e =>󰁾 nf-md-battery_50
    # 0x0f57e=>0xf007f =>󰁿 nf-md-battery_60
    # 0x0f57f=>0xf0080 =>󰂀 nf-md-battery_70
    # 0x0f580=>0xf0081 =>󰂁 nf-md-battery_80
    # 0x0f581=>0xf0082 =>󰂂 nf-md-battery_90
    # 0x0f582=>0xf0083 =>󰂃 nf-md-battery_alert
    # 0x0f583=>0xf0084 =>󰂄 nf-md-battery_charging
    # 0x0f584=>0xf0085 =>󰂅 nf-md-battery_charging_100
    # 0x0f585=>0xf0086 =>󰂆 nf-md-battery_charging_20
    # 0x0f586=>0xf0087 =>󰂇 nf-md-battery_charging_30
    # 0x0f587=>0xf0088 =>󰂈 nf-md-battery_charging_40
    # 0x0f588=>0xf0089 =>󰂉 nf-md-battery_charging_60
    # 0x0f589=>0xf008a =>󰂊 nf-md-battery_charging_80
    # 0x0f58a=>0xf008b =>󰂋 nf-md-battery_charging_90
    # 0x0f58b=>0xf008c =>󰂌 nf-md-battery_minus
    # 0x0f58c=>0xf008d =>󰂍 nf-md-battery_negative
    # 0x0f58d=>0xf008e =>󰂎 nf-md-battery_outline
    # 0x0f58e=>0xf008f =>󰂏 nf-md-battery_plus
    # 0x0f58f=>0xf0090 =>󰂐 nf-md-battery_positive
    # 0x0f590=>0xf0091 =>󰂑 nf-md-battery_unknown
    # --range '0xf578-0xf590=>0xf0079'
    --range 0xf578-0xf590

    # 0x0f5ae=>0xf00af =>󰂯 nf-md-bluetooth
    # 0x0f5af=>0xf00b0 =>󰂰 nf-md-bluetooth_audio
    # || 0x0f5b0 nf-mdi-bluetooth_connect *not included in cozetta font
    # --range '0xf5ae-0xf5af=>0xf00af'
    --range 0xf5ae-0xf5af

    # 0x0f5b1=>0xf00b2 =>󰂲 nf-md-bluetooth_off
    # 0x0f5b2=>0xf00b3 =>󰂳 nf-md-bluetooth_settings
    # --range '0xf5b1-0xf5b2=>0xf00b2'
    --range 0xf5b1-0xf5b2

    # 0x0f5eb=>0xf00ec =>󰃬 nf-md-calculator
    # --range '0xf5eb=>0xf00ec'
    --range 0xf5eb

    # 0x0f831=>0xf0ab7 =>󰪷 nf-md-text_box_multiple
    # --range '0xf831=>0xf0ab7'
    --range 0xf831

    # 0x0f832=>0xf0333 =>󰌳 nf-md-music_box_multiple
    # 0x0f833=>0xf0334 =>󰌴 nf-md-plus_box_multiple
    # 0x0f834=>0xf0335 =>󰌵 nf-md-lightbulb
    # 0x0f835=>0xf0336 =>󰌶 nf-md-lightbulb_outline
    # 0x0f836=>0xf0337 =>󰌷 nf-md-link
    # 0x0f837=>0xf0338 =>󰌸 nf-md-link_off
    # --range '0xf832-0xf837=>0xf0333'
    --range 0xf832-0xf837

    # 0x0fa7d=>0xf057e 墳=>󰕾 nf-md-volume_high
    # 0x0fa7e=>0xf057f 奄=>󰕿 nf-mdi-volume_low
    # 0x0fa7f=>0xf0580 奔=>󰖀 nf-md-volume_medium
    # 0x0fa80=>0xf0581 婢=>󰖁 nf-md-volume_off
    # --range '0xfa7d-0xfa80=>0xf057e'
    --range 0xfa7d-0xfa80

    # 0x0faa8=>0xf05a9 直=>󰖩 nf-md-wifi
    # 0x0faa9=>0xf05aa 睊=>󰖪 nf-md-wifi_off
    # --range '0xfaa8-0xfaa9=>0xf05a9'
    --range 0xfaa8-0xfaa9

    # 0x0fc5b=>0xf075d ﱛ=>󰝝 nf-md-volume_plus
    # 0x0fc5c=>0xf075e ﱜ=>󰝞 nf-md-volume_minus
    # 0x0fc5d=>0xf075f ﱝ=>󰝟 nf-md-volume_mute
    # --range '0xfc5b-0xfc5d=>0xf075d'
    --range 0xfc5b-0xfc5d
  )
endif()
