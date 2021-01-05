#include "knap.h"
#include "baseline.h"
#include "selection.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <getopt.h>

int predation_period = 100;
int mutation_period = 100;
int standard_mutation_rate = 10;
int augmented_mutation_rate = 70;
int diminished_mutation_rate = 2;

int n_elements = 1000;
int min_capacity = 1;
int max_capacity = 1000;
int min_value = 1;
int max_value = 1000;
int min_weight = 1; 
int max_weight = 1000;
int flag_predacao = 0;
int flag_mutacao_variavel = 0;
int flag_verbose;
int seed = -1;
int* (*selection)(long long*, int, int) = &selectionRoulette;

// Replace the index-iest chromosome (worst chromosome) with a random generated one
void randomPredation(bool** population, int popSize, int index){
	for (int i=0; i<popSize; i++){
		population[index][i] = (bool)(rand()%2);
	}
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

// Returns index of inidividual with worst fitness on fitnesses
int minFitness(long long* fitnesses,int fitnessQty){
	int worstFit = 0;	
	for(int i = 1;i < fitnessQty;i++){
		if(fitnesses[i] < fitnesses[worstFit]){
			worstFit = i;
		}
	}

	return worstFit;
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
long long* evaluatePopulation(KnapsackProblem* kProblem,bool** population,int popSize){
	long long* popFitnesses = malloc(popSize * sizeof(long long));
	for(int i = 0;i < popSize;i++){
		popFitnesses[i] = evaluate_solution(*kProblem,population[i]);
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
long long passGeneration(KnapsackProblem* kProblem,bool** population,int popSize,int* select(),long long prevBest, int mutationRate, FILE* report_file, int *genContPredation, int *genContMutation, int *mutationHolderFlag, int flag_predacao, int flag_mutacao_variavel){
	long long* popFitnesses = evaluatePopulation(kProblem,population,popSize);

	int bestFit = maxFitness(popFitnesses,popSize);

	if((*genContPredation) == predation_period && flag_predacao){
		*genContPredation = 0;
		randomPredation(population, popSize, minFitness(popFitnesses,popSize));		
		popFitnesses = evaluatePopulation(kProblem,population,popSize);
	}

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
		mutate(child,chromSize,mutationRate);

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

	report_to_file(popFitnesses, popSize, report_file);

    long long nextBest = popFitnesses[bestFit];
    free(popFitnesses);

    if (nextBest == prevBest && *genContMutation == mutation_period && flag_mutacao_variavel){
		if(*mutationHolderFlag == 0){ //Abaixa a taxa de mutação
			mutationRate = diminished_mutation_rate;
			*mutationHolderFlag = 1;
			*genContMutation = 0;
		} else if (*mutationHolderFlag == 1){ //Aumenta a taxa de mutação
			mutationRate = augmented_mutation_rate;
			*mutationHolderFlag = 2;
			*genContMutation = 0;
		} else { //Volta a taxa de mutação para o padrão
			mutationRate = standard_mutation_rate;
			*mutationHolderFlag = 0;
			*genContMutation = 0;
		}
	} else if (nextBest != prevBest){
		*genContMutation = 0;
	}

	if(flag_predacao){
		(*genContPredation)++;
	}

	if(flag_mutacao_variavel){
	    (*genContMutation)++;
	}

	return nextBest; // so pra printar
}


// Só pro print da população inicial
void printSolution(bool* solution,KnapsackProblem kProblem){
    if (!flag_verbose) return;
	printf("Sol: [ %d",solution[0]);
	for(size_t i = 1; i < kProblem.n_elements; i++){
		printf(", %d",solution[i]);
	}
	printf("] Fitness: %lld\n",evaluate_solution(kProblem,solution));
}


int main(int argc, char const *argv[]){

    while (1){
        static struct option long_options[] = {
            {"verbose", no_argument, &flag_verbose, 1},
            {"quiet", no_argument, &flag_verbose, 0},

            {"n-elements", required_argument, 0, 'n'},
            {"min-capacity", required_argument, 0, 'c'},
            {"max-capacity", required_argument, 0, 'C'},
            {"min-value", required_argument, 0, 'v'},
            {"max-value", required_argument, 0, 'V'},
            {"min-weight", required_argument, 0, 'w'},
            {"max-weight", required_argument, 0, 'W'},
            
            {"selection", required_argument, 0, 'S'},
            {"predation", no_argument, &flag_predacao, 1},
            {"ada-mutation", no_argument, &flag_mutacao_variavel, 1},
            {"seed", required_argument, 0, 's'},

            {"predation-period", required_argument, 0, 'p'},
            {"mutation-period", required_argument, 0, 'P'},
            {"mutation-rate", required_argument, 0, 'm'},
            {"augmented-rate", required_argument, 0, 'A'},
            {"diminished-rate", required_argument, 0, 'D'},

            {0,0,0,0}
        };

        int opt = 0, c;
        c = getopt_long(argc, (char *const *) argv, "n:c:C:v:V:w:W:S:s:p:P:m:A:D:", long_options, &opt);

        if (c == -1) break;

        switch(c){
            case 'n':
                n_elements = atoi(optarg);
                break;
            case 'c':
                min_capacity = atoi(optarg);
                break;
            case 'C':
                max_capacity = atoi(optarg);
                break;
            case 'v':
                min_value = atoi(optarg);
                break;
            case 'V':
                max_value = atoi(optarg);
                break;
            case 'w':
                min_weight = atoi(optarg);
                break;
            case 'W':
                max_weight = atoi(optarg);
                break;
            case 's':
                seed = atoi(optarg);
                break;
            case 'S':
                if (strcmp(optarg, "elitismo") == 0){
                    selection = &selectionElitism;
                }else if (strcmp(optarg, "torneio") == 0){
                    selection = &selectionTourneyOfTwo;
                }else if (strcmp(optarg, "roleta") == 0){
                    selection = &selectionRoulette;
                }else{
                    printf("Invalid selection method\n");
                    return 0;
                }
                break;
            case 'p':
                predation_period = atoi(optarg);
                break;
            case 'P':
                mutation_period = atoi(optarg);
                break;
            case 'm':
                standard_mutation_rate = atoi(optarg);
                break;
            case 'A':
                augmented_mutation_rate = atoi(optarg);
                break;
            case 'D':
                diminished_mutation_rate = atoi(optarg);
                break;
            default:
                break;
                
        }
    }

    if (seed == -1) seed = time(NULL);
	srand(seed);
	KnapsackProblem kProblem = generate_problem(n_elements,min_capacity,max_capacity,
												min_value,max_value,min_weight,max_weight);
	
	printf("Insira o tamanho da população: ");
	int popSize; scanf("%d",&popSize);

	bool** population = malloc(popSize * sizeof(bool*));
	for(int i = 0; i < popSize;i++){
		population[i] = generate_solution(kProblem);
		printSolution(population[i],kProblem);
	}

    FILE* report_file = fopen("report.jsonl", "w");

    long long greedy = greedy_solution(kProblem);
    printf("Greedy solution:  %lld\n", greedy);
    fprintf(report_file, "[%lld]\n", greedy);

    if (kProblem.n_elements * kProblem.capacity < 100000000){
        long long opt = optimal_solution(kProblem);
        printf("Optimal solution: %lld\n", optimal_solution(kProblem));
        fprintf(report_file, "[%lld]\n", opt);
    }else
        printf("Optimal solution could not be calculated\n");

	printf("Insira a quantidade de gerações: ");
	int genQty; scanf("%d",&genQty);

    int mutationRate = standard_mutation_rate;

	long long curBest = -1;
    int *genContPredation = (int*) malloc(sizeof(int));
    int *genContMutation = (int*) malloc(sizeof(int));
    int *mutationHolderFlag = (int*) malloc(sizeof(int));
    *mutationHolderFlag = 0;
    *genContPredation = 0;
    *genContMutation = 0;

	for(int i = 0;i < genQty;i++){
		long long nextBest = passGeneration(&kProblem,population,popSize,selection, curBest, mutationRate, report_file, genContPredation, genContMutation, mutationHolderFlag, flag_predacao, flag_mutacao_variavel);
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
