module.exports = function (options, defines) {
  const customKeycodes = [
    {
      code: 'RHID_TOGG',
      name: 'RHID\n🔁TOGG',
      title: 'Toggle allow or deny access to RAW HID',
      shortName: 'RH🔁'
    },
    {
      code: 'RHID_ON',
      name: 'RHID\n✅ON',
      title: 'Allow access to RAW HID',
      shortName: 'RH✅'
    },
    {
      code: 'RHID_OFF',
      name: 'RHID\n🔳OFF',
      title: 'Deny access to RAW HID',
      shortName: 'RH🔳'
    },
    {
      code: 'MAC_TOGG',
      name: 'Mac\n🔁TOGG',
      title: 'Toggle true apple mode with switching base layer 0(mac) or 1',
      shortName: 'Mac🔁'
    },
    {
      code: 'MAC_ON',
      name: 'Mac\n✅ON',
      title: 'Enable true apple mode with switching base layer 0',
      shortName: 'Mac✅'
    },
    {
      code: 'MAC_OFF',
      name: 'Mac\n🔳OFF',
      title: 'Disable true apple mode with switching base layer 1',
      shortName: 'Mac🔳'
    },
    {
      code: 'USJ_TOGG',
      name: '社畜\n🔁TOGG',
      title: 'Toggle enabling key overridng for ANSI layout on JIS environment',
      shortName: '社🔁'
    },
    {
      code: 'USJ_ON',
      name: '社畜\n✅ON',
      title: 'Enable key overriding for ANSI layout on JIS environment',
      shortName: '社✅'
    },
    {
      code: 'USJ_OFF',
      name: '社畜\n🔳OFF',
      title: 'Disable key overriding for ANSI layout on JIS environment',
      shortName: '社🔳'
    },
    {
      code: 'APPLE_FN',
      name: 'Apple\nfn/🌐',
      title: 'Apple Fn/Globe Key',
      shortName: 'fn/🌐'
    },
    {
      code: 'APPLE_FF',
      name: 'Apple\n🌐+Fn',
      title:
        "Apple Fn/Globe key for the keyboard that dosen't have F1-12 keys. F1-12 keys can be mapped on top row. When mac mode is off, It simulates mac fn functions.",
      shortName: '🌐+Fn'
    },
    {
      code: 'EISU_KANA',
      name: '英数\nかな',
      title: 'Toggle send かな(KC_LNG1) and 英数(KC_LNG2)',
      shortName: '英/か'
    }
  ]
  if (options.RADIAL_CONTROLLER_ENABLE === 'yes') {
    Array.prototype.push.apply(customKeycodes, [
      {
        code: 'RC_BTN',
        name: 'RC⬇️\nBTN️',
        title: 'The button located on radial controller',
        shortName: 'RC⬇️'
      },
      {
        code: 'RC_CCW',
        name: 'RC↪️\nCCW',
        title: 'Counter clockwise rotation of the radial controller',
        shortName: 'RC↪️'
      },
      {
        code: 'RC_CW',
        name: 'RC↩️\nCW',
        title: 'Clockwise rotation of the radial controller',
        shortName: 'RC↩️'
      },
      {
        code: 'RC_FINE',
        name: 'RC🫳\nFINE',
        title: 'Dial rotation speed becomes slow',
        shortName: 'RC🫳️'
      }
    ])
  }
  return customKeycodes
}
