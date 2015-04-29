#!/bin/sh

adb push obj/local/armeabi/zc /data/local/tmp/
adb shell "chmod 777 /data/local/tmp/zc"
