#pragma once

#include <format>
#include <iostream>
#include <random>
#include <set>
#include <string>
#include <vector>
#include "r_tree/r_tree.h"

namespace MyRTree::Testing {

template <size_t D>
void GeneratePoint(Vector<D>& point, std::uniform_real_distribution<Scalar>& rng, std::mt19937_64& gen) {
    for (size_t i = 0; i < D; ++i) {
        point[i] = rng(gen);
    }
}

template <size_t D>
void TestNNQuery(size_t b_factor, double min_children_ratio, size_t points_cnt, size_t queries_cnt, size_t closest_cnt,
                 size_t seed) {
    RTree<D> tree(b_factor, min_children_ratio);

    using Point = Vector<D>;

    std::mt19937_64 gen(seed);
    std::uniform_real_distribution<Scalar> rng(1.0, 2.0);
    std::vector<Point> points(points_cnt);
    for (auto& point : points) {
        GeneratePoint(point, rng, gen);
    }

    for (const auto& point : points) {
        tree.Insert(point);
    }

    std::vector<Point> queries(queries_cnt);
    for (auto& query : queries) {
        GeneratePoint(query, rng, gen);
    }

    std::vector<Scalar> dists(points_cnt);
    for (const auto& query : queries) {
        for (size_t i = 0; i < points_cnt; ++i) {
            dists[i] = GetDistSq(query, points[i]);
        }
        std::sort(dists.begin(), dists.end());

        auto res = tree.GetKClosest(query, closest_cnt);
        assert(res.size() == std::min(closest_cnt, points_cnt));

        for (size_t i = 0; i < closest_cnt; ++i) {
            assert(GetDistSq(query, res[i]) == dists[i]);
        }
    }

    std::string ok_message = std::format(
        "=======================NNQ=======================\n"
        "D = {}\n"
        "b_factor = {}\n"
        "min_children_ratio = {}\n"
        "points_cnt = {}\n"
        "queries_cnt = {}\n"
        "closest_cnt = {}\n"
        "seed = {}\n"
        "=======================OK========================",
        D, b_factor, min_children_ratio, points_cnt, queries_cnt, closest_cnt, seed);
    std::cout << ok_message << std::endl;
}

template <size_t... Ds>
void TestNNQueries(std::index_sequence<Ds...>, size_t b_factor, double min_children_ratio, size_t points_cnt,
                   size_t queries_cnt, size_t closest_cnt, size_t seed) {
    (TestNNQuery<Ds + 1>(b_factor, min_children_ratio, points_cnt, queries_cnt, closest_cnt, seed), ...);
}

template <size_t D>
void TestWindowQuery(size_t b_factor, double min_children_ratio, size_t points_cnt, size_t queries_cnt, size_t seed) {
    RTree<D> tree(b_factor, min_children_ratio);

    using Point = Vector<D>;
    using BBox = BoundingBox<D>;

    std::mt19937_64 gen(seed);
    std::uniform_real_distribution<Scalar> rng(1.0, 2.0);
    std::vector<Point> points(points_cnt);
    for (auto& point : points) {
        GeneratePoint(point, rng, gen);
    }

    for (const auto& point : points) {
        tree.Insert(point);
    }

    std::vector<BBox> queries(queries_cnt);
    for (auto& query : queries) {
        Point a;
        Point b;
        GeneratePoint(a, rng, gen);
        GeneratePoint(b, rng, gen);
        query = BBox(a, b);
    }

    for (const auto& query : queries) {
        std::multiset<Point> ans;
        for (const auto& point : points) {
            if (AreIntersecting(query, point)) {
                ans.insert(point);
            }
        }

        auto res = tree.SearchWindow(query);
        assert(res.size() == ans.size());
        std::multiset<Point> res_ans;
        for (size_t i = 0; i < res.size(); ++i) {
            res_ans.insert(res[i]);
        }
        assert(res_ans == ans);
    }

    std::string ok_message = std::format(
        "======================WINDOW=====================\n"
        "D = {}\n"
        "b_factor = {}\n"
        "min_children_ratio = {}\n"
        "points_cnt = {}\n"
        "queries_cnt = {}\n"
        "seed = {}\n"
        "=======================OK========================",
        D, b_factor, min_children_ratio, points_cnt, queries_cnt, seed);
    std::cout << ok_message << std::endl;
}

template <size_t... Ds>
void TestWindowQueries(std::index_sequence<Ds...>, size_t b_factor, double min_children_ratio, size_t points_cnt,
                       size_t queries_cnt, size_t seed) {
    (TestWindowQuery<Ds + 1>(b_factor, min_children_ratio, points_cnt, queries_cnt, seed), ...);
}

}  // namespace MyRTree::Testing
