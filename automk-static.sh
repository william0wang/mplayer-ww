# !/bin/sh

if [ -f "config.mak" ]; then
  make distclean
fi

. autocfg-static.sh $1

if [ -f "config.mak" ]; then
  make
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
	cp -f -p /usr/local/ffmpeg-ww-full/bin/ffmpeg.exe ./release_console/
	cp -f -p /usr/local/ffmpeg-ww-full/bin/swscale-2.dll ./release_console/
	cp -f -p /usr/local/ffmpeg-ww-full/bin/avutil-50.dll ./release_console/
	cp -f -p /usr/local/ffmpeg-ww-full/bin/avcodec-52.dll ./release_console/
	cp -f -p /usr/local/ffmpeg-ww-full/bin/avfilter-1.dll ./release_console/
	cp -f -p /usr/local/ffmpeg-ww-full/bin/avformat-52.dll ./release_console/
	cp -f -p /usr/local/ffmpeg-ww-full/bin/postproc-51.dll ./release_console/
fi
