/*
 * Operations.h
 *
 *  Created on: 29 jul. 2017
 *      Author: colosu
 */

#ifndef OPERATIONS_H_
#define OPERATIONS_H_

#include <semaphore.h>
#include <fst/fstlib.h>
#include "Graph.h"
#include "Node.h"

namespace fst {

typedef struct {
	StdMutableFst* fsm;
	int qid;
	int iter;
	int length;
	int* inputs;
	std::map<string, int>* mapOtoI;
	string output;
	sem_t* sem;
} args;

void* SqueezinessAux(void * argum);

class Operations {
public:
	Operations();
	~Operations();
	void minimization(Graph* g);
	StdMutableFst* product(Graph* g1, Graph* g2);
	void GenerateRandomTestSuite(Graph* g, int size, list<Node> &TS, bool repTests);
	void GenerateGeneticTestSuite(Graph* g, int size, list<Node> &TS, bool max, string ff);

private:
	int GenerateRandomTest(Graph* g, int size, Node *T);
	map<string, list<string>>* GenerateGrammar(Graph* g);
	void GenerateRandomGrammarTestSuite(map<string, list<string>>* grammar, int size, list<Node> &TS, bool repTests, string start);
	int GenerateRandomTest(map<string, list<string>>* grammar, int size, Node *T);
	void FitnessFunction(Graph* g, list<Node> TS, double &FF, string ff);
	bool StopCriterion(int epochs, int count);
	void Selection(list<Node> TS[], int genSize, bool max, double FF[], double prev);
	void Crossover(map<string, list<string>>* grammar, int size, list<Node> &TS1, list<Node> &TS2);
	Node* Mutation(map<string, list<string>>* grammar, int size, string start);
	bool repeated(Node* T, list<Node> TS);
	string getLabel(string generation);
};

} /* namespace std */

#endif /* OPERATIONS_H_ */
