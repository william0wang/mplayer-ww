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
. autocfg-static.sh $1 

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
  install -m 755 -s mplayer.exe ./release/
fi

