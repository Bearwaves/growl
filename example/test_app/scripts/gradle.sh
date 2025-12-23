#!/bin/bash

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd -P)"

growl_android_dir="${script_dir}/../../../src/platforms/android"

${growl_android_dir}/gradlew -p "${growl_android_dir}" \
  -PgrowlPropertiesFile="${script_dir}/../android.properties" \
  "$@"
