function runIKmod
  global s
  global lambda
  
  N = 6; % neighborhood
  lambda = 2;
  label_cnt = 2;

  s = load('../GraphChopStruct.mat'); %brainStem2 and brainStemIt are linked
  s=s.imageMask; %take out the sturcture that was saved (by command save bbb.mat brainStem

  
  %below using logical indexing b/c s.img is an array, s.mask_bg is LOGICAL ARRAY
  shifted_data=s.img(:)-min(s.img(:));
  minVal=double(min(shifted_data));
  maxVal=double(max(shifted_data));
  step=double((maxVal-minVal)/255);
  s.obj{1}.eval = construct_histogram(shifted_data(s.mask_bg),minVal,maxVal,step); %add field called obj(which is a cell array), element 1 in this cell array has field called "eval", we set that
  s.obj{2}.eval = construct_histogram(shifted_data(s.mask_fg),minVal,maxVal,step);
  
  % perform graph cut
  V = 1 - eye(label_cnt); % uniform label bias (generally ignore this)
  s.map = graphchop(size(s.img), N, label_cnt, @R, @B, V);
  draw(s);
end  
  
  function draw(s)
    % initialization
    %subplot(3,1,1);

  for i=1:size(s.img,3)  
    if sum(sum(s.mask_fg(:,:,i)))~=0
        figure(1)
        img_over = overlay(s.img(:,:,i),    s.mask_fg(:,:,i), 1);
        img_over = overlay(img_over, s.mask_bg(:,:,i), 3);
        imagesc(uint8(img_over)); axis image off; 
    end  
    % histograms
    %subplot(3,1,2);
    figure(2)
    zz = 0:255;
    plot(zz, s.obj{2}.eval(zz), 'r', ...
         zz, s.obj{1}.eval(zz), 'b');
    legend('foreground', 'background'); axis square;
    set(gca, 'XLim', [1 256], 'YTickLabel', []);
    
    % final segmentation
    %subplot(3,1,3);
    figure(3)
    imagesc(uint8(overlay(s.img(:,:,i), s.map(:,:,i)==2, 1))); axis image off;
    
    
    figure(4)
%     imagesc(s.img)
%     hold on
%     contour(s.map,[1 1], 'r', 'linewidth',1)
    contourLabelMap=zeros(size(s.img,1),size(s.img,2));    
    imagesc(uint8(overlay(s.img(:,:,i), contourLabelMap==1,1)));
    hold on 
    contour(s.map,[1 1], 'r', 'linewidth',1)
    fprintf('The curr slice is %d \n',i);
    pause
  end
    
%     new_fg=find(s.map==2);
%     new_bg=find(s.map==1);
%     brainStem.img=s.img;
% %     brainStem.mask_bg=logical(zeros(size(s.img,1),size(s.img,2)));
% %     brainStem.mask_bg(new_bg)=logical(1);
%     brainStem.mask_fg=s.mask_fg;
%     brainStem.mask_bg=logical(zeros(size(s.img,1),size(s.img,2)));
%     brainStem.mask_bg(new_bg)=logical(1);    
%     
%     save brainStemIt.mat brainStem
%     colormap gray
  end


  function w = R(i) % regional term
    global s;
    shifted_data=double(s.img(:)-min(s.img(:))); %shift to make all data start at 1  
    
    [m,k]=ind2sub(size(s.img),double(find(s.mask_fg)));

    
%     for l=1:(size(s.img,1)*size(s.img,2))
%         [m_obj,k_obj]=ind2sub(size(s.img),double(l));
%         pot_dist=sqrt((m-m_obj).^2+(k-k_obj).^2);
%         distances(l)=mean(pot_dist);
%     end
%     
%     save distances_avg.mat distances
%     distances=0; %part of the hack!
%     load distances_avg.mat %HACK!!!
%     norm_dist=distances/max(distances); %norm_dist<=1 so 1./norm_dist>=1

    p = s.obj{i}.eval( ceil(255*(shifted_data/max(shifted_data))) ) ;% +  (.05)*exp(1./norm_dist); %so alpha*e^x where x>=1
    w = -log(p + eps);

    % optional hard constraints from user initialization
    if i == 1
        w(s.mask_bg) = 0;        % bg
        w(s.mask_fg)=200000;%this just needs to be larger than the sum of 1+max(sum(B(p,q))) where p,q are neighbors (Paper "Interactive Graph Cuts for Optimal Boundary ..." pg.108
    elseif i == 2
        w(s.mask_fg) = 0;   % fg
        w(s.mask_bg)=200000;
    end
  end
  
  function w = B(e, p, q) % boundary term
    global lambda
    global s
%     [ip,jp]=ind2sub([512,512],p);
%     [iq,jq]=ind2sub([512,512],q);
%     ip=double(ip);
%     iq=double(iq);
%     jp=double(jp);
%     jq=double(jq);
    %distances=sqrt(double((ip-iq).^2+(jp-jq).^2));
    %w=lambda*exp(-distances);
    %d2 = sum( (s.img(p) - s.img(q)).^2, 1 );
    %w = lambda * exp( -d2/(2*mean(d2)) ) / norm(e);
    w=lambda*exp(1./(1+double(abs(s.img(p)-s.img(q)))));  %cuts accross edges between small differences of intensities between neighbors are expensive
    
  
  end





function fn = construct_histogram(data,minVal,maxVal,step) % construct grayscale histogram
  %h = hist(data, 0:255);
  h = hist(double(data),minVal:step:maxVal);
  k = binomfilt(12)';
  k = k / sum(k);
  h = wkeep(conv(h/sum(h), k), 256); %just smoothed the histogram, keep same num of elements, even after convolution
  fn = @(d) h(d+1); %function handle to histogram (can use 0:255 as input)
end




function img = overlay(img, mask, ch)
  if size(img,3) ~= 3
    img = img(:,:,[1 1 1]);
  end
  
  sz = size(img);
  img = reshape(img, [prod(sz(1:2)) sz(3)]);
  
  img(mask,:) = max(img(mask,:) - 100,  0);
  img(mask,ch) = min(img(mask,ch) + 150, 255);
  
  img = reshape(img, sz);
end




function [h] = binomfilt(order1,order2)%this just performs Gaussian filtering efficiently
% BINOMFILT Binomial Kernel
%   BIMONFILT(order1) constructs binomial kernel of
%   order n = order1. 
%   BIMONFILT(order1,order2) constructs a separable
%   2-D binomial kernel as a tensor (outer) product
%   of two 1-D kernels having the orders n = order1
%   and m = order2, respectively.
%
% written by Oleg Michailovich, July 2004

  h=zeros(order1+1,1);
  for k=0:order1
      h(k+1)=nchoosek(order1,k);
  end
  h=h/(2^order1);

  if (nargin>1)
      g=zeros(order2+1,1);
      for k=0:order2
          g(k+1)=nchoosek(order2,k);
      end
      g=g/(2^order2);
      h=h*g';
  end
end

function q = wkeep(p, w)
% extract middle portion of a 1D array
  a =floor( (numel(p) - w)/2 + 1);
  q = p(a:(a+w-1));
end
