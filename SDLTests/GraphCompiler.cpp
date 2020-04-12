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

    Graph* outputGraph = nullptr;

    if (isDynamic)
    {
        outputGraph = new DynamicGraph(outputNode);
    }
    else
    {
        switch (outputNode->inputs[0]->port.PortType)
        {
        case Data::Data_Type::Boolean:
        {
            typedef NodeBoolean graphType;
            outputGraph = new StaticGraph<graphType>(outputNode->CalculateInputs());
            break;
        }
        case Data::Data_Type::Float:
        {
            typedef NodeFloat graphType;
            outputGraph = new StaticGraph<graphType>(outputNode->CalculateInputs());
            break;
        }
        case Data::Data_Type::Integer:
        {
            typedef NodeInteger graphType;
            outputGraph = new StaticGraph<graphType>(outputNode->CalculateInputs());
            break;
        }
        case Data::Data_Type::String:
        {
            typedef NodeString graphType;
            outputGraph = new StaticGraph<graphType>(outputNode->CalculateInputs());
            break;
        }
        }
        
    }
    return outputGraph;
}


