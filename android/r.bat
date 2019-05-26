@echo off

call ndk-build
call ant debug
call adb install -r bin\neurojam-debug.apk
call adb shell am start -n com.raulrita.neurojam/.StartActivity
set /p DUMMY=Hit ENTER to continue...
call adb shell am force-stop com.raulrita.neurojam
