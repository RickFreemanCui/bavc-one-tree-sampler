#include <iostream>
#include "tree_utils.h"
#include "sampler.h"
#include <vector>
#include <algorithm>

int main(int argc, char *argv[]) {
    using namespace std;
    int csp = 100;
    int w_grind = 0;
    int tau = 50;

    if (argc != 3) {
        cout << "Usage: " << argv[0] << " <csp> <tau>" << endl;
        return 1;
    }
    // Parse command line arguments

    csp = atoi(argv[1]);
    tau = atoi(argv[2]);

    auto [t0, k0, t1, k1] = _vc_param(csp - w_grind, tau);
    auto L = (1LL << k0) * t0 + (1LL << k1) * t1;
    auto max_size = t0 * k0 + t1 * k1;

    cerr << "L = " << L << " max_size = " << max_size << endl; 
    auto dist = sample(L, tau);
    auto hist = get_hist(dist);

    // std::cout << "Histogram for one tree distribution grinded_csp = " << csp - w_grind << " tau = " << tau << std::endl;

    // for (auto & [pnode_size, prob] : hist) {
    //     std::cout << "Size: ";
    //     std::cout << pnode_size << " ";
    //     std::cout << "Probability: " << prob << std::endl;
    // }

    vector<pair<int, double>> cdf(hist.begin(), hist.end());
    double prob_sum = 0.0;
    for (auto & [pnode_size, prob] : cdf) {
        prob_sum += prob;
        prob = prob_sum; // Update probability to be cumulative
        // cout << "CDF Size: " << pnode_size << " CDF Probability: " << prob_sum << std::endl;
    }


    auto rej_1_2_bound = lower_bound(cdf.begin(), cdf.end(), std::make_pair(0, 0.5), 
        [](const std::pair<int, double>& a, const std::pair<int, double>& b) {
            return a.second < b.second;
        });
    auto rej_1_4_bound = lower_bound(cdf.begin(), cdf.end(), std::make_pair(0, 0.25), 
        [](const std::pair<int, double>& a, const std::pair<int, double>& b) {
            return a.second < b.second;
        });

    auto rej_1_8_bound = lower_bound(cdf.begin(), cdf.end(), std::make_pair(0, 0.125), 
        [](const std::pair<int, double>& a, const std::pair<int, double>& b) {
            return a.second < b.second;
        });
    cout << csp << "," 
         << tau << "," 
         << rej_1_8_bound->first << "," 
         << rej_1_4_bound->first << "," 
         << rej_1_2_bound->first << std::endl;

    return 0;
}
