#include <iostream>
#include <map>
#include "../../compress.h"
#include "../../lz4.h"
#include "../../xdelta3/xdelta3.h"
using namespace std;

int main(int argc, char* argv[]) {
	if (argc != 4) {
		cerr << "usage: ./compare_with_bf [filename] [bf_filename] [cluster_filename]\n";
		exit(0);
	}

	DATA_IO f(argv[1]);
	f.read_file();

	FILE* f1 = fopen(argv[2], "rt");
	FILE* f2 = fopen(argv[3], "rt");

	map<int, pair<int, int>> cluster;
	
	int sz, num, ref;
	while (fscanf(f2, "%d", &sz) == 1) {
		for (int i = 0; i < sz; ++i) {
			fscanf(f2, "%d", &num);
			if (i == 0) ref = num;
			cluster[num] = {ref, sz};
		}
	}
	fclose(f2);

	long long flag;
	int i = 0;

	int same_cluster = 0, diff_cluster = 0, itself = 0, diff_ref = 0;
	long long sum_bf = 0, sum_bf_when_diff = 0, sum_cluster_when_diff = 0, sum_itself = 0;

	while (fscanf(f1, "%lld", &flag) == 1) {
		if (flag == 0) {
			fscanf(f1, "%d", &sz);
		}
		else if (flag == 1) {
			fscanf(f1, "%d", &sz);
		}
		else if (flag == 2) {
			fscanf(f1, "%d", &ref);
		}
		else if (flag == 3) {
			fscanf(f1, "%d %d", &sz, &ref);
			if (cluster[i].first == cluster[ref].first) {
				same_cluster++;
			}
			else {
				int bf_sz = xdelta3_compress(f.trace[i], BLOCK_SIZE, f.trace[ref], BLOCK_SIZE, delta_compressed, 1);
				sum_bf += bf_sz;
				diff_cluster++;
				if (cluster[i].second == 1) {
					itself++;
					int comp_self = LZ4_compress_default(f.trace[i], compressed, BLOCK_SIZE, 2 * BLOCK_SIZE);
					sum_itself += comp_self;
				}
				else {
					if (cluster[i].first == i) {
						diff_ref++;
					}
					else {
						int cluster_sz = xdelta3_compress(f.trace[i], BLOCK_SIZE, f.trace[cluster[i].first], BLOCK_SIZE, delta_compressed, 1);
						sum_cluster_when_diff += cluster_sz;
						sum_bf_when_diff += bf_sz;
					}
				}
			}
		}
		else break;
		i++;
	}

	cout << "Same cluster: " << same_cluster << '\n';
	cout << "Different cluster: " << diff_cluster << '\n';
	cout << "Average distance of BF when different: " << (double)sum_bf / diff_cluster << '\n';
	cout << '\n';
	cout << "Cluster itself: " << itself << '\n';
	cout << "Average size of cluster itself: " << (double)sum_itself / itself << '\n';
	cout << '\n';
	cout << "Representative itself when different: " << diff_ref << '\n';
	cout << "Average distance of cluster when different: " << (double)sum_cluster_when_diff / (diff_cluster - itself - diff_ref) << '\n';
	cout << "Average distance of BF when different: " << (double)sum_bf_when_diff / (diff_cluster - itself - diff_ref) << '\n';
}
