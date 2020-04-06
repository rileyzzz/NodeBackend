#pragma once
#include <vector>
#include "DataType.h"
#include "NodeDrawable.h"
#include <iostream>
class Input;
class Output;
class Node;
class Linkable;

class Linkable
{
public:
	Node* ParentNode;
};
class Input : public Linkable
{
public:
	Output* link;
	DataPort port;
	Input(DataPort subPort)
	{
		port = subPort;
		link = nullptr;
	}
};
class Output : public Linkable
{
public:
	//Input* link;
	DataPort port;
	Output(DataPort subPort)
	{
		port = subPort;
		//link = nullptr;
	}
};
class Node
{
	public:
		enum class Node_Type {
			Node_Event,
			Node_Action,
			Node_Calculation,
			Node_Input, //different from classes, an actual data input/output
			Node_Output
		};
		int ID;
		Node_Type type;

		std::vector<Input*> inputs;
		std::vector<Output*> outputs;
		
		const char* title;

		NodeDrawable* renderable;

		//constantly changing dependency values used for calculation - 1:1 with inputs vector
		std::vector<Data*> CalculatedInputs;

		typedef Data* (*CalcFunc)(std::vector<Data*>);

		//Data* DefaultCalculate(std::vector<Data*> calcInputsMoved);
		CalcFunc Calculate;

		Node(int givenID, Node_Type givenType, std::vector<Input*> givenInputs, std::vector<Output*> givenOutputs, const char* givenTitle, int x = 0, int y = 0, Data* (*f)(std::vector<Data*>) = nullptr)
		{
			std::cout << "created node\n";
			ID = givenID;
			type = givenType;
			inputs = givenInputs;
			outputs = givenOutputs;
			//set IO parents
			if (inputs.size())
			{
				/*std::cout << "Registering inputs for ";
				std::cout << givenTitle;
				std::cout << ".\n";*/
				for (int iter = 0; iter < inputs.size(); iter++)
				{
					//std::cout << "test 1";
					inputs[iter]->ParentNode = this;
					//std::cout << "test 2";
				}
			}
			if (outputs.size())
			{
				/*std::cout << "Registering outputs for ";
				std::cout << givenTitle;
				std::cout << ".\n";*/
				for (int iter = 0; iter < outputs.size(); iter++)
				{
					//std::cout << "test 3";
					outputs[iter]->ParentNode = this;
					//std::cout << "test 4";
				}
			}
			
			if (f)
			{
				Calculate = (*f);
			}

			title = givenTitle;
			renderable = new NodeDrawable();
			renderable->x = x;
			renderable->y = y;
		}
};

