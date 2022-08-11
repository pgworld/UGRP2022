#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <sys/stat.h>
#include <sys/types.h>
#include "../util/compress.h"
#define ELEM_PER_CLASS 100
using namespace std;

typedef pair<int, int> ii;

mt19937 gen(922);
uniform_int_distribution<uint32_t> full_uint32_t;

void modify(char* ptr) {
	int num = full_uint32_t(gen) % 9 + 1;
	for (int i = 0; i < num; ++i) {
		int length = full_uint32_t(gen) % 9 + 1;
		int start = full_uint32_t(gen) % (BLOCK_SIZE - length + 1);
		int t = full_uint32_t(gen) % 2;

		if (t == 0 || length == 1) { // add constant
			int offset = full_uint32_t(gen) % 255 + 1;
			for (int j = start; j < start + length; ++j) {
				ptr[j] += offset;
			}
		}
		else { // circular shift
			int* modified = new int[length];
			for (int j = 0; j < length; ++j) modified[j] = 0;
			int offset = full_uint32_t(gen) % (length - 1) + 1;
			for (int j = 0; j < length; ++j) {
				modified[j] = ptr[start + (offset + j) % length];
			}
			for (int j = 0; j < length; ++j) {
				ptr[start + j] = modified[j];
			}
		}
	}
}

int main(int argc, char* argv[]) {
	if (argc != 3) {
		cerr << "usage: ./dataset [input_file] [tag] < [cluster_info]\n";
		exit(0);
	}

	DATA_IO f(argv[1]);
	f.read_file();

	vector<pair<int, vector<int>>> cluster;
	int sz, t;
	while (cin >> sz) {
		if (sz == 1) {
			cin >> t;
			continue;
		}
		else {
			vector<int> now;
			for (int i = 0; i < sz; ++i) {
				cin >> t;
				now.push_back(t);
			}
			cluster.push_back({now.size(), now});
		}
	}
	sort(cluster.begin(), cluster.end(), greater<pair<int, vector<int>>>());

	printf("Total %ld cluster found!\n", cluster.size());
	for (int j = 0; j < 10; ++j) { 
		printf("cluster %d: size = %d, rep = %d\n", j, cluster[j].first, cluster[j].second[0]);
		printf("First 100 byte: ");
		for (int i = 0; i < 100; ++i) {
			printf("%d ", f.trace[cluster[j].second[0]][i]);
		}
		printf("\n\n");
	}

	int choose = min(20000, (int)cluster.size());

	char dirname[100];
	sprintf(dirname, "cluster_%s_%d", argv[2], choose);
	mkdir(dirname, 0755);
	for (int i = 0; i < choose; ++i) {
		sprintf(dirname, "cluster_%s_%d/%d", argv[2], choose, i);
		mkdir(dirname, 0755);
	}

	vector<int> large_cluster(choose);

	for (int i = 0; i < choose; ++i) large_cluster[i] = i;
	random_shuffle(large_cluster.begin(), large_cluster.end());

	char name[100];
	for (int i = 0; i < choose; ++i) {
		int sz = cluster[large_cluster[i]].second.size();
		if (sz <= ELEM_PER_CLASS) {
			for (int j = 0; j < sz; ++j) {
				int u = cluster[large_cluster[i]].second[j];
				sprintf(name, "cluster_%s_%d/%d/%d_%d", argv[2], choose, i, i, j);
				FILE* out = fopen(name, "wb");
				fwrite(f.trace[u], 1, 4096, out);
				fclose(out);
			}
			for (int j = sz; j < ELEM_PER_CLASS; ++j) {
				int u = cluster[large_cluster[i]].second[full_uint32_t(gen) % sz];
				modify(f.trace[u]);
				sprintf(name, "cluster_%s_%d/%d/%d_%d", argv[2], choose, i, i, j);
				FILE* out = fopen(name, "wb");
				fwrite(f.trace[u], 1, 4096, out);
				fclose(out);
			}
		}
		else {
			vector<int> vec;
			for (int j = 0; j < sz; ++j) {
				vec.push_back(cluster[large_cluster[i]].second[j]);
			}
			shuffle(vec.begin(), vec.end(), gen);
			for (int j = 0; j < ELEM_PER_CLASS; ++j) {
				int u = vec[j];
				sprintf(name, "cluster_%s_%d/%d/%d_%d", argv[2], choose, i, i, j);
				FILE* out = fopen(name, "wb");
				fwrite(f.trace[u], 1, 4096, out);
				fclose(out);
			}
		}
	}
}
