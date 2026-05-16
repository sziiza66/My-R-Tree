#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>
#include <memory>
#include <queue>
#include <vector>

#include "../common/geom.h"

namespace MyRTree {

template <size_t D>
class RTree {
    static_assert(D > 0);
    using Point = Vector<D>;
    using BBox = BoundingBox<D>;

    class Proxy {
    public:
        Proxy(const RTree<D>& tree, std::vector<size_t> indices) : tree_(tree), indices_(indices) {
        }

        std::vector<Point> Get() const {
            std::vector<Point> result(indices_.size());
            for (size_t i = 0; i < indices_.size(); ++i) {
                result[i] = *tree_[indices_[i]];
            }
            return result;
        }

        operator std::vector<Point>() const {
            return Get();
        }

        const Point& operator[](size_t index) {
            return tree_[indices_[index]];
        }

        size_t size() const {
            return indices_.size();
        }

    private:
        const RTree<D>& tree_;
        std::vector<size_t> indices_;
    };

    struct Node {
        std::vector<size_t> children;
        std::unique_ptr<BBox> bbox;
        bool is_leaf;
    };

public:
    RTree(size_t b_factor, double min_children_ratio) : b_factor_(b_factor), min_children_ratio_(min_children_ratio) {
        assert(std::ceil(min_children_ratio_ * b_factor_) > 0);
        nodes_.emplace_back(std::vector<size_t>{}, std::make_unique<BBox>(), true);
    }

    void Insert(const Point& p) {
        points_.emplace_back(new Point(p));
        if (DoInsert(p, root_index_)) {
            size_t new_node_index = nodes_.size() - 1;
            nodes_.emplace_back(std::vector<size_t>{root_index_, new_node_index},
                                std::make_unique<BBox>(*nodes_[root_index_].bbox, *nodes_[new_node_index].bbox), false);
            root_index_ = nodes_.size() - 1;
        }
    }

    Proxy GetKClosest(const Point& q, size_t k) const {
        if (k == 0) {
            return Proxy(*this, {});
        }

        std::vector<size_t> result;
        std::vector<std::tuple<size_t, bool>> viewed;
        std::vector<Scalar> dists;

        auto cmp = [&](const size_t& v1, const size_t& v2) { return dists[v1] > dists[v2]; };

        std::priority_queue<size_t, std::vector<size_t>, decltype(cmp)> heap(cmp);

        viewed.emplace_back(root_index_, false);
        dists.emplace_back(std::numeric_limits<Scalar>::infinity());  // doesn't matter
        heap.emplace(0);

        while (!heap.empty() && result.size() != k) {
            auto [ind, is_point] = viewed[heap.top()];
            heap.pop();
            if (is_point) {
                result.emplace_back(ind);
                continue;
            }
            for (size_t child : nodes_[ind].children) {
                viewed.emplace_back(std::make_tuple(child, nodes_[ind].is_leaf));
                dists.emplace_back(nodes_[ind].is_leaf ? GetDistSq(q, *points_[child])
                                                       : GetMinDistSq(*nodes_[child].bbox, q));
                heap.emplace(viewed.size() - 1);
            }
        }
        return Proxy(*this, result);
    }

    // Proxy SearchWindow(const BBox& b) const;

    const Point& operator[](size_t index) const {
        return *points_[index];
    }

private:
    bool IsAboutToOverflow(size_t node_ind) const {
        return nodes_[node_ind].children.size() + 1 > b_factor_;
    }

    bool DoInsert(const Point& p, size_t node_ind) {  // true if new node was created
        if (!nodes_[node_ind].is_leaf) {
            bool new_node_created = DoInsert(p, ChooseSubTree(p, node_ind));
            if (new_node_created) {
                nodes_[node_ind].children.emplace_back(nodes_.size() - 1);
            }
            bool is_spit_needed = new_node_created && IsAboutToOverflow(node_ind);
            if (is_spit_needed) {
                SplitIntermediateNode(node_ind);
                return true;
            }
        } else {
            bool is_spit_needed = IsAboutToOverflow(node_ind);
            nodes_[node_ind].children.emplace_back(points_.size() - 1);
            if (is_spit_needed) {
                SplitLeafNode(node_ind);
                return true;
            }
        }
        nodes_[node_ind].bbox->Add(*points_.back());
        return false;
    }

    void SplitIntermediateNode(size_t node_ind) {
        const auto& children = nodes_[node_ind].children;
        size_t cnt = children.size();
        size_t min_children = std::ceil(min_children_ratio_ * b_factor_);

        std::vector<size_t> optimal_indices;
        size_t optimal_sep;
        Scalar optimal_sum = std::numeric_limits<Scalar>::infinity();

        std::vector<BBox> pref(cnt + 1);
        std::vector<BBox> suff(cnt + 1);
        for (size_t i = 0; i < D; ++i) {
            std::vector<size_t> indices = children;
            std::sort(indices.begin(), indices.end(),
                      [&](size_t a, size_t b) { return nodes_[a].bbox->Min(i) < nodes_[b].bbox->Max(i); });
            for (size_t j = 0; j < cnt; ++j) {
                pref[j + 1] = BBox(pref[j], *nodes_[indices[j]].bbox);
                suff[j + 1] = BBox(suff[j], *nodes_[indices[cnt - j - 1]].bbox);
            }
            for (size_t sep = min_children; sep + min_children < cnt; ++sep) {
                Scalar sum = pref[sep].Perimeter() + suff[cnt - sep].Perimeter();
                if (sum < optimal_sum) {
                    optimal_indices = indices;
                    optimal_sep = sep;
                    optimal_sum = sum;
                }
            }
        }
        assert(!optimal_indices.empty());
        nodes_[node_ind].children = std::vector<size_t>(optimal_indices.begin(), optimal_indices.begin() + optimal_sep);
        nodes_[node_ind].bbox->Clear();
        for (size_t index : nodes_[node_ind].children) {
            nodes_[node_ind].bbox->Add(*nodes_[index].bbox);
        }
        nodes_.emplace_back(std::vector<size_t>(optimal_indices.begin() + optimal_sep, optimal_indices.end()),
                            std::make_unique<BBox>(), false);
        for (size_t index : nodes_.back().children) {
            nodes_.back().bbox->Add(*nodes_[index].bbox);
        }
    }

    void SplitLeafNode(size_t node_ind) {
        const auto& children = nodes_[node_ind].children;
        size_t cnt = children.size();
        size_t min_children = std::ceil(min_children_ratio_ * b_factor_);

        std::vector<size_t> optimal_indices;
        size_t optimal_sep;
        Scalar optimal_sum = std::numeric_limits<Scalar>::infinity();

        std::vector<BBox> pref(cnt + 1);
        std::vector<BBox> suff(cnt + 1);

        for (size_t i = 0; i < D; ++i) {
            std::vector<size_t> indices = children;
            std::sort(indices.begin(), indices.end(),
                      [&](size_t a, size_t b) { return (*points_[a])[i] < (*points_[b])[i]; });
            for (size_t j = 0; j < cnt; ++j) {
                pref[j + 1] = BBox(pref[j], *points_[indices[j]]);
                suff[j + 1] = BBox(suff[j], *points_[indices[cnt - j - 1]]);
            }
            for (size_t sep = min_children; sep + min_children < cnt; ++sep) {
                Scalar sum = pref[sep].Perimeter() + suff[cnt - sep].Perimeter();
                if (sum < optimal_sum) {
                    optimal_indices = indices;
                    optimal_sep = sep;
                    optimal_sum = sum;
                }
            }
        }
        assert(!optimal_indices.empty());
        nodes_[node_ind].children = std::vector<size_t>(optimal_indices.begin(), optimal_indices.begin() + optimal_sep);
        nodes_[node_ind].bbox->Clear();
        for (size_t index : nodes_[node_ind].children) {
            nodes_[node_ind].bbox->Add(*points_[index]);
        }
        nodes_.emplace_back(std::vector<size_t>(optimal_indices.begin() + optimal_sep, optimal_indices.end()),
                            std::make_unique<BBox>(), true);
        for (size_t index : nodes_.back().children) {
            nodes_.back().bbox->Add(*points_[index]);
        }
    }

    size_t ChooseSubTree(const Point& p, size_t node_ind) const {
        Scalar min_inc = std::numeric_limits<Scalar>::infinity();
        size_t result = 0;
        for (size_t candidate : nodes_[node_ind].children) {
            Scalar cand_inc = BBox(*nodes_[candidate].bbox, p).Perimeter() - nodes_[candidate].bbox->Perimeter();
            if (cand_inc < min_inc) {
                min_inc = cand_inc;
                result = candidate;
            }
        }
        return result;
    }

private:
    std::vector<Node> nodes_;
    std::vector<std::unique_ptr<Point>> points_;
    size_t root_index_ = 0;
    size_t b_factor_;
    double min_children_ratio_;
};

}  // namespace MyRTree
