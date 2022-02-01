#!/bin/bash

git ls-files | grep -v '^src/contrib/' | grep -e '\.h$' -e '\.cpp$' -e '\.mm$' | xargs clang-format -i
