all: shineshockd.o document.o
	g++ -o shineshockd shineshockd.o document.o `pkg-config --cflags opencv --libs opencv`

shineshockd.o:
	g++ -c shineshockd.cpp

document.o:
	g++ -c document.cpp

clean:
	rm -f *.png
	rm -f *.o
	rm -f shineshockd