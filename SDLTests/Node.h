#pragma once
#include <vector>
#include "DataType.h"
#include "NodeDrawable.h"
#include <iostream>
class Input;
class Output;
class Node;
class Linkable;
class Link;

class Linkable
{
public:
	Node* ParentNode;
	const char* Label;
	DataPort port;
	enum class Link_Type {
		Input,
		Output
	};
	Link_Type type;
	Link* currentLink;
};
class Input : public Linkable
{
public:
	Output* link;
	
	Input(DataPort subPort, const char* InLabel = "Input")
	{
		port = subPort;
		link = nullptr;
		Label = InLabel;
		type = Linkable::Link_Type::Input;
		currentLink = nullptr;
	}
};
class Output : public Linkable
{
public:
	Input* link;
	Output(DataPort subPort, const char* InLabel = "Output")
	{
		port = subPort;
		link = nullptr;
		Label = InLabel;
		type = Linkable::Link_Type::Output;
		currentLink = nullptr;
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

		Data* CalculateInputs();
};

class DataNode : public Node
{
public:
	DataNode(int givenID, Node_Type givenType, std::vector<Input*> givenInputs, std::vector<Output*> givenOutputs, const char* givenTitle, int x = 0, int y = 0, Data* (*f)(std::vector<Data*>) = nullptr)
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

		//std::cout << "Created node " << title << ".\n";
	}
};

class ActionNode : public Node
{
public:
	//Action nodes are a bit different; they can also have a predefined next and previous. Since only one input can be linked per IO, it is a sequence. They can, however, have several action outputs, so we must account for this.
	//They will take a boolean 
	ActionNode* Previous;
	ActionNode* Next;

	//Optional - can be called by the run function
	std::vector<ActionNode*> OutputActions;

	typedef bool (*RunFunc)(std::vector<Data*>);
	RunFunc RunCalled;

	void Run();

	ActionNode(int givenID, Node::Node_Type givenType, std::vector<Input*> givenInputs, std::vector<Output*> givenOutputs, const char* givenTitle, int x = 0, int y = 0, bool (*f)(std::vector<Data*>) = nullptr)
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
			RunCalled = (*f);
		}

		title = givenTitle;
		renderable = new NodeDrawable();
		renderable->x = x;
		renderable->y = y;

		//std::cout << "Created node " << title << ".\n";
	}
};

class Link
{
	//Calc nodes only!
public:
	Input* LinkInput;
	Output* LinkOutput;
	Link(Node* node1, int node1input, Node* node2, int node2input)
	{
		LinkInput = node1->inputs[node1input];
		LinkOutput = node2->outputs[node2input];
		node1->inputs[node1input]->link = node2->outputs[node2input];
		node2->outputs[node2input]->link = node1->inputs[node1input];

		node1->inputs[node1input]->currentLink = this;
		node2->outputs[node2input]->currentLink = this;
	}
	Link(Output* node1input, Input* node2input)
	{
		LinkInput = node2input;
		LinkOutput = node1input;
		node1input->link = node2input;
		node2input->link = node1input;

		node1input->currentLink = this;
		node2input->currentLink = this;
	}
};