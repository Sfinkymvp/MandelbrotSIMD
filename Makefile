MAKEFLAGS += --no-print-directory

OBJDIR = obj
BINDIR = bin
IMGDIR = img
SRCDIR = source
INCDIR = include
SCRDIR = scripts
RESDIR = results

LIBS = -lraylib -lGL -lm -ldl -lrt -lX11
CC = gcc

DEF_FLAGS = $(AVX_FLAGS) -I$(INCDIR) -pie -fPIE

DEF_FILES = $(SRCDIR)/main.c $(SRCDIR)/fractal.c

AVX_FLAGS = -ffast-math -march=native -mprefer-vector-width=512
NO_OPTI_FILES = $(SRCDIR)/calc_no_opti.c
PACKAGE_OPTI_FILES = $(SRCDIR)/calc_package.c
AVX512_OPTI_FILES = $(SRCDIR)/calc_avx512.c

EXECUTABLE_FILE = mandelbrot.out

EXTRA_DEFS = 
CURRENT_FLAGS = $(DEF_FLAGS)
CURRENT_FILES = $(DEF_FILES)

ITERS ?= 100

ifeq ($(BENCHMARK), YES)
	EXTRA_DEFS += -DBENCHMARK -DBENCH_ITERS=$(ITERS)
endif 

ifeq ($(COLOR), YES)
	EXTRA_DEFS += -DCOLOR_DRAWING
endif

ifeq ($(MULTITHREAD), YES)
	EXTRA_DEFS += -DMULTITHREAD_OPTIMIZATION -fopenmp
endif

ifdef OPTI
	OPT_LEVEL = -$(OPTI)
endif


.PHONY: all run no_opti package_opti avx512_opti .build

all: CURRENT_FILES += $(NO_OPTI_FILES)
all: build

.build: $(CURRENT_FILES) $(BINDIR)
	@$(CC) $(OPT_LEVEL) $(EXTRA_DEFS) $(CURRENT_FLAGS) $(CURRENT_FILES) -o $(BINDIR)/$(EXECUTABLE_FILE) $(LIBS)

run: 
	@./$(BINDIR)/$(EXECUTABLE_FILE)

no_opti: CURRENT_FILES += $(NO_OPTI_FILES)
ifeq ($(BENCHMARK), YES)
no_opti: CURRENT_FLAGS += -DCONFIG='"No optimization $(OPTI)"'
endif
no_opti: .build

package_opti: CURRENT_FILES += $(PACKAGE_OPTI_FILES)
ifeq ($(BENCHMARK), YES)
package_opti: CURRENT_FLAGS += -DCONFIG='"Package optimization $(OPTI)"'
endif
package_opti: EXTRA_DEFS += -DPACKAGE_OPTIMIZATION
package_opti: .build

avx512_opti: CURRENT_FILES += $(AVX512_OPTI_FILES)
ifeq ($(BENCHMARK), YES)
avx512_opti: CURRENT_FLAGS += -DCONFIG='"AVX512 optimization $(OPTI)"' $(AVX_FLAGS)
endif
avx512_opti: EXTRA_DEFS += -DAVX512_OPTIMIZATION
avx512_opti: .build

clean_all: clean_programs clean_results

clean_programs: 
	@rm -rf $(BINDIR)
	@rm -rf $(OBJDIR)

clean_results:
	@rm -rf $(RESDIR)

$(BINDIR):
	@mkdir -p $(BINDIR)