#!/bin/bash

git ls-files | grep -e '\.h$' -e '\.cpp$' | xargs clang-format -i
