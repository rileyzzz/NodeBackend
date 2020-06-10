#pragma once
#include <vector>
#include "Node.h"
#include "ContextMenu.h"
#include "DataType.h"
#include <cstdarg>
#include <locale>

namespace NodeEdit
{
    std::vector<Input*> CreateInputs(std::vector<DataPort> itemarray);

    std::vector<Output*> CreateOutputs(std::vector<DataPort> itemarray);

    Data* CalculateLinkChain(Output* srcLink);

    void Unlink(Link* inLink);

    Data* GetNodeDefault(Data::Data_Type intype);

    //basic example
    ContextMenu* GenerateContextMenu();

    const char* myToUppercase(const char* in);

    template <class I>
    void AddCustomEmptyCalculation(ContextCategory* inCM, const char* name, I(*f)())
    {
        //va_list ap;
        //va_start(ap);

        auto buildfunc = [f]()
        {
            I returnvalue = f();
            NodeWeird<I>* returnnode = new NodeWeird<I>(returnvalue);
            Data* convertnode = (Data*)returnnode;
            return convertnode;
        };

        inCM->Options.push_back(new NodeCreator(Node::Node_Type::Node_Input,
            {  },
            { DataPort(Data::Data_Type::Weird) }, name, inCM, buildfunc));
    }
    //I - function final output type, T - function required input type
    template <class I, class T>
    void AddCustomStandardCalculation(ContextCategory* inCM, const char* name, I(*f)(T))
    {

        auto buildfunc = [f](Data* argument)
        {
            NodeWeird<T>* convertedargument = (NodeWeird<T>*)argument;
            I returnvalue = f(convertedargument->value);
            NodeWeird<I>* returnnode = new NodeWeird<I>(returnvalue);
            Data* convertnode = (Data*)returnnode;
            return convertnode;
        };

        inCM->Options.push_back(new NodeCreator(Node::Node_Type::Node_Calculation,
            { DataPort(Data::Data_Type::Weird) },
            { DataPort(Data::Data_Type::Weird) }, name, inCM, buildfunc));
    }
}