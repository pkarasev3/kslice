function [ imageArray, imgHeader ] = ReadMHA(fileLoc )
%
%  [ imageArray, imgHeader ] = ReadMHA(fileLoc )
%
% Input: file location, must be "full relative path", NOT just in your
% matlab path (because it uses Bash-Shell to do stuff outside of Matlab)
% Output: imageArray MxNxK uint16 and imgHeader list-of-strings
% If something goes wrong reading the file, imageArray will be 1 x P where
% P is not equal to MxNxK (e.g. you're trying to load a huge file on a
% 32Bit machine)
%
% originally by Ivan Kolesov, modifcations by Peter Karasev 
% 2011-2012

    imgHeader = {};
    %%%%%%%%%%%process header info%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    %the arguments we can read:
    argList{1}='ObjectType';
    argList{2}='NDims';    
    argList{3}='BinaryData =';  
    argList{4}='BinaryDataByteOrderMSB';
    argList{5}='TransformMatrix';    
    argList{6}='Offset'; 
    argList{7}='CenterOfRotation';
    argList{8}='ElementSpacing';
    argList{9}='DimSize';    
    argList{10}='AnatomicalOrientation';
    argList{11}='ElementType';
    argList{12}='ElementDataFile';
    argList{13}='CompressedData =';
    argList{14}='CompressedDataSize';
  
    setenv('fileName',fileLoc);
    setenv('numlines','14');
    setenv('maxbytes','2000');
    !head -n $numlines -c $maxbytes $fileName > header.txt
    for k = 1:numel(argList)
      setenv('mha_arg_k',argList{k});
      !cat -A header.txt | grep "$mha_arg_k"  > found_arg_k.txt &&  cat found_arg_k.txt
      found_arg = cell2mat(importdata('found_arg_k.txt'));
      imgHeader{k} = found_arg(1:end-1);
      if (k == 9 )
          dim=sscanf(imgHeader{k},'%*s %*s %d %d %d'); % read dimension sizes
          disp(['dims: ' dim(:)' ] );
      end
      disp(['grabbed arg: ' imgHeader{k} ]);
    end
    
    for k = 1:numel(imgHeader)
      if( isempty( imgHeader{k} ) )
        imgHeader(k) = [];
      end
    end
    numHeaderLines = num2str(numel(imgHeader)+1);
    disp(['found header lines: ' numHeaderLines ] );
    
    rawFileName = 'data.raw';
    setenv('rawFileName',rawFileName);
    setenv('numHeaderLines',numHeaderLines );
    !tail --lines=+$numHeaderLines $fileName > ${rawFileName}
    
    fidFile=fopen(rawFileName); % open file with data only, header is stripped
    precision='int16';
    imageArray=fread(fidFile,prod(dim), [precision '=>' 'int16']);      
    
    if( numel(imageArray) == dim(1) * dim(2) * dim(3) )
      imageArray=reshape(imageArray,dim(1),dim(2),dim(3)); %put back into array of images format
    end
end
%maybe nice?
%!od -w=2048 ${rawFileName}.in > $rawFileName

