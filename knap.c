#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "knap.h"

typedef long long ll;


// Retorna o fitness da solução para o problema proposto
// Se o peso total da solução for maior que a capacidade, retorna -1
ll evaluate (KnapsackProblem p, bool* solution){

    ll value = 0;
    ll weight = 0;

    for (int i = 0; i < p.n_elements; i++){
        if (solution[i]){
            value += p.values[i];
            weight += p.weights[i];
        }
    
        if (weight > p.capacity) return -1;
    }
    
    return value;
}

    
// Cria uma instância aleatória do problema com os parâmetros dados
KnapsackProblem generate_problem(int n_elements, int min_capacity, int max_capacity, int min_value, int max_value, int min_weight, int max_weight){

    KnapsackProblem p;

    p.n_elements = n_elements;
    p.weights = (ll*) malloc(sizeof(ll) * n_elements);
    p.values = (ll*) malloc(sizeof(ll) * n_elements);
    p.capacity = min_capacity + (rand() % (max_capacity - min_capacity)); 

    for (int i = 0; i < n_elements; i++){
        p.weights[i] = min_weight + (rand() % (max_weight - min_weight));
        p.values[i] = min_value + (rand() % (max_value - min_value));
    }

    return p;
}

// Gera uma solução aleatória para o problema
bool* generate_solution(KnapsackProblem p){
    bool* s = malloc(sizeof(bool) * p.n_elements);

    for (int i = 0; i < p.n_elements; i++){
        s[i] = rand() % 2;
    }
    return s;
}
