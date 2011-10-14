#!/bin/sh

test "$1" && extra="-gcc$1"
build=$(date +%Y%m%d)

# get the version number from git log
svn_revision=$(git log --grep='git-svn-id:' -1 | grep  'git-svn-id:' | cut -d "@" -f2 | cut -d " " -f1)

if test -z $svn_revision ; then
# Extract revision number from file used by daily tarball snapshots
# or from the places different Subversion versions have it.
svn_revision=$(cat snapshot_version 2> /dev/null)
test $svn_revision || svn_revision=$(LC_ALL=C svn info 2> /dev/null | grep Revision | cut -d' ' -f2)
test $svn_revision || svn_revision=$(grep revision .svn/entries 2>/dev/null | cut -d '"' -f2)
test $svn_revision || svn_revision=$(sed -n -e '/^dir$/{n;p;q;}' .svn/entries 2>/dev/null)
test $svn_revision && svn_revision=SVN-r$svn_revision
test $svn_revision || svn_revision=UNKNOWN
version=$svn_revision
else
version=SVN-r$svn_revision
fi

NEW_REVISION="#define VERSION \"${version}(${build}${extra})\""
OLD_REVISION=$(head -n 1 version.h 2> /dev/null)
TITLE='#define MP_TITLE "%s "VERSION" (C) 2000-2011 MPlayer Team\n"'

# Update version.h only on revision changes to avoid spurious rebuilds
if test "$NEW_REVISION" != "$OLD_REVISION"; then
    cat <<EOF > version.h
$NEW_REVISION
$TITLE
EOF
fi
