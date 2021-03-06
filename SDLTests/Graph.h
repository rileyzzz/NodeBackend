#pragma once
#include "DataType.h"
#include <vector>
#include "Node.h"
#include <iostream>
#include <fstream>
#include "Exportable.h"
namespace NodeEdit
{
	class Graph
	{
	public:
		bool isDynamic;
		Data::Data_Type outputType;
	};

	template <class T>
	class StaticGraph : public Graph
	{
	public:
		T outputValue;
		//std::vector<char> outputTest;

		StaticGraph(Data* finalReturn)
		{
			T* convert = (T*)finalReturn;
			outputValue = *convert;
			/*switch (finalReturn->type)
			{
			case Data::Data_Type::Boolean:
				NodeBoolean test = *(NodeBoolean*)finalReturn;

				break;
			}*/
			outputType = finalReturn->type;
			isDynamic = false;
		}
		StaticGraph()
		{
			outputType = Data::Data_Type::Boolean;
			//Data* test = new T();

			//outputValue = *test;
			//delete test;
			isDynamic = false;
		}
	};

	class DynamicGraph : public Graph
	{
	public:
		//copy over entire node list and then set their inputs to be part of this new vector
		ExportableNode OutputNode;
		DynamicGraph(Node* outputNode)
		{
			isDynamic = true;

			OutputNode = ExportableNode(outputNode);

		}
		DynamicGraph()
		{

		}
	};

	void ExportGraph(Graph* graphExport);
	Graph* ImportGraph(std::vector<Node*>* inStack);
}