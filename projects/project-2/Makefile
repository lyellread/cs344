all:
	@echo "Choose"
g: readly.game.c
	gcc -o readly.adventure readly.adventure.c -lpthread
r: readly.rooms.c
	gcc -o readly.rooms readly.rooms.c -lpthread
game: readly.game.c
	make g
rooms: readly.rooms.c
	make r
