#include "StandardNodes.h"
#include <math.h>
#include "NodeHelpers.h"
#include <thread>
#include <algorithm>

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

//Trig
Data* NodeTrig::Sin(std::vector<Data*> Inputs)
{
    NodeNumeric* input1 = (NodeNumeric*)Inputs[0];
    double value1 = input1->value;
    return new NodeFloat(std::sin(value1));
}

Data* NodeTrig::Cos(std::vector<Data*> Inputs)
{
    NodeNumeric* input1 = (NodeNumeric*)Inputs[0];
    double value1 = input1->value;
    return new NodeFloat(std::cos(value1));
}

Data* NodeTrig::Tan(std::vector<Data*> Inputs)
{
    NodeNumeric* input1 = (NodeNumeric*)Inputs[0];
    double value1 = input1->value;
    return new NodeFloat(std::tan(value1));
}

Data* NodeTrig::Asin(std::vector<Data*> Inputs)
{
    NodeNumeric* input1 = (NodeNumeric*)Inputs[0];
    double value1 = input1->value;
    return new NodeFloat(std::asin(value1));
}

Data* NodeTrig::Acos(std::vector<Data*> Inputs)
{
    NodeNumeric* input1 = (NodeNumeric*)Inputs[0];
    double value1 = input1->value;
    return new NodeFloat(std::acos(value1));
}

Data* NodeTrig::Atan(std::vector<Data*> Inputs)
{
    NodeNumeric* input1 = (NodeNumeric*)Inputs[0];
    double value1 = input1->value;
    return new NodeFloat(std::atan(value1));
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


Data* NodeCast::WeirdtoFloat(std::vector<Data*> Inputs)
{
    NodeWeird<double>* convert = (NodeWeird<double>*)Inputs[0];
    //NodeFloat* input1 = (NodeFloat*)Inputs[0];
    return new NodeFloat(convert->value);
}

Data* NodeCast::FloattoWeird(std::vector<Data*> Inputs)
{
    NodeFloat* input1 = (NodeFloat*)Inputs[0];
    return new NodeWeird<double>(input1->value);
}

//Actions ========================================

//Debug
std::deque<ConsoleMessage> NodeDebug::console;


void NodeDebug::MessageThread(NodeString* input)
{
    ConsoleMessage NewMessage;
    NewMessage.message = input->value;
    NewMessage.messageLength = 2;

    console.push_front(NewMessage);

    std::this_thread::sleep_for(std::chrono::seconds(NewMessage.messageLength));


}

bool NodeDebug::Print(std::vector<Data*> Inputs)
{
    NodeString* input = (NodeString*)Inputs[0];
    //std::cout << input->value << "\n";

    ConsoleMessage NewMessage;
    NewMessage.message = input->value;
    NewMessage.messageLength = 2;

    console.push_front(NewMessage);

    

    //std::thread t(MessageThread, input);
    //t.detach();


    return true;
}

//Inputs ========================================
time_t NodeInput::start = clock();
Data* NodeInput::Time()
{
    //std::chrono::system_clock::now().time_since_epoch().count()
    NodeFloat* curtime = new NodeFloat(difftime(clock(), start));
    return curtime;
}
