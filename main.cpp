#include <iostream>
#include "tree_utils.h"
#include "sampler.h"
#include <vector>

int main() {
    using namespace std;
    int csp = 320;
    int w_grind = 8;
    int tau = 40;

    auto [t0, k0, t1, k1] = _vc_param(csp - w_grind, tau);
    auto L = (1LL << k0) * t0 + (1LL << k1) * t1;
    auto max_size = t0 * k0 + t1 * k1;

    cout << "L = " << L << " max_size = " << max_size << endl; 
    auto dist = sample(L, tau);
    auto hist = get_hist(dist);

    std::cout << "Histogram for one tree distribution grinded_csp = " << csp - w_grind << " tau = " << tau << std::endl;

    for (auto & [pnode_size, prob] : hist) {
        std::cout << "Size: ";
        std::cout << pnode_size << " ";
        std::cout << "Probability: " << prob << std::endl;
    }

    return 0;
}
