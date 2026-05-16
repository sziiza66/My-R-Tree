#pragma once

#include <array>
#include <cstddef>
#include <limits>

namespace MyRTree {

using Scalar = double;
template <size_t D>
using Vector = std::array<Scalar, D>;

template <size_t D>
Vector<D> ProduceMin(const Vector<D>& p1, const Vector<D>& p2) {
    Vector<D> result;
    for (size_t i = 0; i < D; ++i) {
        result[i] = p1[i] < p2[i] ? p1[i] : p2[i];
    }
    return result;
}

template <size_t D>
Vector<D> ProduceMax(const Vector<D>& p1, const Vector<D>& p2) {
    Vector<D> result;
    for (size_t i = 0; i < D; ++i) {
        result[i] = p1[i] > p2[i] ? p1[i] : p2[i];
    }
    return result;
}

template <size_t D>
class BoundingBox {
public:
    BoundingBox() {
        Clear();
    }

    BoundingBox(const Vector<D>& p) : min_(p), max_(p) {
    }

    BoundingBox(const Vector<D>& p1, const Vector<D>& p2) : min_(ProduceMin(p1, p2)), max_(ProduceMax(p1, p2)) {
    }

    BoundingBox(const BoundingBox& b, const Vector<D>& p) : min_(ProduceMin(b.Min(), p)), max_(ProduceMax(b.Max(), p)) {
    }

    BoundingBox(const BoundingBox& b1, const BoundingBox& b2)
        : min_(ProduceMin(b1.Min(), b2.Min())), max_(ProduceMax(b1.Max(), b2.Max())) {
    }

    void Clear() {
        min_.fill(std::numeric_limits<Scalar>::max());
        max_.fill(std::numeric_limits<Scalar>::min());
        is_stat_actual_ = false;
    }

    void Add(const Vector<D>& p) {
        for (size_t i = 0; i < D; ++i) {
            min_[i] = min_[i] < p[i] ? min_[i] : p[i];
        }
        for (size_t i = 0; i < D; ++i) {
            max_[i] = max_[i] > p[i] ? max_[i] : p[i];
        }
        is_stat_actual_ = false;
    }

    void Add(const BoundingBox& b) {
        for (size_t i = 0; i < D; ++i) {
            min_[i] = min_[i] < b.Min(i) ? min_[i] : b.Min(i);
        }
        for (size_t i = 0; i < D; ++i) {
            max_[i] = max_[i] > b.Max(i) ? max_[i] : b.Max(i);
        }
        is_stat_actual_ = false;
    }

    Scalar Min(size_t d) const {
        return min_[d];
    }

    Scalar Max(size_t d) const {
        return max_[d];
    }

    const Vector<D>& Min() const {
        return min_;
    }

    const Vector<D>& Max() const {
        return max_;
    }

    Scalar Volume() const {
        if (!is_stat_actual_) {
            volume_ = 1;
            for (size_t i = 0; i < D; ++i) {
                volume_ *= Max(i) - Min(i);
            }
            is_stat_actual_ = true;
        }
        return volume_;
    }

    Scalar Perimeter() const {
        if (!is_stat_actual_) {
            perimeter_ = 0;
            Scalar actual_volume = Volume();
            if (actual_volume != 0) {  //  (max - min == 0) -> volume == 0
                for (size_t i = 0; i < D; ++i) {
                    perimeter_ += actual_volume / (Max(i) - Min(i));
                }
            }
        }
        return perimeter_;
    }

private:
    Vector<D> min_;
    Vector<D> max_;
    mutable Scalar volume_;
    mutable Scalar perimeter_;
    mutable bool is_stat_actual_ = false;
};

template <size_t D>
bool AreIntersecting(const BoundingBox<D>& b1, const BoundingBox<D>& b2) {
    for (size_t i = 0; i < D; ++i) {
        if (b1.Max(i) < b2.Min(i) || b2.Max(i) < b1.Min(i)) {
            return false;
        }
    }
    return true;
}

template <size_t D>
Scalar GetDistSq(const Vector<D>& p1, const Vector<D>& p2) {
    Scalar result = 0;
    for (size_t i = 0; i < D; ++i) {
        result += (p1[i] - p2[i]) * (p1[i] - p2[i]);
    }
    return result;
}

template <size_t D>
Scalar GetMinDistSq(const BoundingBox<D>& b, const Vector<D>& p) {
    Scalar result = 0;
    for (size_t i = 0; i < D; ++i) {
        if (p[i] < b.Min(i)) {
            result += (p[i] - b.Min(i)) * (p[i] - b.Min(i));
        } else if (p[i] > b.Max(i)) {
            result += (p[i] - b.Max(i)) * (p[i] - b.Max(i));
        }
    }
    return result;
}

}  // namespace MyRTree
