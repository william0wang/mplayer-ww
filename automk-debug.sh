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
. autocfg-debug.sh $1 

if [ -f "config.mak" ]; then
  echo
  echo Beginning Make...
  echo
  make -j4
fi
