P_NAME	= interface

CFLAGS  = -Wall -O2 -I. `sdl-config --cflags`
LDFLAGS = `sdl-config --libs` -lSDL_image -lSDL_gfx -lSDL_ttf -lSDL_mixer
OBJECTS = obj/main.o obj/entry_list.o obj/font.o

#main game
$(P_NAME)/$(P_NAME): obj $(OBJECTS) $(P_NAME)
	$(CC) $(CFLAGS) $(OBJECTS) $(LDFLAGS) -o $(P_NAME)/$(P_NAME)

#objects
obj/%.o: %.c
	$(CC) $(CFLAGS) -c $<
	mv *.o obj

#for develop and test run
run: $(P_NAME)/$(P_NAME)
	cd $(P_NAME) && ./$(P_NAME)

$(P_NAME):
	mkdir $(P_NAME)
	cp -n themes/Simple/* $(P_NAME)

obj:
	mkdir obj

clean:
	- rm -rf $(P_NAME) obj
