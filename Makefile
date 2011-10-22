all: shineshockd.o document.o
	g++ shinefont.cpp `pkg-config --cflags freetype2 --libs freetype2`
	g++ -o shineshockd shineshockd.o document.o `pkg-config --cflags opencv --libs opencv`

shineshockd.o:
	g++ -c shineshockd.cpp

document.o:
	g++ -c document.cpp

clean:
	rm -f *.png
	rm -f *.o
	rm -f shineshockd
	rm -f a.out