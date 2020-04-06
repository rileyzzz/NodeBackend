#include "Node.h"
#include "NodeHelpers.h"

Data* Node::CalculateInputs()
{
    //Loop through every input required for our output node.
    for (int nodeInputID = 0; nodeInputID < inputs.size(); nodeInputID++)
    {
        Input* nodeInput = inputs[nodeInputID];

        //Recursion through entire link tree.
        CalculatedInputs.push_back(CalculateLinkChain(nodeInput->link));
    }
    //returns the first calculated input, for ease of use with basic output nodes.
    return CalculatedInputs[0];
}

void ActionNode::Run()
{
    if (RunCalled)
    {
        //Do our predefined action.
        RunCalled();
    }
    if (Next)
    {
        //Continue the chain.
        Next->Run();
    }
    //Optional case where there is more than one output - should be run as part of the RunCalled function
}
