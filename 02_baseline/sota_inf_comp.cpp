#include <iostream>
#include <vector>
#include <set>
#include <bitset>
#include <map>
#include <cmath>
#include <algorithm>
#include "lsh.h"
#include "../util/compress.h"
#include "../util/lz4.h"
#include "../util/xxhash.h"
#include "../util/xdelta3/xdelta3.h"
#define INF 987654321
using namespace std;

int main(int argc, char* argv[]) {
	if (argc != 2) {
		cerr << "usage: ./lsh_inf [input_file]\n";
		exit(0);
	}
	int W = 32;

	DATA_IO f(argv[1]);
	f.read_file();

	map<XXH64_hash_t, int> dedup;
	LSH lsh1(BLOCK_SIZE, W, 3, 45);
	LSH lsh2(BLOCK_SIZE, W, 5, 30);
	LSH lsh3(BLOCK_SIZE, W, 10, 50);
	LSH lsh4(BLOCK_SIZE, W, 20, 60);
	LSH lsh5(BLOCK_SIZE, W, 30, 60);

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

		dedup[h] = i;

		int comp_self = LZ4_compress_default(f.trace[i], compressed, BLOCK_SIZE, 2 * BLOCK_SIZE);
		int dcomp_lsh = INF, dcomp_lsh_ref = -1, ref_now;

		ref_now = lsh1.request((unsigned char*)f.trace[i]);
		if (dcomp_lsh_ref == -1) dcomp_lsh_ref = ref_now;
		ref_now = lsh2.request((unsigned char*)f.trace[i]);
		if (dcomp_lsh_ref == -1) dcomp_lsh_ref = ref_now;
		ref_now = lsh3.request((unsigned char*)f.trace[i]);
		if (dcomp_lsh_ref == -1) dcomp_lsh_ref = ref_now;
		ref_now = lsh4.request((unsigned char*)f.trace[i]);
		if (dcomp_lsh_ref == -1) dcomp_lsh_ref = ref_now;
		ref_now = lsh5.request((unsigned char*)f.trace[i]);
		if (dcomp_lsh_ref == -1) dcomp_lsh_ref = ref_now;

		if (dcomp_lsh_ref != -1) {
			dcomp_lsh = xdelta3_compress(f.trace[i], BLOCK_SIZE, f.trace[dcomp_lsh_ref], BLOCK_SIZE, delta_compressed, 1);
		}

		set_offset(r, total);

		if (min(comp_self, BLOCK_SIZE) > dcomp_lsh) { // delta compress
			set_size(r, (unsigned long)(dcomp_lsh - 1));
			set_ref(r, dcomp_lsh_ref);
			set_flag(r, 0b11);
			f.write_file(delta_compressed, dcomp_lsh);
			total += dcomp_lsh;
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
		lsh1.insert(i);
		lsh2.insert(i);
		lsh3.insert(i);
		lsh4.insert(i);
		lsh5.insert(i);
		f.recipe_insert(r);
	}
	f.recipe_write();
	cout << "Total time: " << f.time_check_end() << "us\n";

	printf("SOTA %s\n", argv[1]);
	printf("Final size: %llu, (%.2lf%%)\n", total, (double)total * 100 / f.N / BLOCK_SIZE);
}
