.PHONY: all release debug clean install clean-debug clean-release

CMAKE_PREFIX_PATH=~/Qt56/5.6/gcc_64/lib/cmake/

all: release
release:
	@mkdir -p Release
	cd Release && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH} -DCMAKE_INSTALL_PREFIX=${PREFIX} .. && make

debug:
	@mkdir -p Debug
	@cd Debug && cmake -DCMAKE_BUILD_TYPE=Debug .. && make

install:
	@cd Release && make install

clean: clean-debug clean-release
clean-release:
	@if [ -f Release/Makefile ]; then cd Release && make clean; fi
	@if [ -d Release ]; then rm -rf Release; fi

clean-debug:
	@if [ -f Debug/Makefile ]; then cd Debug && make clean; fi
	@if [ -d Debug ]; then rm -rf Debug; fi
