LDFLAGS = `root-config --glibs`
CFLAGS = `root-config --cflags` -std=c++14
plotter = ${CROMBIEPATH}/bin/crombieplotter
eventdump = ${CROMBIEPATH}/bin/crombieeventdump
cut = ${CROMBIEPATH}/bin/crombiecut
json = ${CROMBIEPATH}/bin/crombiejson


all: $(plotter) $(eventdump) $(cut) $(json)

$(plotter): $(shell crombie depends ${CROMBIEPATH}/src/plotter.cpp)
	g++ $(LDFLAGS) $(CFLAGS) -I${CROMBIEPATH}/src/include ${CROMBIEPATH}/src/plotter.cpp -o $(plotter)

$(eventdump): $(shell crombie depends ${CROMBIEPATH}/src/eventdump.cpp)
	g++ $(LDFLAGS) $(CFLAGS) -I${CROMBIEPATH}/src/include ${CROMBIEPATH}/src/eventdump.cpp -o $(eventdump)

$(cut): $(shell crombie depends ${CROMBIEPATH}/src/cut.cpp)
	g++ $(LDFLAGS) $(CFLAGS) -I${CROMBIEPATH}/src/include ${CROMBIEPATH}/src/cut.cpp -o $(cut)

$(json): $(shell crombie depends ${CROMBIEPATH}/src/json.cpp)
	g++ $(LDFLAGS) $(CFLAGS) -I${CROMBIEPATH}/src/include ${CROMBIEPATH}/src/json.cpp -o $(json)
