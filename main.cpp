/*
 * main.cpp
 *
 *  Created on: 19 sept. 2017
 *      Author: colosu
 */

#include <iterator>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <pthread.h>
#include <semaphore.h>
#include <fst/fst-decl.h>
#include <fst/fstlib.h>
#include <chrono>
#include "src/SqueezinessLib.h"
#include <unistd.h>
#include <stdexcept>

using namespace fst;

#define REP 50
#define MUT 1000
#define EXP 100
#define INI 0

void generate(string ifile, int len, bool max, string ff);
void compare(int len, bool max1, string ff1, bool max2, string ff2);
void wayOfUse();

int main(int argc, char* argv[]) {

	srand(time(NULL));

	if (argc > 1) {
		if (strcmp(argv[1], "generate") == 0) {
			if (argc >= 6) {
				bool max = true;
				if (strcmp(argv[4], "min") == 0) {
					max = false;
				}
				int len = atoi(argv[3]);
				if (len <= 0) {
					cerr << "Non-valid test suite length." << endl;
					return 1;
				}
				try {
					generate(argv[2], len, max, argv[5]);
				} catch (const runtime_error &e) {
					cerr << "Exception: " << e.what() << endl;
					return 1;
				} catch (const exception &e) {
					cerr << "Exception: " << e.what() << endl;
					return 1;
				}
			} else {
				cerr << "Not enough parameters." << endl;
				return 1;
			}
		} else if (strcmp(argv[1], "compare") == 0) {
			if (argc >= 7) {
				bool max1 = true;
				bool max2 = true;
				if (strcmp(argv[3], "min") == 0) {
					max1 = false;
				}
				if (strcmp(argv[5], "min") == 0) {
					max2 = false;
				}
				int len = atoi(argv[2]);
				if (len <= 0) {
					cerr << "Non-valid test suite length." << endl;
					return 1;
				}
				try {
					compare(len, max1, argv[4], max2, argv[6]);
				} catch (const runtime_error &e) {
					cerr << "Exception: " << e.what() << endl;
					return 1;
				} catch (const exception &e) {
					cerr << "Exception: " << e.what() << endl;
					return 1;
				}
			} else {
				cerr << "Not enough parameters." << endl;
				return 1;
			}
		}else {
			wayOfUse();
		}
	} else {
		wayOfUse();
	}

	return 0;
}

void generate(string ifile, int len, bool max, string ff) {

	IOHandler* IOH = new IOHandler();
	Operations* Ops = new Operations();

	std::string Ifile = ifile;
	std::string Ofile = "TestSuite.txt";

	std::ofstream OFile;

	Graph* G;
	list<Node> TS;

	G = IOH->readGraph(Ifile);

	if (G == NULL) {
		throw runtime_error("Couldn't read the graph.");
	}

	try {
		if (ff == "random") {
			Ops->GenerateRandomTestSuite(G, len, TS, false);
		} else {
			Ops->GenerateGeneticTestSuite(G, len, TS, max, ff);
		}
	} catch (const runtime_error &e) {
		throw e;
	} catch (const exception &e) {
		throw e;
	}

	OFile.open(Ofile);
	if (!OFile.is_open()) {
		throw runtime_error("Couldn't create the output file.");
	}

	IOH->print(TS, OFile);

	OFile.close();

	delete IOH;
	delete Ops;
}

void compare(int len, bool max1, string ff1, bool max2, string ff2) {

	//Initialization
	IOHandler* IOH = new IOHandler();
	Mutations* Mutator = new Mutations();
	Checkups* Checker = new Checkups();
	Operations* Ops = new Operations();

	std::string Ifile = "binary.fst";
	std::string Ofile = "Results.txt";

	std::ofstream OFile;

	OFile.open(Ofile);
	if (!OFile.is_open()) {
		throw runtime_error("Couldn't create the output file.");
	}

	OFile << "| #Test | Percentage of success " + ff1 + " | Percentage of success " + ff2 + " | percentage of killed mutants by " + ff1 + " | percentage of killed mutants by " + ff2 + " |" << std::endl;


	Graph* G;
	double wins;
	long double killed1;
	long double killed2;
	double valid;
	list<Node>* TS[2];
	Graph* GM[MUT];
	bool detected[2][MUT];
	int count[2];
	long double meanKilled1 = 0;
	long double meanKilled2 = 0;
	double mean = 0;
	double total = 0;

	for (int J = 0; J < REP; J++) {

		wins = 0;
		valid = 0;
		killed1 = 0;
		killed2 = 0;

		for (int I = INI; I < INI + EXP; I++) {

			Ifile = "./Tests/test" + to_string(I+1) + "/binary.fst";
//			Ifile = "./Tests/Phone/binary.fst";
//			Ifile = "./War of the Worlds/binary.fst";

			G = IOH->readGraph(Ifile);

			if (G == NULL) {
				throw runtime_error("Couldn't read the graph.");
			}

			if (!Checker->is_valid(G)) {
				throw runtime_error("Non-valid graph.");
			}

			try {
				//Generate Test Suites
				for (int i = 0; i < 2; i++) {
					TS[i] = new list<Node>();
				}

				if (ff1 == "random") {
					Ops->GenerateRandomTestSuite(G, len, *TS[0], false);
				} else {
					Ops->GenerateGeneticTestSuite(G, len, *TS[0], max1, ff1);
				}

				if (ff2 == "random") {
					Ops->GenerateRandomTestSuite(G, len, *TS[1], false);
				} else {
					Ops->GenerateGeneticTestSuite(G, len, *TS[1], max2, ff2);
				}

				//Generate Mutants
				for (int i = 0; i < MUT; i++) {
					GM[i] = Mutator->mutateState(G);
					while (!Checker->is_validMutation(GM[i])) {
						delete GM[i];
						GM[i] = Mutator->mutateState(G);
					}
				}

				//Check Fail Detection
				for (int i = 0; i < 2; i++) {
					for (int j = 0; j < MUT; j++) {
						detected[i][j] = Checker->checkMutation(GM[j], *TS[i]);
					}
				}

				//Delete test suites
				for (int i = 0; i < 2; i++) {
					delete TS[i];
				}

				//Delete mutants
				for (int i = 0; i < MUT; i++) {
					delete GM[i];
				}

				//Count fail detection
				for (int i = 0; i < 2; i++) {
					count[i] = 0;
				}
				for (int i = 0; i < 2; i++) {
					for (int j = 0; j < MUT; j++) {
						if(detected[i][j]) {
							count[i]++;
						}
					}
				}

				//Check if our measure detected the best test suite
				if (count[0] > count[1]) {
					wins++;
				}
				if (count[0] != count[1]) {
					valid++;
				}
				killed1 += (double)count[0]/MUT;
				killed2 += (double)count[1]/MUT;

			} catch (const runtime_error &e) {
				throw e;
			} catch (const exception &e) {
				throw e;
			}

			delete G;
			cout << "FSM " << I << endl;
		}

		cout << "test " << to_string(J+1) << endl;
		if (valid != 0) {
			OFile << J+1 << " & " << wins/valid << " & " << 1-(wins/valid) << " & " << killed1/valid << " & " << killed2/valid << " \\\\" << std::endl;
		} else {
			OFile << J+1 << " & ?\\% \\\\" << std::endl;
		}
		OFile << "\\hline" << std::endl;

		mean += wins;
		meanKilled1 += killed1;
		meanKilled2 += killed2;
		total += valid;
	}

	OFile << "Mean & " << mean/total << "\\% & " << 1-(mean/total) << "\\% & " << meanKilled1/total << "\\% & " << meanKilled2/total << "\\% \\\\" << std::endl;
	OFile << "\\hline" << std::endl;

	OFile.close();

	delete IOH;
	delete Mutator;
	delete Checker;
	delete Ops;
}

void wayOfUse() {
	cout << "Way of use:" << endl;
	cout << "gptsg generate inputFile length {max|min} fitness" << endl;
	cout << "gptsg compare length {max|min} fitness {max|min} fitness" << endl;
	cout << "fitness can be: BMI | ITSDm | OTSDm | IOTSDm | Own | random" << endl;
}
