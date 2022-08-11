#include <iostream>
#include <random>
#include <algorithm>
#include "../util/compress.h"
using namespace std;

int main(int argc, char* argv[]) {
	if (argc != 4) {
		cerr << "usage: ./random_sampling [ratio] [input_file] [output_file]\n";
		exit(0);
	}
	int ratio = atoi(argv[1]);

	DATA_IO f(argv[2]);
	f.read_file();

	mt19937 gen(922);
	vector<int> arr;
	for (int i = 0; i < f.N; ++i) arr.push_back(i);

	shuffle(arr.begin(), arr.end(), gen);

	FILE* out = fopen(argv[3], "wb");
	for (int i = 0; i < (f.N * ratio) / 100; ++i) {
		fwrite(f.trace[arr[i]], BLOCK_SIZE, 1, out);
	}
	fclose(out);
}
