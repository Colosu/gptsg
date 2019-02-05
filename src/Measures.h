/*
 * Measures.h
 *
 *  Created on: 28 ene. 2019
 *      Author: colosu
 */

#ifndef SRC_MEASURES_H_
#define SRC_MEASURES_H_

#include <fst/fstlib.h>
#include "Graph.h"
#include "Node.h"

namespace fst {

class Measures {
public:
	Measures();
	virtual ~Measures();
	static void MutualInformation(Graph* g, list<Node> TS, double &FF);
	static void InputTestSetDiameter(Graph* g, list<Node> TS, double &FF);
	static void OutputTestSetDiameter(Graph* g, list<Node> TS, double &FF);
	static void InputOutputTestSetDiameter(Graph* g, list<Node> TS, double &FF);
	static void OwnFunction(Graph* g, list<Node> TS, double &FF);

private:
	static double MutualInformation(map<IOpair, int>* IOmap, Node T1, Node T2);
	static double MutualInformation(map<IOpair, int>* IOmap, IOpair T);
	static string to_St(list<Node> TS, string kind);
	static string to_St(Node TS, string kind);
};

} /* namespace fst */

#endif /* SRC_MEASURES_H_ */
