/*
 * IOHandler.cpp
 *
 *  Created on: 30 jul. 2017
 *      Author: colosu
 */

#include <iostream>
#include <string>
#include <fst/fstlib.h>
#include "Graph.h"
#include "IOHandler.h"

namespace fst {

IOHandler::IOHandler() {

}

IOHandler::~IOHandler() {

}

Graph* IOHandler::readGraph(std::string file) {

	Graph* g = new Graph();
	g->setTransducer(StdMutableFst::Read(file));
	if (g->getTransducer() == NULL) {
		std::cerr << "Unable to load the automaton." << std::endl;
		return NULL;
	}
	return g;

}

void IOHandler::print(list<Node> TS, ofstream &OFile) {
	list<Node> aux = TS;
	int size = aux.size();
	for (int i = 0; i < size; i++) {
		OFile << "Test " << to_string(i) << ":" << endl;
		print(aux.front(), OFile);
		aux.pop_front();
	}
}

void IOHandler::print(Node T, ofstream &OFile) {
	string result = "";
	Node* aux = &T;
	while (aux->getNext() != NULL) {
		result = aux->getInput();
		result += " --> ";
		result += aux->getOutput();
		OFile << result << endl;
		aux = aux->getNext();
	}
}

} /* namespace std */
