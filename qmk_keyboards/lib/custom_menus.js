const _ = require('underscore')

const RADIAL_CONTROLLER_MENU = {
  label: 'Radial Controller',
  content: [
    {
      label: 'Settings',
      content: [
        {
          label: 'Encoder Clicks per Roatation',
          type: 'dropdown',
          options: [
            ['16', 16],
            ['18', 18],
            ['20', 20],
            ['30', 30],
            ['36', 36]
          ],
          content: ['id_custom_rc_encoder_clicks', 5, 1]
        },
        {
          label: 'Angular Speed While Key Pressing',
          type: 'range',
          options: [0, 255],
          content: ['id_custom_rc_key_angular_speed', 5, 2]
        },
        {
          label: 'Fine-tune Ratio',
          type: 'dropdown',
          options: [
            ['None', 0],
            ['1/2', 1],
            ['1/4', 2],
            ['1/8', 3]
          ],
          content: ['id_custom_rc_fine_tune_ratio', 5, 3]
        },
        {
          showIf: '{id_custom_rc_fine_tune_ratio} != 0',
          label: 'Fine-tune Modifier: Ctrl',
          type: 'toggle',
          content: ['id_custom_rc_fine_tune_mod_ctrl', 5, 4]
        },
        {
          showIf: '{id_custom_rc_fine_tune_ratio} != 0',
          label: 'Fine-tune Modifier: Shift',
          type: 'toggle',
          content: ['id_custom_rc_fine_tune_mod_shift', 5, 5]
        },
        {
          showIf: '{id_custom_rc_fine_tune_ratio} != 0',
          label: 'Fine-tune Modifier: Alt',
          type: 'toggle',
          content: ['id_custom_rc_fine_tune_mod_alt', 5, 6]
        },
        {
          showIf: '{id_custom_rc_fine_tune_ratio} != 0',
          label: 'Fine-tune Modifier: Gui',
          type: 'toggle',
          content: ['id_custom_rc_fine_tune_mod_gui', 5, 7]
        }
      ]
    }
  ]
}
function createTapDanceMenu(size) {
  return {
    label: 'Tap Dance',
    content: _.range(size).map((i) => ({
      label:
        'TD' + i + ' - 0x57' + i.toString(16).toUpperCase().padStart(2, '0'),
      content: [
        {
          label: 'Single Tap',
          type: 'keycode',
          content: [`id_custom_td_single_tap`, 6 + i, 1]
        },
        {
          label: 'Hold',
          type: 'keycode',
          content: [`id_custom_td_single_hold`, 6 + i, 2]
        },
        {
          label: 'Double Tap',
          type: 'keycode',
          content: [`id_custom_td_multi_tap`, 6 + i, 3]
        },
        {
          label: 'Tap Hold',
          type: 'keycode',
          content: [`id_custom_td_tap_hold`, 6 + i, 4]
        },
        {
          label: 'Tapping Term',
          type: 'range',
          options: [0, 255],
          content: [`id_custom_td_tapping_term`, 6 + i, 5]
        }
      ]
    }))
  }
}
module.exports = function (options, defines) {
  const customMenus = []
  if (defines.TAP_DANCE_ENTRIES) {
    // TODO dosen't work
    customMenus.push(createTapDanceMenu(defines.TAP_DANCE_ENTRIES))
  }
  if (options.RADIAL_CONTROLLER_ENABLE === 'yes') {
    customMenus.push(RADIAL_CONTROLLER_MENU)
  }
  return customMenus.length ? customMenus : undefined
}
