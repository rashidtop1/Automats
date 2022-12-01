#ifndef EKNA_H
#define EKNA_H

#include "IAutomat.h"
#include "KDA.h"
#include <map>
#include <set>
#include <string>
#include <vector>
#include <iostream>
#include <queue>
#include <Windows.h>
using namespace std;



class EKNA : public IAutomat
{
	private:
		std::map<Key, std::vector<std::string>> table;
		std::set<std::string> states;
		std::set<std::string> symbols;
		std::set<std::string> accessStates;
		std::vector<std::string> wordVector;
		std::vector<std::string > res;

		bool isFoundGoodWord = false;

	public:
		EKNA() = default;

		void Insert(void* arg) override
		{
			if (!arg) return;
			std::pair<Key, std::string>* castArg = (std::pair<Key, std::string>*)arg;
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
			Key* castArg = (Key*)arg;
			table.erase(*castArg);
		}

		void DisplayTable() override
		{
			std::cout << "ЕКНА" << std::endl;
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

		void AddEpsTransitions()
		{
			Key key;
			for (auto& state : states)
			{
				key = Key(state, "eps");
				if (table.count(key))
				{
					table[key].push_back(state);
				}
				else
				{
					std::vector<std::string> value{ state };
					table.insert(std::make_pair(key, value));
				}
			}
		}

		void Prepare(std::ifstream& in) override
		{
			std::string statesString;
			std::getline(in, statesString);

			std::istringstream issStates(statesString);
			states = std::set<std::string>((std::istream_iterator<std::string>(issStates)), std::istream_iterator<std::string>());

			//AddEpsTransitions();

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
					std::cout << "СЛОВО: (done)" << std::endl;
					isFoundGoodWord = true;
				}
				else
				{
					std::cout << "СЛОВО: (fail)" << std::endl;
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
					std::cout << " " << currentState << " --> " << state << " ПО СИМВОЛУ " << wordVector[index] << std::endl;
					std::cout << "СОСТОЯНИЕ: " << state << std::endl;
					HandleWordReccursion(state, index + 1);
				}
			}
			else if (table[Key(currentState, "eps")].size() > 1)
			{
				key = Key(currentState, "eps");

				for (auto& state : table[key])
				{
					if (isFoundGoodWord) return;
					if (state == currentState) continue;

					std::cout << " " << currentState << " --> " << state << " по eps" << std::endl;
					std::cout << "состояние: " << state << std::endl;
					HandleWordReccursion(state, index);
				}
			}
			else
			{
				std::cout << " " << currentState << " по символу " << wordVector[index] << std::endl;
				std::cout << "недостижимая вершина" << std::endl;
				return;
			}
		}

		/*void HandleWord() override
		{
			std::string currentState = *states.begin();
			int index = 0;
			isFoundGoodWord = false;
			HandleWordReccursion(currentState, index);
		}*/

		std::string SetToString(std::set<std::string> _states)
		{
			std::string res = "";
			for (auto& state : _states)
			{
				res += state;
				res += ',';
			}

			return res;
		}

		std::vector<std::string> SetToVector(std::set<std::string> _states)
		{
			std::vector<std::string> res;
			for (auto& state : _states)
			{
				res.push_back(state);
			}

			return res;
		}

		void EcloseOne(std::set<std::string>& stateSet, std::set<std::string>& resOne)
		{
			for (auto& state : stateSet)
			{
				Key key(state, "eps");
				if (table.count(key))
				{
					std::set<std::string> argumentState(table[key].begin(), table[key].end());
					argumentState.erase(state);
					resOne.insert(table[key].begin(), table[key].end());
					if (table[key].size() >= 1) EcloseOne(argumentState, resOne);
				}
			}
		}

		std::set<std::string> EcloseAll(std::set<std::string> stateSet)
		{
			std::set<std::string> resAll;
			std::set<std::string> resOne;
			std::set<std::string> argumentState;

			for (auto& state : stateSet)
			{
				argumentState.insert(state);
				EcloseOne(argumentState, resOne);
				argumentState.clear();
				resAll.insert(resOne.begin(), resOne.end());
			}

			return resAll;
		}

		KNA Eclose()
		{
			AddEpsTransitions();
			set<string> startState{ *states.begin() };
			map<Key, vector<string>> newTable;
			set<string> newSymbols = symbols;
			newSymbols.erase("eps");

			set<string> ecloseArgument;
			set<string> ecloseCurrentStates;
			set<string> accessNewStates;
			for (string currentState : states)
			{
				ecloseArgument.insert(currentState);
				EcloseOne(ecloseArgument, ecloseCurrentStates);

				set<string> translateCurrentStatesBySymbol;
				for (auto& newSymbol : newSymbols)
				{
					Key key;
					for (auto& ecloseCurrenState : ecloseCurrentStates)
					{
						key = Key(ecloseCurrenState, newSymbol);

						if (!table.count(key)) continue;
						translateCurrentStatesBySymbol.insert(table[key].begin(), table[key].end());
					}

					
					if (translateCurrentStatesBySymbol.empty()) continue;

					key = Key(currentState, newSymbol);
					auto vec = SetToVector(translateCurrentStatesBySymbol);

					if (!newTable.count(key))
					{
						newTable.insert(make_pair(key, vec));
					}
					else
					{
						for (auto& vecElement : vec)
						{
							newTable[key].push_back(vecElement);
						}
					}

					translateCurrentStatesBySymbol.clear();
				}

				set<string> temp;
				for (const string& s : ecloseCurrentStates)
				{
					temp.insert(s);
				}
				// set<string> res;
				for (string stateAcc : accessStates)
				{
					if (ecloseCurrentStates.count(stateAcc) > 0)
					{
						accessNewStates.insert(currentState);
					}
				}
				// set_intersection(temp.begin(), temp.end(), accessStates.begin(), accessStates.end(),
				//                  back_inserter(res));
				// if (!res.empty())
				// {
				//   accessNewStates.insert(currentState);
				// }

				ecloseArgument.clear();
				ecloseCurrentStates.clear();
			}

			set<string> tempaccesss = EcloseAll(accessStates);
			for (string isys : tempaccesss)
			{
				accessNewStates.insert(isys);
			}


			return KNA(newTable, states, newSymbols, accessNewStates, wordVector);
		}

		vector<string> getAllStatesFromCurrent(string state, string symbol)
		{
			if (table.count(Key(state, symbol)))
			{
				return table[Key(state, symbol)];
			}
		}

		vector<string> getAllEpsTransictionFromState(string state)
		{
			vector<string> listOfState = { state };
			int countState = 0;
			vector<string> visitedState;
			while (listOfState.size() > countState)
			{
				for (string tempState : listOfState)
				{
					if (find(visitedState.begin(), visitedState.end(), tempState) != visitedState.end())
					{
						continue;
					}
					vector<string> listWithEpsTransiction = getAllStatesFromCurrent(tempState, "eps");
					listOfState.insert(listOfState.end(), listWithEpsTransiction.begin(), listWithEpsTransiction.end());
					visitedState.push_back(tempState);
					countState += 1;
				}
			}
			return listOfState;
		}

		void changeStateNoDeterministicEpsilon(vector<string> inputValue)
		{
			vector<string> listStates = { "1" };

			for (string symbol : inputValue)
			{
				string outp;
				for (string state : listStates)
				{
					outp += state;
					outp += ",";
				}
				cout << "по символу " << symbol << " --> " << outp << endl;

				outp.clear();
				vector<string> newListStates;
				for (string state : listStates)
				{
					vector<string> listWithEpsilonTransiction = getAllEpsTransictionFromState(state);
					newListStates.insert(newListStates.end(), listWithEpsilonTransiction.begin(), listWithEpsilonTransiction.end());
				}
				listStates.insert(listStates.end(), newListStates.begin(), newListStates.end());
				newListStates.clear();

				set<string> s;
				for (auto z : listStates)
					if (s.count(z) == 0)
					{
						s.insert(z);
						newListStates.push_back(z);
					}
				listStates.clear();
				listStates = newListStates;
				newListStates.clear();
				s.clear();

				cout << "               " << endl;

				for (string state : listStates)
				{
					outp += state;
					outp += ",";
				}
				//cout << "after eps: " << outp << endl;
				outp.clear();

				int count = listStates.size();

				for (int i = 0; i < count; ++i)
				{
					vector<string> newStates = getAllStatesFromCurrent(listStates[0], symbol);
					listStates.insert(listStates.end(), newStates.begin(), newStates.end());

					listStates.erase(listStates.begin());

					for (string state : listStates)
					{
						outp += state;
						outp += ",";
					}
					// cout<<"after "<<listStates[0]<<" state: "<< outp<<endl;
					outp.clear();
				}

				for (auto z : listStates)
					if (s.count(z) == 0)
					{
						s.insert(z);
						newListStates.push_back(z);
					}
				listStates.clear();
				listStates = newListStates;
				newListStates.clear();
				s.clear();

				for (string state : listStates)
				{
					outp += state;
					outp += ",";
				}
				cout << "Состояния: " << outp << endl;
				cout << "" << endl;
				outp.clear();
			}

			vector<string> newListStates;
			for (string state : listStates)
			{
				vector<string> listWithEpsilonTransiction = getAllEpsTransictionFromState(state);
				newListStates.insert(newListStates.end(), listWithEpsilonTransiction.begin(), listWithEpsilonTransiction.end());
			}
			listStates.insert(listStates.end(), newListStates.begin(), newListStates.end());
			newListStates.clear();

			set<string> s;
			for (auto z : listStates)
				if (s.count(z) == 0)
				{
					s.insert(z);
					newListStates.push_back(z);
				}
			listStates.clear();
			listStates = newListStates;
			newListStates.clear();
			s.clear();

			string outp;
			for (string state : listStates)
			{
				outp += state;
				outp += ",";
			}
			//cout << "after last eps: " << outp << endl;
			outp.clear();
		/*	vector<int> v2;
			for (size_t i = 0; i < listStates.size(); i++)
			{
				
				v2.push_back(stoi(listStates[i]));
			}

			vector<int> v3;
			for (size_t i = 0; i < accessStates.size(); i++)
			{

				v3.push_back(stoi(accessStates[i]));
			}
*/

			/*std::sort(v2.begin(), v2.end());
			std::sort(v3.begin(), v3.end());*/
			set<string> sos;
			for (auto z : listStates)
				{
				sos.insert(z);
				}

			std::set_intersection(sos.begin(), sos.end(), accessStates.begin(), accessStates.end(), std::back_inserter(res));
			if (res.size() > 0)
			{
				cout << "слово прошло" << outp << endl;
			}
			else 
				cout << "слово не подходит" << outp << endl;
			
		}

		void HandleWord() override
		{
			changeStateNoDeterministicEpsilon(wordVector);
			// vector<string> allVisitesStates;
			// vector<string> listStates;
			// vector<string> allListStates;
			// for (string symbol : wordVector)
			// {
			//     cout<<"symbol"<<symbol;
			//     
			// }


			// string currentState = *states.begin();
			// int index = 0;
			// isFoundGoodWord = false;
			// HandleWordReccursion(currentState, index);
		}


};

#endif