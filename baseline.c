#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "knap.h"
#include "baseline.h"

typedef long long ll;

// Comparing function for qsort_r
int compare_indices(const void* a, const void* b, void* value_density){
    double va = ((double*) value_density)[*((int*)a)];      
    double vb = ((double*) value_density)[*((int*)b)];      

    if (va <= vb) return 1;
    if (va > vb) return -1;
}

// Returns the value of the greedy solution
ll greedy_solution (KnapsackProblem p){

    // Calculates the value density of each object
    double* value_density = (double*) malloc (sizeof (double) * p.n_elements);
    

    int* indices = (int*) malloc (sizeof (int) * p.n_elements);
    

    for (int i = 0; i < p.n_elements; i++){
        value_density[i] = (double) p.values[i] / (double) p.weights[i];
        indices[i] = i;
    }

    // Sorts the indices according to their value density
    qsort_r(indices, p.n_elements, sizeof(int), compare_indices, value_density);
    free(value_density);
    

    // Finds a solution by taking the objects in their order of density
    bool* solution = calloc(p.n_elements, sizeof(bool));
    long long solution_size = 0;

    for (int i = 0; i < p.n_elements; i++){
        if (p.weights[indices[i]] + solution_size > p.capacity) continue;
        solution[indices[i]] = true;
        solution_size += p.weights[indices[i]];
    }
    free(indices);
    long long score = evaluate_solution(p, solution);
    free(solution);
    return score;
}


// Recursive function to solve the knapsack problem using dynamic programming
ll solve(int index, ll weight, KnapsackProblem p, ll** dp){
    
    // Creates a DP table on the first call of the function
    if (dp == 0){
        dp = malloc(sizeof(ll*) * p.n_elements);
        for (int i = 0; i < p.n_elements; i++){
            dp[i] = malloc(sizeof(ll) * (1 + p.capacity));
            for (int j = 0; j < p.capacity + 1; j++) dp[i][j] = -1;
        }
    }
    

    // Verifies if the current arguments are invalid
    if (index >= p.n_elements) return 0;
    if (weight > p.capacity) return 0;

    // Verifies if the current arguments have already been calculated
    if (dp[index][weight] != -1) return dp[index][weight];
    
    // Continues the function calls recursively
    ll v1 = solve(index + 1, weight,  p, dp);

    ll v2 = -1;
    if (p.weights[index] + weight <= p.capacity)
        v2 = solve(index + 1, weight + p.weights[index], p, dp) + p.values[index];
    
    // Verifies which path was better
    if (v1 > v2){
        dp[index][weight] = v1;
    }else{
        dp[index][weight] = v2;
    }

    // If we have done all steps, delete the table
    if (index == 0){
        for (int i = 0; i < p.n_elements; i++) free(dp[i]);
        free(dp);
        
        if (v1 > v2){
            return v1;
        }else{
            return v2;
        }
    }

    return dp[index][weight];
}

ll optimal_solution(KnapsackProblem p){
    return solve(0, 0, p, 0);
}
