#!/bin/bash

git ls-files | grep -e '\.h$' -e '\.cpp$' -e '\.mm$' | xargs clang-format -i
