all: shineshockd.o document.o utf8decoder.o ttgsubtable.o
	g++ -o shineshockd shineshockd.o document.o utf8decoder.o ttgsubtable.o `pkg-config --libs opencv freetype2`

shineshockd.o: shineshockd.cpp
	g++ -c shineshockd.cpp `pkg-config --cflags freetype2`

document.o: document.cpp
	g++ -c document.cpp `pkg-config --cflags opencv freetype2`

utf8decoder.o: utf8decoder.cpp
	g++ -c utf8decoder.cpp 

ttgsubtable.o: ttgsubtable.cpp
	g++ -c ttgsubtable.cpp `pkg-config --cflags freetype2`

clean:
	rm -f *.png
	rm -f *.o
	rm -f shineshockd
	rm -f a.out