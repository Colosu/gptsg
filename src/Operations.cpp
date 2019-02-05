/*
 * Operations.cpp
 *
 *  Created on: 29 jul. 2017
 *      Author: colosu
 */

#include <string>
#include <float.h>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <pthread.h>
#include <semaphore.h>
#include <fst/fstlib.h>
#include <iostream>     // std::cout
#include <algorithm>    // std::shuffle
#include "Graph.h"
#include "Operations.h"
#include "Measures.h"

namespace fst {

Operations::Operations() {

}

Operations::~Operations() {

}

void Operations::minimization(Graph* g) {
	Minimize(g->getTransducer());
}

StdMutableFst* Operations::product(Graph* g1, Graph* g2) {
	StdMutableFst* a1 = g1->getTransducer()->Copy();
	StdMutableFst* a2 = g2->getTransducer()->Copy();
	//StdMutableFst* prod = Times(a1, a2);
	delete a1;
	delete a2;
	return NULL; //prod;
}

void Operations::GenerateRandomTestSuite(Graph* g, int size, list<Node> &TS, bool repTests) {

	int length = 0;
	int tam  = 0;
	Node* T = NULL;
	while (length < size) {
		do {
			if (T != NULL){
				delete T;
			}
			T = new Node("0");
			tam = GenerateRandomTest(g, size - length, T);
		} while (!repTests && repeated(T, TS));
		TS.push_back(*T);
		length += tam;
	}
	delete T;
}

void Operations::GenerateGeneticTestSuite(Graph* g, int size, list<Node> &TS, bool max, string ff) { //Tests

	int genSize = 100;
	list<Node> TSG[genSize];
	double FF[genSize];
	int pos[genSize];
	double prev = 0;
	double newer = 0;
	double epochs = 0;
	double FFBest = 0;
	double count = 0;
	list<Node> TSI;
	list<Node> TSM;
	Node* T = NULL;
	map<string, list<string>>* grammar;

	// Loop variables
	int TSISize = 0;
	int best = 0;

	if (!max) {
		FFBest = DBL_MAX;
	}

	grammar = GenerateGrammar(g);

	for (int i = 0; i < genSize; i++) {
		//Initialize population
		TSG[i] = list<Node>();
		GenerateRandomGrammarTestSuite(grammar, size, TSG[i], false, to_string(g->getTransducer()->Start()));
		//Evaluate population
		try {
			FitnessFunction(g, TSG[i], FF[i], ff);
		} catch (const runtime_error &e) {
			throw e;
		} catch (const exception &e) {
			throw e;
		}
	}

	//Main loop
	while (!StopCriterion(epochs, count)) {

		prev = newer;

		//Select next generation
		Selection(TSG, genSize, max, FF, prev);

		//Perform Crossover
		for (int i = 0; i < genSize; i++) {
			pos[i] = i;
		}
		random_shuffle(&pos[0], &pos[genSize]);

		for (int i = 0; i < genSize-1; i+=2) {
			if (((rand() % RAND_MAX) / (double)RAND_MAX) < 0.9) {
				Crossover(grammar, size, TSG[pos[i]], TSG[pos[i+1]]);
			}
		}

		//Perform Mutation
		for (int i = 0; i < genSize; i++) {
			TSI = TSG[i];
			TSM = list<Node>();
			TSISize = TSI.size();

			for (int j = 0; j < TSISize; j++) {
				if (((rand() % RAND_MAX) / (double)RAND_MAX) < 0.05) {
					T = Mutation(grammar, TSI.front().getDepth(), to_string(g->getTransducer()->Start()));
					TSM.push_back(*T);
					delete T;
					T = NULL;
				} else {
					TSM.push_back(TSI.front());
				}
				TSI.pop_front();
			}
			TSG[i] = TSM;
		}

		//Evaluate population
		for (int i = 0; i < genSize; i++) {
			FitnessFunction(g, TSG[i], FF[i], ff);
		}

		if (max) {
			newer = *max_element(&FF[0], &FF[genSize]);
			epochs++;
			if (prev == newer) {
				count++;
			} else {
				count = 0;
			}

			if (newer > FFBest) {
				best = 0;
				while(FF[best] < newer) {
					best++;
				}
				TS = TSG[best];
				FFBest = FF[best];
			}
		} else {
			newer = *min_element(&FF[0], &FF[genSize]);
			epochs++;
			if (prev == newer) {
				count++;
			} else {
				count = 0;
			}

			if (newer < FFBest) {
				best = 0;
				while(FF[best] > newer) {
					best++;
				}
				TS = TSG[best];
				FFBest = FF[best];
			}
		}
	}

	delete grammar;
}

int Operations::GenerateRandomTest(Graph* g, int size, Node* T) {

	StdMutableFst* transducer = g->getTransducer()->Copy(true);
	int length = 0;
	int state = transducer->Start();
	ArcIterator<StdMutableFst>* arcIter = NULL;
	while (length < size && transducer->NumArcs(state) != 0) {
		arcIter = new ArcIterator<StdMutableFst>(*transducer, state);
		arcIter->Seek(rand()%transducer->NumArcs(state));
		state = arcIter->Value().nextstate;
		T->setVals(to_string(arcIter->Value().ilabel) + "|" + to_string(arcIter->Value().olabel) + "|" + to_string(state));
		length++;
		T = T->getNext();
		delete arcIter;
		arcIter = NULL;
	}
	delete transducer;
	return length;
}

map<string, list<string>>* Operations::GenerateGrammar(Graph* g) {

	map<string, list<string>>* grammar = new map<string, list<string>>();
	StdMutableFst* transducer = g->getTransducer()->Copy(true);
	int state = transducer->Start();
	string input;
	string output;
	string next;
	while (state < transducer->NumStates()) {
		grammar->emplace(to_string(state), list<string>());
		for (ArcIterator<StdMutableFst> arcIter(*transducer, state); !arcIter.Done(); arcIter.Next()) {
			input = to_string(arcIter.Value().ilabel);
			output = to_string(arcIter.Value().olabel);
			next = to_string(arcIter.Value().nextstate);
			grammar->at(to_string(state)).push_back(input + "|" + output + "|" + next);
		}
		state++;
	}

	delete transducer;
	return grammar;
}

void Operations::GenerateRandomGrammarTestSuite(map<string, list<string>>* grammar, int size, list<Node> &TS, bool repTests, string start) {

	int length = 0;
	int tam  = 0;
	Node* T = NULL;
	while (length < size) {
		do {
			if (T != NULL){
				delete T;
			}
			T = new Node(start);
			tam = GenerateRandomTest(grammar, size - length, T);
		} while (!repTests && repeated(T, TS));
		TS.push_back(*T);
		length += tam;
	}
	delete T;
}

int Operations::GenerateRandomTest(map<string, list<string>>* grammar, int size, Node* T) {

	list<string> words;
	int length = 0;
	int lim = 0;
	int i = 0;
	bool dead = false;
	while (length < size && !dead) {
		words = list<string>(grammar->at(T->getLabel()));
		if (words.size() > 0) {
			lim = rand()%words.size();
			i = 0;
			while (i < lim) {
				words.pop_front();
				i++;
			}
			T->setVals(words.front());
			length++;
			T = T->getNext();
		} else {
			dead = true;
		}
	}
	return length;
}

void Operations::FitnessFunction(Graph* g, list<Node> TS, double &FF, string ff) {

	if (ff == "BMI") {
		Measures::MutualInformation(g, TS, FF);
	} else if (ff == "ITSDm") {
		Measures::InputTestSetDiameter(g, TS, FF);
	} else if (ff == "OTSDm") {
		Measures::OutputTestSetDiameter(g, TS, FF);
	} else if (ff == "IOTSDm") {
		Measures::InputOutputTestSetDiameter(g, TS, FF);
	} else if (ff == "Own") {
		Measures::OwnFunction(g, TS, FF);
	} else {
		throw runtime_error("Non-valid fitness function.");
	}
}

bool Operations::StopCriterion(int epochs, int count) {
	bool stop = false;

	if ((count > epochs*0.2 && epochs > 20) || epochs > 100) {
		stop = true;
	}

	return stop;
}

void Operations::Selection(list<Node> TS[], int genSize, bool max, double FF[], double prev) {

	list<Node> TSP[genSize];
	int pos[genSize];
	double mean = accumulate(&FF[0], &FF[genSize], 0.0)/(double)genSize;

	for (int i = 0; i < genSize; i++) {
		TSP[i] = TS[i];
	}

	if (max) {
		for (int i = 0; i < genSize; i++) {
			pos[i] = FF[i] >= mean - rand()%(int)(prev-mean+1) ? i : -1; //pos selection;
		}
	} else {
		for (int i = 0; i < genSize; i++) {
			pos[i] = FF[i] <= mean + rand()%(int)(mean-prev+1) ? i : -1; //pos selection;
		}
	}

	for (int i = 0; i < genSize; i++) {
		while (pos[i] < 0 || pos[i] > genSize) {
			pos[i] = rand()%genSize;
		}
	}
	for (int i = 0; i < genSize; i++) {
		TS[i] = TSP[pos[i]];
	}
}

//void Operations::Crossover(map<string, list<string>>* grammar, int size, list<Node> &TS1, list<Node> &TS2) {
//
//	Node* T1 = NULL;
//	Node* T2 = NULL;
//	Node* Taux = NULL;
//	string NT1[size];
//	int NT1depth[size];
//	string NT2[size];
//	int NT2depth[size];
//	int i = 0;
//	int Nnode = 0;
//	int Npare = 0;
//	int depth = 0;
//	string node;
//	string pare;
//	list<string> generations;
//	int len = 0;
//	bool valid[size];
//	int maxIter = size;
//	int iter = 0;
//	bool match = false;
//	int possize = 0;
//	int pos[size];
//	int Nselected = 0;
//	string selected;
//	int actDepth = 0;
//	string selected1;
//	string selected2;
//	bool finded;
//
//	//Get a list of all grammar symbols.
//	i = 0;
//	for (list<Node>::iterator it = TS1.begin(); it != TS1.end(); it++){
//		T1 = &(*it);
//		while(T1->getDepth() != 0) {
//			NT1[i] = T1->getLabel();
//			NT1depth[i] = T1->getDepth();
//			T1 = T1->getNext();
//			i++;
//		}
//	}
//	T1 = NULL;
//	i = 0;
//	for (list<Node>::iterator it = TS2.begin(); it != TS2.end(); it++){
//		T2 = &(*it);
//		while(T2->getDepth() != 0) {
//			NT2[i] = T2->getLabel();
//			NT2depth[i] = T2->getDepth();
//			T2 = T2->getNext();
//			i++;
//		}
//	}
//	T2 = NULL;
//
//	//Get a feasible substitution
//	iter = 0;
//	while (!match && iter < maxIter) {
//		//Get a random node (different from a start node) to interchange
//		Nnode = rand()%size;
//		node = NT1[Nnode];
//		while(node == TS1.front().getLabel()){
//			Nnode = rand()%size;
//			node = NT1[Nnode];
//		}
//		depth = NT1depth[Nnode];
//
//		//Get the node parent
//		Npare = Nnode - 1;
//		pare = NT1[Npare];
//
//		//Get the possible generations of the node parent
//		generations = list<string>(grammar->at(pare));
//
//		//Get the possible candidates to be interchanged
//		len = generations.size();
//		string candidates[len];
//
//		for (int i = 0; i < len; i++) {
//			candidates[i] = getLabel(generations.front());
//			generations.pop_front();
//		}
//
//		//Search for the candidates with the same length in the second test suite that can have as a son the initial node
//		for (int i = 0; i < size; i++) {
//			valid[i] = false;
//		}
//		for (int i = 0; i < size; i++) {
//			for (int j = 0; j < len; j++) {
//				if (NT2[i] == candidates[j] && NT2depth[i] == depth) {
//					generations = list<string>(grammar->at(NT2[i]));
//					finded = false;
//					while (!finded) {
//						if (getLabel(generations.front()) == node) {
//							selected1 = generations.front();
//							finded = true;
//						}
//						generations.pop_front();
//					}
//					if (finded) {
//						valid[i] = true;
//					}
//				}
//			}
//		}
//
//		//Check if there is at least one candidate
//		for (int i = 0; i < size; i++) {
//			match = match || valid[i];
//		}
//
//		iter++;
//	}
//
//	if (iter < maxIter) {
//		//Select a random valid candidate
//		possize = 0;
//		for (int i = 0; i < size; i++) {
//			if (valid[i]) {
//				pos[possize] = i;
//				possize++;
//			}
//		}
//
//		Nselected = pos[rand()%possize];
//		selected = NT2[Nselected];
//
//		//Perform the crossover
//		actDepth = 0;
//		list<Node>::iterator it1 = TS1.begin();
//		T1 = &(*it1);
//		while (actDepth + T1->getDepth() < Nnode+1){
//			actDepth += T1->getDepth();
//			it1++;
//			T1 = &(*it1);
//		}
//		while(T1->getDepth() > depth+1) {
//			T1 = T1->getNext();
//		}
//
//		actDepth = 0;
//		list<Node>::iterator it2 = TS2.begin();
//		T2 = &(*it2);
//		while (actDepth + T2->getDepth() < Nselected+1){
//			actDepth += T2->getDepth();
//			it2++;
//			T2 = &(*it2);
//		}
//		while(T2->getDepth() > depth+1) {
//			T2 = T2->getNext();
//		}
//
//		generations = list<string>(grammar->at(T1->getLabel()));
//		finded = false;
//
//		while (!finded) {
//			if (getLabel(generations.front()) == T2->getNext()->getLabel()) {
//				selected1 = generations.front();
//				finded = true;
//			}
//			generations.pop_front();
//		}
//
//		generations = list<string>(grammar->at(T2->getLabel()));
//		finded = false;
//
//		while (!finded) {
//			if (getLabel(generations.front()) == T1->getNext()->getLabel()) {
//				selected2 = generations.front();
//				finded = true;
//			}
//			generations.pop_front();
//		}
//
//		Taux = T1->getNext();
//		T1->setNext(T2->getNext());
//		T1->setVals(selected1);
//		T2->setNext(Taux);
//		T2->setVals(selected2);
//		T1 = NULL;
//		T2 = NULL;
//	}
//}

void Operations::Crossover(map<string, list<string>>* grammar, int size, list<Node> &TS1, list<Node> &TS2) {

	Node* T1 = NULL;
	Node* T2 = NULL;
	Node* Taux = NULL;
	string NT1[size];
	int NT1depth[size];
	string NT2[size];
	int NT2depth[size];
	int i = 0;
	int Nnode = 0;
	int depth = 0;
	string node;
	bool valid[size];
	int maxIter = size;
	int iter = 0;
	bool match = false;
	int possize = 0;
	int pos[size];
	int Nselected = 0;
	int actDepth = 0;

	//Get a list of all grammar symbols.
	i = 0;
	for (list<Node>::iterator it = TS1.begin(); it != TS1.end(); it++){
		T1 = &(*it);
		while(T1->getDepth() != 0) {
			NT1[i] = T1->getLabel();
			NT1depth[i] = T1->getDepth();
			T1 = T1->getNext();
			i++;
		}
	}
	T1 = NULL;
	i = 0;
	for (list<Node>::iterator it = TS2.begin(); it != TS2.end(); it++){
		T2 = &(*it);
		while(T2->getDepth() != 0) {
			NT2[i] = T2->getLabel();
			NT2depth[i] = T2->getDepth();
			T2 = T2->getNext();
			i++;
		}
	}
	T2 = NULL;

	//Get a feasible substitution
	iter = 0;
	while (!match && iter < maxIter) {
		//Get a random node (different from a start node) to interchange
		Nnode = rand()%size;
		node = NT1[Nnode];
		while(node == TS1.front().getLabel()){
			Nnode = rand()%size;
			node = NT1[Nnode];
		}
		depth = NT1depth[Nnode];

		//Search for the candidates with the same length in the second test suite that can have as a child the initial node
		for (int i = 0; i < size; i++) {
			valid[i] = false;
		}
		for (int i = 0; i < size; i++) {
			if (NT2[i] == node && NT2depth[i] == depth) {
				valid[i] = true;
			}
		}

		//Check if there is at least one candidate
		for (int i = 0; i < size; i++) {
			match = match || valid[i];
		}

		iter++;
	}

	if (iter < maxIter) {
		//Select a random valid candidate
		possize = 0;
		for (int i = 0; i < size; i++) {
			if (valid[i]) {
				pos[possize] = i;
				possize++;
			}
		}

		Nselected = pos[rand()%possize];

		//Perform the crossover
		actDepth = 0;
		list<Node>::iterator it1 = TS1.begin();
		T1 = &(*it1);
		while (actDepth + T1->getDepth() < Nnode+1){
			actDepth += T1->getDepth();
			it1++;
			T1 = &(*it1);
		}
		while(T1->getDepth() > depth+1) {
			T1 = T1->getNext();
		}

		actDepth = 0;
		list<Node>::iterator it2 = TS2.begin();
		T2 = &(*it2);
		while (actDepth + T2->getDepth() < Nselected+1){
			actDepth += T2->getDepth();
			it2++;
			T2 = &(*it2);
		}
		while(T2->getDepth() > depth+1) {
			T2 = T2->getNext();
		}

		Taux = T1->getNext();
		T1->setNext(T2->getNext());
		T2->setNext(Taux);
		T1 = NULL;
		T2 = NULL;
	}
}

Node* Operations::Mutation(map<string, list<string>>* grammar, int size, string start) {

	Node* T = NULL;
	int tam = 0;
	while(tam != size) {
		if (T != NULL){
			delete T;
		}
		T = new Node(start);
		tam = GenerateRandomTest(grammar, size, T);
	}
	return T;
}

bool Operations::repeated(Node* T, list<Node> TS) {

	bool repeat = false;
	list<Node>::iterator it = TS.begin();
	while(!repeat && it != TS.end()) {
		if(*it == *T) {
			repeat = true;
		}
		it++;
	}
	return repeat;
}

string Operations::getLabel(string generation) {

	bool first = false;
	bool second = false;
	string nextLabel = "";
	for (long unsigned int i = 0; i < generation.length(); i++) {
		if (first && second) {
			nextLabel.append(1, generation.at(i));
		} else if (first && generation.at(i) == '|') {
			second = true;
		} else if (generation.at(i) == '|') {
			first = true;
		}
	}

	return nextLabel;
}

} /* namespace std */
