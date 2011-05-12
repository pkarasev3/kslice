
disp('adding Sag1mm_jpg to path... did you untar it from the data folder!?');
addpath('../../../data/Sag1mm_jpg/');

filelist = importdata('images.jpg.txt');
K = numel(filelist);

fh = figure(1); clf; setFigure(fh,[10 10] );
fh = figure(2); clf; setFigure(fh,[10 800] );
fh = figure(3); clf; setFigure(fh,[600 10] );

thresh_val = .1;
hist_space = linspace(-.5,.5,100);

img_peek = imread( filelist{1} );
label_map = uint16(zeros( size(img_peek,1), size(img_peek,1), K ) );

for k = 1:K 
  
  img = imread( filelist{k} );
  img = double(img) / 255.0;
  
  sfigure(1); imshow(img); title(['input image: ' filelist{k} ]);
  img_gray       = rgb2gray(img);
    
  img_r          = img(:,:,1) - img_gray;
  img_g          = img(:,:,2) - img_gray;
  img_b          = img(:,:,3) - img_gray;
  
  img_thresh     = 0 * img;
  
  sfigure(3);  title('difference (RG, RB) histogram'); 
  plot( hist_space, hist(green_red_diff(:), hist_space) , 'g-.' ); hold on;
  plot( hist_space, hist(blue_red_diff(:),  hist_space) , 'b-.' ); hold off;
  
  idx_red        = find( abs( img_r )  > thresh_val );
  idx_green      = find( abs( img_g )  > thresh_val );
  idx_blue       = find( abs( img_b )  > thresh_val );
  
  img_thresh(:,:,1) = img_r; 
  img_thresh(:,:,2) = img_g;
  img_thresh(:,:,3) = img_b;
  
  img_thresh = img_thresh - min(img_thresh(:));
  img_thresh = img_thresh / max(img_thresh(:));
  img_thresh = mean( img_thresh, 3 );
    
  [level  EM ] = graythresh(img_thresh) %#ok<NOPTS>
  
  BW = img_thresh * 0; BW( img_thresh > level ) = 1;
  idx_high = find( img_thresh > level );
  if( length( idx_high ) > numel(BW)/2 )
    BW( BW == 0 ) = 1;
    BW( idx_high ) = 0;
  end
  BW = uint16( 256^2 * BW );
  
  sfigure(2); imshow(BW); title('threshold image');
  
  disp( ['slice: ' num2str(k) ] ); drawnow;
  
  label_map(:,:,k) = uint16(BW);
end


header_text = { 'ObjectType = Image','NDims = 3','BinaryData = True','BinaryDataByteOrderMSB = False','CompressedData = False','TransformMatrix = 0.0744342 0.997217 0.00418098 -0.0156867 0.00537132 -0.999863 -0.997103 0.0743584 0.0160428','Offset = 125.457 -140.303 101.545','CenterOfRotation = 0 0 0','AnatomicalOrientation = ASL','ElementSpacing = 0.3125 0.3125 0.499437','DimSize = 512 512 232','ElementType = MET_SHORT','ElementDataFile = LOCAL' };
WriteMHA('Sag1mm_jpg_thresh.mha',label_map,header_text) ;
