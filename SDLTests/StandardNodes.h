#pragma once
#include <vector>
#include "Node.h"
//Math
class NodeMath
{
public:
	static Data* Add(std::vector<Data*> Inputs);

	static Data* Subtract(std::vector<Data*> Inputs);
	
	static Data* Multiply(std::vector<Data*> Inputs);

	static Data* Divide(std::vector<Data*> Inputs);

	static Data* Abs(std::vector<Data*> Inputs);

	static Data* Sqrt(std::vector<Data*> Inputs);

	static Data* Round(std::vector<Data*> Inputs);

	static Data* Floor(std::vector<Data*> Inputs);

	static Data* Ceil(std::vector<Data*> Inputs);

	static Data* Log(std::vector<Data*> Inputs);

	static Data* Pow(std::vector<Data*> Inputs);
};

//Casting
class NodeCast
{
public:
	static Data* BooltoInt(std::vector<Data*> Inputs);

	static Data* InttoBool(std::vector<Data*> Inputs);

	static Data* InttoFloat(std::vector<Data*> Inputs);
};

//Debug
class NodeDebug
{
public:
	static bool Print(std::vector<Data*> Inputs);
};
