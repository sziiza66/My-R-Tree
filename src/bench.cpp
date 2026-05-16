#include "bench.h"

namespace MyRTree::Testing {

void BenchNNQueries(size_t b_factor, double min_children_ratio, size_t points_cnt, size_t queries_cnt, size_t k,
                    uint64_t seed) {
    std::ofstream out("benchNNQ.txt");

    ForDims([&]<size_t D>() {
        BenchNNQuerySingle<D>(out, b_factor, min_children_ratio, points_cnt, queries_cnt, k, seed);
    });
}

void BenchInserts(size_t b_factor, double min_children_ratio, size_t points_cnt, uint64_t seed) {
    std::ofstream out("benchInsert.txt");

    ForDims([&]<size_t D>() { BenchInsertSingle<D>(out, b_factor, min_children_ratio, points_cnt, seed); });
}

void BenchWindowQueries(size_t b_factor, double min_children_ratio, size_t points_cnt, size_t queries_cnt,
                        uint64_t seed) {
    std::ofstream out("benchWindowQ.txt");

    ForDims([&]<size_t D>() {
        BenchWindowQuerySingle<D>(out, b_factor, min_children_ratio, points_cnt, queries_cnt, seed);
    });
}

}  // namespace MyRTree::Testing