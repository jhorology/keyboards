#!/bin/bash -eu

cd "$(dirname $0)"

find . -name *.h -not -path "*/\.*" -exec clang-format -i {} \;
find . -name *.c -not -path "*/\.*" -exec clang-format -i {} \;
prettier --write ./**/*.json
