LDFLAGS = `root-config --glibs`
CFLAGS = `root-config --cflags` -std=c++14
plotter = ${CROMBIEPATH}/bin/crombieplotter
eventdump = ${CROMBIEPATH}/bin/crombieeventdump
cut = ${CROMBIEPATH}/bin/crombiecut
cutflow = ${CROMBIEPATH}/bin/crombiecutflow
json = ${CROMBIEPATH}/bin/crombiejson
test = ${CROMBIEPATH}/bin/testing

all: $(plotter) $(eventdump) $(cut) $(json) $(test) $(cutflow)

$(plotter): $(shell crombie depends ${CROMBIEPATH}/src/plotter.cpp)
	g++ $(LDFLAGS) $(CFLAGS) -I${CROMBIEPATH}/src/include ${CROMBIEPATH}/src/plotter.cpp -o $(plotter)

$(eventdump): $(shell crombie depends ${CROMBIEPATH}/src/eventdump.cpp)
	g++ $(LDFLAGS) $(CFLAGS) -I${CROMBIEPATH}/src/include ${CROMBIEPATH}/src/eventdump.cpp -o $(eventdump)

$(cut): $(shell crombie depends ${CROMBIEPATH}/src/cut.cpp)
	g++ $(LDFLAGS) $(CFLAGS) -I${CROMBIEPATH}/src/include ${CROMBIEPATH}/src/cut.cpp -o $(cut)

$(cutflow): $(shell crombie depends ${CROMBIEPATH}/src/cutflow.cpp)
	g++ $(LDFLAGS) $(CFLAGS) -I${CROMBIEPATH}/src/include ${CROMBIEPATH}/src/cutflow.cpp -o $(cutflow)

$(json): $(shell crombie depends ${CROMBIEPATH}/src/json.cpp)
	g++ $(LDFLAGS) $(CFLAGS) -I${CROMBIEPATH}/src/include ${CROMBIEPATH}/src/json.cpp -o $(json)

$(test): $(shell crombie depends ${CROMBIEPATH}/src/test.cpp)
	g++ $(LDFLAGS) $(CFLAGS) -I${CROMBIEPATH}/src/include ${CROMBIEPATH}/src/test.cpp -o $(test)
