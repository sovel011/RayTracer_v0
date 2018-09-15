all: image.o EasyBMP.o scene.o pixel.o
	g++ main.cpp -o main image.o EasyBMP.o scene.o pixel.o 
image.o: image.cpp image.h
	g++ image.cpp -c 
EasyBMP.o: EasyBMP.cpp EasyBMP.h
	g++ EasyBMP.cpp -c 
scene.o: scene.cpp scene.h
	g++ scene.cpp -c 
pixel.o: pixel.cpp pixel.h
	g++ pixel.cpp -c 
clean: 
	-rm image.o EasyBMP.o scene.o pixel.o main 