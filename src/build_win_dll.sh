#cl /Fosimple.obj /c simple.c /nologo -EHsc -DNDEBUG /MD /I. /I..\godot_headers
#link /nologo /dll /out:..\simple\bin\libsimple.dll /implib:..\simple\bin\libsimple.lib simple.obj

x86_64-w64-mingw32-gcc-win32 -std=c11 -fPIC -c -I../godot_headers ./Skyliners/game_board.c ./Skyliners/list.c \
./Skyliners/stack.c ./Skyliners/g_main.c simple.c && mv game_board.o list.o stack.o g_main.o \
simple.o wout/
x86_64-w64-mingw32-gcc-win32 -shared wout/*.o -o ../simple/bin/libsimple.dll
