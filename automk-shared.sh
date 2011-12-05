# !/bin/sh

if [ -f "config.mak" ]; then
  echo
  echo Beginning Clean Up...
  echo
  make distclean
fi

echo
echo Beginning Configrue...
echo
. autocfg-shared.sh $1 

if [ -f "config.mak" ]; then
  echo
  echo Beginning Make...
  echo
  make -j4
fi

if [ -f "mplayer.exe" ]; then
  if [ ! -d "release" ]; then
    mkdir release
  fi
  echo
  echo Beginning Install...
  echo
fi

if [ -d "/usr/local/ffmpeg-ww/bin" ] ; then
	cp -f -p /usr/local/ffmpeg-ww/bin/swscale-2.dll ./
	cp -f -p /usr/local/ffmpeg-ww/bin/avutil-51.dll ./
	cp -f -p /usr/local/ffmpeg-ww/bin/avcodec-53.dll ./
	cp -f -p /usr/local/ffmpeg-ww/bin/avformat-53.dll ./
	cp -f -p /usr/local/ffmpeg-ww/bin/postproc-51.dll ./
fi

