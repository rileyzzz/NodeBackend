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
            if (ParentNode->InputData) 
            {
                returnval = ParentNode->InputData;
            }
            else if(ParentNode->InputCalculate)
            {
                returnval = ParentNode->InputCalculate();
            }
            else if (ParentNode->weirdCalculate)
            {
                returnval = ParentNode->weirdCalculate();
            }
            break;
        case Node::Node_Type::Node_Event:
            if (ParentNode->InputData)
            {
                returnval = ParentNode->InputData;
            }
            else
            {
                returnval = ParentNode->InputCalculate();
            }
            break;
        case Node::Node_Type::Node_Calculation:
            if (ParentNode->inputs.size())
            {
                //ParentNode->CalculatedInputs.clear();
                std::vector<Data*> sendInputs;
                for (int dependency = 0; dependency < ParentNode->inputs.size(); dependency++)
                {
                    if (ParentNode->inputs[dependency]->link)
                    {
                        sendInputs.push_back(CalculateLinkChain(ParentNode->inputs[dependency]->link));
                    }
                    else
                    {
                        sendInputs.push_back(GetNodeDefault(ParentNode->inputs[dependency]->port.PortType));
                    }
                }
                if (ParentNode->Calculate)
                {
                    returnval = ParentNode->Calculate(sendInputs);
                }
                else if (ParentNode->weirdCalculateWithArg)
                {
                    returnval = ParentNode->weirdCalculateWithArg(sendInputs[0]);
                }
                ParentNode->CalculatedInputs = sendInputs;
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
    if (inLink->LinkInput)
    {
        inLink->LinkInput->link = nullptr;
        inLink->LinkInput->currentLink = nullptr;
        inLink->LinkInput = nullptr;
    }
    if (inLink->LinkOutput)
    {
        inLink->LinkOutput->link = nullptr;
        inLink->LinkOutput->currentLink = nullptr;
        inLink->LinkOutput = nullptr;
    }
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
        return new NodeInteger(0);
        break;
    case Data::Data_Type::Numeric:
    case Data::Data_Type::Float:
        return new NodeFloat(0);
        break;
    case Data::Data_Type::String:
        return new NodeString("");
        break;
    case Data::Data_Type::Weird:
        return new NodeFloat(0);
        break;
    default:
        return new NodeNone();
    }
}


//Data* CreateData(bool input)
//{
//    return new NodeBoolean(input);
//}


double TestFunction()
{
    return 100;
}

const char* TestString()
{
    return "This is from a weird function!";
}

ContextMenu* GenerateContextMenu()
{
    ContextMenu* CM = new ContextMenu();

    ContextCategory* Math = new ContextCategory("Math");
    CM->Categories.push_back(Math);
    ContextCategory* Trig = new ContextCategory("Trigonometry");
    CM->Categories.push_back(Trig);
    ContextCategory* Comparison = new ContextCategory("Comparison");
    CM->Categories.push_back(Comparison);
    ContextCategory* Casting = new ContextCategory("Casting");
    CM->Categories.push_back(Casting);
    ContextCategory* Debug = new ContextCategory("Debug");
    CM->Categories.push_back(Debug);
    ContextCategory* CInput = new ContextCategory("Input");
    CM->Categories.push_back(CInput);

    ContextCategory* CSTD = new ContextCategory("Custom");
    CM->Categories.push_back(CSTD);
    //NODE DEFINITIONS

    //Math
    Math->Options.push_back(new NodeCreator(Node::Node_Type::Node_Calculation,
        { DataPort(Data::Data_Type::Numeric), DataPort(Data::Data_Type::Numeric) }, 
        { DataPort(Data::Data_Type::Numeric) }, "Add", Math, NodeMath::Add));
    Math->Options.push_back(new NodeCreator(Node::Node_Type::Node_Calculation,
        { DataPort(Data::Data_Type::Numeric), DataPort(Data::Data_Type::Numeric) },
        { DataPort(Data::Data_Type::Numeric) }, "Subtract", Math, NodeMath::Subtract));
    Math->Options.push_back(new NodeCreator(Node::Node_Type::Node_Calculation,
        { DataPort(Data::Data_Type::Numeric), DataPort(Data::Data_Type::Numeric) },
        { DataPort(Data::Data_Type::Numeric) }, "Multiply", Math, NodeMath::Multiply));
    Math->Options.push_back(new NodeCreator(Node::Node_Type::Node_Calculation,
        { DataPort(Data::Data_Type::Numeric), DataPort(Data::Data_Type::Numeric) },
        { DataPort(Data::Data_Type::Numeric) }, "Divide", Math, NodeMath::Divide));
    Math->Options.push_back(new NodeCreator(Node::Node_Type::Node_Calculation,
        { DataPort(Data::Data_Type::Integer), DataPort(Data::Data_Type::Integer) },
        { DataPort(Data::Data_Type::Integer) }, "Modulo", Math, NodeMath::Mod));
    Math->Options.push_back(new NodeCreator(Node::Node_Type::Node_Calculation,
        { DataPort(Data::Data_Type::Numeric), DataPort(Data::Data_Type::Numeric) },
        { DataPort(Data::Data_Type::Numeric) }, "Pow", Math, NodeMath::Pow));

    Math->Options.push_back(new NodeCreator(Node::Node_Type::Node_Calculation,
        { DataPort(Data::Data_Type::Numeric) },
        { DataPort(Data::Data_Type::Numeric) }, "Abs", Math, NodeMath::Abs));
    Math->Options.push_back(new NodeCreator(Node::Node_Type::Node_Calculation,
        { DataPort(Data::Data_Type::Numeric) },
        { DataPort(Data::Data_Type::Numeric) }, "Square Root", Math, NodeMath::Sqrt));
    Math->Options.push_back(new NodeCreator(Node::Node_Type::Node_Calculation,
        { DataPort(Data::Data_Type::Numeric) },
        { DataPort(Data::Data_Type::Numeric) }, "Round", Math, NodeMath::Round));
    Math->Options.push_back(new NodeCreator(Node::Node_Type::Node_Calculation,
        { DataPort(Data::Data_Type::Numeric) },
        { DataPort(Data::Data_Type::Numeric) }, "Floor", Math, NodeMath::Floor));
    Math->Options.push_back(new NodeCreator(Node::Node_Type::Node_Calculation,
        { DataPort(Data::Data_Type::Numeric) },
        { DataPort(Data::Data_Type::Numeric) }, "Ceil", Math, NodeMath::Ceil));
    Math->Options.push_back(new NodeCreator(Node::Node_Type::Node_Calculation,
        { DataPort(Data::Data_Type::Numeric) },
        { DataPort(Data::Data_Type::Numeric) }, "Log", Math, NodeMath::Log));
    Math->Options.push_back(new NodeCreator(Node::Node_Type::Node_Calculation,
        { DataPort(Data::Data_Type::Numeric) },
        { DataPort(Data::Data_Type::Numeric) }, "Negate", Math, NodeMath::Negate));

    //Trig
    Trig->Options.push_back(new NodeCreator(Node::Node_Type::Node_Calculation,
        { DataPort(Data::Data_Type::Numeric) },
        { DataPort(Data::Data_Type::Numeric) }, "Sine", Trig, NodeTrig::Sin));
    Trig->Options.push_back(new NodeCreator(Node::Node_Type::Node_Calculation,
        { DataPort(Data::Data_Type::Numeric) },
        { DataPort(Data::Data_Type::Numeric) }, "Cosine", Trig, NodeTrig::Cos));
    Trig->Options.push_back(new NodeCreator(Node::Node_Type::Node_Calculation,
        { DataPort(Data::Data_Type::Numeric) },
        { DataPort(Data::Data_Type::Numeric) }, "Tangent", Trig, NodeTrig::Tan));
    Trig->Options.push_back(new NodeCreator(Node::Node_Type::Node_Calculation,
        { DataPort(Data::Data_Type::Numeric) },
        { DataPort(Data::Data_Type::Numeric) }, "Arc Sine", Trig, NodeTrig::Asin));
    Trig->Options.push_back(new NodeCreator(Node::Node_Type::Node_Calculation,
        { DataPort(Data::Data_Type::Numeric) },
        { DataPort(Data::Data_Type::Numeric) }, "Arc Cosine", Trig, NodeTrig::Acos));
    Trig->Options.push_back(new NodeCreator(Node::Node_Type::Node_Calculation,
        { DataPort(Data::Data_Type::Numeric) },
        { DataPort(Data::Data_Type::Numeric) }, "Arc Tangent", Trig, NodeTrig::Atan));

    //Comparison
    Comparison->Options.push_back(new NodeCreator(Node::Node_Type::Node_Calculation,
        { DataPort(Data::Data_Type::Boolean), DataPort(Data::Data_Type::Boolean) },
        { DataPort(Data::Data_Type::Boolean) }, "And", Comparison, NodeComparison::And));
    Comparison->Options.push_back(new NodeCreator(Node::Node_Type::Node_Calculation,
        { DataPort(Data::Data_Type::Boolean), DataPort(Data::Data_Type::Boolean) },
        { DataPort(Data::Data_Type::Boolean) }, "Or", Comparison, NodeComparison::Or));

    Comparison->Options.push_back(new NodeCreator(Node::Node_Type::Node_Calculation,
        { DataPort(Data::Data_Type::Boolean) },
        { DataPort(Data::Data_Type::Boolean) }, "Not", Comparison, NodeComparison::Not));

    //Casting
    Casting->Options.push_back(new NodeCreator(Node::Node_Type::Node_Calculation,
        { DataPort(Data::Data_Type::Boolean) },
        { DataPort(Data::Data_Type::Integer) }, "Bool to Int", Casting, NodeCast::BooltoInt));
    Casting->Options.push_back(new NodeCreator(Node::Node_Type::Node_Calculation,
        { DataPort(Data::Data_Type::Integer) },
        { DataPort(Data::Data_Type::Boolean) }, "Int to Bool", Casting, NodeCast::InttoBool));
    Casting->Options.push_back(new NodeCreator(Node::Node_Type::Node_Calculation,
        { DataPort(Data::Data_Type::Integer) },
        { DataPort(Data::Data_Type::Float) }, "Int to Float", Casting, NodeCast::InttoFloat));

    Casting->Options.push_back(new NodeCreator(Node::Node_Type::Node_Calculation,
        { DataPort(Data::Data_Type::Weird) },
        { DataPort(Data::Data_Type::Float) }, "Weird to Float", Casting, NodeCast::WeirdtoFloat));
    Casting->Options.push_back(new NodeCreator(Node::Node_Type::Node_Calculation,
        { DataPort(Data::Data_Type::Float) },
        { DataPort(Data::Data_Type::Weird) }, "Float to Weird", Casting, NodeCast::FloattoWeird));

    Casting->Options.push_back(new NodeCreator(Node::Node_Type::Node_Calculation,
        { DataPort(Data::Data_Type::Weird) },
        { DataPort(Data::Data_Type::String) }, "Weird to String", Casting, NodeCast::WeirdtoString));
    Casting->Options.push_back(new NodeCreator(Node::Node_Type::Node_Calculation,
        { DataPort(Data::Data_Type::String) },
        { DataPort(Data::Data_Type::Weird) }, "String to Weird", Casting, NodeCast::StringtoWeird));


    //Debug
    Debug->Options.push_back(new NodeCreator(Node::Node_Type::Node_Action,
        { DataPort(Data::Data_Type::String) },
        {  }, "Print", Debug, NodeDebug::Print));

    //Input
    CInput->Options.push_back(new NodeCreator(Node::Node_Type::Node_Input,
        {  },
        { DataPort(Data::Data_Type::Float) }, "Time", CInput, NodeInput::Time));

    AddCustomEmptyCalculation(CSTD, "test", TestFunction);
    AddCustomEmptyCalculation(CSTD, "String Example", TestString);
    //static cast to choose which overload
    AddCustomStandardCalculation(CSTD, "test 2", static_cast<double (*)(double)>(&std::sqrt));


    return CM;
}

//Debug


