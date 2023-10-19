/* AppleVendor Page Top Case (0x00ff) */
const appleVendorTopCasePageUsages = {
  kHIDUsage_AV_TopCase_KeyboardFn: 0x0003,
  kHIDUsage_AV_TopCase_BrightnessUp: 0x0004,
  kHIDUsage_AV_TopCase_BrightnessDown: 0x0005,
  kHIDUsage_AV_TopCase_VideoMirror: 0x0006,
  kHIDUsage_AV_TopCase_IlluminationToggle: 0x0007,
  kHIDUsage_AV_TopCase_IlluminationUp: 0x0008,
  kHIDUsage_AV_TopCase_IlluminationDown: 0x0009,
  kHIDUsage_AV_TopCase_ClamshellLatched: 0x000a,
  kHIDUsage_AV_TopCase_Reserved_MouseData: 0x00c0
}

/* AppleVendor Keyboard Page (0xff01) */
const appleVendorKeyboardPageUsages = {
  kHIDUsage_AppleVendorKeyboard_Spotlight: 0x0001,
  kHIDUsage_AppleVendorKeyboard_Dashboard: 0x0002,
  kHIDUsage_AppleVendorKeyboard_Function: 0x0003,
  kHIDUsage_AppleVendorKeyboard_Launchpad: 0x0004,
  kHIDUsage_AppleVendorKeyboard_Reserved: 0x000a,
  kHIDUsage_AppleVendorKeyboard_CapsLockDelayEnable: 0x000b,
  kHIDUsage_AppleVendorKeyboard_PowerState: 0x000c,
  kHIDUsage_AppleVendorKeyboard_Expose_All: 0x0010,
  kHIDUsage_AppleVendorKeyboard_Expose_Desktop: 0x0011,
  kHIDUsage_AppleVendorKeyboard_Brightness_Up: 0x0020,
  kHIDUsage_AppleVendorKeyboard_Brightness_Down: 0x0021,
  kHIDUsage_AppleVendorKeyboard_Language: 0x0030
}

// usages defined in report descriptor
const appleExtraKeyUsages = [
  // AVT
  'kHIDUsage_AV_TopCase_KeyboardFn', // 0
  'kHIDUsage_AV_TopCase_IlluminationUp', // 1
  'kHIDUsage_AV_TopCase_IlluminationDown', // 2
  // AVK
  'kHIDUsage_AppleVendorKeyboard_Spotlight', // 3
  'kHIDUsage_AppleVendorKeyboard_Dashboard', // 4
  'kHIDUsage_AppleVendorKeyboard_Function', // 5
  'kHIDUsage_AppleVendorKeyboard_Launchpad', // 6
  'kHIDUsage_AppleVendorKeyboard_Reserved', // 7
  'kHIDUsage_AppleVendorKeyboard_CapsLockDelayEnable', // 8
  'kHIDUsage_AppleVendorKeyboard_PowerState', // 9
  'kHIDUsage_AppleVendorKeyboard_Expose_All', // 10
  'kHIDUsage_AppleVendorKeyboard_Expose_Desktop', // 11
  'kHIDUsage_AppleVendorKeyboard_Brightness_Up', // 12
  'kHIDUsage_AppleVendorKeyboard_Brightness_Down', // 13
  'kHIDUsage_AppleVendorKeyboard_Language' //14
]

module.exports = function (options, defines) {
  const ifdef = (key, array) => (options[key] === 'yes' ? array : []),
    ifndef = (key, array) => (options[key] !== 'yes' ? array : [])
  const appleExtrakey = (usage) => ({
    name: usage.startsWith('kHIDUsage_AV_TopCase')
      ? `AVT(${appleVendorTopCasePageUsages[usage]})`
      : `AVK(${appleVendorKeyboardPageUsages[usage]})`,
    title: `*experimental ${usage}`,
    shortName: usage.startsWith('kHIDUsage_AV_TopCase')
      ? `AVT(${appleVendorTopCasePageUsages[usage]})`
      : `AVK(${appleVendorKeyboardPageUsages[usage]})`
  })
  return [
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
    ...ifndef('DIP_SWITCH_ENABLE', [
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
    // Apple Extra keys
    ...ifdef('APPLE_EXTRA_KEY_ENABLE', appleExtraKeyUsages.slice(1).map(appleExtrakey)),
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
    ...ifdef('RADIAL_CONTROLLER_ENABLE', [
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
    ]),
    ...ifdef('OS_FINGERPRINT_DEBUG_ENABLE', [
      {
        name: 'TEST\nOS',
        title: 'SEND_STRING detected os vairant',
        shortName: 'OS'
      }
    ])
  ]
}
