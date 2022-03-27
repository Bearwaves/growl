#!/bin/bash

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd -P)"

git ls-files "${script_dir}/.." | grep -v 'thirdparty/' | grep -e '\.h$' -e '\.cpp$' -e '\.mm$' | xargs clang-format -i
