

setup:
	mkdir -p build
	cd build; \
	cmake .. -DCMAKE_BUILD_TYPE=Release
	cmake --build ./build

run:
	cmake --build ./build
	./build/bin/Lumo ./scene.toml
