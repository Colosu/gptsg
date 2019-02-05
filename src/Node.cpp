/*
 * Node.cpp
 *
 *  Created on: 30 dic. 2018
 *      Author: colosu
 */

#include "Node.h"
#include <string>

namespace fst {

Node::Node() {
	next = NULL;
	pare = NULL;
}

Node::Node(string lab) {
	label = lab;
	next = NULL;
	pare = NULL;
}

Node::Node(string lab, Node* pare) {
	label = lab;
	next = NULL;
	this->pare = pare;
}

Node::Node(const Node &node) {
	this->label = node.label;
	this->input = node.input;
	this->output = node.output;
	this->depth = node.depth;
	this->pare = NULL;
	if (node.next != NULL) {
//		if (this->next != NULL) {
//			delete this->next;
//		}
		this->next = new Node(*node.next);
		this->next->setPare(this);
	} else {
		this->next = NULL;
	}
}

Node::~Node() {
	if (this->next != NULL) {
		delete this->next;
	}
	this->next = NULL;
}

string Node::getInput() {
	return input;
}

void Node::setInput(string input) {
	this->input = input;
}

string Node::getLabel() {
	return label;
}

void Node::setLabel(string label) {
	this->label = label;
}

string Node::getOutput() {
	return output;
}

void Node::setOutput(string output) {
	this->output = output;
}

Node* Node::getNext() {
	return next;
}

void Node::setNext(Node* next) {
	this->next = next;
	this->next->setPare(this);
	updateDepth(next->getDepth() + 1);
}

Node* Node::getPare() {
	return pare;
}

void Node::setPare(Node* pare) {
	this->pare = pare;
}

int Node::getDepth() {
	return depth;
}

void Node::setDepth(int depth) {
	this->depth = depth;
}

void Node::setVals(string vals) {

	bool first = false;
	bool second = false;
	string nextLabel = "";
	for (long unsigned int i = 0; i < vals.length(); i++) {
		if (first && second) {
			nextLabel.append(1, vals.at(i));
		} else if (first && vals.at(i) == '|') {
			second = true;
		} else if (first) {
			output.append(1, vals.at(i));
		} else if (vals.at(i) == '|') {
			first = true;
		} else {
			input.append(1, vals.at(i));
		}
	}

	if (this->next == NULL) {
		next = new Node(nextLabel, this);
	}
	updateDepth(1);
}

void Node::operator=(const Node other) {
	this->label = other.label;
	this->input = other.input;
	this->output = other.output;
	this->depth = other.depth;
	if (other.next != NULL) {
		if (this->next != NULL) {
			delete this->next;
		}
		this->next = new Node(" ", this);
		*(this->next) = *(other.next);
	} else {
		if (this->next != NULL) {
			delete this->next;
		}
		this->next = NULL;
	}
}

bool Node::operator==(const Node other) const {
	bool result = false;

	if (this->depth == other.depth) {
		if (this->label == other.label) {
			if (this->input == other.input) {
				if (this->output == other.output) {
					if (this->next != NULL && other.next != NULL) {
						result = (*(this->next) == *(other.next));
					} else if (this->next == NULL && other.next == NULL) {
						result = true;
					}
				}
			}
		}
	}

	return result;
}

bool Node::operator!=(const Node other) const {
	return !(*this == other);
}

void Node::updateDepth(int depth) {
	this->depth = depth;
	if (pare != NULL) {
		this->pare->updateDepth(depth+1);
	}
}

} /* namespace fst */
