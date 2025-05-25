build-debug:
	clang -o output/PhysSim -lm -lraylib src/vectors.c src/main.c -Werror -fsanitize=address -std=c99

build-release:
	
run:
	./output/PhysSim
clear:
	rm output/PhysSim

build-wasm:
	emcc -o output/sim.html src/main.c -Os -Wall ./emraylib/raylib/src/libraylib.a -I. -I./emraylib/raylib/src/ -L. -L./emraylib/raylib/src/raylib.h -s USE_GLFW=3 -DPLATFORM_WEB
