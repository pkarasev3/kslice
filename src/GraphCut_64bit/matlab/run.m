function run
  N = 16; % neighborhood
  lambda = 1;
  label_cnt = 2;

  s = load('lizard');
  s.obj{1}.eval = construct_histogram(s.img(s.mask_bg));
  s.obj{2}.eval = construct_histogram(s.img(s.mask_fg));
  
  function draw(s)
    % initialization
    subplot(3,1,1);
    img_over = overlay(s.img,    s.mask_fg, 1);
    img_over = overlay(img_over, s.mask_bg, 3);
    imagesc(img_over); axis image off;
    
    % histograms
    subplot(3,1,2);
    zz = 0:255;
    plot(zz, s.obj{2}.eval(zz), 'r', ...
         zz, s.obj{1}.eval(zz), 'b');
    legend('foreground', 'background'); axis square;
    set(gca, 'XLim', [0 255], 'YTickLabel', []);
    
    % final segmentation
    subplot(3,1,3);
    imagesc(overlay(s.img, s.map==2, 1)); axis image off;
  end


  function w = R(i) % regional term
    p = s.obj{i}.eval(s.img);
    w = -log(p + eps);

    % optional hard constraints from user initialization
    if i == 1,       w(s.mask_bg) = 0;        % bg
    elseif i == 2,   w(s.mask_fg) = 0; end    % fg
  end
  
  function w = B(e, p, q) % boundary term
    d2 = sum( (s.img(p) - s.img(q)).^2, 1 );
    w = lambda * exp( -d2/(2*mean(d2)) ) / norm(e);
  end
  
  % perform graph cut
  V = 1 - eye(label_cnt); % uniform label bias (generally ignore this)
  s.map = graphchop(size(s.img), N, label_cnt, @R, @B, V);
  draw(s);
end




function fn = construct_histogram(data) % construct grayscale histogram
  h = hist(data, 0:255);
  k = binomfilt(12)';
  k = k / sum(k);
  h = wkeep(conv(h/sum(h), k), 256);
  fn = @(d) h(d + 1);
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




function [h] = binomfilt(order1,order2)
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
  a = (numel(p) - w)/2 + 1;
  q = p(a:(a+w-1));
end
