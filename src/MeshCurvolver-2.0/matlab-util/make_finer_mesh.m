[xyz] = csvread('../data/tibia-orig-verts.csv');
faces = csvread('../data/tibia-faces.csv');
trisurf(faces, xyz(:,1),xyz(:,2),xyz(:,3) )

[vv ff] = subdivide_tri( xyz, faces );
figure(1);
trisurf(ff, vv(:,1),vv(:,2),vv(:,3) ); axis equal;

figure(2);
[nf nv] = reducepatch(ff,vv,0.20);
p=trisurf(nf, nv(:,1),nv(:,2),nv(:,3) ); axis equal;
set(p, 'FaceColor','interp','EdgeColor','none');
camlight; lighting phong


