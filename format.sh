#!/bin/bash -eu

cd "$(dirname $0)"

find . -name *.h -exec clang-format -i {} \;
find . -name *.c -exec clang-format -i {} \;
prettier --write ./**/*.json
