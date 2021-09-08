# Skyliners
Adaptation of an Algorithms and Data Structures' course project, made in Godot, with C.

## Project Structure:

- `godot_headers` - contains header files that enables the connection with the engine;
- `simple` - contains the game GUI source code and assets made in Godot. Note: to run the project you
need the engine executable (availlable [here](https://godotengine.org/download)), and the respectives
dynamic library files. See [compiling](#Compiling) below;
  - `bin` - the C-source of this project must be compiled as a dylib (`.dll` / `.so`) and saved into
this folder;
  - `assets` - contains data, audio, image textures, etc., used in the project. All thirds resources
are listed in `LICENSE.md` in their respectives folders;
  - `src` - contains the source code of the game GUI (godot-scripts and scenes);
- `src` - contains the C-source code of the dylib;
  - The `simple.c` file is responsible for doing the connection with the C-source and Godot using the
 GdNative API (accessible via `godot_headers` files). Look up [gdnative docs](https://docs.godotengine.org/en/stable/tutorials/plugins/gdnative/gdnative-c-example.html)
for more information;
  - The folder `Skyliners` contains a modified version of the original source code (availlable [here](https://github.com/GersonFeDutra/C-exercises/tree/master/Algoritmos%20%26%20Estruturas%20de%20Dados%20I/Projetos/Skyliners/Skyliners)).
The most noticeble change is that `main.c` was changed to `g_main.c` so it could be used as a dylib;
  - Inside the `src` folder some shell scripts were also included; they have some commands used to
compille the libraries.

## Compiling

To build the project you need the Godot Editor executable. More info in [Godot Docs](https://docs.godotengine.org/en/stable/getting_started/workflow/export/index.html).
The C-source must be compiled as a dynamic libray (`.dll` on windows, or `.so` on linux) and moved to
`simple/bin` so Godot can read them. The game itself can be runned and compiled in the engine editor.

Good Game!
