#include "knap.h"
#include "baseline.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

// Returns indexes of Father and Mother selected by tourney of two
int* selectionTourneyOfTwo(long long* fitnesses,int popSize,int bestFit){
	int firstFather = rand() % popSize;
	int firstMother = rand() % popSize;

	int secondFather = rand() % popSize;
	while(secondFather == firstFather){
		secondFather = rand() % popSize;
	}

	int secondMother = rand() % popSize;
	while(secondMother == firstMother){
		secondMother = rand() % popSize;
	}

	int Father = fitnesses[firstFather] > fitnesses[secondFather]? firstFather : secondFather;
	int Mother = fitnesses[firstMother] > fitnesses[secondMother]? firstMother : secondMother;

	int* parents = malloc(2 * sizeof(int));
	parents[0] = Father;
	parents[1] = Mother;
	return parents;
}

// Returns indexes of bestFit and random Parent that is not best Fit
int* selectionElitism(long long* fitnesses,int popSize,int bestFit){
	int parent = rand() % popSize;
	while(parent == bestFit){
		parent = rand() % popSize;
	}

	int* parents = malloc(2 * sizeof(int));
	parents[0] = bestFit;
	parents[1] = parent;
	return parents;
}

// Returns index of inidividual with best fitness on fitnesses
int maxFitness(long long* fitnesses,int fitnessQty){
	int bestFit = 0;	
	for(int i = 1;i < fitnessQty;i++){
		if(fitnesses[i] > fitnesses[bestFit]){
			bestFit = i;
		}
	}

	return bestFit;
}

// Mix genes of Father and Mother one by one. Randomly selects initial parent
bool* crossover(bool* Father,bool* Mother,int chromSize){
	bool* child = malloc(chromSize * sizeof(bool));
	
	bool* parent[2];
	parent[0] = Father;
	parent[1] = Mother;

	for(int i = 0;i < chromSize;i++){
        child[i] = parent[rand() % 2][i];
	}

	return child;
}

// Binary -> Flips a random gene in cromossome
//TODO: use external mutation rate
//TODO: maybe implement variable mutation rate?
void mutate(bool* cromossome,int chromSize){
	int mutation = rand() % 101;

	if(mutation >= 10){	//PLACEHOLDER: 90% mutation chance
		int mutatedGene = rand() % chromSize;
		cromossome[mutatedGene] = (cromossome[mutatedGene]+1) % 2;
	}
}

// Returns array with fitness of every individual of population
long long* evaluatePopulation(KnapsackProblem* kProblem,bool** population,int popSize){
	long long* popFitnesses = malloc(popSize * sizeof(long long));
	for(int i = 0;i < popSize;i++){
		popFitnesses[i] = evaluate_solution(*kProblem,population[i]);
	}

	return popFitnesses;
}

// prevBest é só pra printar, em funcionalidade não muda nada aliás, a função
// pode retornar void tbm, só retorna o bestFit pra poder printar tbm
long long passGeneration(KnapsackProblem* kProblem,bool** population,int popSize,int* select(),long long prevBest){
	long long* popFitnesses = evaluatePopulation(kProblem,population,popSize);

	int bestFit = maxFitness(popFitnesses,popSize);

	bool** nextPopulation = malloc(popSize * sizeof(bool*));
	int chromSize = kProblem->n_elements;
	for(int i = 0;i < popSize;i++){
		if(i == bestFit){	// Guaranteeing that best fitness isn't lost
			nextPopulation[i] = malloc(kProblem->n_elements);
			nextPopulation[i] = memcpy(nextPopulation[i],population[i],kProblem->n_elements);
			continue;
		}

		// Stores equivalent index of Father + Mother on the population array
		int* parents = select(popFitnesses,popSize,bestFit);

		bool* Father = population[parents[0]];
		bool* Mother = population[parents[1]];
		bool* child = crossover(Father,Mother,chromSize);
		mutate(child,chromSize);

		nextPopulation[i] = child;
		free(parents);
	}

	// Replace previous population with new generation. Best fitness is preserved!
	for(int i = 0;i < popSize;i++){
		free(population[i]);
		population[i] = nextPopulation[i];
	}
	free(nextPopulation);

	// Essa parte é pra garantir que no último print o bestFit vai ser printado pra 
	// rodar com pop mt grande é melhor printar na inicialização de bestFit mesmo
	bestFit = maxFitness(popFitnesses,popSize);
	if(prevBest < popFitnesses[bestFit]){
		printf("Best fitness: %lld",popFitnesses[bestFit]);
	}

    long long v = popFitnesses[bestFit];
    free(popFitnesses);
	return v; // so pra printar
}

// Só pro print da população inicial
void printSolution(bool* solution,KnapsackProblem kProblem){
	printf("Sol: [ %d",solution[0]);
	for(size_t i = 1; i < kProblem.n_elements; i++){
		printf(", %d",solution[i]);
	}
	printf("] Fitness: %lld\n",evaluate_solution(kProblem,solution));
}

int main(int argc, char const *argv[]){
	if(argc != 8){
		printf("Passe os seguintes parâmetros: <n_elementos> <capacidade_min>"
		" <capacidade_max> <val_min> <val_max> <peso_min> <peso_max>\n");
		return 0;
	}

	srand(time(NULL));

	int n_elements = atoi(argv[1]);
    int min_capacity = atoi(argv[2]);
	int max_capacity = atoi(argv[3]);
    int min_value = atoi(argv[4]);
	int max_value = atoi(argv[5]);
    int min_weight = atoi(argv[6]);
	int max_weight = atoi(argv[7]);

	KnapsackProblem kProblem = generate_problem(n_elements,min_capacity,max_capacity,
												min_value,max_value,min_weight,max_weight);
	
	printf("Insira o tamanho da população: ");
	int popSize; scanf("%d",&popSize);

	bool** population = malloc(popSize * sizeof(bool*));
	for(int i = 0; i < popSize;i++){
		population[i] = generate_solution(kProblem);
		printSolution(population[i],kProblem);
	}

    printf("Greedy solution:  %lld\n", greedy_solution(kProblem));
    if (kProblem.n_elements * kProblem.capacity < 100000000)
        printf("Optimal solution: %lld\n", optimal_solution(kProblem));
    else
        printf("Optimal solution could not be calculated\n");

	printf("Insira a quantidade de gerações: ");
	int genQty; scanf("%d",&genQty);

	long long curBest = -1;
	for(int i = 0;i < genQty;i++){
		long long nextBest = passGeneration(&kProblem,population,popSize,selectionElitism,curBest);
		if(nextBest > curBest){
			printf(" on Gen [%d]\n",i);
			curBest = nextBest;
		}
	}

	printf("Final pop: \n");
	for(int i = 0; i < popSize;i++){
		printf("\t");
		printSolution(population[i],kProblem);
        free(population[i]);
	}
	
    free(kProblem.values);
    free(kProblem.weights);
    free(population);
	return 0;
}
