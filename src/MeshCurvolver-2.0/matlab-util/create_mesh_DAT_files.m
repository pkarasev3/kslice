% generate a shape for import somewhere else
% create it as a levelset f(x,y,z) = 0
step = 0.02
[x,y,z] = meshgrid(-2:step:2,-2:step:2,-2:step:2);

alpha = 0.2;
beta = 0.2;
w1 = 2;
w2 = 2;
%fxyz = -0.2+x.^2 + y.^2 + z.^2 - ((sin(w1*atan2(y,x))+alpha).*(sin(w2*atan2(z,sqrt(x.^2+y.^2)))+beta)).^2;
%fxyz = x.^2+y.^2+z.^2 - beta*( sin( x*w1 ).*sin( y*w2 ).*sin( z*w2 ) + alpha );
%fxyz = x.^2+y.^2+z.^2 - (alpha + beta*sin(w1*atan2(y,x))+beta*sin(w1*atan2(z,x)));
fxyz = 1 - (x.^2 + y.^2 + z.^2);

% get faces and vertices
[f,v] = isosurface(x,y,z,fxyz,0.0); 
[nf,nv] = reducepatch(f,v,1.0);
nv = nv;
n = isonormals(x,y,z,fxyz,nv);
trisurf(nf,nv(:,1),nv(:,2),nv(:,3));

 csvwrite('fp.dat',nf);
 csvwrite('vp.dat',nv);
 csvwrite('np.dat',n);
 
% dlmwrite('dd.dat',polys,'delimiter',',','precision',6);