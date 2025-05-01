#include "sampler.h"
#include "tree_utils.h" // Ensure all necessary functions are included

#include <cmath>     // For std::pow, std::log2
#include <vector>
#include <map>
#include <numeric>   // For std::accumulate in get_hist_randonetree calculation
#include <iostream>  // For std::cout in sample function progress
#include <stdexcept> // For potential error handling
#include <optional>
#include <limits>    // For std::numeric_limits

// Helper function to calculate 2^n safely using long long
long long power_of_2(int n) {
    if (n < 0) return 0; // Or throw error
    if (n >= 63) {
        // Handle potential overflow for long long if needed, though unlikely for typical depths
        throw std::overflow_error("Exponent too large for power_of_2");
    }
    return 1LL << n;
}


Distribution sample_once(int num_leaf) {
    Distribution dist;
    if (num_leaf <= 0) {
        // Handle invalid input
        return dist; // Return empty distribution
    }
    int leaf_min = num_leaf; // Smallest index if it were a full tree ending here
    int leaf_max = 2 * num_leaf - 1; // Largest index

    int left_depth = get_depth(leaf_max); // Depth of the deepest node
    int right_depth = get_depth(leaf_min); // Depth of the shallowest node in the last level

    // Full binary tree case
    if (left_depth == right_depth) {
        Config full_tree_config;
        for (int i = 0; i < left_depth; ++i) {
            full_tree_config.push_back({power_of_2(i), 1}); // Each level contributes 2^i nodes
        }
        dist[make_config(full_tree_config)] = 1.0; // Full tree configuration
        return dist; // Return the distribution for a full binary tree
    }


    // Non-full binary tree case
    int num_shallow = power_of_2(left_depth) - num_leaf;
    int num_left, num_right = 0;
    if (num_shallow <= power_of_2(right_depth - 1)) {
        // left tree is a fbt
        num_left = power_of_2(left_depth - 1);
        num_right = num_leaf - num_left;
    } else {
        // right tree is a fbt
        num_right = power_of_2(right_depth - 1);
        num_left = num_leaf - num_right;
    }

    // Sample from left and right subtree
    for (int num_subtree_leaf : {num_left, num_right}) {
        int num_rest = num_leaf - num_subtree_leaf;

        // Probability of choosing a leaf from this subtree
        double prob_subtree = static_cast<double>(num_subtree_leaf) / num_leaf;

        // Recursively get the distribution for the chosen subtree
        Distribution dist_subtree = sample_once(num_subtree_leaf);

        // Create the config representing the remaining part
        const Config config_rest = make_config({{num_rest, 1}}); // Config with one entry for the other subtree

        // Combine results
        for (const auto& pair : dist_subtree) {
            const Config& pnodes = pair.first;
            double sub_prob = pair.second;

            Config new_config = add_config(pnodes, config_rest);
            dist[new_config] += prob_subtree * sub_prob; // Add probability to this combined config
        }
    }
    return dist;

}


Distribution sample(int num_leaf, int steps) {
    if (num_leaf <= 0 || steps < 0) {
        return {}; // Return empty distribution for invalid input
    }

    DpCache dp; // Dynamic programming cache

    // Initial distribution: starts with one tree of size num_leaf
    Distribution dist;
    dist[make_config({{num_leaf, 1}})] = 1.0;

    for (int i = 0; i < steps; ++i) {
        // Optional: Print progress
        std::cout << i << "-th step (out of " << steps << ")" << std::endl;
        int remaining_leaves = num_leaf - i; // Remaining leaves after i splits

        Distribution new_dist;
        for (const auto& config_prob_pair : dist) {
            const Config& config = config_prob_pair.first;
            double prob = config_prob_pair.second; // Probability of current config

            for (const auto& size_count_pair : config) {
                int subtree_size = size_count_pair.first;
                int num_subtree = size_count_pair.second; // Count of subtrees of this size

                // Try to use dynamic programming cache
                Distribution subtree_dist;
                auto dp_it = dp.find(subtree_size);
                if (dp_it == dp.end()) {
                    // Not in cache, compute and store
                    subtree_dist = sample_once(subtree_size);
                    dp[subtree_size] = subtree_dist;
                } else {
                    // Found in cache
                    subtree_dist = dp_it->second;
                }

                double subtree_prob = prob * (static_cast<double>(subtree_size * num_subtree) / remaining_leaves);

                if (subtree_prob == 0) continue;


                for (const auto& sub_config_prob_pair : subtree_dist) {
                    const Config& subtree_config = sub_config_prob_pair.first; // Resulting config from splitting one subtree
                    double subtree_config_prob = sub_config_prob_pair.second; // Prob of that specific split result

                    // Create the new overall config:
                    // 1. Decrease the count of the split subtree size
                    std::optional<Config> temp_config_opt = decrease_config(config, subtree_size);
                    if (!temp_config_opt) {
                         // This should not happen if the config iteration is correct
                         std::cerr << "Error: decrease_config failed unexpectedly for size " << subtree_size << std::endl;
                         continue;
                    }
                    Config new_config_base = *temp_config_opt;

                    // 2. Add the components from the split result
                    Config final_new_config = add_config(new_config_base, subtree_config);

                    // The probability of reaching this final_new_config via this path:
                    double new_prob = subtree_prob * subtree_config_prob;

                    // Add this probability to the new distribution map
                    new_dist[final_new_config] += new_prob;
                }
            }
        }
        dist = new_dist; // Update the distribution for the next step

    }
    return dist;
}


Histogram get_hist_randonetree(int csp, int tau) {
     if (tau <= 0) {
        // Handle invalid tau
        return {}; // Return empty histogram
    }
    // Get VC parameters
    auto [t0, k0, t1, k1] = _vc_param(csp, tau);

    // Calculate L (total initial leaves)
    // Python: L = sum ([2 ** _k * _tau for _k, _tau in k_tau_list])
    // This seems wrong. _tau is the count (t0 or t1). It should be sum(count * leaves_per_subtree)
    // Leaves per subtree of depth k is 2^k.
    // So, L = t0 * 2^k0 + t1 * 2^k1
    long long L_ll = static_cast<long long>(t0) * power_of_2(k0) + static_cast<long long>(t1) * power_of_2(k1);

    // Check for potential overflow if L could be very large
    if (L_ll > std::numeric_limits<int>::max()) {
         throw std::overflow_error("Calculated L exceeds integer limits");
    }
    int L = static_cast<int>(L_ll);

    if (L <= 0) {
        // Handle cases where L is not positive (e.g., if csp is 0)
        return {};
    }

    // Perform the sampling
    // The number of steps should be tau, as per the Python code.
    Distribution final_dist = sample(L, tau);

    // Calculate the histogram from the final distribution
    Histogram hist = get_hist(final_dist);

    return hist;
}
