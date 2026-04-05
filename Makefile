OBJDIR = obj
BINDIR = bin
IMGDIR = img
SRCDIR = source
INCDIR = include
SCRDIR = scripts

LIBS = -lraylib -lGL -lm -ldl -lrt -lX11
FLAGS = -O3 -I$(INCDIR) -fopenmp -ggdb3 -std=c++17 \
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
    -fcheck-new -fsized-deallocation -fstack-protector \
	-fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer \
	-Wstack-usage=8192 -pie -fPIE -Werror=vla \
	-fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

FILES = $(OBJDIR)/main.o $(OBJDIR)/calc.o

EXECUTABLE_FILE = mandelbrot.out

build: $(FILES)
	mkdir -p $(BINDIR)
	@g++ $(FLAGS) $(FILES) -o $(BINDIR)/$(EXECUTABLE_FILE) $(LIBS)

run: build
	@./$(BINDIR)/$(EXECUTABLE_FILE)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	@g++ -c $< $(FLAGS) -o $@ $(LIBS)

clean: 
	@rm -rf $(BINDIR)
	@rm -rf $(OBJDIR)