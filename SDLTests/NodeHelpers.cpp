#include "NodeHelpers.h"


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
                for (int dependency = 0; dependency < ParentNode->inputs.size(); dependency++)
                {
                    ParentNode->CalculatedInputs.push_back(CalculateLinkChain(ParentNode->inputs[dependency]->link));
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
    inLink = nullptr;
}
