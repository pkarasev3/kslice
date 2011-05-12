step = 0.125
[x,y,z] = meshgrid(-2:step:2,-2:step:2,-2:step:2);

fxyz = -0.125 - x.*y + z.^2;

% get faces and vertices
[f,v] = isosurface(x,y,z,fxyz,0.0); 
[nf,nv] = reducepatch(f,v,1.0);
n = isonormals(x,y,z,fxyz,nv);
trisurf(nf,nv(:,1),nv(:,2),nv(:,3));

phi0 = (nv(:,3) > 0.0)*1.0 ;

trisurf(nf,nv(:,1),nv(:,2),nv(:,3),phi0);

numVerts  = size(nv,1);
numFaces  = size(nf,1);
adjimm    = cell(numVerts,1);

for k = 1:numFaces
    verts = nf(k,:);
    for m = 1:length(verts)
       idxm = verts(m);
       for n = 1:length(verts) 
           idxn = verts(n);
           adjimm{idxn} = [adjimm{idxn} idxm];
       end
    end
end

newVerts = [];

for k = 1:numVerts
   adjimm{k} = union( adjimm{k}, adjimm{k} );
   phi0 = 0*phi0;
   neigh_idx = adjimm{k};
    
   breakhere = 1;
   newVerts = [nv(neigh_idx,:)];
   newVerts = [newVerts; newVerts + .01*randn(size(newVerts))];
   
   phi0 = phi0*0;
   phi0(neigh_idx) = 1; phi0(k)=2; trisurf(nf,nv(:,1),nv(:,2),nv(:,3),phi0);
   hold on;
   plot3(newVerts(:,1),newVerts(:,2),newVerts(:,3),'LineStyle','none','Marker','*','MarkerEdgeColor','c','MarkerSize',14); hold off;
   breakhere = 1;
end

phi0 = (nv(:,1)+nv(:,3) > 0.0)*1.0 ;
phi  = 0*phi0;

label = 0*phi0;
init  = phi0;
label( phi0 > 0 )   = 3;
label( phi0 <= 0 )   = -3;

phi  = label;

Lz = [];
for k = 1:numVerts
   neigh = adjimm{k};
   if( init(k) == 1 )
       if ( min( init(neigh) ) == 0 )
       Lz = [Lz k];
       label(k) = 0;
       phi(k) = 0;
       end
   end
end

Ln1 = [];
Lp1 = [];
for idx = 1:numel(Lz)
   k = Lz(idx);
   neigh = adjimm{k};
   for m = 1:numel(neigh)
    q = neigh(m);
    if( label(q) == -3 ) 
      Ln1 = [Ln1 q];
      label(q) = -1;
      phi(q)   = -1.0;
    elseif( label(q) == 3 )
      Lp1 = [Lp1 q];
      label(q) = 1;
      phi(q)   = 1.0;  
    end
   end
end

Ln2 = [];
for idx = 1:numel(Ln1)
   k = Ln1(idx);
   neigh = adjimm{k};
   for m = 1:numel(neigh)
    q = neigh(m);
    if( label(q) == -3 ) 
      Ln2 = [Ln2 q];
      label(q) = -2;
      phi(q)   = -2.0;
    end
   end
end
Lp2 = [];
for idx = 1:numel(Lp1)
   k = Lp1(idx);
   neigh = adjimm{k};
   for m = 1:numel(neigh)
    q = neigh(m);
    if( label(q) == 3 ) 
      Lp2 = [Lp2 q];
      label(q) = 2;
      phi(q)   = 2.0;
    end
   end
end

DrawSets( Lz, Lp1, Ln1,Lp2,Ln2,nf,nv );

Sz = [];
Sn1 = [];
Sn2 = [];
Sp1 = [];
Sp2 = [];

dbstop if error

for its = 1:200
    F = -nv( Lz,3 );
    phi( Lz ) = 0.3 * F + phi( Lz );
    
    Sz = [];
    Sn1 = [];
    Sn2 = [];
    Sp1 = [];
    Sp2 = [];
    
    % Update 0 LS
    Sn1 = Lz( ( phi( Lz ) <= -0.5 ) );
    Sp1 = Lz( ( phi( Lz ) >= 0.5 ) ); 
    Lz( phi(Lz)<= -0.5 ) = [];
    Lz( phi(Lz)>= 0.5 ) = [];
    
    
    k = 1;
    while( ~isempty(Ln1) )
       if( k > numel(Ln1) )
           break;
       end
       neigh = adjimm{ Ln1(k) };
       neigh_labels = label(neigh);
       max_neigh = max(neigh_labels);
       if( max_neigh < 0 )
          Sn2    = [Sn2 Ln1(k)];
          Ln1(k) = []; 
       else
           phi_max = max( phi( neigh ) );
           phi( Ln1(k) ) = phi_max-1;
           if( phi( Ln1(k) ) >= -0.5 )
               Sz    = [Sz Ln1(k)];
               Ln1(k) = []; 
           elseif( phi( Ln1(k) ) < -1.5 )
               Sn2    = [Sn2 Ln1(k)];
               Ln1(k) = []; 
           else
               k = k+1;
           end
       end
    end
    
    k = 1;
    while( ~isempty(Lp1) )
       if( k > numel(Lp1) )
                   break;
       end
       neigh = adjimm{ Lp1(k) };
       neigh_labels = label(neigh);
       min_neigh = min(neigh_labels);
       if( min_neigh > 0 )
          Sp2    = [Sp2 Lp1(k)];
          Lp1(k) = []; 
       else
           phi_min = min( phi( neigh ) );
           phi( Lp1(k) ) = phi_min+1;
           if( phi( Lp1(k) ) <= 0.5 )
               Sz    = [Sz Lp1(k)];
               Lp1(k) = []; 
           elseif( phi( Lp1(k) ) > 1.5 )
               Sp2    = [Sp2 Lp1(k)];
               Lp1(k) = [];
           else
               k = k+1;
               
           end
       end
    end
    
    k = 1;
    while( ~isempty(Ln2) )
       if( k > numel(Ln2) )
                   break;
       end
       neigh = adjimm{ Ln2(k) };
       neigh_labels = label(neigh);
       max_neigh = max(neigh_labels);
       if( max_neigh < -1 )
          label( Ln2(k) ) = -3;
          phi( Ln2(k) ) = -3;
          Ln2(k) = []; 
       else
           phi_max = max( phi( neigh ) );
           phi( Ln2(k) ) = phi_max-1;
           if( phi( Ln2(k) ) >= -1.5 )
               Sn1    = [Sn1 Ln2(k)];
               Ln2(k) = []; 
           elseif( phi( Ln2(k) ) < -2.5 )
               label( Ln2(k) ) = -3;
               phi( Ln2(k) ) = -3;
               Ln2(k) = [];
           else
               k = k+1;
               
           end
       end
    end
    
    k = 1;
    while( ~isempty(Lp2) )
       if( k > numel(Lp2) )
                   break;
       end
       neigh = adjimm{ Lp2(k) };
       neigh_labels = label(neigh);
       min_neigh = min(neigh_labels);
       if( min_neigh > 1 )
          label( Lp2(k) ) = 3;
          phi( Lp2(k) ) = 3;
          Lp2(k) = [];  
       else
           phi_min = min( phi( neigh ) );
           phi( Lp2(k) ) = phi_min+1;
           if( phi( Lp2(k) ) <= 1.5 )
               Sp1    = [Sp1 Lp2(k)];
               Lp2(k) = []; 
           elseif( phi( Lp2(k) ) > 2.5 )
               label( Lp2(k) ) = 3;
               phi( Lp2(k) ) = 3;
               Lp2(k) = [];
           else
               k = k+1;
               
           end
       end
    end
    
    Lz = [ Lz Sz ];
    label( Sz ) = 0;
    
    Ln1 = [ Ln1 Sn1 ];
    label( Sn1 ) = -1;
    for k = 1:numel(Sn1)
       neigh = adjimm{Sn1(k)};
       %for m = 1:numel(neigh1)
        %neigh = adjimm{neigh1(m)};
        idx = neigh( phi(neigh) == -3 );
        phi( idx ) = phi( idx ) - 1;
        Sn2 = [Sn2 idx];
       %end
    end
    
    Lp1 = [ Lp1 Sp1 ];
    label( Sp1 ) = 1;
    for k = 1:numel(Sp1)
       neigh = adjimm{Sp1(k)};
       %for m = 1:numel(neigh1)
        %neigh = adjimm{neigh1(m)};
        idx = neigh( phi(neigh) == 3 );
        phi( idx ) = phi( idx ) + 1;
        Sp2 = [Sp2 idx];
       %end
    end
    
    Ln2 = [Ln2 Sn2];
    label( Sn2 ) = -2;
    label( Sp2 ) = 2;
    Lp2 = [Lp2 Sp2];

    Lp2 = union(Lp2,Lp2);
    Ln2 = union(Ln2,Ln2);
    Lp1 = union(Lp1,Lp1);
    Ln1 = union(Ln1,Ln1);
    Lz  = union(Lz,Lz);
    
    
    DrawSets( Lz, Lp1, Ln1,Lp2,Ln2,nf,nv );
    pause(.25); drawnow;
    breakhere = 1;
    
   

end