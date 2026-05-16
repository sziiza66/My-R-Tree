#include <iostream>
#include "tests.h"

int main() {

    // Тесты NNQ для D in [1, 20]
    MyRTree::Testing::TestNNQueries(
        /*Ds*/ std::make_index_sequence<19>{},
        /*b_factor*/ 10,
        /*min_children_ratio*/ 0.4,
        /*points_cnt*/ 1000,
        /*queries_cnt*/ 1000,
        /*closest_cnt*/ 1000,
        /*seed*/ 6);

    std::cout << "NNQ Tests OK!" << std::endl;
}
