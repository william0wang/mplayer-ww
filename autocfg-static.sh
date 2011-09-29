# !/bin/sh
_gcc=no
test "$1" && _gcc="$1"

if [ ! -d "./ffmpeg" ] ; then
	echo ;
	echo ============================================;
	echo Please checkout \'ffmpeg\' git Submodule first;
	echo ============================================;
	echo ;
	exit 0
fi

_config="configure \
    --disable-menu \
    --disable-tv \
    --disable-sdl \
    --disable-caca \
    --disable-vidix \
    --disable-vidix-pcidb \
    --enable-faac \
    --enable-static \
    --enable-freetype \
    --enable-matrixview \
    --enable-runtime-cpudetection \
	--extra-cflags="-I/usr/local/x264/include" \
	--extra-ldflags="-L/usr/local/x264/lib" "

if test $_gcc != no; then
    _config+="    --cc=$_gcc "
fi

./$_config

