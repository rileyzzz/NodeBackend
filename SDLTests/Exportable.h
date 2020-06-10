#pragma once
#include "Node.h"

namespace NodeEdit
{


	class ExportableNode;
	class ExportInput;
	class ExportOutput;

	//class ExportLink
	//{
	//	//Calc nodes only!
	//public:
	//	ExportInput LinkInput;
	//	ExportOutput LinkOutput;
	//	ExportLink(Link* inLink)
	//	{
	//		LinkInput = ExportInput(inLink->LinkInput);
	//		LinkOutput = ExportOutput(inLink->LinkOutput);
	//	}
	//	ExportLink()
	//	{
	//
	//	}
	//};

	class ExportLinkable
	{
	public:

		//labels arent implemented yet
		//std::string Label;
		DataPort port;

		Linkable::Link_Type type;

	};

	class ExportOutput : public ExportLinkable
	{
	public:
		ExportableNode* ParentNode = nullptr;
		ExportOutput(Output* src)
		{
			//if (src->link)
			//{
				//ParentNode = new ExportableNode(src->ParentNode);
				//newOut.Label = src->Label;
			port = src->port;
			type = src->type;

			//}
		}
		ExportOutput()
		{

		}
	};

	class ExportInput : public ExportLinkable
	{
	public:
		ExportOutput link;
		bool Linked;
		ExportInput(Input* src)
		{
			//cant have parent nodes, this is an input
			//newIn.Label = src->Label;
			port = src->port;
			type = src->type;

			//link = CreateExportOutput(src->link);
		//}
		}
	};

	//ExportInput CreateExportInput(Input* src);
	//ExportOutput CreateExportOutput(Output* src);

	class ExportableNode
	{
	public:
		int ID;
		Node::Node_Type type;

		std::vector<ExportInput> inputs;
		std::vector<ExportOutput> outputs;
		//on export - loop through all possible inputs, then write each of their parentnodes to the file afterwards
		//then use the vector size on import to reassign the pointers

		std::string title;

		NodeDrawable renderable;

		//constantly changing dependency values used for calculation - 1:1 with inputs vector
		//std::vector<Data*> CalculatedInputs;

		//Should this be an input node, have a variable for the input data that can be changed on the fly. This should probably be moved to a subclass at some point
		Data InputData;

		//switch to function indexes at some point, this could be dangerous
		std::function<Data* ()> weirdCalculate;
		std::function<Data* (Data*)> weirdCalculateWithArg;

		bool dynamic = false;

		typedef Data* (*InputFunc)();

		InputFunc InputCalculate;

		typedef Data* (*CalcFunc)(std::vector<Data*>);

		CalcFunc Calculate;
		ExportableNode(Node* copyNode)
		{
			ID = copyNode->ID;
			type = copyNode->type;

			for (auto& copyInput : copyNode->inputs) {
				ExportInput newInput = ExportInput(copyInput);
				if (copyInput->link)
				{
					Output* otherLink = copyInput->link;

					ExportOutput newOut = ExportOutput(otherLink);
					newOut.ParentNode = new ExportableNode(otherLink->ParentNode);
					newInput.link = newOut;

					newInput.Linked = true;
				}
				else
				{
					newInput.Linked = false;
				}
				inputs.push_back(newInput);
			}
			for (auto& copyOutput : copyNode->outputs) {
				outputs.push_back(ExportOutput(copyOutput));
			}
			//std::string strconvert(copyNode->title);
			title = copyNode->title;//dereference?
			renderable = *copyNode->renderable;
			if (copyNode->InputData) InputData = *copyNode->InputData;
			if (copyNode->weirdCalculate) weirdCalculate = copyNode->weirdCalculate;
			if (copyNode->weirdCalculateWithArg) weirdCalculateWithArg = copyNode->weirdCalculateWithArg;

			if (copyNode->InputCalculate) InputCalculate = copyNode->InputCalculate;
			if (copyNode->Calculate) Calculate = copyNode->Calculate;
		}
		ExportableNode()
		{
			ID = 0;
			type = Node::Node_Type::Node_Output;
			InputCalculate = nullptr;
			Calculate = nullptr;
		}
	};

}