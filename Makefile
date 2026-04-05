OBJDIR = obj
BINDIR = bin
IMGDIR = img
SRCDIR = source
INCDIR = include
SCRDIR = scripts

LIBS = -lraylib -lGL -lm -ldl -lrt -lX11
DEF_FLAGS = -I$(INCDIR) -fopenmp -ggdb3 -std=c++17 \
	-Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat \
	-Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts \
	-Wconditionally-supported -Wconversion -Wctor-dtor-privacy \
	-Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security \
	-Wformat-signedness -Wformat=2 -Winline -Wlogical-op \
	-Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked \
	-Wpointer-arith -Winit-self -Wredundant-decls -Wshadow \
	-Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 \
	-Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types \
	-Wsuggest-override -Wswitch-default -Wundef \
	-Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros \
	-Wno-literal-suffix -Wno-missing-field-initializers \
	-Wno-narrowing -Wno-old-style-cast -Wno-varargs \
    -fcheck-new -fsized-deallocation \
	-fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer \
	-Wstack-usage=8192 -pie -fPIE -Werror=vla
AVX_FLAGS += -ffast-math -fopt-info-vec-optimized -march=native -mprefer-vector-width=512


FILES = $(SRCDIR)/main.cpp $(SRCDIR)/calc.cpp

EXECUTABLE_FILE = mandelbrot.out

$(BINDIR):
	@mkdir -p $(BINDIR)

build: $(FILES) $(BINDIR)
	@g++ $(DEF_FLAGS) $(FILES) -o $(BINDIR)/$(EXECUTABLE_FILE) $(LIBS)

run: 
	@./$(BINDIR)/$(EXECUTABLE_FILE)

no_opti_O0: $(FILES) $(BINDIR)
	@g++ -O0 $(DEF_FLAGS) $(FILES) -o $(BINDIR)/$(EXECUTABLE_FILE) $(LIBS)
	@$(MAKE) run

no_opti_O3: $(FILES) $(BINDIR)
	@g++ -O3 $(DEF_FLAGS) $(FILES) -o $(BINDIR)/$(EXECUTABLE_FILE) $(LIBS)
	@$(MAKE) run

package_opti_O0: $(FILES) $(BINDIR)
	@g++ -DPACKAGE_OPTIMIZATION -O0 $(DEF_FLAGS) $(FILES) -o $(BINDIR)/$(EXECUTABLE_FILE) $(LIBS)
	@$(MAKE) run

package_opti_O3: $(FILES) $(BINDIR)
	@g++ -DPACKAGE_OPTIMIZATION -O3 $(DEF_FLAGS) $(FILES) -o $(BINDIR)/$(EXECUTABLE_FILE) $(LIBS)
	@$(MAKE) run

avx512_opti_O0: $(FILES) $(BINDIR)
	@g++ -DAVX512_OPTIMIZATION -O0 $(AVX_FLAGS) $(DEF_FLAGS) $(FILES) -o $(BINDIR)/$(EXECUTABLE_FILE) $(LIBS)
	@$(MAKE) run

avx512_opti_O3: $(FILES) $(BINDIR)
	@g++ -DAVX512_OPTIMIZATION -O3 $(AVX_FLAGS) $(DEF_FLAGS) $(FILES) -o $(BINDIR)/$(EXECUTABLE_FILE) $(LIBS)
	@$(MAKE) run

avx512_opti_O3_w_multithread: $(FILES) $(BINDIR)
	@g++ -DAVX512_OPTIMIZATION -DMULTITHREAD_OPTIMIZATION -O3 $(AVX_FLAGS) $(DEF_FLAGS) \
		$(FILES) -o $(BINDIR)/$(EXECUTABLE_FILE) $(LIBS)
	@$(MAKE) run

clean: 
	@rm -rf $(BINDIR)
	@rm -rf $(OBJDIR)