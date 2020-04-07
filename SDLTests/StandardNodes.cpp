#include "StandardNodes.h"
#include <math.h>

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
bool NodeDebug::Print(std::vector<Data*> Inputs)
{
    NodeString* input = (NodeString*)Inputs[0];
    std::cout << input->value << "\n";
    return true;
}
