# !/bin/sh

if [ -f "config.mak" ]; then
  make distclean
fi

./configure \
	--disable-mencoder \
    --disable-menu \
    --disable-tv \
    --disable-gif \
    --disable-tga \
    --disable-pnm \
    --disable-sdl \
    --disable-caca \
    --disable-faac \
    --disable-x264 \
    --disable-xvid \
    --disable-vidix \
    --disable-vidix-pcidb \
    --disable-aacplus \
    --disable-toolame \
    --disable-twolame \
    --disable-md5sum \
    --disable-vidix-pcidb \
    --disable-libdirac-lavc \
	--disable-ffmpeg_a \
    --enable-freetype \
    --enable-matrixview \
    --enable-runtime-cpudetection \
	--extra-cflags="-I/usr/local/ffmpeg-ww/include -I/usr/local/x264/include" \
	--extra-ldflags="-L/usr/local/ffmpeg-ww/lib -L/usr/local/x264/lib" \
	--extra-libs="-Wl,--enable-auto-import" 

if [ -f "config.mak" ]; then
  make -j4
fi

if [ ! -d "release_player" ]; then
  mkdir release_player
fi
if [ -f "mplayer.exe" ]; then
  install -m 755 -s mplayer.exe ./release_player/
fi

if [ -d "/usr/local/ffmpeg-ww/bin" ] ; then
	cp -f -p /usr/local/ffmpeg-ww/bin/swscale-2.dll ./release_player/
	cp -f -p /usr/local/ffmpeg-ww/bin/avutil-51.dll ./release_player/
	cp -f -p /usr/local/ffmpeg-ww/bin/avcodec-53.dll ./release_player/
	cp -f -p /usr/local/ffmpeg-ww/bin/avformat-53.dll ./release_player/
	cp -f -p /usr/local/ffmpeg-ww/bin/postproc-51.dll ./release_player/
fi
