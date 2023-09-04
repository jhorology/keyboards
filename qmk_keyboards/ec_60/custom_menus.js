const emoji = require('./preset_emoji')

module.exports = function (options, defines) {
  const menus = []

  for (let i = 0; i < defines.EC_NUM_PRESETS; i++) {
    menus.push(createPresetMenu(defines.EC_VIA_CUSTOM_CHANNEL_ID_START + i, i))
  }
  menus.push(
    createCalibrationtMenu(
      defines.EC_VIA_CUSTOM_CHANNEL_ID_START + defines.EC_NUM_PRESETS,
      defines.EC_DEBUG
    )
  )
  return [
    {
      label: 'EC Tools',
      content: menus
    }
  ]
}

function createCalibrationtMenu(channelId, debug) {
  const calibration = {
    label: 'Calibration',
    content: [
      {
        label: 'Bottoming Calibration',
        type: 'toggle',
        content: ['id_ec_bottoming_calibration', channelId, 1]
      },
      {
        label:
          'Show Calibration Data *(In 3 sconds after click, It will send data as keystrokes.)',
        type: 'toggle',
        content: ['id_ec_show_calibration_data', channelId, 2]
      }
    ]
  }
  if (debug) {
    calibration.content.push(
      {
        label:
          'Show Debug Data *(In 3 sconds after click, It will send data as keystrokes.)',
        type: 'toggle',
        content: ['id_ec_debug_send_config', channelId, 3]
      },
      {
        label: 'Bootloader Jump',
        type: 'toggle',
        content: ['id_ec_bootloader_jump', channelId, 4]
      }
    )
  }
  return calibration
}

function createPresetMenu(channelId, presetIndex) {
  const valueIds = {
    1: 'actuation_mode',
    2: 'actuation_threshold',
    3: 'actuation_travel',
    4: 'release_mode',
    5: 'release_threshold',
    6: 'release_travel',
    7: 'deadzone'
  }
  const ref = (id) => `id_ec_preset_${presetIndex}_${valueIds[id]}`
  const content = (id) => [ref(id), channelId, id]
  return {
    label: `Preset ${presetIndex} ${emoji[presetIndex % emoji.length]}`,
    content: [
      {
        label: 'Actuation Mode',
        type: 'dropdown',
        options: [
          ['STATIC', 0],
          ['DYNAMIC', 1]
        ],
        content: content(1)
      },
      {
        showIf: `{${ref(1)}} == 0`,
        label: 'Actuation Threshold (0% | 100%)',
        type: 'range',
        options: [0, 1023],
        content: content(2)
      },
      {
        showIf: `{${ref(1)}} == 1`,
        label: 'Actuation Travel (0% | 50%)',
        type: 'range',
        options: [0, 511],
        content: content(3)
      },
      {
        label: 'Release Mode',
        type: 'dropdown',
        options: [
          ['STATIC', 0],
          ['DYNAMIC', 1]
        ],
        content: content(4)
      },
      {
        showIf: `{${ref(4)}} == 0`,
        label: 'Release Threshold (0% | 100%)',
        type: 'range',
        options: [0, 1023],
        content: content(5)
      },
      {
        showIf: `{${ref(4)}} == 1`,
        label: 'Release Travel (0% | 50%)',
        type: 'range',
        options: [0, 511],
        content: content(6)
      },
      {
        showIf: `{${ref(1)}} == 1 || {${ref(4)}} == 1`,
        label: 'Deadzone (0% | 50%)',
        type: 'range',
        options: [0, 511],
        content: content(7)
      }
    ]
  }
}
