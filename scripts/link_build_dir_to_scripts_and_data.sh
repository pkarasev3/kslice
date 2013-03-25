#!/bin/bash

# calling convention:
#set(SCRIPT_PATH ${CMAKE_SOURCE_DIR}/../scripts CACHE PATH  "scripts folder")
#set(DATA_PATH ${CMAKE_SOURCE_DIR}/../data CACHE PATH  "data folder")
#execute_process(COMMAND "sh"  ${SCRIPT_PATH}/link_build_dir_to_scripts_and_data.sh  "${CMAKE_SOURCE_DIR}"  "${CMAKE_BINARY_DIR}/"   "${SCRIPT_PATH}"  ${DATA_PATH})

echo "received args:  $1  $2 $3  $4"

if [ ! -n "$4" ]; then # -n tests to see if the argument is non empty
 echo "something is wacked, must have 4 args: "
 echo "cmake_source cmake_build script_path data_path"
 exit 1
fi

# step 1, ***copy*** scripts folder to build... 
# yuck, but this way the temp files dumped here can be scrubbed easily
if [ ! -d "$2/scripts" ]; then
    echo "creating script copy in $2/scripts"
    mkdir $2/scripts
    cp -v $3/*.sh  $3/*.bat  $2/scripts/
fi


# step 2. ***symbolic link / shortcut*** data folder to build, 
# so that relative paths always work from inside build folder. not so yuck...
if [ ! -d "$2/data" ]; then
    echo "creating symbolic link (shortcut) in $2/data"
    ln -sf $4  "$2/data"
fi

if [ -d "/u4/public/karasev/data_medical/mha_grantmuller_kslice" ]; then
    echo "creating symbolic link to grant's image files in $2/data/images"
    ln -sf "/u4/public/karasev/data_medical/mha_grantmuller_kslice"  "$2/data/images"
fi

if [ -d "/u4/public/karasev/data_medical/kslice_data" ]; then
    echo "creating symbolic link to PK's test image files in $2/data/kslice_test"
    ln -sf "/u4/public/karasev/data_medical/kslice_data"  "$2/data/kslice_test"
fi

exit 0
