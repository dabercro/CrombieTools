ifeq ($(origin CMSSW_BASE), undefined)

	FLAGS = -L/Applications/root_v6.12.06/lib -lCore -lImt -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lTree -lTreePlayer -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lThread -lMultiProc -lpthread -lm -ldl -pthread -std=c++17 -m64 -I/Applications/root_v6.12.06/include
	COMPILER = /usr/local/bin/g++-8

else

	FLAGS = `root-config --cflags` `root-config --libs`
	COMPILER = g++

endif

PRE = ${CROMBIEPATH}/bin/crombie
SOURCES := $(wildcard *.cpp)
OUTPUTS := $(foreach src, $(SOURCES:.cpp=), $(PRE)$(src))

all: $(OUTPUTS)

.SECONDEXPANSION:
$(PRE)% : $$(shell crombie depends ${CROMBIEPATH}/src/$$*.cpp)
	$(COMPILER) $(FLAGS) -I${CROMBIEPATH}/src/include $(filter %.cpp,$^) -o $@
