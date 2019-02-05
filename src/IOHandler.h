/*
 * IOHandler.h
 *
 *  Created on: 30 jul. 2017
 *      Author: colosu
 */

#ifndef IOHANDLER_H_
#define IOHANDLER_H_

#include <string>
#include "Graph.h"
#include "Node.h"

namespace fst {

class IOHandler {
public:
	IOHandler();
	~IOHandler();
	Graph* readGraph(std::string file);
	void print(list<Node> TS, ofstream &OFile);

private:
	void print(Node TS, ofstream &OFile);

};

} /* namespace std */

#endif /* IOHANDLER_H_ */
