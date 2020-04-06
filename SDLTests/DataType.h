#pragma once
#include <iostream>
class Data
{
public:
	enum class Data_Type {
		Boolean,
		Integer,
		Float,
		String,
		Weird,
		None
	};
	Data_Type type;
};

class NodeBoolean : public Data
{
public:
	bool value;
	NodeBoolean(bool input)
	{
		value = input;
		type = Data_Type::Boolean;
	}
};

class NodeInteger : public Data
{
public:
	int value;
	NodeInteger(int input)
	{
		value = input;
		type = Data_Type::Integer;
	}
};

class NodeFloat : public Data
{
public:
	float value;
	NodeFloat(float input)
	{
		value = input;
		type = Data_Type::Float;
	}
};

class NodeString : public Data
{
public:
	const char* value;
	NodeString(const char* input)
	{
		value = input;
		type = Data_Type::String;
	}
};

class NodeWeird : public Data
{
public:
	void* value;
	NodeWeird(void* input)
	{
		value = input;
		type = Data_Type::Weird;
	}
};

class NodeNone : public Data
{
public:
	NodeNone()
	{
		type = Data_Type::None;
	}
};

class DataPort
{
public:
	Data::Data_Type PortType;
	DataPort(Data::Data_Type inType = Data::Data_Type::None)
	{
		PortType = inType;
	}
};