build-debug:
	clang -o output/PhysSim -lm -lraylib src/main.c -Werror -fsanitize=address

build-release:
	
run:
	./output/PhysSim
clear:
	rm output/PhysSim

build-wasm:
	emcc -o output/wasmHost.html -lm -lraylib -I/usr/include src/main.c
