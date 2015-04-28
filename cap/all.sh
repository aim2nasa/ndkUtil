#!/bin/sh

ndk-build clean
ndk-build

adb push obj/local/armeabi/cap /data/local/tmp/
adb shell "chmod 777 /data/local/tmp/cap"
adb shell "data/local/tmp/cap /data/local/tmp/dump.raw"
