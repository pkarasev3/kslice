clear all
clc
close all

%addpath('/media/Storage/Work/ACLReconstruction/FileIO/MatlabIO/'); %adds the MHA reader to path
%addpath('/u4/home/ikolesov3/Work/ACLReconstruction/FileIO/MatlabIO')
addpath('../MatlabIO');

%define some parameters
% readpathPts='/u4/home/ikolesov3/Work/Data/ACLData/healthy_knee_mult_dir/SE000007/Labels/List.txt';
% readpathMask='/u4/home/ikolesov3/Work/Data/ACLData/healthy_knee_mult_dir/SE000007/Labels/IM0000001-Peach-label_Decompressed_safe.mha';
% writeTunnelLab='/u4/home/ikolesov3/Work/Data/ACLData/healthy_knee_mult_dir/SE000007/Labels/generatedTunnel.mha';

% readpathPts='/u4/home/ikolesov3/Work/Data/ACLData/BasauriGabriela/4151/4152/Labels/List.txt';
% readpathMask='/u4/home/ikolesov3/Work/Data/ACLData/BasauriGabriela/4151/4152/Labels/41510_Label_Decompressed.mha';
% writeTunnelLab='/u4/home/ikolesov3/Work/Data/ACLData/BasauriGabriela/4151/4152/Labels/generatedTunnel.mha';

% readpathPts='/u4/home/ikolesov3/Work/Data/ACLData/HusneyKavanagh/3731/3735/Labels/List.txt';
% readpathMask='/u4/home/ikolesov3/Work/Data/ACLData/HusneyKavanagh/3731/3735/Labels/37391_Label_Decompressed.mha';
% writeTunnelLab='/u4/home/ikolesov3/Work/Data/ACLData/HusneyKavanagh/3731/3735/Labels/generatedTunnel.mha';

% readpathPts='/media/AK_4G/HusneyKavanagh/3731/3735/Labels/List.txt';
% readpathMask='/media/AK_4G/HusneyKavanagh/3731/3735/Labels/37391_Label_Decompressed.mha';
% writeTunnelLab='/media/AK_4G/HusneyKavanagh/3731/3735/Labels/generatedTunnel.mha';

readpathPts='../../../data/List_w_Dane.txt';
readpathMask='../../../data/37391FemurTop_Decompressed.mha';
writeTunnelLab='../../../data/generatedTunnel.mha';
writeChoppedMask='../../../data/37391FemurTop_Decompressed_chopped.mha';

radInMM=3.5; %radius of the cylinder


%parse the fiducial point(the coordinates)
fidPts=fopen(readpathPts);

while 1
    tline = fgetl(fidPts);
    if ~ischar(tline), break, end
    disp(tline)
    
    if ~strcmp(tline(1),'#')
        [ptName,cnt,errmsg,nextindex]=sscanf(tline,['L-P%d']);
        findCommas=tline(nextindex+1:end)==',';
        thisLine=tline(nextindex+1:end);
        thisLine(findCommas)=' ';
        [point1FidCoord,cnt,errmsg,nextindex]=sscanf(thisLine,'%f');
        point1FidCoord=point1FidCoord(1:3)';
        
        tline = fgetl(fidPts);
        [ptName,cnt,errmsg,nextindex]=sscanf(tline,['L-P%d']);
        findCommas=tline(nextindex+1:end)==',';
        thisLine=tline(nextindex+1:end);
        thisLine(findCommas)=' ';
        [point2FidCoord,cnt,errmsg,nextindex]=sscanf(thisLine,'%f');
        point2FidCoord=point2FidCoord(1:3)';      
    end
end
fclose(fidPts);


%read in the label map for the femur to get patient coordinate system (offset of the origin and rotation of the axes with respect to the global coordinates)
[mask, maskHeader]=ReadMHA(readpathMask); %read MRI Image, we will use the same header to write tunnel labelmap

for i=1:length(maskHeader)
    if strcmp(sscanf(maskHeader{i},'%s',1),'TransformMatrix')
        [A,COUNT,ERRMSG,NEXTINDEX]=sscanf(maskHeader{i}(18:end),'%f'); %scan everything after "TransformMatrix = "
        transformMatrix=reshape(A,3,3)';
    elseif strcmp(sscanf(maskHeader{i},'%s',1),'ElementSpacing')
        [spacingAct,COUNT,ERRMSG,NEXTINDEX]=sscanf(maskHeader{i}(18:end),'%f'); %scan everything after "ElementSpacing = " 
        spacingAct=[spacingAct(3) spacingAct(1) spacingAct(2)];
    elseif strcmp(sscanf(maskHeader{i},'%s',1),'Offset')
        [offsetAct,COUNT,ERRMSG,NEXTINDEX]=sscanf(maskHeader{i}(9:end),'%f'); %scan everything after "Offset = " 
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


%shift to origin
point1=point1FidCoord-offsetAct
point2=point2FidCoord-offsetAct

%this is what the transform matrix for an "ASL" orientation looks like
%transformMatrix=[-transformMatrix(3,:); transformMatrix(2,:); -transformMatrix(1,:)]; 
transformMatrix=[-transformMatrix(1,:); transformMatrix(2,:); -transformMatrix(3,:)];                                                                                       


%figure out points in image coordinates corresponding to global coordinates we have from user clicks                                                                                       
%these points should be: x:[0:x_space:imSize1*x_space], y:[0:y_space:imSize2*y_space], z:[0:z_space:imSize3*z_space]
pt1ImgCoord=transformMatrix*(point1)'
pt2ImgCoord=transformMatrix*(point2)'

if sum(pt1ImgCoord>0)==3 & sum(pt2ImgCoord>0)==3
    numSlices=abs(pt1ImgCoord(1)-pt2ImgCoord(1))/spacingAct(1)
else
    fprintf('Wrong \n');
    break
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
if diffVec(3)>0 & diffVec(2)>0     %first quadrant
    attitude=-(pi/2-attitude);
elseif diffVec(3)>0 & diffVec(2)<0 %second quadrant
    attitude=pi/2 - attitude;
elseif diffVec(3)<0 & diffVec(2)<0 %third quadrant
    attitude=pi/2+attitude;
else                               %fourth quadrant
    attitude=-(pi/2+attitude); 
end
Rx=[ 1     0                  0        ;
     0  cos(attitude) -sin(attitude)   ;
     0  sin(attitude)  cos(attitude)   ];


diffVec=Rx'*diffVec;




bank=abs( atan(diffVec(3)/diffVec(1)) );
if diffVec(3)>0 & diffVec(1)>0     %first quadrant
    bank=(pi/2-bank);
elseif diffVec(3)>0 & diffVec(1)<0 %second quadrant
    bank=-(pi/2 - bank);
elseif diffVec(3)<0 & diffVec(1)<0 %third quadrant
    bank=-(pi/2+bank);
else                               %fourth quadrant
    bank=-(pi/2+bank); 
end

Ry=[ cos(bank)     0    sin(bank) ;
        0          1        0     ;
     -sin(bank)    0    cos(bank)];


diffVec=Ry'*diffVec;







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
rotationMat=inv(Rx*Ry);
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
ind=( (X).^2 + (Y).^2 )<=radInMM^2;



pt2ImgCoord=round(pt2ImgCoord);
pt1ImgCoord=round(pt1ImgCoord);

xind1=round(pt1ImgCoord(1)/spacingAct(2));
yind1=round(pt1ImgCoord(2)/spacingAct(3));
zind1=round(pt1ImgCoord(3)/spacingAct(1));

xind2=round(pt2ImgCoord(1)/spacingAct(2));
yind2=round(pt2ImgCoord(2)/spacingAct(3));
zind2=round(pt2ImgCoord(3)/spacingAct(1));



%mark points inside the cylinder
tunnelMask(ind)=1;

mask(:,:,max(zind1,zind2):end)=0;

indOut=(mask==0);

indOut(:,:,1:min(zind1,zind2))=1;
indOut(:,:,max(zind1,zind2):end)=1;


tunnelMask(indOut(:))=0;



%tunnelMask(xind1-4:xind1+4,yind1-4:yind1+4, zind1-4:zind1+4)=1;
%tunnelMask(xind2-4:xind2+4,yind2-4:yind2+4, zind2-4:zind2+4)=1;

mask(tunnelMask==1)=0;

%tunnelMask(:,:,:)=0;

%record our label map
WriteMHA(writeTunnelLab,tunnelMask,maskHeader);

WriteMHA(writeChoppedMask,mask,maskHeader);



%%%%%%%%%%%%%%%%%Short Explanation of how orientation works (copied from a website)%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% /*---------------------------------------------------------------------------*/
% /* 3D IMAGE (VOLUME) ORIENTATION AND LOCATION IN SPACE:
%    ---------------------------------------------------
%    There are 3 different methods by which continuous coordinates can
%    attached to voxels.  The discussion below emphasizes 3D volumes, and
%    the continuous coordinates are referred to as (x,y,z).  The voxel
%    index coordinates (i.e., the array indexes) are referred to as (i,j,k),
%    with valid ranges:
%      i = 0 .. dim[1]-1
%      j = 0 .. dim[2]-1  (if dim[0] >= 2)
%      k = 0 .. dim[3]-1  (if dim[0] >= 3)
%    The (x,y,z) coordinates refer to the CENTER of a voxel.  In methods
%    2 and 3, the (x,y,z) axes refer to a subject-based coordinate system,
%    with
%      +x = Right  +y = Anterior  +z = Superior.
%    This is a right-handed coordinate system.  However, the exact direction
%    these axes point with respect to the subject depends on qform_code
%    (Method 2) and sform_code (Method 3).
% 
%    N.B.: The i index varies most rapidly, j index next, k index slowest.
%     Thus, voxel (i,j,k) is stored starting at location
%       (i + j*dim[1] + k*dim[1]*dim[2]) * (bitpix/8)
%     into the dataset array.
% 
%    N.B.: The ANALYZE 7.5 coordinate system is
%       +x = Left  +y = Anterior  +z = Superior
%     which is a left-handed coordinate system.  This backwardness is
%     too difficult to tolerate, so this NIFTI-1 standard specifies the
%     coordinate order which is most common in functional neuroimaging.
% 
%    N.B.: The 3 methods below all give the locations of the voxel centers
%     in the (x,y,z) coordinate system.  In many cases, programs will wish
%     to display image data on some other grid.  In such a case, the program
%     will need to convert its desired (x,y,z) values into (i,j,k) values
%     in order to extract (or interpolate) the image data.  This operation
%     would be done with the inverse transformation to those described below.
% 
%    N.B.: Method 2 uses a factor 'qfac' which is either -1 or 1; qfac is
%     stored in the otherwise unused pixdim[0].  If pixdim[0]=0.0 (which
%     should not occur), we take qfac=1.  Of course, pixdim[0] is only used
%     when reading a NIFTI-1 header, not when reading an ANALYZE 7.5 header.
% 
%    N.B.: The units of (x,y,z) can be specified using the xyzt_units field.
% 
%    METHOD 1 (the "old" way, used only when qform_code = 0):
%    -------------------------------------------------------
%    The coordinate mapping from (i,j,k) to (x,y,z) is the ANALYZE
%    7.5 way.  This is a simple scaling relationship:
% 
%      x = pixdim[1] * i
%      y = pixdim[2] * j
%      z = pixdim[3] * k
% 
%    No particular spatial orientation is attached to these (x,y,z)
%    coordinates.  (NIFTI-1 does not have the ANALYZE 7.5 orient field,
%    which is not general and is often not set properly.)  This method
%    is not recommended, and is present mainly for compatibility with
%    ANALYZE 7.5 files.
% 
%    METHOD 2 (used when qform_code > 0, which should be the "normal case):
%    ---------------------------------------------------------------------
%    The (x,y,z) coordinates are given by the pixdim[] scales, a rotation
%    matrix, and a shift.  This method is intended to represent
%    "scanner-anatomical" coordinates, which are often embedded in the
%    image header (e.g., DICOM fields (0020,0032), (0020,0037), (0028,0030),
%    and (0018,0050)), and represent the nominal orientation and location of
%    the data.  This method can also be used to represent "aligned"
%    coordinates, which would typically result from some post-acquisition
%    alignment of the volume to a standard orientation (e.g., the same
%    subject on another day, or a rigid rotation to true anatomical
%    orientation from the tilted position of the subject in the scanner).
%    The formula for (x,y,z) in terms of header parameters and (i,j,k) is:
% 
%      [ x ]   [ R11 R12 R13 ] [        pixdim[1] * i ]   [ qoffset_x ]
%      [ y ] = [ R21 R22 R23 ] [        pixdim[2] * j ] + [ qoffset_y ]
%      [ z ]   [ R31 R32 R33 ] [ qfac * pixdim[3] * k ]   [ qoffset_z ]
% 
%    The qoffset_* shifts are in the NIFTI-1 header.  Note that the center
%    of the (i,j,k)=(0,0,0) voxel (first value in the dataset array) is
%    just (x,y,z)=(qoffset_x,qoffset_y,qoffset_z).
% 
%    The rotation matrix R is calculated from the quatern_* parameters.
%    This calculation is described below.
% 
%    The scaling factor qfac is either 1 or -1.  The rotation matrix R
%    defined by the quaternion parameters is "proper" (has determinant 1).
%    This may not fit the needs of the data; for example, if the image
%    grid is
%      i increases from Left-to-Right
%      j increases from Anterior-to-Posterior
%      k increases from Inferior-to-Superior
%    Then (i,j,k) is a left-handed triple.  In this example, if qfac=1,
%    the R matrix would have to be
% 
%      [  1   0   0 ]
%      [  0  -1   0 ]  which is "improper" (determinant = -1).
%      [  0   0   1 ]
% 
%    If we set qfac=-1, then the R matrix would be
% 
%      [  1   0   0 ]
%      [  0  -1   0 ]  which is proper.
%      [  0   0  -1 ]
% 
%    This R matrix is represented by quaternion [a,b,c,d] = [0,1,0,0]
%    (which encodes a 180 degree rotation about the x-axis).
% 
%    METHOD 3 (used when sform_code > 0):
%    -----------------------------------
%    The (x,y,z) coordinates are given by a general affine transformation
%    of the (i,j,k) indexes:
% 
%      x = srow_x[0] * i + srow_x[1] * j + srow_x[2] * j + srow_x[3]
%      y = srow_y[0] * i + srow_y[1] * j + srow_y[2] * j + srow_y[3]
%      z = srow_z[0] * i + srow_z[1] * j + srow_z[2] * j + srow_z[3]
% 
%    The srow_* vectors are in the NIFTI_1 header.  Note that no use is
%    made of pixdim[] in this method.
% 
%    WHY 3 METHODS?
%    --------------
%    Method 1 is provided only for backwards compatibility.  The intention
%    is that Method 2 (qform_code > 0) represents the nominal voxel locations
%    as reported by the scanner, or as rotated to some fiducial orientation and
%    location.  Method 3, if present (sform_code > 0), is to be used to give
%    the location of the voxels in some standard space.  The sform_code
%    indicates which standard space is present.  Both methods 2 and 3 can be
%    present, and be useful in different contexts (method 2 for displaying the
%    data on its original grid; method 3 for displaying it on a standard grid).
% 
%    In this scheme, a dataset would originally be set up so that the
%    Method 2 coordinates represent what the scanner reported.  Later,
%    a registration to some standard space can be computed and inserted
%    in the header.  Image display software can use either transform,
%    depending on its purposes and needs.
% 
%    In Method 2, the origin of coordinates would generally be whatever
%    the scanner origin is; for example, in MRI, (0,0,0) is the center
%    of the gradient coil.
% 
%    In Method 3, the origin of coordinates would depend on the value
%    of sform_code; for example, for the Talairach coordinate system,
%    (0,0,0) corresponds to the Anterior Commissure.
% 
%    QUATERNION REPRESENATION OF ROTATION MATRIX (METHOD 2)
%    ------------------------------------------------------
%    The orientation of the (x,y,z) axes relative to the (i,j,k) axes
%    in 3D space is specified using a unit quaternion [a,b,c,d], where
%    a*a+b*b+c*c+d*d=1.  The (b,c,d) values are all that is needed, since
%    we require that a = sqrt(1.0-b*b+c*c+d*d) be nonnegative.  The (b,c,d)
%    values are stored in the (quatern_b,quatern_c,quatern_d) fields.
% 
%    The quaternion representation is chosen for its compactness in
%    representing rotations. The (proper) 3x3 rotation matrix that
%    corresponds to [a,b,c,d] is
% 
%          [ a*a+b*b-c*c-d*d   2*b*c-2*a*d       2*b*d+2*a*c     ]
%      R = [ 2*b*c+2*a*d       a*a+c*c-b*b-d*d   2*c*d-2*a*b     ]
%          [ 2*b*d-2*a*c       2*c*d+2*a*b       a*a+d*d-c*c-b*b ]
% 
%          [ R11               R12               R13             ]
%        = [ R21               R22               R23             ]
%          [ R31               R32               R33             ]
% 
%    If (p,q,r) is a unit 3-vector, then rotation of angle h about that
%    direction is represented by the quaternion
% 
%      [a,b,c,d] = [cos(h/2), p*sin(h/2), q*sin(h/2), r*sin(h/2)].
% 
%    Requiring a >= 0 is equivalent to requiring -Pi <= h <= Pi.  (Note that
%    [-a,-b,-c,-d] represents the same rotation as [a,b,c,d]; there are 2
%    quaternions that can be used to represent a given rotation matrix R.)
%    To rotate a 3-vector (x,y,z) using quaternions, we compute the
%    quaternion product
% 
%      [0,x',y',z'] = [a,b,c,d] * [0,x,y,z] * [a,-b,-c,-d]
% 
%    which is equivalent to the matrix-vector multiply
% 
%      [ x' ]     [ x ]
%      [ y' ] = R [ y ]   (equivalence depends on a*a+b*b+c*c+d*d=1)
%      [ z' ]     [ z ]
% 
%    Multiplication of 2 quaternions is defined by the following:
% 
%      [a,b,c,d] = a*1 + b*I + c*J + d*K
%      where
%        I*I = J*J = K*K = -1 (I,J,K are square roots of -1)
%        I*J =  K  J*K =  I  K*I =  J
%        J*I = -K  K*J = -I  I*K = -J  (not commutative!)
%      For example
%        [a,b,0,0] * [0,0,0,1] = [0,-b,0,a]
%      since this expands to
%        (a+b*I)*(K) = (a*K+b*I*K) = (a*K-b*J).
% 
%    The above formula shows how to go from quaternion (b,c,d) to
%    rotation matrix and direction cosines.  Conversely, given R,
%    we can compute the fields for the NIFTI-1 header by
% 
%      a = 0.5  * sqrt(1+R11+R22+R33)    (not stored)
%      b = 0.25 * (R32-R23) / a       => quatern_b
%      c = 0.25 * (R13-R31) / a       => quatern_c
%      d = 0.25 * (R21-R12) / a       => quatern_d
% 
%    If a=0 (a 180 degree rotation), alternative formulas are needed.
%    See the nifti1_io.c function mat44_to_quatern() for an implementation
%    of the various cases in converting R to [a,b,c,d].
% 
%    Note that R-transpose (= R-inverse) would lead to the quaternion
%    [a,-b,-c,-d].
% 
%    The choice to specify the qoffset_x (etc.) values in the final
%    coordinate system is partly to make it easy to convert DICOM images to
%    this format.  The DICOM attribute "Image Position (Patient)" (0020,0032)
%    stores the (Xd,Yd,Zd) coordinates of the center of the first voxel.
%    Here, (Xd,Yd,Zd) refer to DICOM coordinates, and Xd=-x, Yd=-y, Zd=z,
%    where (x,y,z) refers to the NIFTI coordinate system discussed above.
%    (i.e., DICOM +Xd is Left, +Yd is Posterior, +Zd is Superior,
%         whereas +x is Right, +y is Anterior  , +z is Superior. )
%    Thus, if the (0020,0032) DICOM attribute is extracted into (px,py,pz), then
%      qoffset_x = -px   qoffset_y = -py   qoffset_z = pz
%    is a reasonable setting for qform_code=NIFTI_XFORM_SCANNER_ANAT.
% 
%    That is, DICOM's coordinate system is 180 degrees rotated about the z-axis
%    from the neuroscience/NIFTI coordinate system.  To transform between DICOM
%    and NIFTI, you just have to negate the x- and y-coordinates.
% 
%    The DICOM attribute (0020,0037) "Image Orientation (Patient)" gives the
%    orientation of the x- and y-axes of the image data in terms of 2 3-vectors.
%    The first vector is a unit vector along the x-axis, and the second is
%    along the y-axis.  If the (0020,0037) attribute is extracted into the
%    value (xa,xb,xc,ya,yb,yc), then the first two columns of the R matrix
%    would be
%               [ -xa  -ya ]
%               [ -xb  -yb ]
%               [  xc   yc ]
%    The negations are because DICOM's x- and y-axes are reversed relative
%    to NIFTI's.  The third column of the R matrix gives the direction of
%    displacement (relative to the subject) along the slice-wise direction.
%    This orientation is not encoded in the DICOM standard in a simple way;
%    DICOM is mostly concerned with 2D images.  The third column of R will be
%    either the cross-product of the first 2 columns or its negative.  It is
%    possible to infer the sign of the 3rd column by examining the coordinates
%    in DICOM attribute (0020,0032) "Image Position (Patient)" for successive
%    slices.  However, this method occasionally fails for reasons that I
%    (RW Cox) do not understand.
% -----------------------------------------------------------------------------*/









































% %use quaternions to define rotation
% %conj(a + b i + c j + d k) = a - b i - c j - d k
% %Pout = q * Pin * conj(q)
% 
% %compute quaternion
% heading=abs(atan(diffVec(2)/diffVec(1)));
% 
% if diffVec(1)>0 & diffVec(2)>0     %first quadrant
%     heading=heading;
% elseif diffVec(1)>0 & diffVec(2)<0 %second quadrant
%     heading=-heading;
% elseif diffVec(1)<0 & diffVec(2)<0 %third quadrant
%     heading=-(pi - heading);
% else                               %fourth quadrant
%     heading=(pi - heading); 
% end
% 
% attitude=abs(atan(diffVec(3)/diffVec(2)));
% % if diffVec(3)>0 & diffVec(2)>0     %first quadrant
% %     attitude=-(pi/2 - attitude);
% % elseif diffVec(3)>0 & diffVec(2)<0 %second quadrant
% %     attitude=pi/2 - attitude;
% % elseif diffVec(3)<0 & diffVec(2)<0 %third quadrant
% %     attitude=pi/2 + attitude;
% % else                               %fourth quadrant
% %     attitude=-(pi/2 + attitude); 
% % end
% if diffVec(3)>0 & diffVec(2)>0     %first quadrant
%     attitude=attitude;
% elseif diffVec(3)>0 & diffVec(2)<0 %second quadrant
%     attitude=pi - attitude;
% elseif diffVec(3)<0 & diffVec(2)<0 %third quadrant
%     attitude=-(pi - attitude);
% else                               %fourth quadrant
%     attitude=-attitude; 
% end
% 
% 
% bank=abs(atan(diffVec(1)/diffVec(3)));
% if diffVec(3)>0 & diffVec(1)>0     %first quadrant
%     bank=bank;
% elseif diffVec(3)>0 & diffVec(1)<0 %second quadrant
%     bank= - bank;
% elseif diffVec(3)<0 & diffVec(1)<0 %third quadrant
%     bank=-(pi - bank);
% else                               %fourth quadrant
%     bank=(pi - bank); 
% end
% 
% 
% c1=cos(heading/2);
% c2=cos(attitude/2);
% c3=cos(bank/2);
% s1=sin(heading/2);
% s2=sin(attitude/2);
% s3=sin(bank/2);
% 
% w=c1*c2*c3-s1*s2*s3;
% x=s1*s2*c3+c1*c2*s3;
% y=s1*c2*c3 + c1*s2*s3;
% z=c1*s2*c3 -s1*c2*s3;
% 
% R=[ 1-2*y^2-2*z^2     2*x*y-2*z*w      2*x*z+2*y*w;
%     2*x*y + 2*z*w     1-2*x^2-2*z^2    2*y*z - 2*x*w;
%     2*x*z-2*y*w       2*y*z+2*x*w      1-2*x^2-2*y^2];
