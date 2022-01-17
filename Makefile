.DEFAULT_GOAL:= gen_mesh

all: clean gen_mesh compdb tests debug

compdb:
	cmake -G Ninja -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=True
	cp ./build/compile_commands.json .

build_files:
	cmake -G Ninja -B build

gen_mesh: build_files
	ninja -C build gen_mesh

debug:
	cmake -G Ninja -B build_debug -DCMAKE_BUILD_TYPE=Debug
	ninja -C build_debug gen_mesh

tests: build_files
	ninja -C build test_qtree

clean:
	ninja -C build -t clean
	ninja -C build_debug -t clean
