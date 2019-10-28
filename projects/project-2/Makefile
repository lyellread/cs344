all:
	@echo "Choose"
g: readly.game.c
	make clean && gcc -o readly.adventure readly.adventure.c -lpthread
r: readly.rooms.c
	make clean && gcc -o readly.rooms readly.rooms.c -lpthread
c:
	rm -rf ./readly.rooms.??*

#abbreviations cause im lazy :)

game: readly.game.c
	make g
rooms: readly.rooms.c
	make r
clean:
	make c
