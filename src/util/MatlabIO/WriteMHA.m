function [ imageArray, imageHeader ] = WriteMHA(fileLoc, imageArray, imageHeader )
    
    %test to see if files exists, if it does, erase it    
    exists=fopen(fileLoc);
    if exists~=-1 %then we opened something that already exists
        fclose(exists);
        delete(fileLoc);
    end
    
    %write header first
    lengthHeader=length(imageHeader);
    for i=1:lengthHeader
        dlmwrite(fileLoc,imageHeader{i},'-append','delimiter','','newline','unix');
    end
    
    if( ispc() )
      disp('Warning, you seem to be on windows. FYI line endings are formatted with \n ending. ');
    end

    
    %now, write raw data
    fidFile=fopen(fileLoc,'a');%open file    

    precision='int16';
    [m,n,p]=size(imageArray);
    
    for j=1:p %dont write all at once so that we dont have just one really long line
        currIm=imageArray(:,:,j); %grab one slice at a time
        fwrite(fidFile,currIm,precision); %write in the original format
    end
    
    fclose(fidFile); %finish working with the file
end
