dim_size   = 128;
dim_size_str = [ num2str(dim_size) ' ' num2str(dim_size) ' ' num2str(dim_size) ]
space_size = 0.1; 
space_size_str = [ num2str(space_size) ' ' num2str(space_size) ' ' num2str(space_size) ]

extents        = 0;%-space_size * (dim_size-1);
extent_str     = [ num2str(-extents/2) ' ' num2str(-extents/2) ' ' num2str(-extents/2) ];

header = { 'ObjectType = Image',...
  'NDims = 3',...
  'BinaryData = True',...
  'BinaryDataByteOrderMSB = False',...
  'CompressedData = False',...
  'TransformMatrix = 1 0 0 0 1 0 0 0 1',...
  ['Offset = ', extent_str],...
  'CenterOfRotation = 0 0 0',...
  'AnatomicalOrientation = RAI',...
  ['ElementSpacing = ', space_size_str],...
  ['DimSize = ', dim_size_str],...
  'ElementType = MET_USHORT',...
  'ElementDataFile = LOCAL'}



tic();
img      = phantom(dim_size);
img3D    = zeros( dim_size, dim_size, dim_size );
img3D( :,:, dim_size/2+3   ) = img;
img3D( :,:, dim_size/2+2   ) = img; 
img3D( :,:, dim_size/2-2 )   = img; 
img3D( :,:, dim_size/2-3 )   = img; 

for k = 1:3
  img3D                        = convn( img3D, ones(3,3,3)/27, 'same' );
  img3D                        = (img3D / max(img3D(:))); 
end
img3D                        = uint16( 12345 * img3D / max(img3D(:)) ); 
toc();  disp('writing ...' );
currpath = pwd();
fileloc  = [currpath '/synth_data_phantom3D.mha']

[img hdr] = WriteMHA(fileloc, img3D, header ); 


