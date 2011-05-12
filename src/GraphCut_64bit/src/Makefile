CFLAGS = -std=c99 -fPIC -Wall -Werror
CXXFLAGS = -fPIC
MAKE=make --no-print-directory
DFLAGS = $(shell test -f .DEBUG && echo -g || echo -O)
CFLAGS += $(DFLAGS)
CXXFLAGS += $(DFLAGS)
LDLIBS = -lstdc++ # so can compile with gcc

INC = -I/opt/matlab/extern/include/
MEXT = mexmaci #- Mac
# MEXT = mexglx #- Linux32
# MEXT = mexa64 #- Linux64


MEX = mex -cxx
CFLAGS += $(INC)
CXXFLAGS += $(INC)

BIN=../graphchop.$(MEXT)
SRC=graphchop main_mex graph maxflow GCoptimization LinkedBlockList

all: .depend $(BIN)

$(BIN): $(patsubst %, %.o, $(SRC))

../%.$(MEXT): %.o
	$(MEX) -o $@ $^ $(LDLIBS)

.depend: *.c *.cc *.h
	$(CXX) -MM $(INC) $(filter-out %.h, $^) >$@

clean:
	rm -f $(patsubst %, %.o, $(SRC)) $(BIN) main_*.o

debug:
	@test -f .DEBUG && (rm .DEBUG; echo off) || (touch .DEBUG; echo on)
	@$(MAKE) clean

-include .depend
