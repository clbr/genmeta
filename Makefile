.PHONY: all clean

name = genmeta

FLTKCONFIG = $(shell which fltk-config13 || which fltk-config)

CXXFLAGS += -Wall -Wextra -g
LDFLAGS += -Wl,-gc-sections -lpng

CXXFLAGS += $(shell $(FLTKCONFIG) --cxxflags | sed 's@-I@-isystem@g')
LDFLAGS += $(shell $(FLTKCONFIG) --ldflags --use-images)

src = $(wildcard *.cpp)
obj = $(src:.cpp=.o)

all: $(name)

$(name): $(obj)
	$(CXX) -o $(name) $(obj) $(CXXFLAGS) $(LDFLAGS)

$(obj): $(wildcard *.h)

clean:
	rm -f $(name) *.o
