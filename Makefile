.DEFAULT_GOAL:= gen_mesh

.PHONY: clean

all: clean gen_mesh compdb tests debug

compdb:
	cmake -G Ninja -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=True
	cp ./build/compile_commands.json .

build_files:
	cmake -G Ninja -B build

build_files_debug:
	cmake -G Ninja -B build_debug -DCMAKE_BUILD_TYPE=Debug -DVERB_LEVEL=VERB_DBG

build_files_test:
	cmake -G Ninja -B tests/build -DCMAKE_BUILD_TYPE=Debug

gen_mesh: build_files
	ninja -C build gen_mesh

debug: build_files_debug
	ninja -C build_debug gen_mesh

tests: build_files_test
	ninja -C tests/build test_qtree

clean:
	./clean.sh
