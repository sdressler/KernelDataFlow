include ../Makefile.in

TARGET = KernelDataFlow

LIBS = -lMicroGraph -lBlockProcessor

BUILDDIR = $(ROOT)build/
SRCDIR = $(realpath .)/
LIBDIR = $(ROOT)lib/

TARGETLIB = $(ROOT)$(TARGET).so

SRC = $(wildcard *.cpp)
OBJ = $(addprefix $(BUILDDIR), $(SRC:%.cpp=%.o))

all: $(TARGETLIB)

$(TARGETLIB): $(OBJ) ../lib/libMicroGraph.a ../lib/libBlockProcessor.a
	@echo "  LD   $(notdir $(OBJ)) $(LIBS) -> $(TARGETLIB)"
	@$(CXX) -L$(LIBDIR) -o $@ $(OBJ) -Wl,--whole-archive $(LIBS) -Wl,--no-whole-archive $(LDFLAGS)

clean:
	rm -f $(OBJ) $(TARGETLIB)

$(OBJ): | $(BUILDDIR) $(LIBDIR)

$(LIBDIR):
	mkdir -p $(LIBDIR)

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

## RULE FOR BUILDING OBJS ##
$(BUILDDIR)%.o : $(SRCDIR)%.cpp
	@echo "  CXX  $(notdir $<) -> $(notdir $@)"
	@$(CXX) $(CXXFLAGS) -c $< -o $@
