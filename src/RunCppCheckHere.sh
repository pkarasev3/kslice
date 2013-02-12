#!/bin/bash

#cppcheck -q --enable=all --force --inline-suppr --template=gcc . > ./cppCheckResults.txt 2>&1

# older syntax for template?
#cppcheck --enable=all --force --inline-suppr --template 'gcc' . > ./cppCheckResults.txt 2>&1
# too much crap, just look at KViewerMain and KSegmentor


cppcheck --enable=all --force --inline-suppr\
         --template 'gcc' KViewerMain > /tmp/KViewcppCheckResults.txt 2>&1
echo "Wrote to: KViewcppCheckResults.txt"

cppcheck --enable=all --force --inline-suppr\
         --template 'gcc' KSegmentor > /tmp/KSegmcppCheckResults.txt 2>&1
echo "Wrote to: KSegmcppCheckResults.txt"

TextViewer='gedit'
$TextViewer  /tmp/K*CheckResults.txt &
