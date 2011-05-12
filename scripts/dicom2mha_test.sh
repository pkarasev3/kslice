#!/bin/bash




#   where a bunch of .dcm images are located
dicomdir="Some/Path/DICOM/PA000000/ST000000/SE000006" 


# where to write to
mhaout="outputFile.mha"


echo "$dicomdir  and $mhaout  must be valid dicom directories and output filenames!"


endianness="big"  # unless there's some weird computer hardware that was little endian
../bin/dicom2mha  $dicomdir  $mhaout  $endianness
