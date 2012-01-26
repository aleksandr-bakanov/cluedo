OBJECTS = obj/CluedoMain.o obj/Player.o obj/Room.o obj/RoomManager.o

all:
	mkdir -p obj
	make cluedo

cluedo: $(OBJECTS)
	g++ -ggdb $(OBJECTS) -lpthread -o cluedo

obj/CluedoMain.o: src/CluedoMain.cpp src/include/CluedoMain.hpp
	g++ -c -ggdb src/CluedoMain.cpp -o obj/CluedoMain.o

obj/Player.o: src/Player.cpp src/include/Player.hpp
	g++ -c -ggdb src/Player.cpp -o obj/Player.o

obj/Room.o: src/Room.cpp src/include/Room.hpp
	g++ -c -ggdb src/Room.cpp -o obj/Room.o

obj/RoomManager.o: src/RoomManager.cpp src/include/RoomManager.hpp
	g++ -c -ggdb src/RoomManager.cpp -o obj/RoomManager.o

clean:
	rm -r obj/ cluedo
