#include "Graph.h"

void ExportGraph(Graph* graphExport)
{
	std::cout << "this is getting called\n";
	std::ofstream outfile;
	outfile.open("C:/Users/riley/source/repos/SDLTests/x64/Debug/export.bin", std::ios::out | std::ios::trunc);

	//outfile << *graphExport << std::endl;
	//Graph copyGraph = *graphExport;
	if (graphExport->isDynamic)
	{

	}
	else
	{
		StaticGraph<NodeFloat> convert = *(StaticGraph<NodeFloat>*)graphExport;
		/*switch (convert->outputValue->type)
		{
		case Data::Data_Type::Boolean:

		}*/
		//sizeof(*convert)
		outfile.write((char*)&convert, sizeof(StaticGraph<NodeFloat>));
	}
	

	outfile.close();
}

void ImportGraph()
{
	std::ifstream infile("C:/Users/riley/source/repos/SDLTests/x64/Debug/export.bin", std::ios::in | std::ios::binary);
	const int size = infile.tellg();
	infile.seekg(0, std::ios::beg);

	//char buffer[size];
	char* buffer = new char[size];
	std::cout << "char array is " << size << std::endl;
	//StaticGraph convert = StaticGraph();
	StaticGraph<NodeFloat>* convert = new StaticGraph<NodeFloat>[1];
	infile.read((char*)convert, sizeof(StaticGraph<NodeFloat>));
	
	NodeFloat test = convert->outputValue;
	//NodeInteger* testconvert = (NodeInteger*)&test;
	if (test.type == Data::Data_Type::Boolean)
	{
		std::cout << "aaaa";
	}
	if (test.type == Data::Data_Type::Integer)
	{
		std::cout << "int";
	}
	if (test.type == Data::Data_Type::Float)
	{
		std::cout << "float";
	}

	std::cout << "read variable is " << test.value << "\n";
}