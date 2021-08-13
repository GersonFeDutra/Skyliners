gcc -std=c11 -fPIC -c -I../godot_headers ./Skyliners/game_board.c ./Skyliners/list.c \
./Skyliners/stack.c ./Skyliners/g_main.c simple.c && mv game_board.o list.o stack.o g_main.o \
simple.o out/
gcc -rdynamic -shared out/*.o -o ../simple/bin/libsimple.so
