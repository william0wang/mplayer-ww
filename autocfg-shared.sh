# !/bin/sh

foo_echo()
{
	echo ;
	echo ============================================;
	echo Run \'./automk.sh full\' in \'ffmpeg\' first;
	echo ============================================;
	echo ;
	exit 0
}

if [ ! -d "./ffmpeg" ] ; then
	echo ;
	echo ============================================;
	echo Please checkout \'ffmpeg\' git Submodule first;
	echo ============================================;
	echo ;
	exit 0
fi

if [ ! -d "/usr/local/ffmpeg-ww-full/include/libavcodec" ] ; then
	foo_echo
elif [ ! -d "/usr/local/ffmpeg-ww-full/lib" ] ; then
	foo_echo
fi

./configure \
    --disable-menu \
    --disable-tv \
    --disable-sdl \
    --disable-caca \
    --disable-vidix \
    --disable-vidix-pcidb \
	--disable-ffmpeg_a \
    --enable-faac \
    --enable-freetype \
    --enable-matrixview \
    --enable-runtime-cpudetection \
	--extra-cflags="-I/usr/local/ffmpeg-ww-full/include -I/usr/local/x264/include" \
	--extra-ldflags="-L/usr/local/ffmpeg-ww-full/lib -L/usr/local/x264/lib" \
	--extra-libs="-Wl,--enable-auto-import" 

