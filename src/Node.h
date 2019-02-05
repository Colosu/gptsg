/*
 * Node.h
 *
 *  Created on: 30 dic. 2018
 *      Author: colosu
 */

#ifndef SRC_NODE_H_
#define SRC_NODE_H_

namespace fst {

class Node {
public:
	Node();
	Node(string lab);
	Node(string lab, Node* pare);
	Node(const Node &node);
	~Node();
	string getInput();
	void setInput(string input);
	string getLabel();
	void setLabel(string label);
	string getOutput();
	void setOutput(string output);
	Node* getNext();
	void setNext(Node* next);
	Node* getPare();
	void setPare(Node* pare);
	int getDepth();
	void setDepth(int depth);
	void setVals(string vals);
	void operator=(const Node other);
	bool operator==(const Node other) const;
	bool operator!=(const Node other) const;
	void updateDepth(int depth);

private:
	string input = "";
	string output = "";
	string label = "";
	Node* next;
	Node* pare;
	int depth = 0;
};

} /* namespace fst */

#endif /* SRC_NODE_H_ */
