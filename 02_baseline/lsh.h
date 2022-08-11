#include <tuple>
#include <limits>
#include <map>
#include <vector>
#include <algorithm>
#include <functional>
#include <random>
#include "../util/xxhash.h"

// LSH

class LSH {
	private:
	std::mt19937 gen1, gen2;
	std::uniform_int_distribution<uint32_t> full_uint32_t;

	int BLOCK_SIZE, W;
	int SF_NUM, FEATURE_NUM;

	uint32_t* TRANSPOSE_M;
	uint32_t* TRANSPOSE_A;

	const uint32_t A = 37, MOD = 1000000007;
	uint64_t Apower = 1;

	uint32_t* feature;
	uint64_t* superfeature;

	std::map<uint64_t, std::vector<int>>* sfTable;
	public:
	LSH(int _BLOCK_SIZE, int _W, int _SF_NUM, int _FEATURE_NUM) {
		gen1 = std::mt19937(922);
		gen2 = std::mt19937(314159);
		full_uint32_t = std::uniform_int_distribution<uint32_t>(std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max());

		BLOCK_SIZE = _BLOCK_SIZE;
		W = _W;
		SF_NUM = _SF_NUM;
		FEATURE_NUM = _FEATURE_NUM;

		TRANSPOSE_M = new uint32_t[FEATURE_NUM];
		TRANSPOSE_A = new uint32_t[FEATURE_NUM];

		feature = new uint32_t[FEATURE_NUM];
		superfeature = new uint64_t[SF_NUM];

		sfTable = new std::map<uint64_t, std::vector<int>>[SF_NUM];

		for (int i = 0; i < FEATURE_NUM; ++i) {
			TRANSPOSE_M[i] = ((full_uint32_t(gen1) >> 1) << 1) + 1;
			TRANSPOSE_A[i] = full_uint32_t(gen1);
		}
		for (int i = 0; i < W - 1; ++i) {
			Apower *= A;
			Apower %= MOD;
		}
	}
	~LSH() {
		delete[] TRANSPOSE_M;
		delete[] TRANSPOSE_A;
		delete[] feature;
		delete[] superfeature;
		delete[] sfTable;
	}
	int request(unsigned char* ptr);
	void insert(int label);
};

int LSH::request(unsigned char* ptr) {
	for (int i = 0; i < FEATURE_NUM; ++i) feature[i] = 0;
	for (int i = 0; i < SF_NUM; ++i) superfeature[i] = 0;

	int64_t fp = 0;

	for (int i = 0; i < W; ++i) {
		fp *= A;
		fp += (unsigned char)ptr[i];
		fp %= MOD;
	}

	for (int i = 0; i < BLOCK_SIZE - W + 1; ++i) {
		for (int j = 0; j < FEATURE_NUM; ++j) {
			uint64_t trans = TRANSPOSE_M[j] * fp + TRANSPOSE_A[j];
			uint32_t real = (uint32_t)(trans & 0xffffffff);
			if (real > feature[j]) feature[j] = real;
		}
		fp -= (ptr[i] * Apower) % MOD;
		while (fp < 0) fp += MOD;
		if (i != BLOCK_SIZE - W) {
			fp *= A;
			fp += ptr[i + W];
			fp %= MOD;
		}
	}

	for (int i = 0; i < SF_NUM; ++i) {
		superfeature[i] = XXH64(&feature[i * FEATURE_NUM / SF_NUM], sizeof(uint32_t) * FEATURE_NUM / SF_NUM, 0);
	}

	uint32_t r = full_uint32_t(gen2) % SF_NUM;
	for (int i = 0; i < SF_NUM; ++i) {
		int index = (r + i) % SF_NUM;
		if (sfTable[index].count(superfeature[index])) {
			return sfTable[index][superfeature[index]].back();
		}
	}
	return -1;
}

// insert "prev calculated" sf: label
void LSH::insert(int label) {
	for (int i = 0; i < SF_NUM; ++i) {
		sfTable[i][superfeature[i]].push_back(label);
	}
}
