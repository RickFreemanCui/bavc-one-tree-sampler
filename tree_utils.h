#ifndef TREE_UTILS_H
#define TREE_UTILS_H

#include <vector>
#include <map>
#include <utility> // For std::pair
#include <cmath>   // For std::floor, std::log2, std::pow, std::ceil
#include <numeric> // For std::accumulate
#include <tuple>   // For std::tuple
#include <optional> // For decrease_config return

// Define Config as a type alias for clarity
using Config = std::vector<std::pair<int, int>>;
using ConfigMap = std::map<int, int>;
using Distribution = std::map<Config, double>;
using Histogram = std::vector<std::pair<int, double>>;

// Function declarations corresponding to the Python code

/**
 * @brief Calculates the depth of a node in a binary tree (root at index 1).
 * @param index The index of the node.
 * @return The depth of the node (root depth is 0).
 */
int get_depth(int index);

/**
 * @brief Calculates the left and right bounds of indices at a given depth within a subtree.
 * @param root_index The index of the subtree root.
 * @param depth The relative depth within the subtree.
 * @return A pair containing the left and right bounds (inclusive).
 */
std::pair<int, int> get_lr_bound(int root_index, int depth);

/**
 * @brief Checks if a leaf node is within the subtree rooted at root_index.
 * @param root_index The index of the subtree root.
 * @param leaf_index The index of the leaf node to check.
 * @return True if leaf_index is in the subtree, False otherwise.
 */
bool in_subtree(int root_index, int leaf_index);

/**
 * @brief Creates a sorted configuration from a list of (leaf_size, num) pairs.
 * @param leaf_size_num_list A vector of pairs (subtree_size, num_subtree).
 * @return A sorted Config (vector of pairs).
 */
Config make_config(const std::vector<std::pair<int, int>>& leaf_size_num_list);

/**
 * @brief Converts a configuration tuple (vector of pairs) to a map.
 * @param config_tuple The configuration represented as a sorted vector of pairs.
 * @return A map where keys are subtree sizes and values are counts.
 */
ConfigMap config_tuple_to_dict(const Config& config_tuple);

/**
 * @brief Converts a configuration map back to a sorted tuple (vector of pairs).
 * @param config_dict The configuration represented as a map.
 * @return A sorted Config (vector of pairs).
 */
Config config_dict_to_tuple(const ConfigMap& config_dict);

/**
 * @brief Adds two configurations together.
 * @param config1 The first configuration.
 * @param config2 The second configuration.
 * @return The resulting configuration after adding counts for matching subtree sizes.
 */
Config add_config(const Config& config1, const Config& config2);

/**
 * @brief Decreases the count of a specific subtree size in a configuration.
 * @param config The configuration to modify.
 * @param num_leaf The subtree size whose count should be decreased.
 * @return An optional containing the modified configuration if successful, or std::nullopt if the size doesn't exist.
 */
std::optional<Config> decrease_config(const Config& config, int num_leaf);

/**
 * @brief Calculates the histogram of the total number of nodes from a distribution.
 * @param dist A map where keys are configurations and values are probabilities.
 * @return A histogram represented as a vector of pairs (total_nodes, probability).
 */
Histogram get_hist(const Distribution& dist);

/**
 * @brief Calculates the expected number of nodes from a histogram.
 * @param hist The histogram (vector of pairs: total_nodes, probability).
 * @return The expected number of nodes.
 */
double expect_pnodes(const Histogram& hist);

/**
 * @brief Rounds a number up to the nearest multiple of 8.
 * @param n The number to round.
 * @return The rounded number.
 */
int round_to_byte(double n);

/**
 * @brief Helper function for calculating VC parameters.
 * @param csp Parameter csp.
 * @param tau Parameter tau.
 * @return A tuple containing (t0, k0, t1, k1).
 */
std::tuple<int, int, int, int> _vc_param(int csp, int tau);

/**
 * @brief Calculates the Topen parameter based on csp and tau.
 * @param csp Parameter csp.
 * @param tau Parameter tau.
 * @return The calculated Topen value.
 */
int vc_param(int csp, int tau);

#endif // TREE_UTILS_H
