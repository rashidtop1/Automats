#ifndef KDA_H
#define KDA_H

#include "IAutomat.h"
#include <map>
#include <set>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <iterator>
#include <unordered_set>

class KDA : public IAutomat 
{
	private:
		std::map<Key, std::string> table;
		std::unordered_set<std::string> states;
		std::set<std::string> symbols;
		std::set<std::string> accessStates;
		std::vector<std::string> wordVector;

	public:
		KDA() = default;
		KDA(std::map<Key, std::string> _table, \
			std::unordered_set<std::string> _states, \
			std::set<std::string> _symbols, \
			std::set<std::string> _accessStates, \
			std::vector<std::string> _wordVector) : \
			table(_table), states(_states), symbols(_symbols), accessStates(_accessStates), wordVector(_wordVector) {}

		KDA& operator=(const KDA& kda)
		{
			table = kda.table;
			states = kda.states;
			symbols = kda.symbols;
			accessStates = kda.accessStates;

			return *this;
		}

		void Insert(void* arg) override
		{
			if (!arg) return;
			std::pair<Key, std::string>* castArg = (std::pair<Key, std::string>*)arg;
			states.insert(castArg->first.state);
			symbols.insert(castArg->first.symbol);
			table.insert(*castArg);
		}

		void Remove(void* arg) override 
		{
			if (!arg) return;
			Key* castArg = (Key *)arg;
			table.erase(*castArg);
		}

		void DisplayTable() override 
		{
			std::cout << "КДА" << std::endl;
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

				Key key;
				for (auto& symbol : symbols) 
				{
					key = Key(state, symbol);
					tp << (table.count(key) ? table[Key(state, symbol)] : "-");
				}
			}

			tp.PrintFooter();
		}

		void Prepare(std::ifstream &in) override 
		{
			std::string statesString;
			std::getline(in, statesString);

			std::istringstream issStates(statesString);
			states = std::unordered_set<std::string>((std::istream_iterator<std::string>(issStates)), std::istream_iterator<std::string>());

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

		void HandleWord() override 
		{
			std::string currentState = *states.begin();

			for (auto& wordElement : wordVector)
			{
				Key keyTable(currentState, wordElement);

				if (table.count(keyTable))
				{
					std::cout << "  " << keyTable.state << " --> " << table[keyTable] << " по символу " << keyTable.symbol << std::endl;
					currentState = table[keyTable];
					std::cout << "СОСТОЯНИЕ: " << currentState << std::endl;
				}
				else 
				{
					std::cout << " " << keyTable.state << "  по символу " << keyTable.symbol << std::endl;
					std::cout << "слово не проходит, недостижимая вершина" << std::endl;
					return;
				}
			}

			std::cout << (accessStates.count(currentState) ? "слово прошло" : "слово не проходит, недостижимая вершина") << std::endl;
		}
};

#endif