all:
	g++ -o shineshockd shineshockd.cpp `pkg-config --cflags opencv --libs opencv`
clean:
	rm -f *.png
	rm -f shineshockd