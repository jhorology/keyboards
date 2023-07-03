export const data = [
  {
    os: 'macOS Ventura 13.4.1',
    arch: 'x86_64',
    keyboard: 'kbdfans tofu60',
    packets: [
      {
        bmRequestType: {
          recipient: '0:DEVICE',
          type: '0:STANDARD',
          direction: '0:TO_DEVICE'
        },
        bRequest: '5:SET_ADDRESS',
        wValue: 0x003d,
        wIndex: 0x0000,
        wLength: 0x0000
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
        wLength: 0x001e
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
          recipient: '1:INTERFACE',
          type: '0:STANDARD',
          direction: '1:TO_HOST'
        },
        bRequest: '6:GET_DESCRIPTOR',
        descType: '34:CLASS_CONFIGURATION',
        descIndex: 0,
        wIndex: 0x0002,
        wLength: 0x0067
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
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '34:UNKNOWN',
        wValue: 0x0003,
        wIndex: 0x0000,
        wLength: 0x0000
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      }
    ]
  },
  {
    os: 'macOS Ventura 13.4.1',
    arch: 'x86_64',
    keyboard: 'Keychron Q60',
    packets: [
      {
        bmRequestType: {
          recipient: '0:DEVICE',
          type: '0:STANDARD',
          direction: '0:TO_DEVICE'
        },
        bRequest: '5:SET_ADDRESS',
        wValue: 0x002b,
        wIndex: 0x0000,
        wLength: 0x0000
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
        wLength: 0x001a
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
        wLength: 0x0067
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
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '34:UNKNOWN',
        wValue: 0x0003,
        wIndex: 0x0000,
        wLength: 0x0000
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      }
    ]
  },
  {
    os: 'macOS Ventura 13.4.1',
    arch: 'x86_64',
    keyboard: 'BT60',
    packets: [
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
        wLength: 0x000a
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
        wLength: 0x0067
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
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '34:UNKNOWN',
        wValue: 0x0003,
        wIndex: 0x0000,
        wLength: 0x0000
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      }
    ]
  },
  {
    os: 'macOS Ventura 13.4.1',
    arch: 'arm64',
    keyboard: 'kbdfans tofu60',
    packets: [
      {
        bmRequestType: {
          recipient: '0:DEVICE',
          type: '0:STANDARD',
          direction: '0:TO_DEVICE'
        },
        bRequest: '5:SET_ADDRESS',
        wValue: 0x0014,
        wIndex: 0x0000,
        wLength: 0x0000
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
        wLength: 0x001e
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
          recipient: '1:INTERFACE',
          type: '0:STANDARD',
          direction: '1:TO_HOST'
        },
        bRequest: '6:GET_DESCRIPTOR',
        descType: '34:CLASS_CONFIGURATION',
        descIndex: 0,
        wIndex: 0x0002,
        wLength: 0x0067
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
        wLength: 0x00ff
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '34:UNKNOWN',
        wValue: 0x0003,
        wIndex: 0x0000,
        wLength: 0x0000
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '0:DEVICE',
          type: '0:STANDARD',
          direction: '0:TO_DEVICE'
        },
        bRequest: '5:SET_ADDRESS',
        wValue: 0x0014,
        wIndex: 0x0000,
        wLength: 0x0000
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
        wLength: 0x001e
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
          recipient: '1:INTERFACE',
          type: '0:STANDARD',
          direction: '1:TO_HOST'
        },
        bRequest: '6:GET_DESCRIPTOR',
        descType: '34:CLASS_CONFIGURATION',
        descIndex: 0,
        wIndex: 0x0002,
        wLength: 0x0067
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
        wLength: 0x00ff
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '34:UNKNOWN',
        wValue: 0x0003,
        wIndex: 0x0000,
        wLength: 0x0000
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipient: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      }
    ]
  },
  {
    os: 'Windows 11 Pro',
    uname: '',
    keyboard: 'kbdfans tofu60',
    packets: [
      {
        bmRequestType: {
          recipent: '0:DEVICE',
          type: '0:STANDARD',
          direction: '1:TO_HOST'
        },
        bRequest: '6:GET_DESCRIPTOR',
        descType: '1:DEVICE',
        descIndex: 0,
        wIndex: 0x0000,
        wLength: 0x0040
      },
      {
        bmRequestType: {
          recipent: '0:DEVICE',
          type: '0:STANDARD',
          direction: '0:TO_DEVICE'
        },
        bRequest: '5:SET_ADDRESS',
        wValue: 0x0002,
        wIndex: 0x0000,
        wLength: 0x0000
      },
      {
        bmRequestType: {
          recipent: '0:DEVICE',
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
          recipent: '0:DEVICE',
          type: '0:STANDARD',
          direction: '1:TO_HOST'
        },
        bRequest: '6:GET_DESCRIPTOR',
        descType: '2:CONFIGURATION',
        descIndex: 0,
        wIndex: 0x0000,
        wLength: 0x00ff
      },
      {
        bmRequestType: {
          recipent: '0:DEVICE',
          type: '0:STANDARD',
          direction: '1:TO_HOST'
        },
        bRequest: '6:GET_DESCRIPTOR',
        descType: '3:STRING',
        descIndex: 3,
        wIndex: 0x0409,
        wLength: 0x00ff
      },
      {
        bmRequestType: {
          recipent: '0:DEVICE',
          type: '0:STANDARD',
          direction: '1:TO_HOST'
        },
        bRequest: '6:GET_DESCRIPTOR',
        descType: '3:STRING',
        descIndex: 0,
        wIndex: 0x0000,
        wLength: 0x00ff
      },
      {
        bmRequestType: {
          recipent: '0:DEVICE',
          type: '0:STANDARD',
          direction: '1:TO_HOST'
        },
        bRequest: '6:GET_DESCRIPTOR',
        descType: '3:STRING',
        descIndex: 2,
        wIndex: 0x0409,
        wLength: 0x00ff
      },
      {
        bmRequestType: {
          recipent: '0:DEVICE',
          type: '0:STANDARD',
          direction: '1:TO_HOST'
        },
        bRequest: '6:GET_DESCRIPTOR',
        descType: '6:DEVICE_QUALIFIER',
        descIndex: 0,
        wIndex: 0x0000,
        wLength: 0x000a
      },
      {
        bmRequestType: {
          recipent: '0:DEVICE',
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
          recipent: '0:DEVICE',
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
          recipent: '0:DEVICE',
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
          recipent: '0:DEVICE',
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
          recipent: '0:DEVICE',
          type: '0:STANDARD',
          direction: '1:TO_HOST'
        },
        bRequest: '6:GET_DESCRIPTOR',
        descType: '3:STRING',
        descIndex: 2,
        wIndex: 0x0409,
        wLength: 0x0004
      },
      {
        bmRequestType: {
          recipent: '0:DEVICE',
          type: '0:STANDARD',
          direction: '1:TO_HOST'
        },
        bRequest: '6:GET_DESCRIPTOR',
        descType: '3:STRING',
        descIndex: 2,
        wIndex: 0x0409,
        wLength: 0x001e
      },
      {
        bmRequestType: {
          recipent: '0:DEVICE',
          type: '0:STANDARD',
          direction: '1:TO_HOST'
        },
        bRequest: '6:GET_DESCRIPTOR',
        descType: '3:STRING',
        descIndex: 0,
        wIndex: 0x0000,
        wLength: 0x00ff
      },
      {
        bmRequestType: {
          recipent: '0:DEVICE',
          type: '0:STANDARD',
          direction: '1:TO_HOST'
        },
        bRequest: '6:GET_DESCRIPTOR',
        descType: '3:STRING',
        descIndex: 0,
        wIndex: 0x0000,
        wLength: 0x00ff
      },
      {
        bmRequestType: {
          recipent: '0:DEVICE',
          type: '0:STANDARD',
          direction: '1:TO_HOST'
        },
        bRequest: '6:GET_DESCRIPTOR',
        descType: '3:STRING',
        descIndex: 2,
        wIndex: 0x0409,
        wLength: 0x0004
      },
      {
        bmRequestType: {
          recipent: '0:DEVICE',
          type: '0:STANDARD',
          direction: '1:TO_HOST'
        },
        bRequest: '6:GET_DESCRIPTOR',
        descType: '3:STRING',
        descIndex: 1,
        wIndex: 0x0409,
        wLength: 0x00ff
      },
      {
        bmRequestType: {
          recipent: '0:DEVICE',
          type: '0:STANDARD',
          direction: '1:TO_HOST'
        },
        bRequest: '6:GET_DESCRIPTOR',
        descType: '3:STRING',
        descIndex: 2,
        wIndex: 0x0409,
        wLength: 0x001e
      },
      {
        bmRequestType: {
          recipent: '0:DEVICE',
          type: '0:STANDARD',
          direction: '1:TO_HOST'
        },
        bRequest: '6:GET_DESCRIPTOR',
        descType: '3:STRING',
        descIndex: 0,
        wIndex: 0x0000,
        wLength: 0x00ff
      },
      {
        bmRequestType: {
          recipent: '0:DEVICE',
          type: '0:STANDARD',
          direction: '1:TO_HOST'
        },
        bRequest: '6:GET_DESCRIPTOR',
        descType: '3:STRING',
        descIndex: 1,
        wIndex: 0x0409,
        wLength: 0x00ff
      },
      {
        bmRequestType: {
          recipent: '1:INTERFACE',
          type: '1:CLASS',
          direction: '1:TO_HOST'
        },
        bRequest: '33:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipent: '0:DEVICE',
          type: '0:STANDARD',
          direction: '1:TO_HOST'
        },
        bRequest: '6:GET_DESCRIPTOR',
        descType: '3:STRING',
        descIndex: 2,
        wIndex: 0x0409,
        wLength: 0x00ff
      },
      {
        bmRequestType: {
          recipent: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '34:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0000
      },
      {
        bmRequestType: {
          recipent: '0:DEVICE',
          type: '0:STANDARD',
          direction: '1:TO_HOST'
        },
        bRequest: '6:GET_DESCRIPTOR',
        descType: '3:STRING',
        descIndex: 1,
        wIndex: 0x0409,
        wLength: 0x00ff
      },
      {
        bmRequestType: {
          recipent: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipent: '0:DEVICE',
          type: '0:STANDARD',
          direction: '1:TO_HOST'
        },
        bRequest: '6:GET_DESCRIPTOR',
        descType: '3:STRING',
        descIndex: 2,
        wIndex: 0x0409,
        wLength: 0x00ff
      },
      {
        bmRequestType: {
          recipent: '1:INTERFACE',
          type: '1:CLASS',
          direction: '1:TO_HOST'
        },
        bRequest: '33:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipent: '0:DEVICE',
          type: '0:STANDARD',
          direction: '1:TO_HOST'
        },
        bRequest: '6:GET_DESCRIPTOR',
        descType: '3:STRING',
        descIndex: 2,
        wIndex: 0x0409,
        wLength: 0x00ff
      },
      {
        bmRequestType: {
          recipent: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '10:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0002,
        wLength: 0x0000
      },
      {
        bmRequestType: {
          recipent: '1:INTERFACE',
          type: '0:STANDARD',
          direction: '1:TO_HOST'
        },
        bRequest: '6:GET_DESCRIPTOR',
        descType: '34:CLASS_CONFIGURATION',
        descIndex: 0,
        wIndex: 0x0002,
        wLength: 0x00a7
      },
      {
        bmRequestType: {
          recipent: '0:DEVICE',
          type: '0:STANDARD',
          direction: '1:TO_HOST'
        },
        bRequest: '6:GET_DESCRIPTOR',
        descType: '3:STRING',
        descIndex: 3,
        wIndex: 0x0409,
        wLength: 0x0402
      },
      {
        bmRequestType: {
          recipent: '0:DEVICE',
          type: '0:STANDARD',
          direction: '1:TO_HOST'
        },
        bRequest: '6:GET_DESCRIPTOR',
        descType: '3:STRING',
        descIndex: 1,
        wIndex: 0x0409,
        wLength: 0x0402
      },
      {
        bmRequestType: {
          recipent: '0:DEVICE',
          type: '0:STANDARD',
          direction: '1:TO_HOST'
        },
        bRequest: '6:GET_DESCRIPTOR',
        descType: '3:STRING',
        descIndex: 2,
        wIndex: 0x0409,
        wLength: 0x0402
      },
      {
        bmRequestType: {
          recipent: '0:DEVICE',
          type: '0:STANDARD',
          direction: '1:TO_HOST'
        },
        bRequest: '6:GET_DESCRIPTOR',
        descType: '3:STRING',
        descIndex: 3,
        wIndex: 0x0409,
        wLength: 0x0402
      },
      {
        bmRequestType: {
          recipent: '0:DEVICE',
          type: '0:STANDARD',
          direction: '1:TO_HOST'
        },
        bRequest: '6:GET_DESCRIPTOR',
        descType: '3:STRING',
        descIndex: 1,
        wIndex: 0x0409,
        wLength: 0x0402
      },
      {
        bmRequestType: {
          recipent: '0:DEVICE',
          type: '0:STANDARD',
          direction: '1:TO_HOST'
        },
        bRequest: '6:GET_DESCRIPTOR',
        descType: '3:STRING',
        descIndex: 2,
        wIndex: 0x0409,
        wLength: 0x0402
      },
      {
        bmRequestType: {
          recipent: '1:INTERFACE',
          type: '1:CLASS',
          direction: '1:TO_HOST'
        },
        bRequest: '33:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipent: '1:INTERFACE',
          type: '1:CLASS',
          direction: '1:TO_HOST'
        },
        bRequest: '33:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipent: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipent: '1:INTERFACE',
          type: '1:CLASS',
          direction: '1:TO_HOST'
        },
        bRequest: '33:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipent: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '34:UNKNOWN',
        wValue: 0x0003,
        wIndex: 0x0000,
        wLength: 0x0000
      },
      {
        bmRequestType: {
          recipent: '1:INTERFACE',
          type: '1:CLASS',
          direction: '0:TO_DEVICE'
        },
        bRequest: '32:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      },
      {
        bmRequestType: {
          recipent: '1:INTERFACE',
          type: '1:CLASS',
          direction: '1:TO_HOST'
        },
        bRequest: '33:UNKNOWN',
        wValue: 0x0000,
        wIndex: 0x0000,
        wLength: 0x0007
      }
    ]
  }
]
