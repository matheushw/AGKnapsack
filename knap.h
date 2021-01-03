#include <stdbool.h>
#pragma once

// Estrutura representando uma instância do problema knapsack
typedef struct {
    int n_elements; // Número de elementos
    long long * weights;   // Vetor de pesos dos objetos
    long long * values;    // Vetor de valores dos objetos
    long long capacity;    // Capacidade máxima da mochila

} KnapsackProblem;




// Retorna o fitness da solução para o problema proposto
// Se o peso total da solução for maior que a capacidade, retorna -1
long long evaluate_solution (KnapsackProblem p, bool* solution);



// Cria uma instância aleatória do problema com os parâmetros dados
KnapsackProblem generate_problem (
        int n_elements,
        int min_capacity, int max_capacity,
        int min_value, int max_value,
        int min_weight, int max_weight);


// Gera uma solução aleatória para o problema
bool* generate_solution(KnapsackProblem p);
