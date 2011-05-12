function DrawSets( Lz, Lp1, Ln1,Lp2,Ln2,nf,nv )
   Cflat =0*( nv(:,1) );
   nv_ = nv;
   nv  = 0.9*nv_;
   trisurf(nf,nv(:,1),nv(:,2),nv(:,3),Cflat); hold on;
   hold on;
   nv = nv_;
   plot3(nv(Ln2,1),nv(Ln2,2),nv(Ln2,3),'LineStyle','none','Marker','*','MarkerEdgeColor','b','MarkerSize',14); 
   plot3(nv(Ln1,1),nv(Ln1,2),nv(Ln1,3),'LineStyle','none','Marker','*','MarkerEdgeColor','y','MarkerSize',14); 
   plot3(nv(Lp2,1),nv(Lp2,2),nv(Lp2,3),'LineStyle','none','Marker','*','MarkerEdgeColor','r','MarkerSize',14); 
   plot3(nv(Lp1,1),nv(Lp1,2),nv(Lp1,3),'LineStyle','none','Marker','*','MarkerEdgeColor','m','MarkerSize',14);
   plot3(nv(Lz,1),nv(Lz,2),nv(Lz,3),'LineStyle','none','Marker','*','MarkerEdgeColor','c','MarkerSize',14); hold off;

end