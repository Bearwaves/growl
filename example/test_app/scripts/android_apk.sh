#!/bin/bash
# Invoke this from your build directory.

set -xe

make -j 8 
mkdir -p apk/lib/armeabi-v7a
mkdir -p apk/assets
cp libgrowl-test-app.so apk/lib/armeabi-v7a/
cp assets.growl apk/assets/
"${ANDROID_SDK}"/build-tools/30.0.3/aapt package -f -M ../AndroidManifest.xml -I "${ANDROID_SDK}"/platforms/android-33/android.jar -S ../res/android -F apk-unaligned.apk apk
"${ANDROID_SDK}"/build-tools/30.0.3/zipalign -f 4 apk-unaligned.apk apk-unsigned.apk
"${ANDROID_SDK}"/build-tools/30.0.3/apksigner sign --ks ~/.android/debug.keystore --ks-key-alias=androiddebugkey --ks-pass=pass:android --out apk-signed.apk apk-unsigned.apk
"${ANDROID_SDK}"/platform-tools/adb install -r apk-signed.apk
