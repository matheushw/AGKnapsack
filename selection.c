#include <stdio.h>
#include <stdlib.h>


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


int binary_search(int begin, int end, double r, double* acc){
    if (begin == end)
        return begin;
    
    int meio = (begin + end) / 2;
    if (r >= acc[meio] && r < acc[meio+1]){
        return meio;
    }else if (r < acc[meio]){
        return binary_search(begin, meio, r, acc);
    }else{
        return binary_search(meio+1, end, r, acc);
    }
}

int* selectionRoulette(long long* fitnesses, int popSize, int bestFit){
    long long min = 9999999999;
    long long total = 0;
    double* acc = malloc(sizeof(double) * popSize);
    acc[0] = 0.;

    for (int i = 0; i < popSize; i++) if (fitnesses[i] < min) min = fitnesses[i];
    for (int i = 0; i < popSize; i++) total += (fitnesses[i] - min);
    
    for (int i = 1; i < popSize; i++) acc[i] = acc[i-1] + (double) (fitnesses[i-1] - min) / total;
    
	int* parents = malloc(2 * sizeof(int));

    double r1 = rand() / (RAND_MAX + 1.);
    double r2 = rand() / (RAND_MAX + 1.);

    parents[0] = binary_search(0, popSize - 1, r1, acc);
    parents[1] = binary_search(0, popSize - 1, r2, acc);
    free(acc);

    return parents;
}
