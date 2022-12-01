#include "KDA.h"
#include "KNA.h"
#include "EKNA.h"
#include <iostream>
#include <fstream>
#include <memory>
using namespace std;

int main()
{
	setlocale(LC_ALL, "Rus");
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	EKNA automat;


	std::ifstream in("input.txt");

	//automat.Prepare(in);
	string first_stroka = " ";
	getline(cin, first_stroka);

	string second_stroka = " ";
	getline(cin, second_stroka);

	string third_stroka = " ";
	getline(cin, third_stroka);

	string fourth_stroka = " ";
	getline(cin, fourth_stroka);

	int count;
	cin >> count;

	std::string startState, symbol, endState;

	for (int i = 0; i < count; i++) 
	{
		std::cin >> startState >> symbol >> endState;
	}

	

	int numRecords;
	in >> numRecords;
	for (int i = 0; i < numRecords; ++i)
	{
		std::string stateKey, symbolKey, stateValue;
		in >> stateKey >> symbolKey >> stateValue;
		auto pairTable = std::make_pair(Key(stateKey, symbolKey), stateValue);
		std::shared_ptr<std::pair<Key, std::string>> arg(new std::pair<Key, std::string>(pairTable));
		automat.Insert(arg.get());
	}

	automat.HandleWord();
	automat.DisplayTable();

	KNA kna = automat.Eclose();
	kna.HandleWord();
	kna.DisplayTable();

	KDA kda = kna.AlgorithmTompson();
	kda.HandleWord();
	kda.DisplayTable();
}
