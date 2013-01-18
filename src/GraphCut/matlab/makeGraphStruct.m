clear all
clc

addpath('../')
imArray=ReadRaw('../0070/pat0070.raw',512,512,198,'int32=>int');%record the raw image as int in matlab
workingSlice=imArray(:,:,127); %152 is brain stem,127 is mandible
%workingSlice=double(rgb2gray(imread('airplane.jpg')));
Kolesov_M3(workingSlice);

pause
load myLabels.mat

brainStem.img=workingSlice;
brainStem.mask_bg=logical(labelMat(:,:,1));
brainStem.mask_fg=logical(labelMat(:,:,2));

save brainStem2.mat brainStem
