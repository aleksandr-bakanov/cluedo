INC = src/include

OBJECTS = obj/CluedoMain.o obj/Player.o obj/Room.o \
		  obj/RoomManager.o obj/BytesConverter.o

all:
	mkdir -p obj
	make cluedo

cluedo: $(OBJECTS)
	g++ -ggdb -std=c++0x obj/* -lpthread -o cluedo

obj/CluedoMain.o: src/CluedoMain.cpp $(INC)/CluedoMain.hpp
	g++ -c -ggdb -std=c++0x src/CluedoMain.cpp -o obj/CluedoMain.o

obj/Player.o: src/Player.cpp $(INC)/Player.hpp
	g++ -c -ggdb -std=c++0x src/Player.cpp -o obj/Player.o

obj/Room.o: src/Room.cpp $(INC)/Room.hpp
	g++ -c -ggdb -std=c++0x src/Room.cpp -o obj/Room.o

obj/RoomManager.o: src/RoomManager.cpp $(INC)/RoomManager.hpp
	g++ -c -ggdb -std=c++0x src/RoomManager.cpp -o obj/RoomManager.o

obj/BytesConverter.o: src/BytesConverter.cpp $(INC)/BytesConverter.hpp
	g++ -c -ggdb -std=c++0x src/BytesConverter.cpp -o obj/BytesConverter.o

clean:
	rm -r obj/ cluedo
