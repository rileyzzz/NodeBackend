#pragma once
#include <vector>
#include "Node.h"
#include <functional>
class ContextOption;
class ContextCategory;
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
	ContextCategory* Parent;
};

class NodeCreator : public ContextOption
{
public:
	std::vector<DataPort> inPorts;
	std::vector<DataPort> outPorts;
	Node::Node_Type NodeType;
	typedef Data* (*MyFunction)(std::vector<Data*>);
	MyFunction Function;

	typedef bool (*Action)(std::vector<Data*>);
	Action nodeAction;

	typedef Data* (*MyInput)();
	MyInput nodeInput;

	bool weird;
	std::function<Data * ()> altInput;

	NodeCreator(Node::Node_Type inType, std::vector<DataPort> inputs, std::vector<DataPort> outputs, const char* inName, ContextCategory* inParent, Data* (*f)(std::vector<Data*>) = nullptr)
	{
		Type = Option_Type::CreateNode;
		NodeType = inType;
		inPorts = inputs;
		outPorts = outputs;
		Name = inName;
		Selected = false;
		Parent = inParent;
		Renderable = new ContextRenderable();
		weird = false;
		if (f)
		{
			Function = f;
		}
	}
	NodeCreator(Node::Node_Type inType, std::vector<DataPort> inputs, std::vector<DataPort> outputs, const char* inName, ContextCategory* inParent, bool (*f)(std::vector<Data*>) = nullptr)
	{
		Type = Option_Type::CreateNode;
		NodeType = inType;
		inPorts = inputs;
		outPorts = outputs;
		Name = inName;
		Selected = false;
		Parent = inParent;
		weird = false;
		Renderable = new ContextRenderable();
		if (f)
		{
			nodeAction = f;
		}
	}
	NodeCreator(Node::Node_Type inType, std::vector<DataPort> inputs, std::vector<DataPort> outputs, const char* inName, ContextCategory* inParent, std::function<Data*()> f)
	{
		Type = Option_Type::CreateNode;
		NodeType = inType;
		inPorts = inputs;
		outPorts = outputs;
		Name = inName;
		Selected = false;
		Parent = inParent;
		weird = true;
		Renderable = new ContextRenderable();
		if (f)
		{
			altInput = f;
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

