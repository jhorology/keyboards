#!/usr/bin/env node

const path = require('path'),
  fs = require('fs/promises'),
  _ = require('underscore'),
  {
    keyboardDefinitionV2ToVIADefinitionV2,
    isVIADefinitionV2,
    isKeyboardDefinitionV2,
    keyboardDefinitionV3ToVIADefinitionV3,
    isVIADefinitionV3,
    isKeyboardDefinitionV3
  } = require('via-reader'),
  QMK_HOME = process.env['QMK_HOME'],
  VIA_VERSION = parseInt(process.env['VIA_VERSION']),
  MAKE_TARGETS = process.argv.slice(2),
  PROJECT_DIR = path.join(__dirname, '..'),
  OUTPUT_DIR = process.env['OUTPUT_DIR'] || path.join(PROJECT_DIR, 'dist'),
  KEYBOARDS_DIR = path.join(PROJECT_DIR, 'qmk_keyboards'),
  LIB_DIR = path.join(KEYBOARDS_DIR, 'lib')

async function build(target) {
  const targetDir = path.join(KEYBOARDS_DIR, target),
    options = await getMakeOptions(targetDir),
    defines = await getDefineValues(targetDir),
    info = await getInfo(targetDir, options, defines),
    via = {
      name: info.keyboard_name,
      vendorId: info.usb.vid,
      productId: info.usb.pid
    }

  if (VIA_VERSION === 2) {
    // lighting
    via.lighting =
      options.BACKLIGHT_ENABLE === 'yes' && options.RGBLIGHT_ENABLE === 'yes'
        ? 'qmk_backlight_rgblight'
        : options.BACKLIGHT_ENABLE === 'yes'
        ? 'qmk_backlight'
        : options.RGBLIGHT_ENABLE === 'yes'
        ? 'qmk_rgblight'
        : options.RGB_MATRIX_ENABLE === 'yes'
        ? { extends: 'none', keycodes: 'qmk' }
        : 'none'
  } else if (VIA_VERSION === 3) {
    // https://www.caniusevia.com/docs/v3_changes
    via.firmwareVersion = parseInt(defines.VIA_FIRMWARE_VERSION)
    const lighting =
        options.BACKLIGHT_ENABLE === 'yes' && options.RGBLIGHT_ENABLE === 'yes'
          ? 'qmk_backlight_rgblight'
          : options.BACKLIGHT_ENABLE === 'yes'
          ? 'qmk_backlight'
          : options.RGBLIGHT_ENABLE === 'yes'
          ? 'qmk_rgblight'
          : options.RGB_MATRIX_ENABLE === 'yes'
          ? await getRgbMatrixEffects(targetDir)
          : undefined,
      customMenus = await getCustomMenus(targetDir, options, defines)

    if (lighting) {
      via.keycodes = ['qmk_lighting']
    }
    if (lighting || customMenus.length) {
      via.menus = [...(lighting ? [lighting] : []), ...customMenus]
    }
  }
  // matrix
  via.matrix = info.matrix_pins
    ? {
        rows: info.matrix_pins.rows.length,
        cols: info.matrix_pins.cols.length
      }
    : { rows: defines.MATRIX_ROWS, cols: defines.MATRIX_COLS }

  // layouts
  via.layouts = await getLayouts(targetDir, options, defines)

  // customKeycodes
  via.customKeycodes = await getCustomKeycodes(targetDir, options, defines)

  // validate
  validate(target, via)

  // output JSON
  await fs.writeFile(
    path.join(OUTPUT_DIR, `${target}_via_v${VIA_VERSION}.json`),
    JSON.stringify(via, null, 2)
  )
  // alternate device ID
  if (defines.ALTERNATE_VENDOR_ID && defines.ALTERNATE_PRODUCT_ID) {
    via.vendorId =
      '0x' + defines.ALTERNATE_VENDOR_ID.toString(16).padStart(4, '0')
    via.productId =
      '0x' + defines.ALTERNATE_PRODUCT_ID.toString(16).padStart(4, '0')
    await fs.writeFile(
      path.join(OUTPUT_DIR, `${target}_via_v${VIA_VERSION}_alt.json`),
      JSON.stringify(via, null, 2)
    )
  }
}

async function getInfo(targetDir, options, defines) {
  return await readJson(
    path.join(targetDir, 'info.json'),
    options,
    defines,
    undefined
  )
}

async function getLayouts(targetDir, options, defines) {
  return await readJson(
    path.join(targetDir, 'layouts.json'),
    options,
    defines,
    undefined
  )
}

async function getDefineValues(targetDir) {
  const files = [
      path.join(KEYBOARDS_DIR, 'config.h'),
      path.join(targetDir, 'config.h')
    ],
    secureConfig = path.join(targetDir, 'secure_config.h')
  if (readable(secureConfig)) {
    files.push(secureConfig)
  }
  return await find_all_with_reduce(
    files,
    /^\s*#\s*define\s+(\w+)\s+(0x[0-9a-fA-F]+|[0-9]+)\b/gm,
    (obj, match) => {
      obj[match[1]] = parseInt(match[2])
      return obj
    },
    {}
  )
}

async function getMakeOptions(targetDir) {
  return await find_all_with_reduce(
    [path.join(KEYBOARDS_DIR, 'rules.mk'), path.join(targetDir, 'rules.mk')],
    /^\s*(\w+)\s*=\s*(yes|no)/gm
  )
}

async function getCustomKeycodes(targetDir, options, defines) {
  return [
    ...(await readJsOrJson(
      path.join(LIB_DIR, 'custom_keycodes'),
      options,
      defines,
      []
    )),
    ...(await readJsOrJson(
      path.join(targetDir, 'custom_keycodes'),
      options,
      defines,
      []
    ))
  ]
}

async function getCustomMenus(targetDir, options, defines) {
  return [
    ...(await readJsOrJson(
      path.join(LIB_DIR, 'custom_menus'),
      options,
      defines,
      []
    )),
    ...(await readJsOrJson(
      path.join(targetDir, 'custom_menus'),
      options,
      defines,
      []
    ))
  ]
}

async function getRgbMatrixEffects(targetDir) {
  const enabledEffects = await find_all(
      path.join(targetDir, 'config.h'),
      /^\s*#\s*define\s+ENABLE_RGB_MATRIX_(\w+)\b/gm
    ),
    effectFiles = await find_all(
      path.join(
        QMK_HOME,
        'quantum/rgb_matrix/animations/rgb_matrix_effects.inc'
      ),
      /^#include\s*"(\w+\.h)"/gm,
      (match) => path.join(QMK_HOME, 'quantum/rgb_matrix/animations', match[1])
    ),
    effectOptions = [
      'ALL_OFF',
      ...(await find_all(effectFiles, /^\s*RGB_MATRIX_EFFECT\(\s*(\w+)\s*\)/gm))
    ]
      .filter(
        (e) =>
          e === 'ALL_OFF' || e === 'SOLID_COLOR' || enabledEffects.includes(e)
      )
      .map((e, index) => [
        e
          .split('_')
          .map((w) => w[0] + w.substr(1).toLowerCase())
          .join(' '),
        index
      ])
  return {
    label: 'Lighting',
    content: [
      {
        label: 'Backlight',
        content: [
          {
            label: 'Brightness',
            type: 'range',
            options: [0, 255],
            content: ['id_qmk_rgb_matrix_brightness', 3, 1]
          },
          {
            label: 'Effect',
            type: 'dropdown',
            content: ['id_qmk_rgb_matrix_effect', 3, 2],
            options: effectOptions
          },
          {
            showIf: '{id_qmk_rgb_matrix_effect} != 0',
            label: 'Effect Speed',
            type: 'range',
            options: [0, 255],
            content: ['id_qmk_rgb_matrix_effect_speed', 3, 3]
          },
          {
            showIf: '{id_qmk_rgb_matrix_effect} != 0',
            label: 'Color',
            type: 'color',
            content: ['id_qmk_rgb_matrix_color', 3, 4]
          }
        ]
      }
    ]
  }
}

async function readJsOrJson(filePath, options, defines, emptyValue) {
  if (await readable(filePath + '.js')) {
    const dataOrFunction = require(filePath + '.js')
    let result
    if (typeof dataOrFunction === 'function') {
      if (dataOrFunction.constructor.name === 'AsyncFunction') {
        result = await dataOrFunction(options, defines)
      } else {
        result = dataOrFunction(options, defines)
      }
    } else {
      result = dataOrFunction
    }
    return result ? result : emptyValue
  }
  return await readJson(filePath + '.json', options, defines, emptyValue)
}

async function readJson(filePath, options, defines, emptyValue) {
  // if tempalte file exits
  if (await readable(filePath + '.tpl')) {
    return JSON.parse(
      _.template(await fs.readFile(filePath + '.tpl', 'utf-8'))({
        options,
        defines
      })
    )
  }
  if (await readable(filePath)) {
    return JSON.parse(await fs.readFile(filePath, 'utf-8'))
  }
  return emptyValue
}

async function readable(filePath) {
  try {
    await fs.access(filePath, fs.constants.R_OK)
    return true
  } catch {
    return false
  }
}

function validate(target, via) {
  const validator = [
    undefined,
    undefined,
    isKeyboardDefinitionV2,
    isKeyboardDefinitionV3
  ][VIA_VERSION]
  if (!validator) {
    throw new Error(
      `via_json_generator: keyboard: ${target} - unsuppoted VIA version: ${VIA_VERSION}`
    )
  }
  if (!validator(via)) {
    throw new Error(
      `via_json_generator: keyboard: ${target} - VIA v${VIA_VERSION} validation fails with errors:\n ${JSON.stringify(
        validator.errors,
        null,
        2
      )}`
    )
  }
}

async function find_all(fileOrFiles, regexp, mapper = (match) => match[1]) {
  if (Array.isArray(fileOrFiles)) {
    const result = []
    for (let i = 0; i < fileOrFiles.length; i++) {
      Array.prototype.push.apply(
        result,
        await find_all(fileOrFiles[i], regexp, mapper)
      )
    }
    return result
  }
  return [...(await fs.readFile(fileOrFiles, 'utf-8')).matchAll(regexp)].map(
    mapper
  )
}

async function find_all_with_reduce(
  fileOrFiles,
  regexp,
  reducer = (obj, match) => {
    obj[match[1]] = match[2]
    return obj
  },
  initialValue = {}
) {
  if (Array.isArray(fileOrFiles)) {
    let result = initialValue
    for (let i = 0; i < fileOrFiles.length; i++) {
      result = await find_all_with_reduce(
        fileOrFiles[i],
        regexp,
        reducer,
        result
      )
    }
    return result
  }
  return [...(await fs.readFile(fileOrFiles, 'utf-8')).matchAll(regexp)].reduce(
    reducer,
    initialValue
  )
}

Promise.all(
  MAKE_TARGETS.map((target) =>
    build(target.replace(/^my_keyboards\//, '').replaceAll('/', '_'))
  )
).catch((err) => {
  console.error(err)
  process.exit(1)
})
