#pragma once
#include "Node.h"
#include "Graph.h"
class GraphCompiler
{
private:
	static bool DetermineDynamic(Node* ParentNode);
public:
	static Graph* CompileGraphfromOutput(Node* outputNode);
};

