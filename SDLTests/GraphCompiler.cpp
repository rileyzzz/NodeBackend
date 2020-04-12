#include "GraphCompiler.h"



bool GraphCompiler::DetermineDynamic(Node* ParentNode)
{
    bool returnval = ParentNode->dynamic;
    if (ParentNode)
    {
        for (auto& dependency : ParentNode->inputs) {
            if (dependency->link)
            {
                returnval = returnval || DetermineDynamic(dependency->link->ParentNode);
            }
        }
    }
	return returnval;
}

Graph* GraphCompiler::CompileGraphfromOutput(Node* outputNode)
{
    bool isDynamic = DetermineDynamic(outputNode);

    Graph* outputGraph;

    if (isDynamic)
    {
        outputGraph = new DynamicGraph(outputNode);
    }
    else
    {
        outputGraph = new StaticGraph<NodeFloat>(outputNode->CalculateInputs());
    }
    return outputGraph;
}
