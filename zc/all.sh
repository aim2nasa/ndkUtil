#!/bin/sh

ndk-build clean
ndk-build

adb shell "rm -r /data/local/tmp/zc"
adb push obj/local/armeabi/zc /data/local/tmp/
adb shell "chmod 777 /data/local/tmp/zc"
adb shell "/data/local/tmp/zc 3 576 1024 /data/local/tmp/resized.raw /data/local/tmp/resized.zip 1"
adb pull /data/local/tmp/resized.zip .
