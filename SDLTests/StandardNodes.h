#pragma once
#include <vector>
#include "Node.h"
#include <deque>

//Math
class NodeMath
{
public:
	static Data* Add(std::vector<Data*> Inputs);

	static Data* Subtract(std::vector<Data*> Inputs);
	
	static Data* Multiply(std::vector<Data*> Inputs);

	static Data* Divide(std::vector<Data*> Inputs);

	static Data* Mod(std::vector<Data*> Inputs);

	static Data* Abs(std::vector<Data*> Inputs);

	static Data* Sqrt(std::vector<Data*> Inputs);

	static Data* Round(std::vector<Data*> Inputs);

	static Data* Floor(std::vector<Data*> Inputs);

	static Data* Ceil(std::vector<Data*> Inputs);

	static Data* Log(std::vector<Data*> Inputs);

	static Data* Pow(std::vector<Data*> Inputs);

	static Data* Negate(std::vector<Data*> Inputs);
};

class NodeTrig
{
public:
	static Data* Sin(std::vector<Data*> Inputs);

	static Data* Cos(std::vector<Data*> Inputs);

	static Data* Tan(std::vector<Data*> Inputs);

	static Data* Asin(std::vector<Data*> Inputs);

	static Data* Acos(std::vector<Data*> Inputs);

	static Data* Atan(std::vector<Data*> Inputs);
};

class NodeComparison
{
public:
	static Data* And(std::vector<Data*> Inputs);

	static Data* Or(std::vector<Data*> Inputs);

	static Data* Not(std::vector<Data*> Inputs);
};

//Casting
class NodeCast
{
public:
	static Data* BooltoInt(std::vector<Data*> Inputs);

	static Data* InttoBool(std::vector<Data*> Inputs);

	static Data* InttoFloat(std::vector<Data*> Inputs);

	static Data* WeirdtoFloat(std::vector<Data*> Inputs);

	static Data* FloattoWeird(std::vector<Data*> Inputs);

	static Data* WeirdtoString(std::vector<Data*> Inputs);

	static Data* StringtoWeird(std::vector<Data*> Inputs);
};

//Debug
struct ConsoleMessage
{
	const char* message;
	int messageLength;
};
class NodeDebug
{
public:
	static bool Print(std::vector<Data*> Inputs);
	static std::deque<ConsoleMessage> console;
	static void MessageThread(NodeString* input);
};

class NodeInput
{
	static time_t start;
public:
	static Data* Time();
};
