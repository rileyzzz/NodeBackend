#include "Node.h"
#include "NodeHelpers.h"

Data* Node::CalculateInputs()
{
    //CalculatedInputs.clear();
    std::vector<Data*> sendInputs;
    //Loop through every input required for our output node.
    for (int nodeInputID = 0; nodeInputID < inputs.size(); nodeInputID++)
    {
        Input* nodeInput = inputs[nodeInputID];

        //Recursion through entire link tree.
        if (nodeInput->link)
        {
            sendInputs.push_back(CalculateLinkChain(nodeInput->link));
        }
        else
        {
            sendInputs.push_back(GetNodeDefault(nodeInput->port.PortType));
        }
    }
    //returns the first calculated input, for ease of use with basic output nodes.
    //this does however support multiple inputs, simply access the CalculatedInputs vector.
    CalculatedInputs = sendInputs;
    return sendInputs[0];
}

void ActionNode::Run()
{
    if (type == Node::Node_Type::Node_Action)
    {
        //Do our predefined action.
        if (inputs.size())
        {
            CalculateInputs();
        }
        RunCalled(CalculatedInputs);
    }
    //Our node has been called.
    if (Next)
    {
        //Continue the chain.
        Next->Run();
    }
    //Optional case where there is more than one output - should be run as part of the RunCalled function
}
