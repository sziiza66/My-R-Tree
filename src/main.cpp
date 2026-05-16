#include "bench.h"
#include "tests.h"

int main() {

    // // Тесты NNQ для D in [1, kNNQ_kDs]
    // constexpr size_t kNNQ_kDs = 19;
    // MyRTree::Testing::TestNNQueries(
    //     /*Ds*/ std::make_index_sequence<kNNQ_kDs>{},
    //     /*b_factor*/ 10,
    //     /*min_children_ratio*/ 0.4,
    //     /*points_cnt*/ 1000,
    //     /*queries_cnt*/ 1000,
    //     /*closest_cnt*/ 1000,
    //     /*seed*/ 6);

    // std::cout << "NNQ Tests OK!" << std::endl;

    // // Тесты Window для D in [1, KWindow_Ds]
    // constexpr size_t KWindow_Ds = 19;
    // MyRTree::Testing::TestWindowQueries(
    //     /*Ds*/ std::make_index_sequence<KWindow_Ds>{},
    //     /*b_factor*/ 10,
    //     /*min_children_ratio*/ 0.4,
    //     /*points_cnt*/ 1000,
    //     /*queries_cnt*/ 1000,
    //     /*seed*/ 6);

    // std::cout << "Window Tests OK!" << std::endl;

    // MyRTree::Testing::BenchNNQueries(
    //     /*b_factor*/ 10,
    //     /*min_children_ratio*/ 0.4,
    //     /*points_cnt*/ 50,
    //     /*queries_cnt*/ 100,
    //     /*closest_cnt*/ 10,
    //     /*seed*/ 6);

    MyRTree::Testing::BenchInserts(
        /*b_factor*/ 10,
        /*min_children_ratio*/ 0.4,
        /*points_cnt*/ 50,
        /*seed*/ 6);

    // MyRTree::Testing::BenchWindowQueries(
    //     /*b_factor*/ 10,
    //     /*min_children_ratio*/ 0.4,
    //     /*points_cnt*/ 50,
    //     /*queries_cnt*/ 100,
    //     /*seed*/ 6);
}
