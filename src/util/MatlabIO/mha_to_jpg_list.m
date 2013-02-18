function mha_to_png_list(mha_filename, output_dir, verbose_out)
% given an mha file and desired output dir
% write each slice out as a PNG
% (facilitates matlab/opencv ease of use)
%
% peter karasev, ivan kolesov
%
% sample usage:
% output_dir = '/u4/home/pkarasev3/source/svn-robotview-data/medical_imaging/ACL_test/';
% mha_filename = '/u4/home/ikolesov3/ACLData/healthy_knee_mult_dir/SE000007/Transformed/Sag1mmDecompressed.mha';
% mha_to_png_list(mha_filename, output_dir, 1);

if( nargin < 3 )
  verbose_out = 0;
end

[stuff header] = ReadMHA(mha_filename); %#ok<NASGU>


[M N K] = size(stuff);

bEqualize = 1;
if( bEqualize )
  tmp = histeq(stuff(:));
  stuff = reshape(tmp,M,N,K);
end

the_max = max(double(stuff(:)));

for k = 1:K
  
  img = stuff(:,:,k);
  img = double(img) / double(the_max) * 255.0;
  img = repmat( uint8(img),[1,1,3] );
  
  % Be tricky and set RGB to up and down neighbors
  if( k > 1 )
     img(:,:,2) = uint8(  double( stuff(:,:,k-1) ) / the_max * 255.0   );
  end
  if( k < K )
     img(:,:,3) = uint8(  double( stuff(:,:,k+1) ) / the_max * 255.0   );
  end

  
  strname = num2str(k);
  while( numel(strname) < 10)
    strname = [ '0' strname ];
  end
  % TODO: does it make sense to set bit depth !?  ...'Bitdepth',12
  imwrite( img, [output_dir '/slice_' strname '.jpg'], 'jpg','Quality',95.0);
  
  if( verbose_out )
    imagesc(img); title( ['slice ' strname ' of ' num2str(K)] );drawnow;
  end
end

% make a file list, facilitate opencv parsing
setenv('mha_to_jpg_path',output_dir);
!ls $mha_to_jpg_path | grep jpg | grep slice > $mha_to_png_path/images.jpg.txt

end
