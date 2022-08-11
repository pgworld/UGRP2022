CFLAGS = -O2

make:
	g++ $(CFLAGS) -o random_sampling random_sampling.cpp
	g++ $(CFLAGS) -pthread -o coarse ../util/xdelta3/xdelta3.c ../util/xxhash.c coarse_xdelta3.cpp
	g++ $(CFLAGS) -pthread -o fine ../util/xdelta3/xdelta3.c ../util/lz4.c fine_xdelta3.cpp
