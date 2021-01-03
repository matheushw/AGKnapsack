#include "knap.h"
#include <stdbool.h>
#pragma once

// Returns the value of an efficient greedy solution
// Space Complexity: O(N)
// Time Complexity: O(N log N)
long long greedy_solution(KnapsackProblem p);

// Returns the value of an optimal solution to the problem
// Space and Time Complexity: O(Capacity * N)
long long optimal_solution(KnapsackProblem p);
