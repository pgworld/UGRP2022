#include <iostream>
#include <vector>
#include <set>
#include <bitset>
#include <map>
#include <cmath>
#include <algorithm>
#include <random>
#include "../util/compress.h"
#include "../util/lz4.h"
#include "../util/xxhash.h"
#include "../util/xdelta3/xdelta3.h"
#define INF 987654321
using namespace std;

int main(int argc, char* argv[]) {
	if (argc != 2) {
		cerr << "usage: ./random_inf [input_file]\n";
		exit(0);
	}

	DATA_IO f(argv[1]);
	f.read_file();

	map<XXH64_hash_t, int> dedup;

	unsigned long long total = 0;
	f.time_check_start();
	for (int i = 0; i < f.N; ++i) {
		RECIPE r;

		XXH64_hash_t h = XXH64(f.trace[i], BLOCK_SIZE, 0);

		if (dedup.count(h)) { // deduplication
			set_ref(r, dedup[h]);
			set_flag(r, 0b10);
			f.recipe_insert(r);
			continue;
		}

		int comp_self = LZ4_compress_default(f.trace[i], compressed, BLOCK_SIZE, 2 * BLOCK_SIZE);
		int dcomp = INF, dcomp_ref;

		if (i != 0) {
			dcomp_ref = i - 1;
			dcomp = xdelta3_compress(f.trace[i], BLOCK_SIZE, f.trace[dcomp_ref], BLOCK_SIZE, delta_compressed, 1);
		}
		set_offset(r, total);

		if (min(comp_self, BLOCK_SIZE) > dcomp) { // delta compress
			set_size(r, (unsigned long)(dcomp - 1));
			set_ref(r, dcomp_ref);
			set_flag(r, 0b11);
			f.write_file(delta_compressed, dcomp);
			total += dcomp;
		}
		else {
			if (comp_self < BLOCK_SIZE) { // self compress
				set_size(r, (unsigned long)(comp_self - 1));
				set_flag(r, 0b01);
				f.write_file(compressed, comp_self);
				total += comp_self;
			}
			else { // no compress
				set_flag(r, 0b00);
				f.write_file(f.trace[i], BLOCK_SIZE);
				total += BLOCK_SIZE;
			}
		}

		dedup[h] = i;

		f.recipe_insert(r);
	}
	f.recipe_write();
	cout << "Total time: " << f.time_check_end() << "us\n";

	printf("Recent %s\n", argv[1]);
	printf("Final size: %llu, (%.2lf%%)\n", total, (double)total * 100 / f.N / BLOCK_SIZE);
}
