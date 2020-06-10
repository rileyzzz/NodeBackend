#pragma once
#include "Node.h"
#include "Graph.h"
namespace NodeEdit
{
	class GraphCompiler
	{
	private:
		static bool DetermineDynamic(Node* ParentNode);
	public:
		static Graph* CompileGraphfromOutput(Node* outputNode);
	};
}
