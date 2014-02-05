#!/bin/sh
# Check help message header files for conversion specifications and
# valid string constant definitions.

CHECK=checkhelp

SYMCONST_REGEX="[A-Za-z0-9_]\\+"
CONVSPEC_REGEX="%[^diouxXeEfFgGaAcspn%]*[diouxXeEfFgGaAcspn%][0-9]*"
# the [0-9]* is for inttypes' printf specifier macros and hopefully won't hurt otherwise

trap "rm -f ${CHECK}.c ${CHECK}.o" EXIT

CC=$1
shift

# gather definitions containing conversion specifications from master file

while read line; do

  DEFINE=$(echo "$line" | sed -n "s:^[ \t]*#define[ \t]\+\($SYMCONST_REGEX\)[ \t]\+\(.*\):\1=\2:; s:'::g; s:=:=':p")

  case "$DEFINE" in
    *%*) eval "$DEFINE'";;
  esac

done < help/help_mp-en.h

# create statements from definitions in the translated header file for a test
# compilation and compare conversion specifications from the translated header
# file with those of the master file (if any specification in any of the two
# files)

for h in "$@"; do

  cat <<EOF > ${CHECK}.c
#include <inttypes.h>
#include <string.h>
#include "config.h"
#include "$h"
void $CHECK () {
EOF

  while read line; do

    DEFINE=$(echo "$line" | sed -n "s:^[ \t]*#define[ \t]\+\($SYMCONST_REGEX\)[ \t]\+\(.*\):NAME=\1;STRING=\2:; s:'::g; s:STRING=:STRING=':p")

    if [ "$DEFINE" ]; then
      eval "$DEFINE'"
      echo "strdup($NAME);" >> ${CHECK}.c
      ANY_CONVSPEC="$(eval "echo \$${NAME} \${STRING}")"
    else
      ANY_CONVSPEC=""
    fi

    case "$ANY_CONVSPEC" in
      *%*) ;;
        *) continue;;
    esac

    CONVSPECS=$(echo $STRING | sed -n "s:[^%]*\($CONVSPEC_REGEX\)[^%]*: \1:gp")
    MCONVSPECS=$(eval echo \$${NAME} | sed -n "s:[^%]*\($CONVSPEC_REGEX\)[^%]*: \1:gp")

    if [ "$CONVSPECS" != "$MCONVSPECS" ]; then
      echo "$h: $NAME conversion specification mismatch:${MCONVSPECS:- (none)} has been translated${CONVSPECS:- (none)}"
      exit 2
    fi

  done < "$h"

  echo "}" >> ${CHECK}.c
  $CC -Werror -c -o ${CHECK}.o ${CHECK}.c || exit

done
