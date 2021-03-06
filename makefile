CPPC = clang++
CPPC_INCLUDE_FLAGS = -I ./Src/
CPPC_FLAGS = $(CPPC_INCLUDE_FLAGS) -stdlib=libc++ \
			 -std=c++17 -Werror -Wall -Wextra -O2 -DAGZ_USE_SSE2

CPP_FILES = $(shell find ./Src/ -name "*.cpp")
CPP_FILES = $(shell find ./Test/ -name "*.cpp")
OPP_FILES = $(patsubst %.cpp, %.o, $(CPP_FILES))
DPP_FILES = $(patsubst %.cpp, %.d, $(CPP_FILES))

TARGET = ./testProg

$(TARGET) : $(OPP_FILES)
	$(CPPC) $(CPPC_FLAGS) $^ -lc++fs -o $@

$(OPP_FILES) : %.o : %.cpp
	$(CPPC) $(CPPC_FLAGS) -c $< -o $@

$(DPP_FILES) : %.d : %.cpp
	@set -e; \
	rm -f $@; \
	$(CPPC) -MM $(CPPC_FLAGS) $< > $@.$$$$.dtmp; \
	sed 's,\(.*\)\.o\:,$*\.o $*\.d\:,g' < $@.$$$$.dtmp > $@; \
	rm -f $@.$$$$.dtmp

-include $(DPP_FILES)

.PHONY : test
test :
	make $(TARGET)
	$(TARGET)

.PHONY : clean
clean :
	rm -f $(TARGET)
	rm -f $(OPP_FILES) $(DPP_FILES)
	rm -f $(shell find ./Src/ -name "*.dtmp")
	rm -f $(shell find ./Test/ -name "*.dtmp")
