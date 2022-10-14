const _ = require('underscore'),
  ID_CUSTOM_MAGIC_CHANNEL = 5,
  ID_CUSTOM_RC_CHANNEL = 6,
  ID_CUSTOM_TD_CHANNEL_START = 7

const QMK_MAGIC_MENU = {
  label: 'QMK Magic',
  content: [
    {
      label: 'Settings',
      content: [
        {
          label: 'Swap Caps Lock and Left Control',
          type: 'toggle',
          content: [
            'id_custom_magic_swap_control_capslock',
            ID_CUSTOM_MAGIC_CHANNEL,
            1
          ]
        },
        {
          label: 'Swap Caps Lock and Escape',
          type: 'toggle',
          content: [
            'id_custom_magic_swap_escape_capslock',
            ID_CUSTOM_MAGIC_CHANNEL,
            2
          ]
        },
        {
          label: 'Treat Caps Lock as Control',
          type: 'toggle',
          content: [
            'id_custom_magic_capslock_to_control',
            ID_CUSTOM_MAGIC_CHANNEL,
            3
          ]
        },
        {
          label: 'Swap Left Control and GUI',
          type: 'toggle',
          content: [
            'id_custom_magic_swap_lctl_lgui',
            ID_CUSTOM_MAGIC_CHANNEL,
            4
          ]
        },
        {
          label: 'Swap Right Control and GUI',
          type: 'toggle',
          content: [
            'id_custom_magic_swap_rctl_rgui',
            ID_CUSTOM_MAGIC_CHANNEL,
            5
          ]
        },
        {
          label: 'Swap Left Alt and GUI',
          type: 'toggle',
          content: [
            'id_custom_magic_swap_lalt_lgui',
            ID_CUSTOM_MAGIC_CHANNEL,
            6
          ]
        },
        {
          label: 'Swap Right Alt and GUI',
          type: 'toggle',
          content: [
            'id_custom_magic_swap_ralt_rgui',
            ID_CUSTOM_MAGIC_CHANNEL,
            7
          ]
        },
        {
          label: 'Disable the GUI keys',
          type: 'toggle',
          content: ['id_custom_magic_no_gui', ID_CUSTOM_MAGIC_CHANNEL, 8]
        },
        {
          label: 'Swap ` and Escape',
          type: 'toggle',
          content: [
            'id_custom_magic_swap_grave_esc',
            ID_CUSTOM_MAGIC_CHANNEL,
            9
          ]
        },
        {
          label: 'Enable N-key rollover',
          type: 'toggle',
          content: ['id_custom_magic_host_nkro', ID_CUSTOM_MAGIC_CHANNEL, 10]
        },
        {
          label:
            'Set the master half of a split keyboard as the left hand (for EE_HANDS)',
          type: 'toggle',
          content: [
            'id_custom_magic_ee_hands_left',
            ID_CUSTOM_MAGIC_CHANNEL,
            11
          ]
        }
      ]
    }
  ]
}

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
          content: ['id_custom_rc_encoder_clicks', ID_CUSTOM_RC_CHANNEL, 1]
        },
        {
          label: 'Angular Speed While Key Pressing: (15 - 270 deg/sec)',
          type: 'range',
          options: [0, 255],
          content: ['id_custom_rc_key_angular_speed', ID_CUSTOM_RC_CHANNEL, 2]
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
          content: ['id_custom_rc_fine_tune_ratio', ID_CUSTOM_RC_CHANNEL, 3]
        },
        {
          showIf: '{id_custom_rc_fine_tune_ratio} != 0',
          label: 'Fine-tune Modifier: Ctrl',
          type: 'toggle',
          content: ['id_custom_rc_fine_tune_mod_ctrl', ID_CUSTOM_RC_CHANNEL, 4]
        },
        {
          showIf: '{id_custom_rc_fine_tune_ratio} != 0',
          label: 'Fine-tune Modifier: Shift',
          type: 'toggle',
          content: ['id_custom_rc_fine_tune_mod_shift', ID_CUSTOM_RC_CHANNEL, 5]
        },
        {
          showIf: '{id_custom_rc_fine_tune_ratio} != 0',
          label: 'Fine-tune Modifier: Alt',
          type: 'toggle',
          content: ['id_custom_rc_fine_tune_mod_alt', ID_CUSTOM_RC_CHANNEL, 6]
        },
        {
          showIf: '{id_custom_rc_fine_tune_ratio} != 0',
          label: 'Fine-tune Modifier: Gui',
          type: 'toggle',
          content: ['id_custom_rc_fine_tune_mod_gui', ID_CUSTOM_RC_CHANNEL, 7]
        }
      ]
    }
  ]
}
function createTapDanceMenu(size) {
  // id_field should be unique at all of content scope
  return {
    label: 'Tap Dance',
    content: _.range(size).map((i) => ({
      label:
        'TD' + i + ' - 0x57' + i.toString(16).toUpperCase().padStart(2, '0'),
      content: [
        {
          label: 'Single Tap',
          type: 'keycode',
          content: [
            `id_custom_td_${i}_single_tap`,
            ID_CUSTOM_TD_CHANNEL_START + i,
            1
          ]
        },
        {
          label: 'Hold',
          type: 'keycode',
          content: [
            `id_custom_td_${i}_single_hold`,
            ID_CUSTOM_TD_CHANNEL_START + i,
            2
          ]
        },
        {
          label: 'Double Tap',
          type: 'keycode',
          content: [
            `id_custom_td_${i}_multi_tap`,
            ID_CUSTOM_TD_CHANNEL_START + i,
            3
          ]
        },
        {
          label: 'Tap Hold',
          type: 'keycode',
          content: [
            `id_custom_td_${i}_tap_hold`,
            ID_CUSTOM_TD_CHANNEL_START + i,
            4
          ]
        },
        {
          label: 'Tapping Term: (50 - 1000 ms)',
          type: 'range',
          options: [50, 1000],
          content: [
            `id_custom_td_${i}_tapping_term`,
            ID_CUSTOM_TD_CHANNEL_START + i,
            5
          ]
        }
      ]
    }))
  }
}

module.exports = function (options, defines) {
  const customMenus = [QMK_MAGIC_MENU]
  if (defines.TAP_DANCE_ENTRIES) {
    customMenus.push(createTapDanceMenu(defines.TAP_DANCE_ENTRIES))
  }
  if (options.RADIAL_CONTROLLER_ENABLE === 'yes') {
    customMenus.push(RADIAL_CONTROLLER_MENU)
  }
  return customMenus.length ? customMenus : undefined
}
