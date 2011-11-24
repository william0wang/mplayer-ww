# !/bin/sh

if [ -f "config.mak" ]; then
  make distclean
fi

. autocfg-shared.sh $1

if [ -f "config.mak" ]; then
  make -j4
fi

if [ ! -d "release_console" ]; then
  mkdir release_console
fi
if [ -f "mplayer.exe" ]; then
  install -m 755 -s mplayer.exe ./release_console/
fi
if [ -f "mencoder.exe" ]; then
  install -m 755 -s mencoder.exe ./release_console/
fi

if [ -d "/usr/local/ffmpeg-ww-full/bin" ] ; then
	cp -f -p /usr/local/ffmpeg-ww-full/bin/avconv.exe ./release_console/
	cp -f -p /usr/local/ffmpeg-ww-full/bin/ffmpeg.exe ./release_console/
	cp -f -p /usr/local/ffmpeg-ww-full/bin/avcodec-53.dll ./release_console/
	cp -f -p /usr/local/ffmpeg-ww-full/bin/avfilter-2.dll ./release_console/
	cp -f -p /usr/local/ffmpeg-ww-full/bin/avutil-51.dll ./release_console/
	cp -f -p /usr/local/ffmpeg-ww-full/bin/avformat-53.dll ./release_console/
	cp -f -p /usr/local/ffmpeg-ww-full/bin/postproc-51.dll ./release_console/
	cp -f -p /usr/local/ffmpeg-ww-full/bin/swresample-0.dll ./release_console/
	cp -f -p /usr/local/ffmpeg-ww-full/bin/swscale-2.dll ./release_console/
fi
