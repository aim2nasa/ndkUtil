#!/bin/sh

adb push obj/local/armeabi/resize /data/local/tmp/
adb shell "chmod 777 /data/local/tmp/resize"
