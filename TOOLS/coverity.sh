MPLAYER_COV_OPTS="--enable-xvmc --enable-menu --enable-gui --enable-mga --enable-bl --enable-joystick --enable-radio --enable-s3fb --enable-tdfxfb --enable-tdfxvid --enable-wii --yasm=nasm"
rm -rf cov-int MPlayer.tgz
make distclean
svn up
./configure $MPLAYER_COV_OPTS && make -j5 ffmpeglibs || exit 1
"$MPLAYER_COV_PATH"/bin/cov-build --dir cov-int make -j5 || exit 1
tar -czf MPlayer.tgz cov-int
curl --form file=@MPlayer.tgz --form token="$MPLAYER_COV_PWD" --form email="$MPLAYER_COV_EMAIL" --form version=2.5 --form description="automated run" https://scan.coverity.com/builds?project=MPlayer

