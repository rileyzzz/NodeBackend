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
		Numeric,
		Weird,
		None
	};
	Data_Type type;
	Data_Type subtype;
};

class NodeNumeric : public Data
{
public:
	double value = 0;
};

class NodeBoolean : public Data
{
public:
	bool value;
	NodeBoolean(bool input)
	{
		value = input;
		type = Data_Type::Boolean;
		subtype = Data_Type::Boolean;
	}
	NodeBoolean()
	{
		value = false;
		type = Data_Type::Boolean;
		subtype = Data_Type::Boolean;
	}
};

class NodeInteger : public NodeNumeric
{
public:
	NodeInteger(int input)
	{
		value = input;
		type = Data_Type::Integer;
		subtype = Data_Type::Numeric;
	}
	NodeInteger()
	{
		value = 0;
		type = Data_Type::Integer;
		subtype = Data_Type::Numeric;
	}
};

class NodeFloat : public NodeNumeric
{
public:
	NodeFloat(double input)
	{
		value = input;
		type = Data_Type::Float;
		subtype = Data_Type::Numeric;
	}
	NodeFloat()
	{
		value = 0;
		type = Data_Type::Float;
		subtype = Data_Type::Numeric;
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
		subtype = Data_Type::String;
	}
	NodeString()
	{
		value = "";
		type = Data_Type::String;
		subtype = Data_Type::String;
	}
};

template <class T>
class NodeWeird : public Data
{
public:
	T value;
	NodeWeird(T input)
	{
		value = input;
		type = Data_Type::Weird;
		subtype = Data_Type::Weird;
	}
};

class NodeNone : public Data
{
public:
	NodeNone()
	{
		type = Data_Type::None;
		subtype = Data_Type::None;
	}
};

class DataPort
{
public:
	Data::Data_Type PortType;
	Data::Data_Type SubType;
	int renderX, renderY;
	int offsetX, offsetY;
	DataPort(Data::Data_Type inType = Data::Data_Type::None)
	{
		PortType = inType;
		if (inType == Data::Data_Type::Integer || inType == Data::Data_Type::Float)
		{
			SubType = Data::Data_Type::Numeric;
		}
		else
		{
			SubType = inType;
		}
		renderX = 0;
		renderY = 0;
		offsetX = 0;
		offsetY = 0;
	}
};

