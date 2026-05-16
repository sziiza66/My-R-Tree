#pragma once

#include <array>
#include <chrono>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>

#include "r_tree/r_tree.h"

namespace MyRTree::Testing {

using Clock = std::chrono::high_resolution_clock;

constexpr std::array<size_t, 28> kDs = {1,  2,  3,   4,   6,   8,   12,  16,  24,   32,   48,
                                                64, 96, 128, 192, 256, 384, 512, 768, 1024, 2048, 3072, 4096, 6124, 8164, 12266, 16000, 32000};

template <size_t D>
Vector<D> GeneratePoint(std::uniform_real_distribution<Scalar>& dist, std::mt19937_64& gen) {
    Vector<D> p;

    for (size_t i = 0; i < D; ++i) {
        p[i] = dist(gen);
    }

    return p;
}

template <size_t D>
BoundingBox<D> GenerateWindow(std::uniform_real_distribution<Scalar>& dist, std::mt19937_64& gen,
                              Scalar radius = 0.1) {
    auto center = GeneratePoint<D>(dist, gen);

    Vector<D> p1;
    Vector<D> p2;

    for (size_t i = 0; i < D; ++i) {
        p1[i] = center[i] - radius;
        p2[i] = center[i] + radius;
    }

    return BoundingBox<D>(p1, p2);
}

template <size_t D>
void BenchNNQuerySingle(std::ofstream& out, size_t b_factor, double min_children_ratio, size_t points_cnt,
                        size_t queries_cnt, size_t k, uint64_t seed) {
    std::mt19937_64 gen(seed);
    std::uniform_real_distribution<Scalar> dist(0.0, 1.0);

    RTree<D> tree(b_factor, min_children_ratio);

    for (size_t i = 0; i < points_cnt; ++i) {
        tree.Insert(GeneratePoint<D>(dist, gen));
    }

    std::vector<Vector<D>> queries(queries_cnt);

    for (size_t i = 0; i < queries_cnt; ++i) {
        queries[i] = GeneratePoint<D>(dist, gen);
    }

    auto begin = Clock::now();

    for (const auto& q : queries) {
        tree.GetKClosest(q, k);
    }

    auto end = Clock::now();

    double ms = std::chrono::duration<double, std::milli>(end - begin).count();

    out << D << " " << ms / queries_cnt << '\n';

    std::cout << "[NN] D=" << D << " avg time=" << ms / queries_cnt << " ms\n";
}

template <size_t D>
void BenchInsertSingle(std::ofstream& out, size_t b_factor, double min_children_ratio, size_t points_cnt,
                       uint64_t seed) {
    std::mt19937_64 gen(seed);
    std::uniform_real_distribution<Scalar> dist(0.0, 1.0);

    std::vector<Vector<D>> points(points_cnt);

    for (size_t i = 0; i < points_cnt; ++i) {
        points[i] = GeneratePoint<D>(dist, gen);
    }

    RTree<D> tree(b_factor, min_children_ratio);

    auto begin = Clock::now();

    for (const auto& p : points) {
        tree.Insert(p);
    }

    auto end = Clock::now();

    double ms = std::chrono::duration<double, std::milli>(end - begin).count();

    out << D << " " << ms / points_cnt << '\n';

    std::cout << "[Insert] D=" << D << " avg time=" << ms / points_cnt << " ms\n";
}

template <size_t D>
void BenchWindowQuerySingle(std::ofstream& out, size_t b_factor, double min_children_ratio, size_t points_cnt,
                            size_t queries_cnt, uint64_t seed) {
    std::mt19937_64 gen(seed);
    std::uniform_real_distribution<Scalar> dist(0.0, 1.0);

    RTree<D> tree(b_factor, min_children_ratio);

    for (size_t i = 0; i < points_cnt; ++i) {
        tree.Insert(GeneratePoint<D>(dist, gen));
    }

    std::vector<BoundingBox<D>> windows(queries_cnt);

    for (size_t i = 0; i < queries_cnt; ++i) {
        windows[i] = GenerateWindow<D>(dist, gen);
    }

    auto begin = Clock::now();

    for (const auto& w : windows) {
        tree.SearchWindow(w);
    }

    auto end = Clock::now();

    double ms = std::chrono::duration<double, std::milli>(end - begin).count();

    out << D << " " << ms / queries_cnt << '\n';

    std::cout << "[Window] D=" << D << " avg time=" << ms / queries_cnt << " ms\n";
}

template <typename F, size_t... Is>
void ForDimsImpl(F&& f, std::index_sequence<Is...>) {
    (f.template operator()<kDs[Is]>(), ...);
}

template <typename F>
void ForDims(F&& f) {
    ForDimsImpl(std::forward<F>(f), std::make_index_sequence<kDs.size()>{});
}

void BenchNNQueries(size_t b_factor, double min_children_ratio, size_t points_cnt, size_t queries_cnt, size_t k,
                    uint64_t seed);

void BenchInserts(size_t b_factor, double min_children_ratio, size_t points_cnt, uint64_t seed);

void BenchWindowQueries(size_t b_factor, double min_children_ratio, size_t points_cnt, size_t queries_cnt,
                        uint64_t seed);

}  // namespace MyRTree::Testing
