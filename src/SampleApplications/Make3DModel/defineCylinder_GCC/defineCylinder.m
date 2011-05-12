function numSlices = defineCylinder(radInMMstr,readpathPts,readpathMask)
% Input: radius in mm, later: input/output files names
% Output: write to disk (somewhere...)

% TODO: add point list as input arg (e.g. string)


%addpath('../MatlabIO')
%readpathPts='../../../data/List.txt';
%readpathMask='../../../data/37391FemurTop_Decompressed.mha';
writeTunnelLab='generatedTunnel.mha';
writeChoppedMask='37391FemurTop_Decompressed_chopped.mha';

radInMM=str2num(radInMMstr); %radius of the cylinder


%parse the fiducial point(the coordinates)
fidPts=fopen(readpathPts);

MaxIters = 1e7;
kiter = 1;
while (1 && kiter < MaxIters )
    tline = fgetl(fidPts);
    if ~ischar(tline), break, end
    disp(tline)
    
    if ~strcmp(tline(1),'#')
        [~,~,~,nextindex]=sscanf(tline,('L-P%d'));
        findCommas=tline(nextindex+1:end)==',';
        thisLine=tline(nextindex+1:end);
        thisLine(findCommas)=' ';
        [point1FidCoord]=sscanf(thisLine,'%f');
        point1FidCoord=point1FidCoord(1:3)';
        
        tline = fgetl(fidPts);
        [~,~,~,nextindex]=sscanf(tline,('L-P%d'));
        findCommas=tline(nextindex+1:end)==',';
        thisLine=tline(nextindex+1:end);
        thisLine(findCommas)=' ';
        [point2FidCoord]=sscanf(thisLine,'%f');
        point2FidCoord=point2FidCoord(1:3)';      
    end
    kiter = kiter+1;
end
fclose(fidPts);

%read in the label map for the femur to get patient coordinate system (offset of the origin and rotation of the axes with respect to the global coordinates)
[mask, maskHeader]=ReadMHA(readpathMask); %read MRI Image, we will use the same header to write tunnel labelmap

for i=1:length(maskHeader)
    if strcmp(sscanf(maskHeader{i},'%s',1),'TransformMatrix')
        [A]=sscanf(maskHeader{i}(18:end),'%f'); %scan everything after "TransformMatrix = "
        transformMatrix=reshape(A,3,3)';
    elseif strcmp(sscanf(maskHeader{i},'%s',1),'ElementSpacing')
        [spacingAct]=sscanf(maskHeader{i}(18:end),'%f'); %scan everything after "ElementSpacing = " 
        spacingAct=[spacingAct(3) spacingAct(1) spacingAct(2)];
    elseif strcmp(sscanf(maskHeader{i},'%s',1),'Offset')
        [offsetAct]=sscanf(maskHeader{i}(9:end),'%f'); %scan everything after "Offset = " 
        offsetAct=offsetAct';
        offsetAct=[-offsetAct(1) -offsetAct(2) offsetAct(3)];       
    end
end


%Slicer's fiducials define the coordinate system as:
%coronal plane: slides from front to back
%sagittal plane: slides from one side to other
%axial plane: slides up and down
%x: axis sagittal plane slides on (the depth direction)
%y: axis coronal plane slides on (horizontal direction in 2D slice)
%z: axis the axial plane slides on (vertical direction in 2D slice)
%the origin location, defined by "offset" is the top, left corner
%in slicer's viewer


%shift to origin;
point1=point1FidCoord-offsetAct;
point2=point2FidCoord-offsetAct;

%this is what the transform matrix for an "ASL" orientation looks like
%transformMatrix=[-transformMatrix(3,:); transformMatrix(2,:); -transformMatrix(1,:)]; 
transformMatrix=[-transformMatrix(1,:); transformMatrix(2,:); -transformMatrix(3,:)];                                                                                       


%figure out points in image coordinates corresponding to global coordinates we have from user clicks                                                                                       
%these points should be: x:[0:x_space:imSize1*x_space], y:[0:y_space:imSize2*y_space], z:[0:z_space:imSize3*z_space]
pt1ImgCoord=transformMatrix*(point1)';
pt2ImgCoord=transformMatrix*(point2)';

numSlices = 0;
if sum(pt1ImgCoord>0)==3 && sum(pt2ImgCoord>0)==3;
    numSlices=abs(pt1ImgCoord(1)-pt2ImgCoord(1))/spacingAct(1);
else
    fprintf('Wrong \n');
   % break
end

%prepare grid to compute our tunnel's coordinates
[i,j,k]=size(mask);
%[X,Y,Z]=meshgrid( 0:spacingAct(3):(j-1)*spacingAct(3), 0:spacingAct(2):(i-1)*spacingAct(2),0:spacingAct(1):(k-1)*spacingAct(1));

[X,Y,Z]=meshgrid( 0:spacingAct(3):(j-1)*spacingAct(3),  0:spacingAct(2):(i-1)*spacingAct(2), 0:spacingAct(1):(k-1)*spacingAct(1));
%[X,Y,Z]=meshgrid(0:spacingAct(1):(k-1)*spacingAct(1),0:spacingAct(2):(i-1)*spacingAct(2), 0:spacingAct(3):(j-1)*spacingAct(3));

%stopInd=find( Z>max(pt1ImgCoord(1),pt2ImgCoord(1)) || Z>max(pt1ImgCoord(1),pt2ImgCoord(1)) );

%Question: what points in image coordinates are part of the cylinder (vertical cylinder along z axis, centered at origin in x,y plane) in another coordinate system
%%%%%%%%%%%%%%%%%%%%%%%%%Rotation Matrices defined%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
diffVec=pt2ImgCoord-pt1ImgCoord;
% diffVec(1)=pt2ImgCoord(2)-pt1ImgCoord(2);
% diffVec(2)=-(pt2ImgCoord(3)-pt1ImgCoord(3));
% diffVec(3)=pt2ImgCoord(1)-pt1ImgCoord(1);




%Rx rotates the y-axis towards the z-axis angle measured from the +y-axis
%Ry rotates the z-axis towards the x-axis angle measured from the +x-axis
%Rz rotates the x-axis towards the y-axis angle measured from the +x-axis

attitude=abs(atan(diffVec(3)/diffVec(2)));
if diffVec(3)>0 && diffVec(2)>0     %first quadrant
    attitude=-(pi/2-attitude);
elseif diffVec(3)>0 && diffVec(2)<0 %second quadrant
    attitude=pi/2 - attitude;
elseif diffVec(3)<0 && diffVec(2)<0 %third quadrant
    attitude=pi/2+attitude;
else                               %fourth quadrant
    attitude=-(pi/2+attitude); 
end
Rx=[ 1     0                  0        ;
     0  cos(attitude) -sin(attitude)   ;
     0  sin(attitude)  cos(attitude)   ];


diffVec=Rx'*diffVec;




bank=abs( atan(diffVec(3)/diffVec(1)) );
if diffVec(3)>0 && diffVec(1)>0     %first quadrant
    bank=(pi/2-bank);
elseif diffVec(3)>0 && diffVec(1)<0 %second quadrant
    bank=-(pi/2 - bank);
elseif diffVec(3)<0 && diffVec(1)<0 %third quadrant
    bank=-(pi/2+bank);
else                               %fourth quadrant
    bank=-(pi/2+bank); 
end

Ry=[ cos(bank)     0    sin(bank) ;
        0          1        0     ;
     -sin(bank)    0    cos(bank)];


%diffVec=Ry'*diffVec;







% heading=abs(atan(diffVec(2)/diffVec(1)));
% if diffVec(1)>0 & diffVec(2)>0     %first quadrant
%     heading=heading;
% elseif diffVec(1)>0 & diffVec(2)<0 %second quadrant
%     heading=-heading;
% elseif diffVec(1)<0 & diffVec(2)<0 %third quadrant
%     heading=-(pi - heading);
% else                               %fourth quadrant
%     heading=(pi - heading); 
% end
% Rz=[ cos(heading)   -sin(heading)   0  ;
%      sin(heading)    cos(heading)   0  ;
%         0                 0         1 ];

    
    




%Entire rotation matrix 
rotationMat=(Rx*Ry)';
%%%%%%%%%%%%%%%%%%%Done Definition of Rotation Matrices%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



%shift our volume so that pt1 is at the origin
Y=Y-pt1ImgCoord(1);
X=X-pt1ImgCoord(2);
Z=Z-pt1ImgCoord(3);


transCoord=rotationMat*[Y(:)'; X(:)'; Z(:)'];
%rotate so that the axis of our cylinder (from point1 to point2) is now the z-axis
% X1=X(:,:,1:k/2);
% X2=X(:,:,k/2 +1:end);
% Y1=Y(:,:,1:k/2);
% Y2=Y(:,:,k/2+1:end);
% Z1=Z(:,:,1:k/2);
% Z2=Z(:,:,k/2+1:end);
% 
% 
% clear X Y Z
% 
% transCoord1=single(rotationMat*[X1(:)' ; Y1(:)'; Z1(:)']);
% clear X1 Y1 Z1
% 
% transCoord2=single(rotationMat*[X2(:)' ; Y2(:)'; Z2(:)']);
% 
% clear X2 Y2 Z2
% 
% transCoord=[transCoord1 transCoord2];
% 
% clear transCoord1 transCoord2
% 
% % X=reshape(transCoord(1,:),[i j k]);
% % Y=reshape(transCoord(2,:),[i j k]);
% % Z=reshape(transCoord(3,:),[i j k]);

X=transCoord(1,:);
Y=transCoord(2,:);

%create empty label map
tunnelMask=zeros(i,j,k);

%Define the cylinder
disp(['size of radInMM:' num2str(radInMM)  num2str(size(radInMM))] );
radInMM(:)
size(X)
size(Y)
interior = repmat(radInMM,1,size(X,2) ) ;
size(interior)
rad_vals = sqrt( (X).^2 + (Y).^2 );
size(rad_vals)
ind = find( rad_vals < interior );



pt2ImgCoord=round(pt2ImgCoord);
pt1ImgCoord=round(pt1ImgCoord);

%xind1=round(pt1ImgCoord(1)/spacingAct(2));
%yind1=round(pt1ImgCoord(2)/spacingAct(3));
zind1=round(pt1ImgCoord(3)/spacingAct(1));

%xind2=round(pt2ImgCoord(1)/spacingAct(2));
%yind2=round(pt2ImgCoord(2)/spacingAct(3));
zind2=round(pt2ImgCoord(3)/spacingAct(1));



%mark points inside the cylinder
tunnelMask(ind)=1;

mask(:,:,max(zind1,zind2):end)=0;

indOut=(mask==0);

indOut(:,:,1:min(zind1,zind2)-1)=1;
indOut(:,:,max(zind1,zind2)-1:end)=1;


tunnelMask(indOut(:))=0;



%tunnelMask(xind1-4:xind1+4,yind1-4:yind1+4, zind1-4:zind1+4)=1;
%tunnelMask(xind2-4:xind2+4,yind2-4:yind2+4, zind2-4:zind2+4)=1;

mask(tunnelMask==1)=0;

%tunnelMask(:,:,:)=0;

%record our label map
WriteMHA(writeTunnelLab,tunnelMask,maskHeader);

WriteMHA(writeChoppedMask,mask,maskHeader);


end


