LDFLAGS = `root-config --glibs`
CFLAGS = `root-config --cflags`
plotter = ${CROMBIEPATH}/bin/crombieplotter

all: $(plotter)

$(plotter): ${CROMBIEPATH}/src/include/* ${CROMBIEPATH}/src/plotter.cpp
	g++ $(LDFLAGS) $(CFLAGS) -I${CROMBIEPATH}/src/include ${CROMBIEPATH}/src/plotter.cpp -o $(plotter)
