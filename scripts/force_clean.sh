#!/bin/bash

# arg1: bin folder
# arg2: lib folder

echo "force clean $1 and $2"

tmp_dir=/tmp/`whoami`_KSlice_Trash
if [ ! -d "$tmp_dir" ]; then
    mkdir "$tmp_dir"
fi

touch $1/bogus1
touch $2/bogus2

mv  `ls $1/*`  "$tmp_dir"
mv  `ls $2/*`  "$tmp_dir"

#echo "erasing possibly stale moc files... in case anyone has multiple QT versions mixed up"
StaleMocs=`ls ../src/KViewerMain/src | grep moc_`
for stale_moc in ${StaleMocs}
do
  mv   ../src/KViewerMain/src/${stale_moc}   "$tmp_dir"
  echo "scrubbed ${stale_moc}"
done

exit 0
