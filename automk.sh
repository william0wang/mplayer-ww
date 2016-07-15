# !/bin/sh
_tg=no
test "$1" && _tg="$1"

if test $_tg = st || test $_tg = shared; then
  echo
  echo Beginning build static version...
  echo
  ./automk-shared.sh $2 $3 $4
else
  echo
  echo Beginning build sharded version...
  echo
  ./automk-static.sh $2 $3 $4
fi

