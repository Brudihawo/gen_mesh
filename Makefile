gen_mesh:
	ninja -C build

gen_build_files:
	cmake -G Ninja -B build

compdb:
	cmake -G Ninja -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=True
	cp ./build/compile_commands.json .
