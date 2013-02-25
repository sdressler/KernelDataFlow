include Makefile.in

TARGET = KernelDataFlow.so

ROOT = $(realpath .)/
SRCDIR = src/

SUBDIRS = 	src \
			src/BlockProcessor src/MicroGraph

BUILDDIRS = $(SUBDIRS:%=build-%)
CLEANDIRS = $(SUBDIRS:%=clean-%)

OBJ = $(wildcard build/*.o)

all: $(BUILDDIRS)

$(SUBDIRS): $(BUILDDIRS)

$(BUILDDIRS):
	mkdir -p graphs
	@$(MAKE) ROOT=$(ROOT) -C $(@:build-%=%)

build-src: 	build-src/BlockProcessor build-src/MicroGraph

clean: $(CLEANDIRS)
	rm -f $(TARGET)

$(CLEANDIRS):
	$(MAKE) ROOT=$(ROOT) -C $(@:clean-%=%) clean
	rm -rf build lib graph*

.PHONY: subdirs $(SUBDIRS)
.PHONY: subdirs $(BUILDDIRS)
.PHONY: subdirs $(CLEANDIRS)
.PHONY: all clean
