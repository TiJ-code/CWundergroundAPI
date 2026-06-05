all:
	rm -rf build
	cmake -S . -B build
	cmake --build build
	ctest --test-dir build
