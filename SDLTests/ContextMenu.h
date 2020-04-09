#pragma once
#include <vector>
#include "Node.h"
//basic data definition for how categories work - a context menu should be declared once and then referenced on right click etc
class ContextRenderable
{
	//used for hitbox purposes only - exact screen coordinates
public:
	int x, y, w, h;
};
class ContextOption
{
	//typically a node or UI function
public:
	enum class Option_Type {
		CreateNode,
		UI
	};
	Option_Type Type;
	const char* Name;
	ContextRenderable* Renderable;
	bool Selected;
};

class NodeCreator : public ContextOption
{
public:
	std::vector<DataPort> inPorts;
	std::vector<DataPort> outPorts;
	Node::Node_Type NodeType;
	typedef Data* (*MyFunction)(std::vector<Data*>);
	MyFunction Function;
	NodeCreator(Node::Node_Type inType, std::vector<DataPort> inputs, std::vector<DataPort> outputs, const char* inName, Data* (*f)(std::vector<Data*>) = nullptr)
	{
		Type = Option_Type::CreateNode;
		NodeType = inType;
		inPorts = inputs;
		outPorts = outputs;
		Name = inName;
		Selected = false;
		Renderable = new ContextRenderable();
		if (f)
		{
			Function = f;
		}
	}
};

class ContextCategory
{
public:
	const char* CategoryName;
	std::vector<ContextOption*> Options;
	bool Show;
	ContextRenderable* Renderable;
	ContextCategory(const char* name)
	{
		CategoryName = name;
		Show = false;
		Renderable = new ContextRenderable();
	}
};

class ContextMenu
{
public:
	std::vector<ContextCategory*> Categories;
};

