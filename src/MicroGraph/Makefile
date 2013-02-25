include ../../Makefile.in

TARGET = MicroGraph

BUILDDIR = $(ROOT)build/$(TARGET)/
SRCDIR = $(realpath .)/
LIBDIR = $(ROOT)lib/

TARGETLIB = $(LIBDIR)lib$(TARGET).a

SRC = $(wildcard *.cpp)
OBJ = $(addprefix $(BUILDDIR), $(SRC:%.cpp=%.o))

all: $(TARGETLIB)

$(TARGETLIB): $(OBJ)
	@echo "  AR   $(notdir $(OBJ)) -> $(TARGETLIB)"
	@$(AR) cr $(TARGETLIB) $(OBJ)

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
