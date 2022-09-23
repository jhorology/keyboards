#!/usr/bin/env node

const path = require('path'),
  build = require('./lib/json_builder'),
  qmk_home = process.env['QMK_HOME'],
  make_targets = process.argv.slice(2),
  libDir = path.join(qmk_home, 'keyboards', 'my_keyboards', 'lib')

Promise.all(
  make_targets.map((target) =>
    build(
      'vial',
      libDir,
      path.join(qmk_home, 'keyboards', target),
      path.join(
        qmk_home,
        'keyboards',
        target,
        'keymaps',
        'default',
        'vial.json'
      )
    )
  )
).catch((err) => {
  console.error(err)
  process.exit(1)
})
