# !/bin/sh
_gcc=no
test "$1" && _gcc="$1"

_config="--disable-mencoder \
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
	--disable-ffmpeg_a \
	--enable-debug=3 \
    --enable-freetype \
    --enable-runtime-cpudetection \
	--extra-cflags="-I/usr/local/ffmpeg-ww/include" \
	--extra-ldflags="-L/usr/local/ffmpeg-ww/lib" \
	--extra-libs-mplayer="-Wl,--enable-auto-import" \
    --extra-libs="-mwindows" "

if test $_gcc != no; then
    _config+="    --cc=$_gcc "
fi

./configure $_config

