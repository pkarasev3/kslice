clear all
clc
load brainStem2.mat
currIm=double(brainStem.img);

g = fspecial('gaussian', 3, .5);   % biggish Laplacian of Gaussian mask
smoothIm = conv2(currIm, g, 'valid');
figure(1)
imagesc(currIm)
figure(2)
imagesc(smoothIm)


[idx,C,sumd,D]=kmeans(smoothIm(:),25,'EmptyAction','drop', 'MaxIter', 500);
[val,index]=min(D,[],2);
clusteredIm=index;
figure(3)
imagesc(reshape(clusteredIm,size(currIm,1),size(currIm,2)));