function setFigure(fh,pos,scalein,ratioin)
    if( nargin < 3 )
        scalein =1;
    end
    if( nargin < 4 )
      ratioin = 1;
    end
    set(fh,'Position', [pos 400*scalein*ratioin 300*scalein]);
end
