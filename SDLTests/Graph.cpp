#include "Graph.h"

std::vector<ExportableNode*> curNodeStack;
std::vector<ExportableNode*> importStack;

void SetupTree(ExportableNode* inNode)
{
	if (!inNode->inputs.empty())
	{
		for (auto& curInput : inNode->inputs) {
			if (curInput.link.ParentNode)
			{
				ExportableNode* nextNode = curInput.link.ParentNode;

				//order is important here - we want all input child objects to come after a parent
				curNodeStack.push_back(nextNode);
				SetupTree(nextNode);
			}
		}
	}
}

void DeconstructTree(ExportableNode* mainOutput, std::ifstream* infile)
{

	if (!mainOutput->inputs.empty())
	{
		for (auto& curInput : mainOutput->inputs) {
			//if (curInput.link.ParentNode)
			//{

			ExportableNode* nextNode = new ExportableNode;
			infile->read((char*)nextNode, sizeof(ExportableNode));
				
			std::cout << "Read node " << nextNode->title << " from file.\n";
			//order is important here - we want all input child objects to come after a parent
			importStack.push_back(nextNode);
			DeconstructTree(nextNode, infile);
				//SetupTree(nextNode);
			//}
		}
	}
}

void ExportGraph(Graph* graphExport)
{
	curNodeStack.clear();
	std::cout << "this is getting called\n";
	std::ofstream outfile;
	outfile.open("C:/Users/riley/source/repos/SDLTests/x64/Debug/export.graph", std::ios::out | std::ios::trunc);

	//write header
	//format: bool dynamic, enum datatype, graph data
	outfile.write((char*)&graphExport->isDynamic, sizeof(bool));
	outfile.write((char*)&graphExport->outputType, sizeof(Data::Data_Type));

	if (graphExport->isDynamic)
	{
		//maybe export my weird custom calculation functions as a variable
		std::cout << "it's dynamic\n";
		DynamicGraph* convert = (DynamicGraph*)graphExport;
		outfile.write((char*)convert, sizeof(DynamicGraph));

		SetupTree(&convert->OutputNode);
		for (auto& curNode : curNodeStack) {
			outfile.write((char*)curNode, sizeof(ExportableNode));
		}

		//loop through and write each input's referenced node?
	}
	else
	{
		switch (graphExport->outputType)
		{
		case Data::Data_Type::Boolean:
		{
			typedef NodeBoolean graphType;
			StaticGraph<graphType> convert = *(StaticGraph<graphType>*)graphExport;
			outfile.write((char*)&convert, sizeof(StaticGraph<graphType>));
			break;
		}
		case Data::Data_Type::Float:
		{
			typedef NodeFloat graphType;
			StaticGraph<graphType> convert = *(StaticGraph<graphType>*)graphExport;
			outfile.write((char*)&convert, sizeof(StaticGraph<graphType>));
			break;
		}
		case Data::Data_Type::Integer:
		{
			typedef NodeInteger graphType;
			StaticGraph<graphType> convert = *(StaticGraph<graphType>*)graphExport;
			outfile.write((char*)&convert, sizeof(StaticGraph<graphType>));
			break;
		}
		case Data::Data_Type::String:
		{
			typedef NodeString graphType;
			StaticGraph<graphType> convert = *(StaticGraph<graphType>*)graphExport;
			outfile.write((char*)&convert, sizeof(StaticGraph<graphType>));
			break;
		}
		}
		
	}
	

	outfile.close();
}

Graph* ImportGraph(std::vector<Node*>* inStack)
{
	std::ifstream infile("C:/Users/riley/source/repos/SDLTests/x64/Debug/export.graph", std::ios::in | std::ios::binary);
	const int size = infile.tellg();
	infile.seekg(0, std::ios::beg);
	
	//read header info
	bool* isDynamic = new bool;
	infile.read((char*)isDynamic, sizeof(bool));
	Data::Data_Type* type = new Data::Data_Type;
	infile.read((char*)type, sizeof(Data::Data_Type));

	std::cout << "dynamic read as " << *isDynamic << std::endl;
	Graph* outputGraph = new Graph;

	if (*isDynamic)
	{
		importStack.clear();
		DynamicGraph* createdGraph = new DynamicGraph;
		infile.read((char*)createdGraph, sizeof(DynamicGraph));

		DeconstructTree(&createdGraph->OutputNode, &infile);

		std::cout << "read finished.\n";

		std::cout << "Adding to scene...\n";

		for (auto& curNode : importStack) {
			std::cout << "curnode name is " << curNode->title << std::endl;
			switch (curNode->type)
			{
			case Node::Node_Type::Node_Action:
			{
				break;
			}
			case Node::Node_Type::Node_Calculation:
			{
				std::cout << "Calculation node " << curNode->title << std::endl;
				std::vector<Input*> inputs;
				std::vector<Output*> outputs;
				for (auto& curInput : curNode->inputs) {
					DataPort newPort = DataPort(curInput.port.PortType);
					//DataPort newPort = DataPort(Data::Data_Type::Float);
					Input* createdInput = new Input(newPort);
					//createdInput->port = curInput.port;
					createdInput->type = curInput.type;
					if (curInput.Linked)
					{
						//add the output to a stack and link the two later
						
					}
					inputs.push_back(createdInput);
				}

				for (auto& curOutput : curNode->outputs) {
					Output* createdOutput = new Output(curOutput.port);
					createdOutput->port = curOutput.port;
					createdOutput->type = curOutput.type;

					outputs.push_back(createdOutput);
				}

				DataNode* NewNode = new DataNode(curNode->ID, curNode->type, inputs, outputs, curNode->title.c_str(), 300, 300, curNode->Calculate);

				inStack->push_back(NewNode);
				break;
			}
			case Node::Node_Type::Node_Event:
			{
				break;
			}
			case Node::Node_Type::Node_Input:
			{
				break;
			}
			}
			//Node* copyNode = new Node;
		
		}

	}
	else
	{
		switch (*type)
		{
		case Data::Data_Type::Boolean:
		{
			typedef NodeBoolean graphType;
			StaticGraph<graphType>* createdGraph = new StaticGraph<graphType>[1];
			infile.read((char*)createdGraph, sizeof(StaticGraph<graphType>));
			outputGraph = createdGraph;
			break;
		}
		case Data::Data_Type::Float:
		{
			typedef NodeFloat graphType;
			StaticGraph<graphType>* createdGraph = new StaticGraph<graphType>[1];
			infile.read((char*)createdGraph, sizeof(StaticGraph<graphType>));
			outputGraph = createdGraph;
			break;
		}
		case Data::Data_Type::Integer:
		{
			typedef NodeInteger graphType;
			StaticGraph<graphType>* createdGraph = new StaticGraph<graphType>[1];
			infile.read((char*)createdGraph, sizeof(StaticGraph<graphType>));
			outputGraph = createdGraph;
			break;
		}
		case Data::Data_Type::String:
		{
			typedef NodeString graphType;
			StaticGraph<graphType>* createdGraph = new StaticGraph<graphType>[1];
			infile.read((char*)createdGraph, sizeof(StaticGraph<graphType>));
			outputGraph = createdGraph;
			break;
		}
		}

		if (outputGraph)
		{
			StaticGraph<NodeFloat>* convert = (StaticGraph<NodeFloat>*)outputGraph;
			NodeFloat test = convert->outputValue;

			std::cout << "read variable is " << test.value << "\n";
		}
		
	}
	
	return outputGraph;
}