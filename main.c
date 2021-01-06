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

typedef struct KNAPSACK_POPULATION_{
	bool** population;
	int bestFit;
	int size;
	int mutationRate;
}KnapsackPopulation;

KnapsackPopulation* generatePopulation(KnapsackProblem* kProblem,int size){
	KnapsackPopulation* kPop = malloc(sizeof(KnapsackPopulation));

	kPop->size = size;
	kPop->bestFit = 0;
	kPop->mutationRate = standard_mutation_rate;

	kPop->population = malloc(kPop->size * sizeof(bool*));
	for(int i = 0; i < kPop->size;i++){
		kPop->population[i] = generate_solution(*kProblem);
	}

	return kPop;
}

// Replace the index-iest chromosome (worst chromosome) with a random generated one
void randomPredation(KnapsackPopulation* kPop, int index){
	for (int i=0; i<kPop->size; i++){
		kPop->population[index][i] = 0;
	}
}

void delPopulation(KnapsackPopulation* kPop){
	for(int i = 0;i < kPop->size;i++){
		free(kPop->population[i]);
	}
	free(kPop->population);

	free(kPop);
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
    if(rand() % 101 <= mutationRate){
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
long long passGeneration(KnapsackProblem* kProblem,KnapsackPopulation* kPop,int* select(), FILE* reportFile, int *genContPredation){
	long long* popFitnesses = evaluatePopulation(kProblem,kPop);

	kPop->bestFit = maxFitness(popFitnesses,kPop->size);

	if (flag_predacao){
		if((*genContPredation) == predation_period){
			*genContPredation = 0;
			randomPredation(kPop, minFitness(popFitnesses, kPop->size));		
			popFitnesses = evaluatePopulation(kProblem, kPop);
		}

		(*genContPredation)++;
	}

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

long long evolutiveSolution(KnapsackProblem* kProblem, KnapsackPopulation* kPop, int genQty,FILE* reportFile){
	kPop->mutationRate = standard_mutation_rate;

	long long curBest = -1;
    int genContMutation = 0;
    int mutationHolderFlag = 0;
    int *genContPredation = (int*) malloc(sizeof(int));
    *genContPredation = 0; 
	
	for(int i = 0;i < genQty;i++){
		long long nextBest = passGeneration(kProblem,kPop,selection,reportFile, genContPredation);
		
		if(nextBest > curBest){
			printf("Best fitness: %lld on Gen [%d]\tRate: %d\n",nextBest,i,kPop->mutationRate);
			curBest = nextBest;
		} else if (nextBest == curBest){
			genContMutation++;
			if(genContMutation == mutation_period){
				if(mutationHolderFlag == 0){ //Abaixa a taxa de mutação
					kPop->mutationRate = diminished_mutation_rate;
					mutationHolderFlag = 1;
					genContMutation = 0;
				} else if(mutationHolderFlag == 1){ //Aumenta a taxa de mutação
					kPop->mutationRate = augmented_mutation_rate;
					mutationHolderFlag = 2;
					genContMutation = 0;
				} else{ 	//Volta a taxa de mutação para o padrão
					kPop->mutationRate = standard_mutation_rate;
					mutationHolderFlag = 0;
					genContMutation = 0;
				}
			}
		} else if (nextBest != curBest){
			genContMutation = 0;
		} 
	}

	return curBest;
}

int main(int argc, char const *argv[]){
    int popSize = 0, genQty = 0;

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
            {"mutation-period", required_argument, 0, 'X'},
            {"mutation-rate", required_argument, 0, 'm'},
            {"augmented-rate", required_argument, 0, 'A'},
            {"diminished-rate", required_argument, 0, 'D'},

            {"population_size", required_argument, 0, 'P'},
            {"generations", required_argument, 0, 'G'},

            {0,0,0,0}
        };

        flag_predacao = 1;
        flag_mutacao_variavel = 1;

        int opt = 0, c;
        c = getopt_long(argc, (char *const *) argv, "n:c:C:v:V:w:W:S:s:p:P:m:A:D:X:G:", long_options, &opt);

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
            case 'X':
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
            case 'P':
                popSize = atoi(optarg);
                break;
            case 'G':
                genQty = atoi(optarg);
                break;
            default:
                break;
                
        }
    }

    if (seed == -1) seed = time(NULL);
	srand(seed);
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

	if (popSize == 0){
	    printf("Insira o tamanho da população: ");
	    scanf("%d",&popSize);
    }

	KnapsackPopulation* kPop = generatePopulation(&kProblem,popSize);

    if (genQty == 0){
        printf("Insira a quantidade de gerações: ");
        scanf("%d",&genQty);
    }

	start = clock();
	long long evolutive = evolutiveSolution(&kProblem,kPop,genQty,reportFile);
	printf("Final best: %lld\tAchieved in %lf\n",evolutive,(double)(clock() - start) / CLOCKS_PER_SEC);

	fclose(reportFile);

    free(kProblem.values);
    free(kProblem.weights);
	
	delPopulation(kPop);

	return 0;
}
