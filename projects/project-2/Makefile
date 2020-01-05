all:
	make clean && make rooms_base && make game_base
game: readly.adventure.c
	make clean && make game_base
rooms: readly.buildrooms.c
	make clean && make rooms_base
clean:
	rm -rf ./readly.rooms.??*
	rm ./readly.buildrooms
	rm ./readly.adventure
#abbreviations cause im lazy :)

a:
	make all
g: readly.adventure.c
	make game
r: readly.buildrooms.c
	make rooms
c:
	make clean


game_base: readly.adventure.c
	gcc -o readly.adventure readly.adventure.c -lpthread
rooms_base: readly.buildrooms.c
	gcc -o readly.buildrooms readly.buildrooms.c
