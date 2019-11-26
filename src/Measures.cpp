/*
 * Measures.cpp
 *
 *  Created on: 28 ene. 2019
 *      Author: colosu
 */

#include "Measures.h"
#include "zlc/zlibcomplete.hpp"

namespace fst {

Measures::Measures() {}

Measures::~Measures() {}

void Measures::MutualInformation(Graph* g, list<Node> TS, double &FF) {

	FF = 0;
	list<Node> aux1 = TS;
	list<Node> aux2;
	int size1 = TS.size();
	int size2 = 0;
	for (int i = 0; i < size1; i++) {
		aux2 = aux1;
		size2 = aux2.size();
		for (int j = 0; j < size2; j++) {
			FF += MutualInformation(g->getIOmap(), aux1.front(), aux2.front());
			aux2.pop_front();
		}
		aux1.pop_front();
	}
}

void Measures::InputTestSetDiameter(Graph* g, list<Node> TS, double &FF) {
	zlibcomplete::ZLibCompressor* zlib = new zlibcomplete::ZLibCompressor();
	string orig = to_St(TS, "input");
	string comp = "";
	comp = zlib->compress(orig);
	FF = comp.size();
	delete zlib;
}

void Measures::OutputTestSetDiameter(Graph* g, list<Node> TS, double &FF) {
	zlibcomplete::ZLibCompressor* zlib = new zlibcomplete::ZLibCompressor();
	string orig = to_St(TS, "output");
	string comp = "";
	comp = zlib->compress(orig);
	FF = comp.size();
	delete zlib;
}

void Measures::InputOutputTestSetDiameter(Graph* g, list<Node> TS, double &FF) {
	zlibcomplete::ZLibCompressor* zlib = new zlibcomplete::ZLibCompressor();
	string orig = to_St(TS, "");
	string comp = "";
	comp = zlib->compress(orig);
	FF = comp.size();
	delete zlib;
}

void Measures::OwnFunction(Graph* g, list<Node> TS, double &FF) {
	//TODO: Implement your own fitness function.
	FF = 0;
}

double Measures::MutualInformation(map<IOpair, int>* IOmap, Node T1, Node T2) {

	double MI = 0;
	Node* aux1 = &T1;
	Node* aux3 = &T2;
	Node* aux2;
	IOpair IO;
	while (aux1->getNext() != NULL) {
		if (T1 == T2) {
			aux3 = aux3->getNext();
		}
		aux2 = aux3;
		while (aux2 != NULL) {
			if (aux1->getInput() == aux2->getInput() && aux1->getOutput() == aux2->getOutput()) {
				IO.setInput(aux1->getInput());
				IO.setOutput(aux1->getOutput());
				MI += MutualInformation(IOmap, IO);
			}
			aux2 = aux2->getNext();
		}
		aux1 = aux1->getNext();
	}

	aux1 = NULL;
	aux2 = NULL;
	aux3 = NULL;
	return MI;
}

double Measures::MutualInformation(map<IOpair, int>* IOmap, IOpair T) {

	double reps = IOmap->at(T);
	return std::log2(reps+1)/reps;
}

string Measures::to_St(list<Node> TS, string kind) {
	string result = "";
	list<Node> aux = TS;
	int size = aux.size();
	for (int i = 0; i < size; i++) {
		result += to_St(aux.front(), kind);
		aux.pop_front();
	}
	return result;
}

string Measures::to_St(Node T, string kind) {
	string result = "";
	Node* aux = &T;
	while (aux->getNext() != NULL) {
		if (kind == "input") {
			result += aux->getInput();
		} else if (kind == "output") {
			result += aux->getOutput();
		} else {
			result += aux->getInput();
			result += aux->getOutput();
		}
		aux = aux->getNext();
	}
	aux = NULL;
	return result;
}

} /* namespace fst */
