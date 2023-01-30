module.exports = function (options, defines) {
  return [
    {
      name: 'RHID🔁\nTOGG',
      title: 'Toggle allow or deny access to RAW HID',
      shortName: 'RH🔁'
    },
    {
      name: 'RHID🗹\nON',
      title: 'Allow access to RAW HID',
      shortName: 'RH🗹'
    },
    {
      name: 'RHID☐\nOFF',
      title: 'Deny access to RAW HID',
      shortName: 'RH☐'
    },
    ...(options.DIP_SWITCH_ENABLE === 'yes'
      ? []
      : [
          {
            name: 'Mac🔁\nTOGG',
            title:
              'Toggle enabling or disabling mac mode with switching base layer 0(mac) or 1',
            shortName: 'Mac🔁'
          },
          {
            name: 'Mac🗹\nON',
            title: 'Enable mac mode with switching base layer 0',
            shortName: 'Mac🗹'
          },
          {
            name: 'Mac☐\nOFF',
            title: 'Disable mac mode with switching base layer 1',
            shortName: 'Mac☐'
          }
        ]),
    {
      name: 'AUT🔁\nTOGG',
      title: 'Toggle enabling or disabling auto detection of mac mode',
      shortName: 'AUT🔁'
    },
    {
      name: 'AUT🗹\nON',
      title: 'Enable auto detection of mac mode',
      shortName: 'AUT🗹'
    },
    {
      name: 'AUT☐\nOFF',
      title: 'Disable auto detection of mac mode',
      shortName: 'AUT☐'
    },
    {
      name: '社畜🔁\nTOGG',
      title:
        'Toggle enabling key overridng for ANSI layout under JIS environment',
      shortName: '社🔁'
    },
    {
      name: '社畜🗹\nON',
      title: 'Enable key overriding for ANSI layout under JIS environment',
      shortName: '社🗹'
    },
    {
      name: '社畜☐\nOFF',
      title: 'Disable key overriding for ANSI layout under JIS environment',
      shortName: '社☐'
    },
    {
      name: 'Apple\n🌐',
      title: 'Apple Fn/Globe Key',
      shortName: 'fn🌐'
    },
    {
      name: 'Apple\n🌐+Fn',
      title:
        "Apple Fn/Globe key for the keyboard that dosen't have F1-12 keys. F1-12 keys can be mapped on top row.",
      shortName: '🌐+Fn'
    },
    {
      name: 'Apple\nVideo\nMirror',
      title: 'for experimental purpose.',
      shortName: 'V.MIRR'
    },
    {
      name: 'Apple\nCramshell\nLatched',
      title: 'for experimental purpose.',
      shortName: 'Crams\nhell'
    },
    {
      name: 'Apple\nSpotlight',
      title: 'Open Spotlight',
      shortName: 'Spot\nLight'
    },
    {
      name: 'Apple\nDashboard',
      title: 'for experimental purpose.',
      shortName: 'Dashboard'
    },
    {
      name: 'Apple\nFunction',
      title: 'for experimental purpose.',
      shortName: 'Function'
    },
    {
      name: 'Apple\nLaunch\nPad',
      title: 'Open LaunchPad',
      shortName: 'Launch\nPad'
    },
    {
      name: 'Apple\nExpose\nAll',
      title: 'Mission Control',
      shortName: 'Expose\nAll'
    },
    {
      name: 'Apple\nExpose\nDesktop',
      title: 'for experimental purpose.',
      shortName: 'Expose\nDesktop'
    },
    {
      name: 'Apple\nLanguage',
      title: 'for experimental purpose.',
      shortName: 'Language'
    },
    {
      name: 'Apple\nDictation',
      title: 'Activate dictation / Siri(Long press)',
      shortName: '🎤'
    },
    {
      name: 'Apple\nDnD',
      title: 'Turn Do Not Disturb on or off',
      shortName: 'DnD'
    },
    {
      name: '英数\nかな',
      title: 'Toggle send かな(KC_LNG1) and 英数(KC_LNG2)',
      shortName: '英か'
    },
    {
      name: '🖥️🔒\nLOCK',
      title: 'Terminal Lock / Screen Saver',
      shortName: '🖥️🔒'
    },
    ...(options.RADIAL_CONTROLLER_ENABLE === 'yes'
      ? [
          {
            name: 'RC↧️\nBTN️',
            title: 'The button located on radial controller',
            shortName: 'RC↧️'
          },
          {
            name: 'RC↺️\nCCW',
            title: 'Counter clockwise rotation of the radial controller',
            shortName: 'RC↺️'
          },
          {
            name: 'RC↻️\nCW',
            title: 'Clockwise rotation of the radial controller',
            shortName: 'RC↻️'
          },
          {
            name: 'RC🫳\nFINE',
            title: 'Dial rotation speed becomes slow',
            shortName: 'RC🫳️'
          }
        ]
      : []),
    ...(options.OS_DETECTION_DEBUG_ENABLE === 'yes'
      ? [
          {
            name: 'TEST\nOS',
            title: 'SEND_STRING detected os vairant',
            shortName: 'OS'
          }
        ]
      : [])
  ]
}
