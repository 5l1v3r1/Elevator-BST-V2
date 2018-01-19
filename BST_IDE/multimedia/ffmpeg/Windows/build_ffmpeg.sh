#/bin/bash

IS_MINGW=`uname -a |grep -i mingw`
IS_MINGW=`test -n "$IS_MINGW"`
IS_MSYS=`uname -a |grep -i msys`
IS_MSYS=`test -n "$IS_MSYS"`

type -a yasm &>/dev/null || echo "You'd better to install yasm!"
DXVA="--enable-dxva2  --enable-hwaccel=h264_dxva2 --enable-hwaccel=mpeg2_dxva2 --enable-hwaccel=vc1_dxva2 --enable-hwaccel=wmv3_dxva2 "
#DXVA=

#SYS_ROOT=/opt/QtSDK/Maemo/4.6.2/sysroots/fremantle-arm-sysroot-20.2010.36-2-slim
#--arch=armv7l --cpu=armv7l 
#CLANG=clang
if [ -n "$CLANG" ]; then
  CLANG_CFLAGS="-target arm-none-linux-gnueabi"
  CLANG_LFLAGS="-target arm-none-linux-gnueabi"
  HOSTCC=clang
  MAEMO5OPT="--host-cc=$HOSTCC --cc=$HOSTCC --enable-gpl --enable-version3 --enable-nonfree --enable-cross-compile  --target-os=linux --arch=armv7-a --sysroot=$SYS_ROOT"
else
  HOSTCC=gcc
  MAEMO5OPT="--host-cc=$HOSTCC --enable-gpl --enable-version3 --enable-nonfree --enable-cross-compile --cross-prefix=arm-none-linux-gnueabi- --target-os=linux --arch=armv7-a --sysroot=$SYS_ROOT"
fi
MAEMO5OPT=
#CONFIGURE="./configure $MAEMO5OPT --disable-static --enable-shared --enable-runtime-cpudetect --enable-memalign-hack --disable-avdevice --enable-avfilter --enable-avresample --disable-postproc --enable-ffprobe --disable-muxers --disable-encoders --enable-pthreads --disable-iconv --disable-bzlib  --enable-hwaccels  $DXVA --extra-cflags=\"-O3  -ftree-vectorize -Wundef -Wdisabled-optimization -Wall -Wno-switch -Wpointer-arith -Wredundant-decls -foptimize-sibling-calls -fstrength-reduce -frerun-loop-opt -frename-registers -ffast-math -fomit-frame-pointer  --sysroot=$SYS_ROOT $CLANG_CFLAGS\" --extra-ldflags=\"--sysroot=$SYS_ROOT $CLANG_LFLAGS\""
CONFIGURE="./configure $MAEMO5OPT --disable-static --enable-shared --enable-runtime-cpudetect --enable-memalign-hack --disable-avdevice --enable-avfilter --enable-avresample --disable-postproc --enable-ffprobe --disable-muxers --disable-encoders --enable-pthreads --disable-iconv --disable-bzlib  --enable-hwaccels  $DXVA --extra-cflags=\"-O3  -ftree-vectorize -Wundef -Wdisabled-optimization -Wall -Wno-switch -Wpointer-arith -Wredundant-decls -foptimize-sibling-calls -fstrength-reduce -frerun-loop-opt -frename-registers -ffast-math -fomit-frame-pointer  $CLANG_CFLAGS\" "

echo $CONFIGURE

time eval $CONFIGURE

# --enable-pic is default  --enable-lto
#http://cmzx3444.iteye.com/blog/1447366
#--enable-openssl  --enable-hardcoded-tables  --enable-librtmp --enable-zlib
