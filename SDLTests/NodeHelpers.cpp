#include "NodeHelpers.h"
#include "StandardNodes.h"


std::vector<Input*> CreateInputs(std::vector<DataPort> itemarray)
{
    std::vector<Input*> createdInputs;
    //populate inputs
    for (int i = 0; i < itemarray.size(); i++) {
        createdInputs.push_back(new Input(itemarray[i]));
    }
    return createdInputs;
}

std::vector<Output*> CreateOutputs(std::vector<DataPort> itemarray)
{
    std::vector<Output*> createdOutputs;
    //populate outputs
    for (int i = 0; i < itemarray.size(); i++) {
        createdOutputs.push_back(new Output(itemarray[i]));
    }
    return createdOutputs;
}

Data* CalculateLinkChain(Output* srcLink)
{
    Node* ParentNode = srcLink->ParentNode;

    if (ParentNode)
    {
        Data* returnval = nullptr;
        switch (ParentNode->type)
        {
        case Node::Node_Type::Node_Input:
            returnval = ParentNode->InputData;
            break;
        case Node::Node_Type::Node_Event:
            returnval = ParentNode->InputData;
            break;
        case Node::Node_Type::Node_Calculation:
            if (ParentNode->inputs.size())
            {
                ParentNode->CalculatedInputs.clear();
                for (int dependency = 0; dependency < ParentNode->inputs.size(); dependency++)
                {
                    if (ParentNode->inputs[dependency]->link)
                    {
                        ParentNode->CalculatedInputs.push_back(CalculateLinkChain(ParentNode->inputs[dependency]->link));
                    }
                    else
                    {
                        ParentNode->CalculatedInputs.push_back(GetNodeDefault(ParentNode->inputs[dependency]->port.PortType));
                    }
                }
                returnval = ParentNode->Calculate(ParentNode->CalculatedInputs);
            }
            break;

        }
        return returnval;
    }
    else
    {
        return new NodeNone();
    }
}


void Unlink(Link* inLink)
{
    inLink->LinkInput->link = nullptr;
    inLink->LinkOutput->link = nullptr;
    inLink->LinkInput->currentLink = nullptr;
    inLink->LinkOutput->currentLink = nullptr;
    inLink->LinkInput = nullptr;
    inLink->LinkOutput = nullptr;
    //inLink = nullptr;
    delete inLink;
}

Data* GetNodeDefault(Data::Data_Type intype)
{
    switch (intype)
    {
    case Data::Data_Type::Boolean:
        return new NodeBoolean(false);
        break;
    case Data::Data_Type::Integer:
        return new NodeInteger(0.0);
        break;
    case Data::Data_Type::Numeric:
    case Data::Data_Type::Float:
        return new NodeFloat(0.0);
        break;
    case Data::Data_Type::String:
        return new NodeString("");
        break;
    case Data::Data_Type::Weird:
        return new NodeNone();
        break;
    default:
        return new NodeNone();
    }
}

ContextMenu* GenerateContextMenu()
{
    ContextMenu* CM = new ContextMenu();

    ContextCategory* Math = new ContextCategory("Math");
    CM->Categories.push_back(Math);
    ContextCategory* Comparison = new ContextCategory("Comparison");
    CM->Categories.push_back(Comparison);
    ContextCategory* Casting = new ContextCategory("Casting");
    CM->Categories.push_back(Casting);
    ContextCategory* Debug = new ContextCategory("Debug");
    CM->Categories.push_back(Debug);

    //NODE DEFINITIONS

    //Math
    Math->Options.push_back(new NodeCreator(Node::Node_Type::Node_Calculation,
        { DataPort(Data::Data_Type::Numeric), DataPort(Data::Data_Type::Numeric) }, 
        { DataPort(Data::Data_Type::Numeric) }, "Add", NodeMath::Add));
    Math->Options.push_back(new NodeCreator(Node::Node_Type::Node_Calculation,
        { DataPort(Data::Data_Type::Numeric), DataPort(Data::Data_Type::Numeric) },
        { DataPort(Data::Data_Type::Numeric) }, "Subtract", NodeMath::Subtract));
    Math->Options.push_back(new NodeCreator(Node::Node_Type::Node_Calculation,
        { DataPort(Data::Data_Type::Numeric), DataPort(Data::Data_Type::Numeric) },
        { DataPort(Data::Data_Type::Numeric) }, "Multiply", NodeMath::Multiply));
    Math->Options.push_back(new NodeCreator(Node::Node_Type::Node_Calculation,
        { DataPort(Data::Data_Type::Numeric), DataPort(Data::Data_Type::Numeric) },
        { DataPort(Data::Data_Type::Numeric) }, "Divide", NodeMath::Divide));
    Math->Options.push_back(new NodeCreator(Node::Node_Type::Node_Calculation,
        { DataPort(Data::Data_Type::Integer), DataPort(Data::Data_Type::Integer) },
        { DataPort(Data::Data_Type::Integer) }, "Modulo", NodeMath::Mod));
    Math->Options.push_back(new NodeCreator(Node::Node_Type::Node_Calculation,
        { DataPort(Data::Data_Type::Numeric), DataPort(Data::Data_Type::Numeric) },
        { DataPort(Data::Data_Type::Numeric) }, "Pow", NodeMath::Pow));

    Math->Options.push_back(new NodeCreator(Node::Node_Type::Node_Calculation,
        { DataPort(Data::Data_Type::Numeric) },
        { DataPort(Data::Data_Type::Numeric) }, "Abs", NodeMath::Abs));
    Math->Options.push_back(new NodeCreator(Node::Node_Type::Node_Calculation,
        { DataPort(Data::Data_Type::Numeric) },
        { DataPort(Data::Data_Type::Numeric) }, "Square Root", NodeMath::Sqrt));
    Math->Options.push_back(new NodeCreator(Node::Node_Type::Node_Calculation,
        { DataPort(Data::Data_Type::Numeric) },
        { DataPort(Data::Data_Type::Numeric) }, "Round", NodeMath::Round));
    Math->Options.push_back(new NodeCreator(Node::Node_Type::Node_Calculation,
        { DataPort(Data::Data_Type::Numeric) },
        { DataPort(Data::Data_Type::Numeric) }, "Floor", NodeMath::Floor));
    Math->Options.push_back(new NodeCreator(Node::Node_Type::Node_Calculation,
        { DataPort(Data::Data_Type::Numeric) },
        { DataPort(Data::Data_Type::Numeric) }, "Ceil", NodeMath::Ceil));
    Math->Options.push_back(new NodeCreator(Node::Node_Type::Node_Calculation,
        { DataPort(Data::Data_Type::Numeric) },
        { DataPort(Data::Data_Type::Numeric) }, "Log", NodeMath::Log));


    //Debug
    Debug->Options.push_back(new NodeCreator(Node::Node_Type::Node_Action,
        { DataPort(Data::Data_Type::String) },
        {  }, "Print", NodeDebug::Print));

    //temporary
    //Math->Show = true;
    //Math->Options[0]->Selected = true;

    return CM;
}
