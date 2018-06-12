LDFLAGS = `root-config --glibs`
CFLAGS = `root-config --cflags`
plotter = ${CROMBIEPATH}/bin/crombieplotter
eventdump = ${CROMBIEPATH}/bin/crombieeventdump

all: $(eventdump)

$(plotter): ${CROMBIEPATH}/src/include/* ${CROMBIEPATH}/src/plotter.cpp
	g++ $(LDFLAGS) $(CFLAGS) -I${CROMBIEPATH}/src/include ${CROMBIEPATH}/src/plotter.cpp -o $(plotter)

$(eventdump): ${CROMBIEPATH}/src/include/* ${CROMBIEPATH}/src/eventdump.cpp
	g++ $(LDFLAGS) $(CFLAGS) -I${CROMBIEPATH}/src/include ${CROMBIEPATH}/src/eventdump.cpp -o $(eventdump)
