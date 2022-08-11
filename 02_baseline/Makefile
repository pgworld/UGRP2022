CFLAGS = -g

make:
	g++ $(CFLAGS) -o lsh_inf_comp ../util/lz4.c ../util/xdelta3/xdelta3.c ../util/xxhash.c lsh_inf_comp.cpp
	g++ $(CFLAGS) -o random_inf_comp ../util/lz4.c ../util/xdelta3/xdelta3.c ../util/xxhash.c random_inf_comp.cpp
	g++ $(CFLAGS) -o recent_inf_comp ../util/lz4.c ../util/xdelta3/xdelta3.c ../util/xxhash.c recent_inf_comp.cpp
	g++ $(CFLAGS) -o sota_inf_comp ../util/lz4.c ../util/xdelta3/xdelta3.c ../util/xxhash.c sota_inf_comp.cpp
