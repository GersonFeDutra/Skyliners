gcc -std=c11 -fPIC -c -I../godot_headers simple.c -o simple.o
gcc -rdynamic -shared simple.o -o ../simple/bin/libsimple.so
