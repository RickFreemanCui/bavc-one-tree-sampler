#ifndef SAMPLER_H
#define SAMPLER_H

#include "tree_utils.h" // Includes Config, Distribution, Histogram, etc.
#include <map>
#include <vector>

// Type alias for the dynamic programming cache used in sample
using DpCache = std::map<int, Distribution>;

/**
 * @brief Performs one step of the sampling process for a given number of leaves.
 * @param num_leaf The number of leaves in the current (sub)tree.
 * @return A Distribution representing the possible configurations and their probabilities after one split.
 */
Distribution sample_once(int num_leaf);

/**
 * @brief Performs the sampling process for a specified number of steps.
 * @param num_leaf The initial number of leaves.
 * @param steps The number of sampling steps to perform.
 * @return The final Distribution after the specified number of steps.
 */
Distribution sample(int num_leaf, int steps);

/**
 * @brief Calculates the histogram of node counts based on VC parameters and sampling.
 * @param csp Parameter csp.
 * @param tau Parameter tau.
 * @return A Histogram representing the distribution of total node counts.
 */
Histogram get_hist_randonetree(int csp, int tau);


#endif // SAMPLER_H
