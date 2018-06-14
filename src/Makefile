LDFLAGS = `root-config --glibs`
CFLAGS = `root-config --cflags` -std=c++14
plotter = ${CROMBIEPATH}/bin/crombieplotter
eventdump = ${CROMBIEPATH}/bin/crombieeventdump
cut = ${CROMBIEPATH}/bin/crombiecut
json = ${CROMBIEPATH}/bin/crombiejson


all: $(plotter) $(eventdump) $(cut) $(json)

$(plotter): ${CROMBIEPATH}/src/include/* ${CROMBIEPATH}/src/plotter.cpp
	g++ $(LDFLAGS) $(CFLAGS) -I${CROMBIEPATH}/src/include ${CROMBIEPATH}/src/plotter.cpp -o $(plotter)

$(eventdump): ${CROMBIEPATH}/src/include/* ${CROMBIEPATH}/src/eventdump.cpp
	g++ $(LDFLAGS) $(CFLAGS) -I${CROMBIEPATH}/src/include ${CROMBIEPATH}/src/eventdump.cpp -o $(eventdump)

$(cut): ${CROMBIEPATH}/src/include/Selection.h ${CROMBIEPATH}/src/cut.cpp
	g++ $(LDFLAGS) $(CFLAGS) -I${CROMBIEPATH}/src/include ${CROMBIEPATH}/src/cut.cpp -o $(cut)

$(json): ${CROMBIEPATH}/src/include/* ${CROMBIEPATH}/src/json.cpp
	g++ $(LDFLAGS) $(CFLAGS) -I${CROMBIEPATH}/src/include ${CROMBIEPATH}/src/json.cpp -o $(json)
