// TODO sync with source code
const CHANNEL_ID = 16

module.exports = [
  {
    label: 'EC Tools',
    content: [
      {
        label: 'Actuation',
        content: [
          {
            label: 'Mode',
            type: 'dropdown',
            options: [
              ['APC', 0],
              ['Rapid Trigger', 1]
            ],
            content: ['id_ec_actuation_mode', CHANNEL_ID, 1]
          },
          {
            showIf: '{id_ec_actuation_mode} == 0',
            content: [
              {
                label: 'Actuation Level (0% | 100%)',
                type: 'range',
                options: [1, 1023],
                content: ['id_ec_mode_0_actuation_threshold', CHANNEL_ID, 2]
              },
              {
                label: 'Release Level (0% | 100%, ALWAYS < Actuation Level)',
                type: 'range',
                options: [1, 1023],
                content: ['id_ec_mode_0_release_threshold', CHANNEL_ID, 3]
              }
            ]
          },
          {
            showIf: '{id_ec_actuation_mode} == 1',
            content: [
              {
                label: 'Initial Deadzone Offset (0% | 100%)',
                type: 'range',
                options: [1, 1023],
                content: ['id_ec_mode_1_initial_deadzone_offset', CHANNEL_ID, 4]
              },
              {
                label: 'Actuation Sensitivity (1-255)',
                type: 'range',
                options: [1, 255],
                content: ['id_ec_mode_1_actuation_sensitivity', CHANNEL_ID, 5]
              },
              {
                label: 'Release Sensitivity (1-255)',
                type: 'range',
                options: [1, 255],
                content: ['id_ec_mode_1_release_sensitivity', CHANNEL_ID, 6]
              }
            ]
          }
        ]
      },
      {
        label: 'Calibration',
        content: [
          {
            label: 'Bottoming Calibration',
            type: 'toggle',
            content: ['id_ec_bottoming_calibration', CHANNEL_ID, 7]
          },
          {
            label:
              'Show Calibration Data *(In 3 sconds after click,It will send data as keystrokes. Quickly focus on a text field on editor or something.)',
            type: 'toggle',
            content: ['id_ec_show_calibration_data', CHANNEL_ID, 8]
          }
        ]
      }
    ]
  }
]
