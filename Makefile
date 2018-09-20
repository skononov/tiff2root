CXXFLAGS += $(shell root-config --cflags) -g
LDFLAGS += $(shell root-config --ldflags)
LIBS:=-ltiff $(shell root-config --libs) 

EXECS:=tiff2root test_tiff

CLEAN:=$(EXECS) $(addsuffix .o,$(EXECS))

all: $(EXECS)

test_tiff: test_tiff.o
	gcc $^ -ltiff -o $@

tiff2root: tiff2root.o
	g++ $(LDFLAGS) $^ $(LIBS) -o $@

clean:
	rm -f $(CLEAN)
