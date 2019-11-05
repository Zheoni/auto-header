CXX = clang++
CXXFLAGS = -std=c++17 -Wall -O3

VPATH = . ./parsegar/

OBJS = parsegar.o template-support.o
EXES = auto-header

all: $(EXES)

auto-header: $(OBJS)

$(OBJS): 

clean:
	$(RM) $(OBJS) $(EXES)

.PHONY: clean all