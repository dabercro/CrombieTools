LDFLAGS = `root-config --glibs`
CFLAGS = `root-config --cflags`
PRE = ${CROMBIEPATH}/bin/crombie
SOURCES := $(wildcard *.cpp)
OUTPUTS := $(foreach src, $(SOURCES:.cpp=), $(PRE)$(src))

all: $(OUTPUTS)

.SECONDEXPANSION:
$(PRE)% : $$(shell crombie depends ${CROMBIEPATH}/src/$$*.cpp)
	g++ $(LDFLAGS) $(CFLAGS) -I${CROMBIEPATH}/src/include $(filter %.cpp,$^) -o $@
