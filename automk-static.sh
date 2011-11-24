# !/bin/sh

if [ -f "config.mak" ]; then
  make distclean
fi

. autocfg-static.sh $1

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
