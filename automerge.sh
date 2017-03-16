# !/bin/sh
tmpFile="autom.sh"

echo "git checkout official" >> $tmpFile
echo "git pull official master" >> $tmpFile
echo "git checkout master" >> $tmpFile
echo "git merge official" >> $tmpFile
echo "rm -f "$tmpFile >> $tmpFile
chmod 777 $tmpFile
exec ./$tmpFile
