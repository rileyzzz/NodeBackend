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

		//Should this be an input node, have a variable for the input data that can be changed on the fly. This should probably be moved to a subclass at some point
		Data* InputData;

		typedef Data* (*CalcFunc)(std::vector<Data*>);

		CalcFunc Calculate;

		Node(int givenID, Node_Type givenType, std::vector<Input*> givenInputs, std::vector<Output*> givenOutputs, const char* givenTitle, int x = 0, int y = 0, Data* (*f)(std::vector<Data*>) = nullptr)
		{
			ID = givenID;
			type = givenType;
			inputs = givenInputs;
			outputs = givenOutputs;
			//set IO parents
			if (inputs.size())
			{
				for (int iter = 0; iter < inputs.size(); iter++)
				{
					inputs[iter]->ParentNode = this;
				}
			}
			if (outputs.size())
			{
				for (int iter = 0; iter < outputs.size(); iter++)
				{
					outputs[iter]->ParentNode = this;
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

			std::cout << "Created node " << title << ".\n";
		}
};

