#include "StandardNodes.h"
#include <math.h>
#include "NodeHelpers.h"
#include <thread>

//Calculations ========================================

//Math
Data* NodeMath::Add(std::vector<Data*> Inputs)
{
    NodeNumeric* input1 = (NodeNumeric*)Inputs[0];
    double value1 = input1->value;
    NodeNumeric* input2 = (NodeNumeric*)Inputs[1];
    double value2 = input2->value;
    return new NodeFloat(value1 + value2);
}
Data* NodeMath::Subtract(std::vector<Data*> Inputs)
{
    NodeNumeric* input1 = (NodeNumeric*)Inputs[0];
    double value1 = input1->value;
    NodeNumeric* input2 = (NodeNumeric*)Inputs[1];
    double value2 = input2->value;
    return new NodeFloat(value1 - value2);
}

Data* NodeMath::Multiply(std::vector<Data*> Inputs)
{
    NodeNumeric* input1 = (NodeNumeric*)Inputs[0];
    double value1 = input1->value;
    NodeNumeric* input2 = (NodeNumeric*)Inputs[1];
    double value2 = input2->value;
    return new NodeFloat(value1 * value2);
}

Data* NodeMath::Divide(std::vector<Data*> Inputs)
{
    NodeNumeric* input1 = (NodeNumeric*)Inputs[0];
    double value1 = input1->value;
    NodeNumeric* input2 = (NodeNumeric*)Inputs[1];
    double value2 = input2->value;
    return new NodeFloat(value1 / value2);
}

Data* NodeMath::Mod(std::vector<Data*> Inputs)
{
    NodeNumeric* input1 = (NodeNumeric*)Inputs[0];
    int value1 = input1->value;
    NodeNumeric* input2 = (NodeNumeric*)Inputs[1];
    int value2 = input2->value;
    return new NodeInteger(value1 % value2);
}

Data* NodeMath::Abs(std::vector<Data*> Inputs)
{
    NodeNumeric* input1 = (NodeNumeric*)Inputs[0];
    double value1 = input1->value;
    return new NodeFloat(std::abs(value1));
}

Data* NodeMath::Sqrt(std::vector<Data*> Inputs)
{
    NodeNumeric* input1 = (NodeNumeric*)Inputs[0];
    double value1 = input1->value;
    return new NodeFloat(std::sqrt(value1));
}

Data* NodeMath::Round(std::vector<Data*> Inputs)
{
    NodeNumeric* input1 = (NodeNumeric*)Inputs[0];
    double value1 = input1->value;
    return new NodeFloat(std::round(value1));
}

Data* NodeMath::Floor(std::vector<Data*> Inputs)
{
    NodeNumeric* input1 = (NodeNumeric*)Inputs[0];
    double value1 = input1->value;
    return new NodeFloat(std::floor(value1));
}

Data* NodeMath::Ceil(std::vector<Data*> Inputs)
{
    NodeNumeric* input1 = (NodeNumeric*)Inputs[0];
    double value1 = input1->value;
    return new NodeFloat(std::ceil(value1));
}

Data* NodeMath::Log(std::vector<Data*> Inputs)
{
    NodeNumeric* input1 = (NodeNumeric*)Inputs[0];
    double value1 = input1->value;
    return new NodeFloat(std::log(value1));
}

Data* NodeMath::Pow(std::vector<Data*> Inputs)
{
    NodeNumeric* input1 = (NodeNumeric*)Inputs[0];
    double value1 = input1->value;
    NodeNumeric* input2 = (NodeNumeric*)Inputs[1];
    double value2 = input2->value;
    return new NodeFloat(std::pow(value1, value2));
}

Data* NodeMath::Negate(std::vector<Data*> Inputs)
{
    NodeNumeric* input1 = (NodeNumeric*)Inputs[0];
    double value1 = input1->value;
    return new NodeFloat(-value1);
}

//Comparison
Data* NodeComparison::And(std::vector<Data*> Inputs)
{
    NodeBoolean* input1 = (NodeBoolean*)Inputs[0];
    NodeBoolean* input2 = (NodeBoolean*)Inputs[1];
    return new NodeBoolean(input1->value && input2->value);
}

Data* NodeComparison::Or(std::vector<Data*> Inputs)
{
    NodeBoolean* input1 = (NodeBoolean*)Inputs[0];
    NodeBoolean* input2 = (NodeBoolean*)Inputs[1];
    return new NodeBoolean(input1->value || input2->value);
}

Data* NodeComparison::Not(std::vector<Data*> Inputs)
{
    NodeBoolean* input1 = (NodeBoolean*)Inputs[0];
    return new NodeBoolean(!input1->value);
}

//Casting
Data* NodeCast::BooltoInt(std::vector<Data*> Inputs)
{
    NodeBoolean* input1 = (NodeBoolean*)Inputs[0];
    return new NodeInteger(input1->value);
}

Data* NodeCast::InttoBool(std::vector<Data*> Inputs)
{
    NodeInteger* input1 = (NodeInteger*)Inputs[0];
    return new NodeBoolean(input1->value);
}

Data* NodeCast::InttoFloat(std::vector<Data*> Inputs)
{
    NodeInteger* input1 = (NodeInteger*)Inputs[0];
    return new NodeFloat(input1->value);
}

//Actions ========================================

//Debug
std::vector<ConsoleMessage> NodeDebug::console;


void NodeDebug::MessageThread(int MessageIndex, int waitTime)
{
    std::this_thread::sleep_for(std::chrono::seconds(waitTime));
    console.erase(console.begin());
}

void NodeDebug::PrinttoScreen(ConsoleMessage text)
{
    console.push_back(text);
    std::thread t(MessageThread, console.size() - 1, text.messageLength);
    t.detach();
}
bool NodeDebug::Print(std::vector<Data*> Inputs)
{
    NodeString* input = (NodeString*)Inputs[0];
    //std::cout << input->value << "\n";
    ConsoleMessage NewMessage;
    NewMessage.message = input->value;
    NewMessage.messageLength = 2;

    PrinttoScreen(NewMessage);
    return true;
}
