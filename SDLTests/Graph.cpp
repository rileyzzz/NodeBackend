#include "Graph.h"

void ExportGraph(Graph* graphExport)
{
	std::cout << "this is getting called\n";
	std::ofstream outfile;
	outfile.open("C:/Users/riley/source/repos/SDLTests/x64/Debug/export.graph", std::ios::out | std::ios::trunc);

	//outfile << *graphExport << std::endl;
	//Graph copyGraph = *graphExport;
	if (graphExport->isDynamic)
	{

	}
	else
	{
		//write header
		//format: bool dynamic, enum datatype, graph data
		outfile.write((char*)&graphExport->isDynamic, sizeof(bool));
		outfile.write((char*)&graphExport->outputType, sizeof(Data::Data_Type));

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

Graph* ImportGraph()
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