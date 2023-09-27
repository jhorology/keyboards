const macos_sonoma_intel = [
  {
    bmRequestType: {
      recipient: '0:DEVICE',
      type: '0:STANDARD',
      direction: '1:TO_HOST'
    },
    bRequest: '6:GET_DESCRIPTOR',
    descType: '1:DEVICE',
    descIndex: 0,
    wIndex: 0x0000,
    wLength: 0x0008
  },
  {
    bmRequestType: {
      recipient: '0:DEVICE',
      type: '0:STANDARD',
      direction: '1:TO_HOST'
    },
    bRequest: '6:GET_DESCRIPTOR',
    descType: '1:DEVICE',
    descIndex: 0,
    wIndex: 0x0000,
    wLength: 0x0012
  },
  {
    bmRequestType: {
      recipient: '0:DEVICE',
      type: '0:STANDARD',
      direction: '1:TO_HOST'
    },
    bRequest: '6:GET_DESCRIPTOR',
    descType: '3:STRING',
    descIndex: 2,
    wIndex: 0x0409,
    wLength: 0x0002
  },
  {
    bmRequestType: {
      recipient: '0:DEVICE',
      type: '0:STANDARD',
      direction: '1:TO_HOST'
    },
    bRequest: '6:GET_DESCRIPTOR',
    descType: '3:STRING',
    descIndex: 2,
    wIndex: 0x0409,
    wLength: 0x001c
  },
  {
    bmRequestType: {
      recipient: '0:DEVICE',
      type: '0:STANDARD',
      direction: '1:TO_HOST'
    },
    bRequest: '6:GET_DESCRIPTOR',
    descType: '3:STRING',
    descIndex: 1,
    wIndex: 0x0409,
    wLength: 0x0002
  },
  {
    bmRequestType: {
      recipient: '0:DEVICE',
      type: '0:STANDARD',
      direction: '1:TO_HOST'
    },
    bRequest: '6:GET_DESCRIPTOR',
    descType: '3:STRING',
    descIndex: 1,
    wIndex: 0x0409,
    wLength: 0x0018
  },
  {
    bmRequestType: {
      recipient: '0:DEVICE',
      type: '0:STANDARD',
      direction: '1:TO_HOST'
    },
    bRequest: '6:GET_DESCRIPTOR',
    descType: '3:STRING',
    descIndex: 3,
    wIndex: 0x0409,
    wLength: 0x0002
  },
  {
    bmRequestType: {
      recipient: '0:DEVICE',
      type: '0:STANDARD',
      direction: '1:TO_HOST'
    },
    bRequest: '6:GET_DESCRIPTOR',
    descType: '3:STRING',
    descIndex: 3,
    wIndex: 0x0409,
    wLength: 0x0022
  },
  {
    bmRequestType: {
      recipient: '0:DEVICE',
      type: '0:STANDARD',
      direction: '1:TO_HOST'
    },
    bRequest: '6:GET_DESCRIPTOR',
    descType: '15:BOS',
    descIndex: 0,
    wIndex: 0x0000,
    wLength: 0x0005
  },
  {
    bmRequestType: {
      recipient: '0:DEVICE',
      type: '0:STANDARD',
      direction: '1:TO_HOST'
    },
    bRequest: '6:GET_DESCRIPTOR',
    descType: '15:BOS',
    descIndex: 0,
    wIndex: 0x0000,
    wLength: 0x0005
  },
  {
    bmRequestType: {
      recipient: '0:DEVICE',
      type: '0:STANDARD',
      direction: '1:TO_HOST'
    },
    bRequest: '6:GET_DESCRIPTOR',
    descType: '15:BOS',
    descIndex: 0,
    wIndex: 0x0000,
    wLength: 0x0005
  },
  {
    bmRequestType: {
      recipient: '0:DEVICE',
      type: '0:STANDARD',
      direction: '1:TO_HOST'
    },
    bRequest: '6:GET_DESCRIPTOR',
    descType: '2:CONFIGURATION',
    descIndex: 0,
    wIndex: 0x0000,
    wLength: 0x0009
  },
  {
    bmRequestType: {
      recipient: '0:DEVICE',
      type: '0:STANDARD',
      direction: '1:TO_HOST'
    },
    bRequest: '6:GET_DESCRIPTOR',
    descType: '2:CONFIGURATION',
    descIndex: 0,
    wIndex: 0x0000,
    wLength: 0x0064
  },
  {
    bmRequestType: {
      recipient: '0:DEVICE',
      type: '0:STANDARD',
      direction: '0:TO_DEVICE'
    },
    bRequest: '9:SET_CONFIGURATION',
    wValue: 0x0001,
    wIndex: 0x0000,
    wLength: 0x0000
  },
  {
    bmRequestType: {
      recipient: '0:DEVICE',
      type: '0:STANDARD',
      direction: '0:TO_DEVICE'
    },
    bRequest: '3:SET_FEATURE',
    wValue: 0x0001,
    wIndex: 0x0000,
    wLength: 0x0000
  },
  {
    bmRequestType: {
      recipient: '1:INTERFACE',
      type: '0:STANDARD',
      direction: '1:TO_HOST'
    },
    bRequest: '6:GET_DESCRIPTOR',
    descType: '34:CLASS_CONFIGURATION',
    descIndex: 0,
    wIndex: 0x0002,
    wLength: 0x009f
  },
  {
    bmRequestType: {
      recipient: '1:INTERFACE',
      type: '1:CLASS',
      direction: '0:TO_DEVICE'
    },
    bRequest: '9:UNKNOWN',
    wValue: 0x0309,
    wIndex: 0x0002,
    wLength: 0x0003
  },
  {
    bmRequestType: {
      recipient: '0:DEVICE',
      type: '0:STANDARD',
      direction: '1:TO_HOST'
    },
    bRequest: '6:GET_DESCRIPTOR',
    descType: '3:STRING',
    descIndex: 0,
    wIndex: 0x0000,
    wLength: 0x0002
  },
  {
    bmRequestType: {
      recipient: '0:DEVICE',
      type: '0:STANDARD',
      direction: '1:TO_HOST'
    },
    bRequest: '6:GET_DESCRIPTOR',
    descType: '3:STRING',
    descIndex: 0,
    wIndex: 0x0000,
    wLength: 0x0004
  },
  {
    bmRequestType: {
      recipient: '0:DEVICE',
      type: '0:STANDARD',
      direction: '1:TO_HOST'
    },
    bRequest: '6:GET_DESCRIPTOR',
    descType: '3:STRING',
    descIndex: 0,
    wIndex: 0x0000,
    wLength: 0x00ff
  }
]
