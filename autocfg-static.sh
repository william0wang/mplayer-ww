# !/bin/sh
_gcc=no
test "$1" && _gcc="$1"

_config="configure \
    --disable-mencoder \
    --disable-menu \
    --disable-gif \
    --disable-tga \
    --disable-pnm \
    --disable-sdl \
    --disable-caca \
    --disable-faac \
    --disable-x264 \
    --disable-xvid \
    --disable-vidix \
    --disable-aacplus \
    --disable-toolame \
    --disable-twolame \
    --disable-md5sum \
    --disable-vidix-pcidb \
    --disable-libdirac-lavc \
    --enable-static \
    --enable-freetype \
    --enable-runtime-cpudetection \
    --extra-libs="-mwindows" "

if test $_gcc != no; then
    _config+="    --cc=$_gcc "
fi

./$_config

