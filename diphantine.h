#ifndef SIMPLE_GENETIC_ALGORITHM_DIPHANTINE_H
#define SIMPLE_GENETIC_ALGORITHM_DIPHANTINE_H

#include <stdlib.h>
#include <time.h>

#define MAXPOP 50
#define ITER_NUM 1000

struct gene {
    int alleles[4];
    int fitness;
    float likelihood;

    // Test for equality.
    bool operator == (gene gn) {
        for (int i = 0; i < 4; i++) {
            if (gn.alleles[i] != alleles[i]) {
                return false;
            }
        }
        return true;
    }
};

class CDiophantine {
public:
    CDiophantine(int, int, int, int, int);  // Constructor with coefficients.

    int Solve();

    gene GetGene(int i) {
        return population[i];
    }
protected:
    int ca, cb, cc, cd;  // The coefficients.
    int result;
    gene population[MAXPOP];  // Population.

    int Fitness(gene &);  // Fitness function.
    void GenerateLikelihoods();  // Generate likelihoods.
    float MultInv();  // Creates the multiplicative inverse.
    int CreateFitnesses();
    void CreateNewPopulation();
    int GetIndex(float val);

    gene Breed(int p1, int p2);
};

CDiophantine::CDiophantine(int a, int b, int c, int d, int res) : ca(a), cb(b), cc(c), cd(d), result(res) {}

int CDiophantine::Solve() {
    int fitness = -1;

    // Generate initial population.
    srand((unsigned)time(NULL));

    for (int i = 0; i < MAXPOP; i++) {  // Fill the population with numbers between.
        for (int j = 0; j < 4; j++) {  // 0 and the result.
            population[i].alleles[j] = rand() % (result + 1);
        }
    }

    fitness = CreateFitnesses();
    if (fitness) {
        return fitness;
    }

    int iterations = 0;  // Keep record of the iterations.
    while (fitness != 0 || iterations < ITER_NUM) {  // Repeat until solution found, or over 50 iterations.
        GenerateLikelihoods();  // Create the likelihoods.
        CreateNewPopulation();
        fitness = CreateFitnesses();
        if (fitness) {
            return fitness;
        }
        iterations++;
    }

    return -1;
}

int CDiophantine::Fitness(gene &gn) {
    int total = ca * gn.alleles[0] + cb * gn.alleles[1] + cc * gn.alleles[2] + cd * gn.alleles[3];

    return gn.fitness = abs(total - result);
}

int CDiophantine::CreateFitnesses() {
    float avgfit = 0;
    int fitness = 0;

    for (int i = 0; i < MAXPOP; i++) {
        fitness = Fitness(population[i]);
        avgfit += fitness;
        if (fitness == 0) {
            return i;
        }
    }

    return 0;
}

float CDiophantine::MultInv() {
    float sum = 0;

    for(int i = 0; i < MAXPOP; i++) {
        sum += 1 / ((float)population[i].fitness);
    }

    return sum;
}

void CDiophantine::GenerateLikelihoods() {
    float multinv = MultInv();
    float last = 0;

    for (int i = 0; i < MAXPOP; i++) {
        population[i].likelihood = last = last + ((1 / ((float)population[i].fitness) / multinv) * 100);
    }
}

int CDiophantine::GetIndex(float val) {
    float last = 0;
    for (int i = 0; i < MAXPOP; i++) {
        if (last <= val && val <= population[i].likelihood) {
            return i;
        }
        else {
            last = population[i].likelihood;
        }

        return 4;
    }
}

gene CDiophantine::Breed(int p1, int p2) {
    int crossover = rand() % 3+1;// Create the crossover point (not first).
    int first = rand() % 100;// Which parent comes first?

    gene child = population[p1];// Child is all first parent initially.

    int initial = 0, final = 3;// The crossover boundaries.
    if (first < 50) initial = crossover;	// If first parent first. start from crossover.
    else final = crossover+1;// Else end at crossover.

    for(int i=initial;i<final;i++) {// Crossover!
        child.alleles[i] = population[p2].alleles[i];
        if (rand() % 101 < 5) child.alleles[i] = rand() % (result + 1);
    }

    return child;// Return the kid...
}

void CDiophantine::CreateNewPopulation() {
    gene temppop[MAXPOP];

    for(int i=0;i<MAXPOP;i++) {
        int parent1 = 0, parent2 = 0, iterations = 0;
        while(parent1 == parent2 || population[parent1] == population[parent2]) {
            parent1 = GetIndex((float)(rand() % 101));
            parent2 = GetIndex((float)(rand() % 101));
            if (++iterations > 25) break;
        }

        temppop[i] = Breed(parent1, parent2);// Create a child.
    }

    for(int i = 0;i < MAXPOP; i++) {
        population[i] = temppop[i];
    }
}

#endif //SIMPLE_GENETIC_ALGORITHM_DIPHANTINE_H
