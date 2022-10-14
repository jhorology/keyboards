module.exports = function (options, defines) {
  const customKeycodes = [
    {
      name: 'R.HID\nTOGG',
      title: 'Toggle allow or deny access to RAW HID',
      shortName: 'R.HID\nTOGG'
    },
    {
      name: 'R.HID\nON',
      title: 'Allow access to RAW HID',
      shortName: 'R.HID\nON'
    },
    {
      name: 'R.HID\nOFF',
      title: 'Deny access to RAW HID',
      shortName: 'R.HID\nOFF'
    },
    {
      name: 'Mac\nTOGG',
      title: 'Toggle true apple mode with switching base layer 0(mac) or 1',
      shortName: 'Mac\nTOGG'
    },
    {
      name: 'Mac\nON',
      title: 'Enable true apple mode with switching base layer 0',
      shortName: 'Mac\nON'
    },
    {
      name: 'Mac\nOFF',
      title: 'Enable true apple mode with switching base layer 1',
      shortName: 'Mac\nOFF'
    },
    {
      name: 'USJ\nTOGG',
      title: 'Toggle enabling key overridng for ANSI layout on JIS environment',
      shortName: 'USJ\nTOGG'
    },
    {
      name: 'USJ\nON',
      title: 'Enable key overriding for ANSI layout on JIS environment',
      shortName: 'USJ\nON'
    },
    {
      name: 'USJ\nOFF',
      title: 'Disable key overriding for ANSI layout on JIS environment',
      shortName: 'USJ\nOFF'
    },
    {
      name: 'Apple\nfn',
      title: 'Apple Fn/Globe Key',
      shortName: 'Apple\nfn'
    },
    {
      name: 'Apple\nfn+FK',
      title:
        "Apple Fn/Globe key for the keyboard that dosen't have F1-12 keys. F1-12 keys can be mapped on top row. When mac mode is off, It simulates mac fn functions.",
      shortName: 'Apple\nfn+FK'
    },
    {
      name: 'EISU\nKANA',
      title: 'Toggle send かな and 英数',
      shortName: 'EISU\nKANA'
    }
  ]
  if (options.RADIAL_CONTROLLER_ENABLE === 'yes') {
    Array.prototype.push.apply(customKeycodes, [
      {
        name: 'RC\nBTN',
        title: 'The button located on radial controller',
        shortName: 'RC\nBTN'
      },
      {
        name: 'RC\nCCW',
        title: 'Counter clockwise rotation of the radial controller',
        shortName: 'RC\nLeft'
      },
      {
        name: 'RC\nCW',
        title: 'Clockwise rotation of the radial controller',
        shortName: 'RC\nRight'
      }
    ])
  }
  return customKeycodes
}
