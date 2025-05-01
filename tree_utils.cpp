#include "tree_utils.h"
#include <algorithm> // For std::sort, std::find_if
#include <iostream>  // For std::cerr in decrease_config (optional error message)
#include <cmath>     // For std::floor, std::log2, std::pow, std::ceil
#include <numeric>   // For std::accumulate

// Note: Assumes root node index = 1, left child = index * 2, right child = index * 2 + 1

int get_depth(int index) {
    if (index <= 0) {
        // Handle invalid index, perhaps throw an exception or return -1
        return -1; // Or throw std::invalid_argument("Index must be positive");
    }
    // log2(index) gives the depth assuming root is at depth 0
    return static_cast<int>(std::floor(std::log2(static_cast<double>(index))));
}

std::pair<int, int> get_lr_bound(int root_index, int depth) {
    if (root_index <= 0 || depth < 0) {
        // Handle invalid input
        return {-1, -1}; // Or throw
    }
    auto root_depth = get_depth(root_index);
    auto depth_multiplier = 1 << root_depth;
    auto left_bound = depth_multiplier * root_index;
    auto right_bound = left_bound + depth_multiplier - 1;

    return {left_bound, right_bound}; // Return as a pair
}


bool in_subtree(int root_index, int leaf_index) {
    if (root_index <= 0 || leaf_index <= 0) {
        return false; // Invalid indices
    }
    int root_depth = get_depth(root_index);
    int leaf_depth = get_depth(leaf_index);

    if (leaf_depth < root_depth) {
        return false; // Leaf cannot be shallower than the root
    }

    int relative_depth = leaf_depth - root_depth;
    std::pair<int, int> bounds = get_lr_bound(root_index, relative_depth);

    return leaf_index >= bounds.first && leaf_index <= bounds.second;
}

Config make_config(const std::vector<std::pair<int, int>>& leaf_size_num_list) {
    Config config = leaf_size_num_list; // Copy the input vector
    // Sort based on the first element of the pair (subtree_size)
    std::sort(config.begin(), config.end());
    return config;
}

ConfigMap config_tuple_to_dict(const Config& config_tuple) {
    ConfigMap config_dict;
    for (const auto& pair : config_tuple) {
        config_dict[pair.first] = pair.second;
    }
    return config_dict;
}

Config config_dict_to_tuple(const ConfigMap& config_dict) {
    Config config_list;
    // Maps iterate in key order, so the result will be sorted by subtree_size
    for (const auto& pair : config_dict) {
        config_list.push_back({pair.first, pair.second});
    }
    return config_list;
}

Config add_config(const Config& config1, const Config& config2) {
    ConfigMap config_new_dict = config_tuple_to_dict(config1);
    for (const auto& pair : config2) {
        int subtree_size = pair.first;
        int num_subtree = pair.second;
        // If key exists, add to it; otherwise, insert it.
        config_new_dict[subtree_size] += num_subtree;
    }
    return config_dict_to_tuple(config_new_dict);
}

std::optional<Config> decrease_config(const Config& config, int num_leaf) {
    ConfigMap config_new_dict = config_tuple_to_dict(config);
    auto it = config_new_dict.find(num_leaf);

    if (it == config_new_dict.end()) {
        // Key not found
        std::cerr << "Error: trying to decrease a non-existing subtree size: " << num_leaf << std::endl;
        return std::nullopt; // Indicate failure
    }

    it->second -= 1; // Decrease the count

    if (it->second == 0) {
        config_new_dict.erase(it); // Remove the entry if count reaches zero
    }

    return config_dict_to_tuple(config_new_dict); // Return the modified config
}


Histogram get_hist(const Distribution& dist) {
    std::map<int, double> hist_dict;
    for (const auto& dist_pair : dist) {
        const Config& pnodes = dist_pair.first;
        double prob = dist_pair.second;

        // Calculate the sum of counts in the config
        int num_pnodes = std::accumulate(pnodes.begin(), pnodes.end(), 0,
                                         [](int sum, const std::pair<int, int>& p) {
                                             return sum + p.second;
                                         });

        hist_dict[num_pnodes] += prob; // Add probability to the corresponding node count
    }

    Histogram hist_list;
    // Convert map to vector of pairs (already sorted by key in map)
    for (const auto& hist_pair : hist_dict) {
        hist_list.push_back({hist_pair.first, hist_pair.second});
    }
    return hist_list;
}

double expect_pnodes(const Histogram& hist) {
    double expected = 0.0;
    for (const auto& pair : hist) {
        expected += static_cast<double>(pair.first) * pair.second;
    }
    return expected;
}

int round_to_byte(int n) {
    return ((n + 7) / 8) * 8; // Round up to the nearest multiple of 8
}

std::tuple<int, int, int, int> _vc_param(int csp, int tau) {
     if (tau <= 0) {
        // Handle division by zero or invalid tau
        // Consider throwing an exception or returning an error indicator
        // For now, returning zeros as a placeholder for error
        return std::make_tuple(0, 0, 0, 0);
        // Or: throw std::invalid_argument("tau must be positive");
    }
    int k0 = static_cast<int>(std::ceil(static_cast<double>(csp) / tau));
    int k1 = static_cast<int>(std::floor(static_cast<double>(csp) / tau));
    int t0 = csp % tau;
    int t1 = tau - t0;
    return std::make_tuple(t0, k0, t1, k1);
}

int vc_param(int csp, int tau) {
    if (tau <= 0) {
         // Handle error consistently with _vc_param
         return 0; // Or throw
    }
    auto [t0, k0, t1, k1] = _vc_param(csp, tau); // Structured binding (C++17)
    // Use long long for intermediate calculation to prevent potential overflow
    long long topen_ll = static_cast<long long>(t0) * k0 + static_cast<long long>(t1) * k1;

    // Check for overflow before casting back to int (optional)
    // if (topen_ll > std::numeric_limits<int>::max() || topen_ll < std::numeric_limits<int>::min()) {
    //     // Handle overflow
    //     return -1; // Or throw
    // }
    return static_cast<int>(topen_ll);
}
