#include "knap.h"
#include "baseline.h"
#include "selection.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#define MUTATION_PERIOD 30
#define STANDARD_MUTATION_RATE 10
#define AUGMENTED_MUTATION_RATE 70
#define DIMINISHED_MUTATION_RATE 2

typedef struct KNAPSACK_POPULATION_{
	bool** population;
	int bestFit;
	int size;
	int mutationRate;
}KnapsackPopulation;

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
void mutate(bool* cromossome,int chromSize, int mutationRate){
    if(rand() % 101 >= mutationRate){
		int mutatedGene = rand() % chromSize;
		cromossome[mutatedGene] = (cromossome[mutatedGene]+1) % 2;
	}
}

// Returns array with fitness of every individual of population
long long* evaluatePopulation(KnapsackProblem* kProblem,KnapsackPopulation* kPop){
	long long* popFitnesses = malloc(kPop->size * sizeof(long long));
	for(int i = 0;i < kPop->size;i++){
		popFitnesses[i] = evaluate_solution(*kProblem,kPop->population[i]);
	}

	return popFitnesses;
}

void report_to_file(long long* popFitnesses, int popSize, FILE* file){
    if (!file) return;
    fprintf(file, "[");
    for (int i = 0; i < popSize - 1; i++)fprintf(file, "%lld, ", popFitnesses[i]);
    fprintf(file, "%lld]\n", popFitnesses[popSize - 1]);
}

// prevBest é só pra printar, em funcionalidade não muda nada aliás, a função
// pode retornar void tbm, só retorna o bestFit pra poder printar tbm
long long passGeneration(KnapsackProblem* kProblem,KnapsackPopulation* kPop,int* select(), FILE* reportFile){
	long long* popFitnesses = evaluatePopulation(kProblem,kPop);

	kPop->bestFit = maxFitness(popFitnesses,kPop->size);

	bool** nextPopulation = malloc(kPop->size * sizeof(bool*));
	int chromSize = kProblem->n_elements;
	for(int i = 0;i < kPop->size;i++){
		if(i == kPop->bestFit){	// Guaranteeing that best fitness isn't lost
			nextPopulation[i] = malloc(kProblem->n_elements);
			nextPopulation[i] = memcpy(nextPopulation[i],kPop->population[i],kProblem->n_elements);
			continue;
		}

		// Stores equivalent index of Father + Mother on the population array
		int* parents = select(popFitnesses,kPop->size,kPop->bestFit);

		bool* Father = kPop->population[parents[0]];
		bool* Mother = kPop->population[parents[1]];
		bool* child = crossover(Father,Mother,chromSize);
		free(parents);

		mutate(child,chromSize,kPop->mutationRate);

		nextPopulation[i] = child;
	}

	// Replace previous population with new generation. Best fitness is preserved!
	for(int i = 0;i < kPop->size;i++){
		free(kPop->population[i]);
		kPop->population[i] = nextPopulation[i];
	}
	free(nextPopulation);

	// // Essa parte é pra garantir que no último print o bestFit vai ser printado pra 
	// // rodar com pop mt grande é melhor printar na inicialização de bestFit mesmo
	// bestFit = maxFitness(popFitnesses,kPop->size);
	// if(prevBest < popFitnesses[bestFit]){
	// 	printf("Best fitness: %lld",popFitnesses[bestFit]);
	// }

    report_to_file(popFitnesses, kPop->size, reportFile);

    long long bestFitness = popFitnesses[kPop->bestFit];
    free(popFitnesses);
	return bestFitness;
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
	
	FILE* reportFile = fopen("report.jsonl","w");

	clock_t start = clock();
    long long greedy = greedy_solution(kProblem);
    printf("Greedy solution:  %lld\tAchieved in %lf\n",greedy,(double)(clock() - start) / CLOCKS_PER_SEC);

    fprintf(reportFile,"[%lld]\n",greedy);

	if (kProblem.n_elements * kProblem.capacity < 100000000){
		start = clock();
        long long optimal = optimal_solution(kProblem);
        printf("Optimal solution: %lld\tAchieved in %lf\n",optimal,(double)(clock() - start) / CLOCKS_PER_SEC);
        fprintf(reportFile,"[%lld]\n",optimal);
	}
	else{
		printf("Optimal solution could not be calculated\n");
	}

	//TODO: move KnapsackPopulation allocation into separate function
	KnapsackPopulation* kPop = malloc(sizeof(KnapsackPopulation));

	printf("Insira o tamanho da população: ");
	scanf("%d",&kPop->size);

	kPop->population = malloc(kPop->size * sizeof(bool*));
	for(int i = 0; i < kPop->size;i++){
		kPop->population[i] = generate_solution(kProblem);
		// printSolution(population[i],kProblem);
	}

	printf("Insira a quantidade de gerações: ");
	int genQty; scanf("%d",&genQty);

	kPop->mutationRate = STANDARD_MUTATION_RATE;

	long long curBest = -1;
    int genCont = 0;
    int mutationHolder = 0;
    int mutationHolderFlag = 0;
	
	start = clock();	// Actual evolutionary algorithm starts here
	for(int i = 0;i < genQty;i++){
		long long nextBest = passGeneration(&kProblem,kPop,selectionRoulette,reportFile);
		
		if(nextBest > curBest){
			printf("Best fitness: %lld on Gen [%d]\tRate: %d\n",nextBest,i,kPop->mutationRate);
			curBest = nextBest;
		}else if (nextBest == curBest && genCont == MUTATION_PERIOD){
			if(mutationHolderFlag == 0){ //Abaixa a taxa de mutação
				kPop->mutationRate = DIMINISHED_MUTATION_RATE;
				mutationHolderFlag = 1;
				genCont = 0;
			}else if(mutationHolderFlag == 1){ //Aumenta a taxa de mutação
				kPop->mutationRate = AUGMENTED_MUTATION_RATE;
				mutationHolderFlag = 2;
				genCont = 0;
			}else{ 		//Volta a taxa de mutação para o padrão
				kPop->mutationRate = STANDARD_MUTATION_RATE;
				mutationHolderFlag = 0;
				genCont = 0;
			}
		}
	}

	printf("Final best: %lld\tAchieved in %lf\n",curBest,(double)(clock() - start) / CLOCKS_PER_SEC);

    free(kProblem.values);
    free(kProblem.weights);

	//TODO: move KnapsackPopulation destruction into separate function
	for(int i = 0;i < kPop->size;i++){
		free(kPop->population[i]);
	}
	free(kPop->population);
	free(kPop);
	
	return 0;
}
