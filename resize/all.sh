#!/bin/sh

ndk-build clean
ndk-build

adb shell "rm -r /data/local/tmp/resize"
adb push obj/local/armeabi/resize /data/local/tmp/
adb shell "chmod 777 /data/local/tmp/resize"
adb shell "/data/local/tmp/resize 4 1440 2560 /data/local/tmp/dump.raw /data/local/tmp/resized.raw"
adb pull /data/local/tmp/resized.raw .
