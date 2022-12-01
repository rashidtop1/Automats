#ifndef KNA_H
#define KNA_H

#include "IAutomat.h"
#include <map>
#include <set>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <queue>
#include "KDA.h"

class KNA : public IAutomat 
{
	private:
		std::map<Key, std::vector<std::string>> table;
		std::set<std::string> states;
		std::set<std::string> symbols;
		std::set<std::string> accessStates;
		std::vector<std::string> wordVector;

		bool isFoundGoodWord = false;

	public:
		KNA() = default;
		KNA(std::map<Key, std::vector<std::string>> _table, \
			std::set<std::string> _states, \
			std::set<std::string> _symbols, \
			std::set<std::string> _accessStates, \
			std::vector<std::string> _wordVector) : \
			table(_table), states(_states), symbols(_symbols), accessStates(_accessStates), wordVector(_wordVector) {}
		KNA& operator=(const KNA& kna)
		{
			table = kna.table;
			states = kna.states;
			symbols = kna.symbols;
			accessStates = kna.accessStates;
			wordVector = kna.wordVector;

			return *this;
		}

		void Insert(void* arg) override
		{
			if (!arg) return;
			std::pair<Key,std::string>* castArg = (std::pair<Key, std::string>*)arg;
			states.insert(castArg->first.state);
			symbols.insert(castArg->first.symbol);
			if (!table.count(castArg->first))
			{
				std::vector<std::string> tableValue;
				table[castArg->first] = tableValue;
			}

			table[castArg->first].push_back(castArg->second);
		}

		void Remove(void* arg) override
		{
			if (!arg) return;
			Key* castArg = (Key *)arg;
			table.erase(*castArg);
		}

		void DisplayTable() override
		{
			std::cout << "КНА" << std::endl;
			bprinter::TablePrinter tp(&std::cout);

			tp.AddColumn("СОСТОЯНИЕ", 10);

			for (auto& symbol : symbols)
			{
				tp.AddColumn(symbol, 10);
			}

			tp.PrintHeader();

			for (auto& state : states)
			{
				if (state == *states.begin())
				{
					if (accessStates.count(state)) tp << "->*" + state;
					else tp << "->" + state;
				}
				else if (accessStates.count(state)) tp << "*" + state;
				else tp << state;

				for (auto& symbol : symbols)
				{
					if (!table.count(Key(state, symbol)))
					{
						tp << "-";
						continue;
					}

					std::string tableValue = "";
					for (auto& vecElement : table[Key(state, symbol)])
					{
						tableValue += vecElement;
						tableValue += ',';
					}

					tp << tableValue;
				}
			}

			tp.PrintFooter();
		}

		void Prepare(std::ifstream& in) override
		{
			std::string statesString;
			std::getline(in, statesString);

			std::istringstream issStates(statesString);
			states = std::set<std::string>((std::istream_iterator<std::string>(issStates)), std::istream_iterator<std::string>());

			std::string symbolsString;
			std::getline(in, symbolsString);

			std::istringstream issSymbols(symbolsString);
			symbols = std::set<std::string>((std::istream_iterator<std::string>(issSymbols)), std::istream_iterator<std::string>());

			std::string accessStatesString;
			std::getline(in, accessStatesString);

			std::istringstream issAccessStates(accessStatesString);
			accessStates = std::set<std::string>((std::istream_iterator<std::string>(issAccessStates)), std::istream_iterator<std::string>());

			std::string wordString;
			std::getline(in, wordString);

			std::istringstream issWord(wordString);
			wordVector = std::vector<std::string>((std::istream_iterator<std::string>(issWord)), std::istream_iterator<std::string>());
		}

		bool CheckWord(std::string state, int index)
		{
			if (index >= wordVector.size())
			{
				if (accessStates.count(state))
				{
					std::cout << "слово прошло" << std::endl;
					isFoundGoodWord = true;
				}
				else
				{
					std::cout << "слово не проходит" << std::endl;
				}

				return true;
			}

			return false;
		}

		void HandleWordReccursion(std::string currentState, int index)
		{
			if (CheckWord(currentState, index)) return;
			Key key(currentState, wordVector[index]);

			if (table.count(key))
			{
				for (auto& state : table[key])
				{
					if (isFoundGoodWord) return;
					std::cout << " " << currentState << " --> " << state << " по символу " << wordVector[index] << std::endl;
					std::cout << "состояние: " << state << std::endl;
					HandleWordReccursion(state, index + 1);
				}
			}
			else
			{
				std::cout << " " << currentState << " по символу " << wordVector[index] << std::endl;
				std::cout << "недостижимая вершина" << std::endl;
				return;
			}
		}

		void HandleWord() override
		{
			std::string currentState = *states.begin();
			int index = 0;
			isFoundGoodWord = false;
			HandleWordReccursion(currentState, 0);
		}

		std::string SetToString(std::set<std::string> states)
		{
			std::string res = "";
			for (auto& state : states)
			{
				res += state;
				res += ' ';
			}

			return res;
		}

		KDA AlgorithmTompson()
		{
			std::queue<std::set<std::string>> queueStates;
			std::unordered_set<std::string> queueNewStates;
			std::map<Key, std::string> newTable;
			std::set<std::string> startState{ *states.begin() };

			queueStates.push(startState);
			queueNewStates.insert(SetToString(startState));

			while (!queueStates.empty())
			{
				auto currentStates = queueStates.front();
				queueStates.pop();

				for (auto& symbol : symbols)
				{
					std::set<std::string> translateCurrentStatesBySymbol;
					Key key;
					for (auto& currentState : currentStates)
					{
						key = Key(currentState, symbol);
						if (!table.count(key)) continue;
						for (auto& state : table[key])
						{
							translateCurrentStatesBySymbol.insert(state);
						}
					}

					// не учитываются недостижимые переходы из начального состояния
					if (translateCurrentStatesBySymbol.empty()) continue;
					std::string _translateCurrentStatesBySymbol = SetToString(translateCurrentStatesBySymbol);

					newTable.insert(std::make_pair(Key(SetToString(currentStates), symbol), SetToString(translateCurrentStatesBySymbol)));

					if (!queueNewStates.count(_translateCurrentStatesBySymbol))
					{
						queueStates.push(translateCurrentStatesBySymbol);
						queueNewStates.insert(_translateCurrentStatesBySymbol);
					}
				}
			}

			std::set<std::string> accessNewStates;
			for (auto& state : queueNewStates)
			{
				std::istringstream iss(state);
				std::set<std::string> splitStatesString = std::set<std::string>((std::istream_iterator<std::string>(iss)), std::istream_iterator<std::string>());
				for (auto& splitState : splitStatesString)
				{
					if (accessStates.count(splitState)) accessNewStates.insert(state);
				}
			}

			return KDA(newTable, queueNewStates, symbols, accessNewStates, wordVector);
		}
};

#endif