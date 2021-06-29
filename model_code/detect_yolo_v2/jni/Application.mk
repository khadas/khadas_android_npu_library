#APP_PLATFORM := android-28

APP_STL      := gnustl_static

APP_CFLAGS   := -Wno-error=format-security 
APP_CPPFLAGS := -frtti -fexceptions
APP_ABI      := armeabi-v7a
APP_PLATFORM := android-9
APP_CPPFLAGS := -std=c++11
#APP_ABI      := armeabi armeabi-v7a arm64-v8a

APP_OPTION   := release
APP_ALLOW_MISSING_DEPS=true
