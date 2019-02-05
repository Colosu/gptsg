/*
 * Checkups.cpp
 *
 *  Created on: 29 jul. 2017
 *      Author: colosu
 */

#include <fst/fstlib.h>
#include "Graph.h"
#include "Checkups.h"
#include "Operations.h"
#include "Node.h"
#include <unistd.h>
#include <time.h>

namespace fst {

Checkups::Checkups() {

}

Checkups::~Checkups() {

}

bool Checkups::are_equivalent(Graph* g1, Graph* g2) {

	if (Equivalent(*(g1->getTransducer()->Copy()), *(g2->getTransducer()->Copy()))) {
		cout << "are equal" << endl;
		return true;
	} else {
		return false;
	}
}

bool Checkups::is_valid(Graph* g) {

	if (g->getTransducer()->Properties(kIDeterministic, true) == kIDeterministic) {
		if (g->getTransducer()->Properties(kAccessible, true) == kAccessible) {
			if (g->getTransducer()->Properties(kCoAccessible, true) == kCoAccessible) {
				return true;
			}
		}
	}
	return false;
}

bool Checkups::is_validMutation(Graph* g) {

	if (g->getTransducer()->Properties(kIDeterministic, true) == kIDeterministic) {
		//if (g->getTransducer()->Properties(kCoAccessible, true) == kCoAccessible) {
			return true;
		//}
	}
	return false;
}

bool Checkups::checkMutation(Graph* g, list<Node> TS) {

	bool detected = false;
	list<Node>::iterator it = TS.begin();
	while (!detected && it != TS.end()) {
		detected = detected || checkMutation(g, &(*it));
		it++;
	}
	return detected;
}

bool Checkups::checkMutation(Graph* g, Node* T) {

	bool detected = false;
	StdMutableFst* transducer = g->getTransducer()->Copy(true);
	int state = transducer->Start();
	int oldState = state;
	int size = T->getDepth();
	int i = 0;
	ArcIterator<StdMutableFst>* arcIter = NULL;
	while (!detected && i < size) {
		arcIter = new ArcIterator<StdMutableFst>(*transducer, state);
		while (!detected && state == oldState && !arcIter->Done()) {
			if (to_string(arcIter->Value().ilabel) == T->getInput()) {
				if (to_string(arcIter->Value().olabel) == T->getOutput()) {
					state = arcIter->Value().nextstate;
					T = T->getNext();
				}
			}
			if(state == oldState) {
				arcIter->Next();
			}
		}
		if(arcIter->Done()) {
			detected = true;
		} else {
			oldState = state;
		}
		i++;
		delete arcIter;
		arcIter = NULL;
	}
	delete transducer;
	return detected;
}

} /* namespace std */
