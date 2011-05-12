function compile
  mex -O ../src/graphchop.cc ../src/GCoptimization.cpp ../src/LinkedBlockList.cpp ../src/graph.cpp ../src/main_mex.cpp ../src/maxflow.cpp  -I../include/ 
