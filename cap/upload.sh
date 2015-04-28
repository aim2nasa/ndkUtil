#!/bin/sh

adb push obj/local/armeabi/cap /data/local/tmp/
adb shell "chmod 777 /data/local/tmp/cap"
