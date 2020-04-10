// SDLTests.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "SDL.h"
#include <SDL_image.h> 
#include <SDL_timer.h> 
#include <math.h>  
#include <vector>
#include "Node.h"
#include "NodeHelpers.h"
#include "StandardNodes.h"
#include "SDL_ttf.h"
#include <string>
#include <algorithm>
extern "C" { FILE __iob_func[3] = { *stdin,*stdout,*stderr }; }

//each programmable object will contain a nodestack and id count that persist
int IDcount = 0;
std::vector<Node*> NodeStack;
std::vector<Link*> LinkStack;
std::vector<ContextCategory*> CategoryStack;
std::vector<ContextOption*> OptionStack;
int gridoffsetX = 300;
int gridoffsetY = 300;
int MMoffsetX = 0;
int MMoffsetY = 0;
int gridParallaxAmount = 4;

int globalScaleFactor = 1;

bool DrawNodeOutput = true;
bool ctrlHeld = false;

DataNode* CreateNode(Node::Node_Type type, std::vector<Input*> inputs, std::vector<Output*> outputs, const char* title, int x = 0, int y = 0, Data* (*f)(std::vector<Data*>) = nullptr)
{
    DataNode* NewNode = new DataNode(IDcount, type, inputs, outputs, title, x, y, f);
    IDcount++;
    NodeStack.push_back(NewNode);
    return NewNode;
}
DataNode* CreateInputNode(Node::Node_Type type, std::vector<Input*> inputs, std::vector<Output*> outputs, const char* title, int x = 0, int y = 0, Data* (*f)() = nullptr)
{
    DataNode* NewNode = new DataNode(IDcount, type, inputs, outputs, title, x, y, f);
    IDcount++;
    NodeStack.push_back(NewNode);
    return NewNode;
}

DataNode* CreateWeirdInput(Node::Node_Type type, std::vector<Input*> inputs, std::vector<Output*> outputs, const char* title, std::function<Data * ()> f, int x = 0, int y = 0)
{
    DataNode* NewNode = new DataNode(IDcount, type, inputs, outputs, title, f, x, y);
    IDcount++;
    NodeStack.push_back(NewNode);
    return NewNode;
}

DataNode* CreateWeirdCalc(Node::Node_Type type, std::vector<Input*> inputs, std::vector<Output*> outputs, const char* title, std::function<Data * (Data*)> f, int x = 0, int y = 0)
{
    DataNode* NewNode = new DataNode(IDcount, type, inputs, outputs, title, f, x, y);
    IDcount++;
    NodeStack.push_back(NewNode);
    return NewNode;
}

static void DestroyNode(Node* node)
{
    NodeStack.erase(std::remove(NodeStack.begin(), NodeStack.end(), node), NodeStack.end());
}

ActionNode* CreateActionNode(Node::Node_Type type, std::vector<Input*> inputs, std::vector<Output*> outputs, const char* title, ActionNode::ActionType FlowType, int x = 0, int y = 0, bool (*f)(std::vector<Data*>) = nullptr)
{
    ActionNode* NewNode = new ActionNode(IDcount, type, inputs, outputs, title, FlowType, x, y, f);
    IDcount++;
    NodeStack.push_back(NewNode);
    return NewNode;
}


void GetScreenCoordinates(int x, int y, int* sendX, int* sendY)
{
    //int returnarr[2];
    *sendX = (gridoffsetX + MMoffsetX) + x * globalScaleFactor;
    *sendY = (gridoffsetY + MMoffsetY) + y * globalScaleFactor;
}

void GetGridCoordinates(int x, int y, int* sendX, int* sendY)
{
    //int returnarr[2];
    *sendX = (x - (gridoffsetX + MMoffsetX)) / globalScaleFactor;
    *sendY = (y - (gridoffsetY + MMoffsetY)) / globalScaleFactor;
}

void LinkActionNodes(ActionNode* Node1, ActionNode* Node2)
{
    Node1->Next = Node2;
    Node2->Previous = Node1;
}
void UnlinkActionNodes(ActionNode* Node1, ActionNode* Node2)
{
    Node1->Next = nullptr;
    Node2->Previous = nullptr;
}

int main(int argc, char* argv[])
{
    std::cout << "Starting SDL2\n";

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("error initializing SDL: %s\n", SDL_GetError());
    }

    int scrw = 1920;
    int scrh = 1080;
    SDL_Window* win = SDL_CreateWindow("NodeEditor", // creates a window 
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        scrw, scrh, 0);

    //int scrw = 3840;
    //int scrh = 2160;
    //SDL_Window* win = SDL_CreateWindow("NodeEditor", // creates a window 
    //    SDL_WINDOWPOS_CENTERED,
    //    SDL_WINDOWPOS_CENTERED,
    //    scrw, scrh, SDL_WINDOW_FULLSCREEN);

    // triggers the program that controls 
    // your graphics hardware and sets flags 
    Uint32 render_flags = SDL_RENDERER_ACCELERATED;

    // creates a renderer to render our images 
    SDL_Renderer* rend = SDL_CreateRenderer(win, -1, render_flags);

    // creates a surface to load an image into the main memory 
    SDL_Surface* surface;

    // please provide a path for your image 
    surface = IMG_Load("C:/Users/riley/Pictures/grid.png");

    // loads image to our graphics hardware memory. 
    SDL_Texture* tex = SDL_CreateTextureFromSurface(rend, surface);

    // clears main-memory 
    SDL_FreeSurface(surface);

    // let us control our image position 
    // so that we can move it with our keyboard. 
    SDL_Rect dest;

    // connects our texture with dest to control position 
    SDL_QueryTexture(tex, NULL, NULL, &dest.w, &dest.h);
    
    int close = 0;
    int speed = 300;

    bool MiddleMouse = false;
    int startX, startY;
    

    int zoomLevel = 1;
    int zoomMax = 5;
    int zoomMin = 1;

    

    bool showNodeList = false;
    bool draggingNode = false;
    std::vector<Node*> currentDrag;
    bool draggingPort = false;
    DataPort* currentDragPort = nullptr;
    Linkable* currentDragPortParent = nullptr;
    bool draggingNewPort = false;
    DataPort* currentDragNewPort = nullptr;
    Linkable* currentDragNewPortParent = nullptr;

    bool draggingFlow = false;
    DataFlow* currentDragFlow = nullptr;
    bool draggingNewFlow = false;
    DataFlow* currentDragNewFlow = nullptr;

    ContextMenu* RightClickContext = GenerateContextMenu();
    bool ContextMenuOpen = false;
    int ContextX, ContextY;
    CategoryStack = RightClickContext->Categories;
    for (const auto& value : RightClickContext->Categories) {
        OptionStack.insert(OptionStack.end(), value->Options.begin(), value->Options.end());
    }

    //Create example node
    std::vector<DataPort> inPorts{  };
    std::vector<DataPort> outPorts{ DataPort(Data::Data_Type::Integer) };
    std::vector<Input*> nodeInputs;
    std::vector<Output*> nodeOutputs = CreateOutputs(outPorts);

    DataNode* ExampleNode = CreateNode(Node::Node_Type::Node_Input, nodeInputs, nodeOutputs, "Integer Input", 0, -20);

    //set the initial input value! this is important
    ExampleNode->InputData = new NodeInteger(5);


    std::vector<DataPort> tempinPorts{  };
    std::vector<DataPort> tempoutPorts{ DataPort(Data::Data_Type::Float) };
    std::vector<Input*> tempnodeInputs;
    std::vector<Output*> tempnodeOutputs = CreateOutputs(tempoutPorts);

    DataNode* tempExampleNode = CreateNode(Node::Node_Type::Node_Input, tempnodeInputs, tempnodeOutputs, "Float Input", 0, 50);

    //set the initial input value! this is important
    tempExampleNode->InputData = new NodeFloat(2.5);

    std::vector<DataPort> inPorts2{ DataPort(Data::Data_Type::Numeric), DataPort(Data::Data_Type::Numeric) };
    std::vector<DataPort> outPorts2{ DataPort(Data::Data_Type::Float) };
    std::vector<Input*> nodeInputs2 = CreateInputs(inPorts2);
    std::vector<Output*> nodeOutputs2 = CreateOutputs(outPorts2);

    DataNode* ExampleNode2 = CreateNode(Node::Node_Type::Node_Calculation, nodeInputs2, nodeOutputs2, "Add", 150, 0, NodeMath::Add);
    

    std::vector<DataPort> inPorts3{ DataPort(Data::Data_Type::Integer) };
    std::vector<DataPort> outPorts3{ };
    std::vector<Input*> nodeInputs3 = CreateInputs(inPorts3);
    std::vector<Output*> nodeOutputs3;

    DataNode* ExampleNode3 = CreateNode(Node::Node_Type::Node_Output, nodeInputs3, nodeOutputs3, "Output", 300, 0);

    //Setup example connections
    //ExampleNode3->inputs[0]->link = ExampleNode2->outputs[0];
    //ExampleNode2->inputs[0]->link = ExampleNode->outputs[0];
    //ExampleNode2->inputs[1]->link = tempExampleNode->outputs[0];
    LinkStack.push_back(new Link(ExampleNode3, 0, ExampleNode2, 0));
    LinkStack.push_back(new Link(ExampleNode2, 0, ExampleNode, 0));
    LinkStack.push_back(new Link(ExampleNode2, 1, tempExampleNode, 0));


    //Data* outputresult = ExampleNode3->CalculateInputs();
    //NodeFloat* calcvalue = (NodeFloat*)outputresult;

    //std::cout << "Output result is: ";
    //if (outputresult->type == Data::Data_Type::Float)
    //{
    //    std::cout << "success - value is: ";
    //    std::cout << calcvalue->value;
    //}
    //else {
    //    std::cout << "fail";
    //}
    //std::cout << "\n";


    //Example Action Graph
    std::vector<DataPort> EventinPorts{  };
    std::vector<DataPort> EventoutPorts{ DataPort(Data::Data_Type::String) };
    std::vector<Input*> EventnodeInputs;
    std::vector<Output*> EventnodeOutputs = CreateOutputs(EventoutPorts);

    ActionNode* EventExampleNode = CreateActionNode(Node::Node_Type::Node_Event, EventnodeInputs, EventnodeOutputs, "Key Enter", ActionNode::ActionType::RightOutput, 0, 200);


    std::vector<DataPort> PrintinPorts{ DataPort(Data::Data_Type::String) };
    std::vector<DataPort> PrintoutPorts{  };
    std::vector<Input*> PrintnodeInputs = CreateInputs(PrintinPorts);
    std::vector<Output*> PrintnodeOutputs;

    ActionNode* PrintExampleNode = CreateActionNode(Node::Node_Type::Node_Action, PrintnodeInputs, PrintnodeOutputs, "Print", ActionNode::ActionType::DoubleSided, 150, 200, NodeDebug::Print);

    //Link the flow between event and print
    LinkActionNodes(EventExampleNode, PrintExampleNode);

    //Link the IO string
    //PrintExampleNode->inputs[0]->link = EventExampleNode->outputs[0];
    LinkStack.push_back(new Link(PrintExampleNode, 0, EventExampleNode, 0));

    //Once again, event input data is managed through the InputData variable, since an event can also act as an input - completely optional
    EventExampleNode->InputData = new NodeString("Testing from the Event node!");


    std::vector<DataPort> strinPorts{  };
    std::vector<DataPort> stroutPorts{ DataPort(Data::Data_Type::String) };
    std::vector<Input*> strnodeInputs;
    std::vector<Output*> strnodeOutputs = CreateOutputs(stroutPorts);

    DataNode* strExampleNode = CreateNode(Node::Node_Type::Node_Input, strnodeInputs, strnodeOutputs, "String Input", 0, 300);

    //set the initial input value! this is important
    strExampleNode->InputData = new NodeString("This is a test string!");



    //boolean example
    std::vector<DataPort> boolinPorts{  };
    std::vector<DataPort> booloutPorts{ DataPort(Data::Data_Type::Boolean) };
    std::vector<Input*> boolnodeInputs;
    std::vector<Output*> boolnodeOutputs = CreateOutputs(booloutPorts);

    DataNode* boolExampleNode = CreateNode(Node::Node_Type::Node_Input, boolnodeInputs, boolnodeOutputs, "True Input", 0, 400);

    //set the initial input value! this is important
    boolExampleNode->InputData = new NodeBoolean(true);

    std::vector<DataPort> bool2inPorts{  };
    std::vector<DataPort> bool2outPorts{ DataPort(Data::Data_Type::Boolean) };
    std::vector<Input*> bool2nodeInputs;
    std::vector<Output*> bool2nodeOutputs = CreateOutputs(bool2outPorts);

    DataNode* bool2ExampleNode = CreateNode(Node::Node_Type::Node_Input, bool2nodeInputs, bool2nodeOutputs, "False Input", 0, 450);

    //set the initial input value! this is important
    bool2ExampleNode->InputData = new NodeBoolean(false);


    std::vector<DataPort> AndinPorts{ DataPort(Data::Data_Type::Boolean), DataPort(Data::Data_Type::Boolean) };
    std::vector<DataPort> AndoutPorts{ DataPort(Data::Data_Type::Boolean) };
    std::vector<Input*> AndnodeInputs = CreateInputs(AndinPorts);
    std::vector<Output*> AndnodeOutputs = CreateOutputs(AndoutPorts);

    DataNode* AndExampleNode = CreateNode(Node::Node_Type::Node_Calculation, AndnodeInputs, AndnodeOutputs, "And", 150, 400, NodeComparison::And);

    std::vector<DataPort> OrinPorts{ DataPort(Data::Data_Type::Boolean), DataPort(Data::Data_Type::Boolean) };
    std::vector<DataPort> OroutPorts{ DataPort(Data::Data_Type::Boolean) };
    std::vector<Input*> OrnodeInputs = CreateInputs(OrinPorts);
    std::vector<Output*> OrnodeOutputs = CreateOutputs(OroutPorts);

    DataNode* OrExampleNode = CreateNode(Node::Node_Type::Node_Calculation, OrnodeInputs, OrnodeOutputs, "Or", 150, 475, NodeComparison::Or);

    std::vector<DataPort> CompinPorts{ DataPort(Data::Data_Type::Boolean) };
    std::vector<DataPort> CompoutPorts{ };
    std::vector<Input*> CompnodeInputs = CreateInputs(CompinPorts);
    std::vector<Output*> CompnodeOutputs;

    DataNode* CompExampleNode = CreateNode(Node::Node_Type::Node_Output, CompnodeInputs, CompnodeOutputs, "Output", 300, 425);






    //Node texture
    SDL_Surface* Nodesurface;

    Nodesurface = IMG_Load("C:/Users/riley/Pictures/H2x1_SNES_SuperMarioWorld_image1600w.jpg");
    SDL_Texture* Nodetex = SDL_CreateTextureFromSurface(rend, Nodesurface);
    SDL_FreeSurface(Nodesurface);

    SDL_Rect Nodedest;
    SDL_Rect NodeElement;
    SDL_Rect Portdest;
    SDL_Surface* Portsurface;
    SDL_Surface* Flowsurface;
    Portsurface = IMG_Load("C:/Users/riley/source/repos/SDLTests/x64/Debug/io.png");
    SDL_Texture* Porttex = SDL_CreateTextureFromSurface(rend, Portsurface);
    SDL_FreeSurface(Portsurface);

    Flowsurface = IMG_Load("C:/Users/riley/source/repos/SDLTests/x64/Debug/io.png");
    SDL_Texture* Flowtex = SDL_CreateTextureFromSurface(rend, Flowsurface);
    SDL_FreeSurface(Flowsurface);

    SDL_QueryTexture(tex, NULL, NULL, &Nodedest.w, &Nodedest.h);

    TTF_Init();


    int PortHitboxSize = 8 * (globalScaleFactor * 1.5);

    // animation loop 
    while (!close) {
        SDL_Event event;
        
        // Events mangement 
        while (SDL_PollEvent(&event)) {
            switch (event.type) {

            case SDL_QUIT:
                // handling of close button 
                close = 1;
                break;

            case SDL_KEYDOWN:
                // keyboard API for key pressed 
                switch (event.key.keysym.scancode) {
                case SDL_SCANCODE_W:
                case SDL_SCANCODE_UP:
                    gridoffsetY -= speed / 30;
                    break;
                case SDL_SCANCODE_A:
                case SDL_SCANCODE_LEFT:
                    gridoffsetX -= speed / 30;
                    break;
                case SDL_SCANCODE_S:
                case SDL_SCANCODE_DOWN:
                    gridoffsetY += speed / 30;
                    break;
                case SDL_SCANCODE_D:
                case SDL_SCANCODE_RIGHT:
                    gridoffsetX += speed / 30;
                    break;
                case SDL_SCANCODE_LSHIFT:
                    ctrlHeld = true;
                    break;
                case SDL_SCANCODE_ESCAPE:
                    currentDrag.clear();
                    for (const auto& unselectNode : NodeStack) {
                        unselectNode->Selected = false;
                    }
                    break;
                case SDL_SCANCODE_DELETE:
                    for (const auto& deleteNode : NodeStack) {
                        if (deleteNode->Selected && deleteNode->type != Node::Node_Type::Node_Output)
                        {
                            for (const auto& deleteInput : deleteNode->inputs) {
                                if (deleteInput->currentLink)
                                {
                                    LinkStack.erase(std::remove(LinkStack.begin(), LinkStack.end(), deleteInput->currentLink), LinkStack.end());
                                    Unlink(deleteInput->currentLink);
                                }
                            }
                            for (const auto& deleteOutput : deleteNode->outputs) {
                                if (deleteOutput->currentLink)
                                {
                                    LinkStack.erase(std::remove(LinkStack.begin(), LinkStack.end(), deleteOutput->currentLink), LinkStack.end());
                                    Unlink(deleteOutput->currentLink);
                                }
                            }
                            DestroyNode(deleteNode);
                        }
                    }
                    break;
                case SDL_SCANCODE_RETURN:
                {
                    //Run our blueprint
                    EventExampleNode->Run();

                    Data* returnval = ExampleNode3->CalculateInputs();
                    NodeFloat* calcvalue = (NodeFloat*)returnval;

                    std::cout << "Output result is: ";
                    std::cout << calcvalue->value;
                    std::cout << "\n";

                    Data* boolcalc = CompExampleNode->CalculateInputs();
                    NodeBoolean* boolresult = (NodeBoolean*)boolcalc;
                    if (boolresult->value == true)
                    {
                        std::cout << "Boolean returned true.\n";
                    }
                    else
                    {
                        std::cout << "Boolean returned false.\n";
                    }
                    break;
                } 
                case SDL_SCANCODE_SPACE:
                    showNodeList = !showNodeList;
                    break;
                }
                break;
            case SDL_KEYUP:
                switch (event.key.keysym.scancode) {
                case SDL_SCANCODE_LSHIFT:
                    ctrlHeld = false;
                    break;
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);
                switch (event.button.button) {
                case SDL_BUTTON_LEFT:
                {
                    //std::cout << "Left mouse pressed\n";
                    bool nodeCreated = false;
                    //Check for any selected context menu options
                    if (ContextMenuOpen)
                    {
                        for (const auto& Option : OptionStack) {
                            if (Option->Selected)
                            {
                                //A node has been selected for creation
                                nodeCreated = true;
                                switch (Option->Type)
                                {
                                case ContextOption::Option_Type::CreateNode:
                                {
                                    //downcast the option to a NodeCreator
                                    NodeCreator* NodeConstructor = (NodeCreator*)Option;

                                    //get node grid position
                                    int NewNodeX, NewNodeY;
                                    GetGridCoordinates(mouseX, mouseY, &NewNodeX, &NewNodeY);
                                    NewNodeX -= 40;
                                    NewNodeY -= 40;
                                    //create the node
                                    std::vector<Input*> NewInputs = CreateInputs(NodeConstructor->inPorts);
                                    std::vector<Output*> NewOutputs = CreateOutputs(NodeConstructor->outPorts);
                                    Node* NewNode;
                                    switch (NodeConstructor->NodeType)
                                    {
                                    case Node::Node_Type::Node_Calculation:
                                        if (!NodeConstructor->weird)
                                        {
                                            NewNode = CreateNode(NodeConstructor->NodeType, NewInputs, NewOutputs, NodeConstructor->Name, NewNodeX, NewNodeY, NodeConstructor->Function);
                                        }
                                        else
                                        {
                                            NewNode = CreateWeirdCalc(NodeConstructor->NodeType, NewInputs, NewOutputs, NodeConstructor->Name, NodeConstructor->altCalc, NewNodeX, NewNodeY);
                                        }
                                        break;
                                    case Node::Node_Type::Node_Action:
                                        NewNode = CreateActionNode(NodeConstructor->NodeType, NewInputs, NewOutputs, NodeConstructor->Name, ActionNode::ActionType::DoubleSided, NewNodeX, NewNodeY, NodeConstructor->nodeAction);
                                        break;
                                    case Node::Node_Type::Node_Input:
                                        if (!NodeConstructor->weird)
                                        {
                                            NewNode = CreateInputNode(NodeConstructor->NodeType, NewInputs, NewOutputs, NodeConstructor->Name, NewNodeX, NewNodeY, NodeConstructor->nodeInput);
                                        }
                                        else
                                        {
                                            NewNode = CreateWeirdInput(NodeConstructor->NodeType, NewInputs, NewOutputs, NodeConstructor->Name, NodeConstructor->altInput, NewNodeX, NewNodeY);
                                        }
                                        break;
                                    }
                                    break;
                                }
                                case ContextOption::Option_Type::UI:
                                    //UI functions currently unimplemented
                                    break;
                                }
                            }
                        }
                        ContextMenuOpen = false;
                    }
                    //break here - we don't want any hit tests if a node was created
                    if (nodeCreated) break;
                    //hit test

                    //port test
                    for (int Nodecount = 0; Nodecount < NodeStack.size(); Nodecount++)
                    {
                        Node* checkNode = NodeStack[Nodecount];
                        //inputs
                        for (int Portcount = 0; Portcount < checkNode->inputs.size(); Portcount++)
                        {
                            DataPort* Curport = &checkNode->inputs[Portcount]->port;

                            if (mouseX > Curport->renderX - PortHitboxSize && mouseX < Curport->renderX + PortHitboxSize && mouseY > Curport->renderY - PortHitboxSize && mouseY < Curport->renderY + PortHitboxSize)
                            {
                                //make sure it's already linked
                                if (checkNode->inputs[Portcount]->link)
                                {
                                    currentDragPort = Curport;
                                    currentDragPortParent = checkNode->inputs[Portcount];
                                    draggingPort = true;
                                }
                            }
                            if (draggingPort) break;
                        }
                        if (draggingPort) break;
                        //outputs
                        for (int Portcount = 0; Portcount < checkNode->outputs.size(); Portcount++)
                        {
                            DataPort* Curport = &checkNode->outputs[Portcount]->port;

                            if (mouseX > Curport->renderX - PortHitboxSize && mouseX < Curport->renderX + PortHitboxSize && mouseY > Curport->renderY - PortHitboxSize && mouseY < Curport->renderY + PortHitboxSize)
                            {
                                currentDragNewPort = Curport;
                                currentDragNewPortParent = checkNode->outputs[Portcount];
                                draggingNewPort = true;
                            }
                            if (draggingNewPort) break;
                        }
                        if (draggingNewPort) break;
                    }
                    if (draggingPort) break;
                    if (draggingNewPort) break;
                    //Flow Test
                    for (int Nodecount = 0; Nodecount < NodeStack.size(); Nodecount++)
                    {
                        Node* checkNode = NodeStack[Nodecount];
                        if (checkNode->type == Node::Node_Type::Node_Action || checkNode->type == Node::Node_Type::Node_Event)
                        {
                            ActionNode* checkNodeCast = (ActionNode*)checkNode;

                            //inputs / left
                            if (checkNodeCast->FlowType == ActionNode::ActionType::DoubleSided || checkNodeCast->FlowType == ActionNode::ActionType::LeftInput)
                            {
                                
                                DataFlow* Curport = &checkNodeCast->LeftPort;

                                if (mouseX > Curport->renderX - PortHitboxSize && mouseX < Curport->renderX + PortHitboxSize && mouseY > Curport->renderY - PortHitboxSize && mouseY < Curport->renderY + PortHitboxSize)
                                {
                                    //make sure it's already linked
                                    if (checkNodeCast->Previous)
                                    {
                                        currentDragFlow = Curport;
                                        //currentDragPortParent = checkNode->inputs[Portcount];
                                        draggingFlow = true;
                                    }
                                }
                            }
                            if (draggingFlow) break;
                            //outputs / right
                            if (checkNodeCast->FlowType == ActionNode::ActionType::DoubleSided || checkNodeCast->FlowType == ActionNode::ActionType::RightOutput)
                            {
                                DataFlow* Curport = &checkNodeCast->RightPort;

                                if (mouseX > Curport->renderX - PortHitboxSize && mouseX < Curport->renderX + PortHitboxSize && mouseY > Curport->renderY - PortHitboxSize && mouseY < Curport->renderY + PortHitboxSize)
                                {
                                    currentDragNewFlow = Curport;
                                    //currentDragNewPortParent = checkNode->outputs[Portcount];
                                    //erase pre existing flow
                                    
                                    draggingNewFlow = true;
                                }
                            }
                            if (draggingNewPort) break;
                        }
                    }
                    if (draggingPort) break;
                    if (draggingNewPort) break;
                    if (draggingFlow) break;
                    if (draggingNewFlow) break;
                    //node test
                    bool HitNode = false;
                    for (int Nodecount = 0; Nodecount < NodeStack.size(); Nodecount++)
                    {
                        Node* checkNode = NodeStack[Nodecount];
                        NodeDrawable* renderable = checkNode->renderable;

                        if (mouseX > renderable->renderX&& mouseX < renderable->renderX + renderable->width * globalScaleFactor && mouseY > renderable->renderY&& mouseY < renderable->renderY + renderable->currentHeight)
                        {
                            //std::cout << "clicked a node!";
                            HitNode = true;
                            renderable->mouseStartX = mouseX;
                            renderable->mouseStartY = mouseY;
                            renderable->StartX = renderable->x;
                            renderable->StartY = renderable->y;

                            bool removed = false;
                            if (ctrlHeld && std::count(currentDrag.begin(), currentDrag.end(), checkNode))
                            {
                                removed = true;
                                currentDrag.erase(std::remove(currentDrag.begin(), currentDrag.end(), checkNode), currentDrag.end());
                                checkNode->Selected = false;
                            }

                            if (!ctrlHeld)
                            {
                                for (const auto& unselectNode : NodeStack) {
                                    unselectNode->Selected = false;
                                }
                                currentDrag.clear();
                            }
                            else
                            {
                                for (const auto& selectNode : currentDrag) {
                                    selectNode->renderable->mouseStartX = mouseX;
                                    selectNode->renderable->mouseStartY = mouseY;
                                    selectNode->renderable->StartX = selectNode->renderable->x;
                                    selectNode->renderable->StartY = selectNode->renderable->y;
                                }
                            }
                            if (!removed)
                            {
                                currentDrag.push_back(checkNode);
                                checkNode->Selected = true;
                            }

                            draggingNode = true;

                            
                            
                            
                        }
                    }
                    if (!HitNode)
                    {
                        currentDrag.clear();
                        for (const auto& unselectNode : NodeStack) {
                            unselectNode->Selected = false;
                        }
                    }

                    break;
                } 
                case SDL_BUTTON_RIGHT:
                    //std::cout << "Right mouse pressed\n";
                    
                    ContextMenuOpen = !ContextMenuOpen;
                    if (ContextMenuOpen)
                    {
                        ContextX = mouseX;
                        ContextY = mouseY;
                    }
                    break;
                case SDL_BUTTON_MIDDLE:
                    //std::cout << "Middle mouse pressed\n";
                    SDL_GetMouseState(&startX, &startY);
                    MiddleMouse = true;
                    MMoffsetX = 0;
                    MMoffsetY = 0;
                    break;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                switch (event.button.button) {
                case SDL_BUTTON_LEFT:
                {
                    //std::cout << "Left mouse released\n";
                    int mouseX, mouseY;
                    SDL_GetMouseState(&mouseX, &mouseY);
                    //port test
                    bool HitPort = false;
                    bool HitFlow = false;
                    for (int Nodecount = 0; Nodecount < NodeStack.size(); Nodecount++)
                    {
                        Node* checkNode = NodeStack[Nodecount];

                        //inputs
                        if (draggingPort)
                        {
                            for (int Portcount = 0; Portcount < checkNode->inputs.size(); Portcount++)
                            {
                                DataPort* Curport = &checkNode->inputs[Portcount]->port;

                                if (mouseX > Curport->renderX - PortHitboxSize && mouseX < Curport->renderX + PortHitboxSize && mouseY > Curport->renderY - PortHitboxSize && mouseY < Curport->renderY + PortHitboxSize)
                                {
                                    HitPort = true;
                                    if (currentDragPort != Curport && currentDragPortParent->type == Linkable::Link_Type::Input && currentDragPort->SubType == checkNode->inputs[Portcount]->port.SubType)
                                    { 
                                        //need to get the linked IO, not the IO itself - this will be out new link source
                                        Input* movedIO = (Input*)currentDragPortParent;
                                        Output* other = movedIO->link;

                                        //destroy both links entirely
                                        //and remove from link stack
                                        if (movedIO->currentLink)
                                        {
                                            LinkStack.erase(std::remove(LinkStack.begin(), LinkStack.end(), movedIO->currentLink), LinkStack.end());
                                            Unlink(movedIO->currentLink);
                                        }

                                        if (checkNode->inputs[Portcount]->currentLink)
                                        {
                                            LinkStack.erase(std::remove(LinkStack.begin(), LinkStack.end(), checkNode->inputs[Portcount]->currentLink), LinkStack.end());
                                            Unlink(checkNode->inputs[Portcount]->currentLink);
                                        }
                                        LinkStack.push_back(new Link(other, checkNode->inputs[Portcount]));
                                    }
                                }
                            }
                        }

                        //outputs
                        if (draggingNewPort)
                        {
                            for (int Portcount = 0; Portcount < checkNode->inputs.size(); Portcount++)
                            {
                                DataPort* Curport = &checkNode->inputs[Portcount]->port;

                                if (mouseX > Curport->renderX - PortHitboxSize && mouseX < Curport->renderX + PortHitboxSize && mouseY > Curport->renderY - PortHitboxSize && mouseY < Curport->renderY + PortHitboxSize)
                                {
                                    if (currentDragNewPort != Curport && currentDragNewPortParent->ParentNode != checkNode->inputs[Portcount]->ParentNode && currentDragNewPortParent->type == Linkable::Link_Type::Output && currentDragNewPort->SubType == checkNode->inputs[Portcount]->port.SubType)
                                    {
                                        //std::cout << "Dragged to different output!";
                                        if (checkNode->inputs[Portcount]->currentLink)
                                        {
                                            LinkStack.erase(std::remove(LinkStack.begin(), LinkStack.end(), checkNode->inputs[Portcount]->currentLink), LinkStack.end());
                                            Unlink(checkNode->inputs[Portcount]->currentLink);
                                        }
                                        LinkStack.push_back(new Link((Output*)currentDragNewPortParent, checkNode->inputs[Portcount]));
                                    }

                                }
                            }
                        }

                        //Flow

                        //inputs / left
                        if (draggingFlow)
                        {
                            for (int Portcount = 0; Portcount < checkNode->inputs.size(); Portcount++)
                            {
                                if (checkNode->type == Node::Node_Type::Node_Action || checkNode->type == Node::Node_Type::Node_Event)
                                {
                                    ActionNode* checkActionNode = (ActionNode*)checkNode;
                                    DataFlow* Curport = &checkActionNode->LeftPort;

                                    if (mouseX > Curport->renderX - PortHitboxSize && mouseX < Curport->renderX + PortHitboxSize && mouseY > Curport->renderY - PortHitboxSize && mouseY < Curport->renderY + PortHitboxSize)
                                    {
                                        HitFlow = true;
                                        if (currentDragFlow != Curport)
                                        {
                                            //need to get the linked IO, not the IO itself - this will be out new link source
                                            //Input* movedIO = (Input*)currentDragFlowParent; moved io is where original link is from to
                                            //Output* other = movedIO->link;

                                            //destroy both links entirely
                                            //and remove from link stack
                                            ActionNode* orig = (ActionNode*)currentDragFlow->parent;
                                            ActionNode* from = orig->Previous;
                                            //if (from->Next)
                                            //{
                                            
                                                //UnlinkActionNodes(checkActionNode->Previous, checkActionNode);
                                            UnlinkActionNodes(from, orig);
                                                //checkActionNode->Previous = nullptr;
                                            //}

                                            /*if (checkNode->inputs[Portcount]->currentLink)
                                            {
                                                LinkStack.erase(std::remove(LinkStack.begin(), LinkStack.end(), checkNode->inputs[Portcount]->currentLink), LinkStack.end());
                                                Unlink(checkNode->inputs[Portcount]->currentLink);
                                            }*/
                                            LinkActionNodes(from, checkActionNode);
                                            //LinkActionNodes(checkActionNode, from);
                                            //from->Next = checkActionNode;
                                            //checkActionNode->Previous = from;
                                        }
                                    }
                                }
                            }
                        }

                        //outputs / right
                        if (draggingNewFlow)
                        {
                            for (int Portcount = 0; Portcount < checkNode->inputs.size(); Portcount++)
                            {
                                if (checkNode->type == Node::Node_Type::Node_Action || checkNode->type == Node::Node_Type::Node_Event)
                                {
                                    ActionNode* checkActionNode = (ActionNode*)checkNode;
                                    DataFlow* Curport = &checkActionNode->LeftPort;

                                    if (mouseX > Curport->renderX - PortHitboxSize && mouseX < Curport->renderX + PortHitboxSize && mouseY > Curport->renderY - PortHitboxSize && mouseY < Curport->renderY + PortHitboxSize)
                                    {
                                        if (currentDragNewFlow != Curport && currentDragNewFlow->parent != Curport->parent)
                                        {
                                            //std::cout << "Dragged to different output!";
                                            ActionNode* from = (ActionNode*)currentDragNewFlow->parent;
                                            if (checkActionNode->Previous)
                                            {
                                                //LinkStack.erase(std::remove(LinkStack.begin(), LinkStack.end(), checkNode->inputs[Portcount]->currentLink), LinkStack.end());
                                                //Unlink(checkNode->inputs[Portcount]->currentLink);
                                                checkActionNode->Previous = nullptr;
                                            }
                                            from->Next = checkActionNode;
                                            checkActionNode->Previous = from;
                                            //LinkStack.push_back(new Link((Output*)currentDragNewPortParent, checkNode->inputs[Portcount]));
                                        }

                                    }
                                }
                            }
                        }
                    }

                    if (currentDragPortParent && currentDragPortParent->currentLink && !HitPort)
                    {
                        LinkStack.erase(std::remove(LinkStack.begin(), LinkStack.end(), currentDragPortParent->currentLink), LinkStack.end());
                        Unlink(currentDragPortParent->currentLink);
                    }
                    if (currentDragPort)
                    {
                        currentDragPort->offsetX = 0;
                        currentDragPort->offsetY = 0;
                    }
                    
                    if (currentDragFlow && !HitFlow)
                    {
                        ActionNode* orig = (ActionNode*)currentDragFlow->parent;
                        ActionNode* from = orig->Previous;
                        if(orig && from) UnlinkActionNodes(from, orig);
                    }
                    if (currentDragFlow)
                    {
                        currentDragFlow->offsetX = 0;
                        currentDragFlow->offsetY = 0;
                    }

                    draggingNode = false;
                    //currentDrag.clear();
                    draggingPort = false;
                    draggingFlow = false;
                    draggingNewFlow = false;
                    currentDragFlow = nullptr;
                    currentDragNewFlow = nullptr;
                    currentDragPort = nullptr;
                    currentDragPortParent = nullptr;
                    draggingNewPort = false;
                    currentDragNewPort = nullptr;
                    currentDragNewPortParent = nullptr;
                    break;
                }
                case SDL_BUTTON_RIGHT:
                    //std::cout << "Right mouse released\n";
                    break;
                case SDL_BUTTON_MIDDLE:
                    //std::cout << "Middle mouse released\n";
                    MiddleMouse = false;
                    gridoffsetX += (MMoffsetX);
                    gridoffsetY += (MMoffsetY);
                    MMoffsetX = 0;
                    MMoffsetY = 0;
                    break;
                }
                break;
            case SDL_MOUSEWHEEL:
                if (event.wheel.y > 0) // scroll up
                {
                    if (zoomLevel < zoomMax) zoomLevel++;
                    if (globalScaleFactor < zoomMax) globalScaleFactor++;
                }
                else if (event.wheel.y < 0) // scroll down
                {
                    if (zoomLevel > zoomMin) zoomLevel--;
                    if (globalScaleFactor > zoomMin) globalScaleFactor--;
                }
                break;
            }
        }


        if (MiddleMouse)
        {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            MMoffsetX = mouseX - startX;
            MMoffsetY = mouseY - startY;
            
        }
        if (draggingNode)
        {
            for (const auto& SelectedNode : currentDrag) {
                NodeDrawable* renderable = SelectedNode->renderable;

                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);
                int deltaMouseX = mouseX - renderable->mouseStartX;
                int deltaMouseY = mouseY - renderable->mouseStartY;

                renderable->x = renderable->StartX + deltaMouseX / globalScaleFactor;
                renderable->y = renderable->StartY + deltaMouseY / globalScaleFactor;
            }
        }
        //context menu hit test
        if(ContextMenuOpen)
        {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);

            
            bool skipCategoryCheck = false;
            //check option stack first, then only do a category search if one isnt selected
            for (const auto& Option : OptionStack) {
                ContextRenderable* renderable = Option->Renderable;
                if (Option->Parent->Show && mouseX > renderable->x && mouseX < renderable->x + renderable->w && mouseY > renderable->y && mouseY < renderable->y + renderable->h)
                {
                    for (const auto& otherOption : OptionStack) {
                        otherOption->Selected = false;
                    }
                    Option->Selected = true;
                    skipCategoryCheck = true;
                    //for (const auto& Category : CategoryStack) {
                    //    Category->Show = false;
                    //}
                    //Option->Parent->Show = true;
                }
                else
                {
                    Option->Selected = false;
                }
            }
            if (!skipCategoryCheck)
            {
                int CategoryRightTolerance = 4;
                for (const auto& Category : CategoryStack) {
                    ContextRenderable* renderable = Category->Renderable;
                    if (mouseX > renderable->x && mouseX < renderable->x + renderable->w + CategoryRightTolerance && mouseY > renderable->y&& mouseY < renderable->y + renderable->h)
                    {
                        //for (const auto& otherCategory : CategoryStack) {
                        //    otherCategory->Show = false;
                        //}
                        Category->Show = true;
                    }
                    else
                    {
                        Category->Show = false;
                        //for (const auto& otherOption : Category->Options) {
                        //    otherOption->Selected = false;
                        //}
                    }
                }
            }
        }

        dest.w = 48 + 32 * zoomLevel;
        dest.h = 48 + 32 * zoomLevel;
        // clears the screen 
        SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
        SDL_RenderClear(rend);
        //Begin Draw

        //Draw Grid
        for (int myX = 0; myX <= ceil(scrw/dest.w) + 2; myX++)
        {
            for (int myY = 0; myY <= ceil(scrw / dest.h) + 2; myY++)
            {
                // sets initial x-position of object 
                dest.x = (myX - 1) * dest.w + ((gridoffsetX + MMoffsetX) / gridParallaxAmount % dest.w);

                // sets initial y-position of object 
                dest.y = (myY - 1) * dest.h + ((gridoffsetY + MMoffsetY) / gridParallaxAmount % dest.h);

                SDL_RenderCopy(rend, tex, NULL, &dest);
            }
        }

        if (draggingPort)
        {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);

            currentDragPort->offsetX = mouseX - currentDragPort->renderX;
            currentDragPort->offsetY = mouseY - currentDragPort->renderY;
        }


        if (draggingNewPort)
        {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            SDL_SetRenderDrawColor(rend, 255, 255, 255, SDL_ALPHA_OPAQUE);
            SDL_RenderDrawLine(rend, currentDragNewPortParent->port.renderX, currentDragNewPortParent->port.renderY, mouseX, mouseY);
        }

        if (draggingFlow)
        {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);

            currentDragFlow->offsetX = mouseX - currentDragFlow->renderX;
            currentDragFlow->offsetY = mouseY - currentDragFlow->renderY;
        }


        if (draggingNewFlow)
        {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            SDL_SetRenderDrawColor(rend, 255, 255, 255, SDL_ALPHA_OPAQUE);
            SDL_RenderDrawLine(rend, currentDragNewFlow->renderX, currentDragNewFlow->renderY, mouseX, mouseY);
        }

        //Draw Links
        for (int curLink = 0; curLink < LinkStack.size(); curLink++)
        {
            Link* current = LinkStack[curLink];

            SDL_SetRenderDrawColor(rend, 255, 255, 255, SDL_ALPHA_OPAQUE);
            SDL_RenderDrawLine(rend, current->LinkInput->port.renderX + current->LinkInput->port.offsetX, current->LinkInput->port.renderY + current->LinkInput->port.offsetY, current->LinkOutput->port.renderX + current->LinkOutput->port.offsetX, current->LinkOutput->port.renderY + current->LinkOutput->port.offsetY);
        }

        //Draw Flow
        for (auto& curAction : NodeStack) {
            if (curAction->type == Node::Node_Type::Node_Action || curAction->type == Node::Node_Type::Node_Event)
            {
                ActionNode* curActionCast = (ActionNode*)curAction;
                if (curActionCast->Next)
                {
                    SDL_SetRenderDrawColor(rend, 255, 255, 255, SDL_ALPHA_OPAQUE);
                    SDL_RenderDrawLine(rend, curActionCast->RightPort.renderX + curActionCast->RightPort.offsetX, curActionCast->RightPort.renderY + curActionCast->RightPort.offsetY, curActionCast->Next->LeftPort.renderX + curActionCast->Next->LeftPort.offsetX, curActionCast->Next->LeftPort.renderY + curActionCast->Next->LeftPort.offsetY);
                }
            }
        }

        //Draw Nodes
        for (int NodeIter = 0; NodeIter < NodeStack.size(); NodeIter++)
        {
            
            Node* curNode = NodeStack[NodeIter];
            NodeDrawable* renderable = curNode->renderable;
            int drawX, drawY;
            GetScreenCoordinates(renderable->x, renderable->y, &drawX, &drawY);

            int inputcount = curNode->inputs.size();
            int outputcount = curNode->outputs.size();

            //With margin
            int PortSize = 24 * globalScaleFactor;
            int midheight = std::fmax(inputcount, outputcount) * PortSize;

            
            NodeElement.w = renderable->width * globalScaleFactor;
            NodeElement.h = renderable->topMargin * globalScaleFactor + midheight + renderable->bottomMargin * globalScaleFactor;
            renderable->currentHeight = renderable->topMargin * globalScaleFactor + midheight + renderable->bottomMargin * globalScaleFactor;
            NodeElement.x = drawX;
            NodeElement.y = drawY;
            renderable->renderX = drawX;
            renderable->renderY = drawY;

            if (curNode->Selected)
            {
                int BorderThickness = 1;
                SDL_Rect SelectionBG;
                SelectionBG.x = NodeElement.x - BorderThickness;
                SelectionBG.y = NodeElement.y - BorderThickness;
                SelectionBG.w = NodeElement.w + BorderThickness * 2;
                SelectionBG.h = NodeElement.h + BorderThickness * 2;
                SDL_SetRenderDrawColor(rend, 255, 147, 0, 255);
                SDL_RenderFillRect(rend, &SelectionBG);
            }

            SDL_SetRenderDrawColor(rend, 40, 40, 50, 255);
            SDL_RenderFillRect(rend, &NodeElement);

            //Render text
            

            TTF_Font* Sans = TTF_OpenFont("C:/Users/riley/source/repos/SDLTests/x64/Debug/arial.ttf", 12 * globalScaleFactor); //this opens a font style and sets a size


            SDL_Color White = { 255, 255, 255 };  // this is the color in rgb format, maxing out all would give you the color white, and it will be your text's color

            const char* text = curNode->title;
            SDL_Surface* nodeMessage = TTF_RenderText_Solid(Sans, text, White); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first

            SDL_Texture* Message = SDL_CreateTextureFromSurface(rend, nodeMessage); //now you can convert it into a texture

            SDL_Rect Message_rect; //create a rect
            Message_rect.x = drawX + 2 * globalScaleFactor;  //controls the rect's x coordinate 
            Message_rect.y = drawY + 2 * globalScaleFactor; // controls the rect's y coordinte

            TTF_SizeText(Sans, text, &Message_rect.w, &Message_rect.h);


            SDL_RenderCopy(rend, Message, NULL, &Message_rect);

            
            SDL_DestroyTexture(Message);
            SDL_FreeSurface(nodeMessage);
            TTF_CloseFont(Sans);

            //Display output
            int OutputTextSize = 8 * globalScaleFactor;
            if (DrawNodeOutput && curNode->type == Node::Node_Type::Node_Output)
            {
                
                TTF_Font* OutputFont = TTF_OpenFont("C:/Users/riley/source/repos/SDLTests/x64/Debug/consola.ttf", OutputTextSize);
                std::string Outputtext = "undefined";
                curNode->CalculateInputs();
                if (!curNode->CalculatedInputs.empty())
                {
                    Data* CalculatedInput = curNode->CalculatedInputs[0];
                    switch (CalculatedInput->subtype)
                    {
                        case Data::Data_Type::Boolean:
                        {
                            NodeBoolean* castData = (NodeBoolean*)CalculatedInput;
                            bool value = castData->value;
                            Outputtext = std::to_string(value);
                            break;
                        }
                        case Data::Data_Type::Numeric:
                        case Data::Data_Type::Float:
                        {
                            NodeFloat* castData = (NodeFloat*)CalculatedInput;
                            double value = castData->value;
                            Outputtext = std::to_string(value);
                            break;
                        }
                        case Data::Data_Type::Integer:
                        {
                            NodeInteger* castData = (NodeInteger*)CalculatedInput;
                            int value = castData->value;
                            Outputtext = std::to_string(value);
                            break;
                        }
                        case Data::Data_Type::String:
                        {
                            NodeString* castData = (NodeString*)CalculatedInput;
                            const char* value = castData->value;
                            Outputtext = value;
                            break;
                        }
                    }
                }
                const char* OutputChar = Outputtext.c_str();
                SDL_Surface* OutputnodeMessage = TTF_RenderText_Solid(OutputFont, OutputChar, White);

                SDL_Texture* OutputMessage = SDL_CreateTextureFromSurface(rend, OutputnodeMessage);

                SDL_Rect OutputMessage_rect;
                OutputMessage_rect.x = drawX + 2 * globalScaleFactor;
                OutputMessage_rect.y = drawY + (NodeElement.h - OutputTextSize) - 2 * globalScaleFactor;

                TTF_SizeText(OutputFont, OutputChar, &OutputMessage_rect.w, &OutputMessage_rect.h);

                SDL_RenderCopy(rend, OutputMessage, NULL, &OutputMessage_rect);

                SDL_DestroyTexture(OutputMessage);
                SDL_FreeSurface(OutputnodeMessage);
                TTF_CloseFont(OutputFont);
            }
            
            //draw ports
            
            

            //actual circle
            int IOSize = 8 * globalScaleFactor;

            //Draw Event ports
            
            if (curNode->type == Node::Node_Type::Node_Event || curNode->type == Node::Node_Type::Node_Action)
            {
                ActionNode* eventNode = (ActionNode*)curNode;
                switch (eventNode->FlowType)
                {
                    case ActionNode::ActionType::DoubleSided:
                    {
                        
                        int topY = (renderable->topMargin - 12) * globalScaleFactor + drawY;

                        SDL_Rect FlowPort;
                        FlowPort.x = drawX - 4 * globalScaleFactor;
                        FlowPort.y = topY;
                        FlowPort.w = IOSize;
                        FlowPort.h = IOSize;

                        if (&eventNode->LeftPort != currentDragPort)
                        {
                            eventNode->LeftPort.renderX = FlowPort.x + IOSize / 2;
                            eventNode->LeftPort.renderY = FlowPort.y + IOSize / 2;
                        }

                        SDL_RenderCopy(rend, Flowtex, NULL, &FlowPort);

                        FlowPort.w = IOSize;
                        FlowPort.h = IOSize;
                        FlowPort.x = renderable->width * globalScaleFactor + drawX - 4 * globalScaleFactor;
                        FlowPort.y = topY;

                        eventNode->RightPort.renderX = FlowPort.x + IOSize / 2;
                        eventNode->RightPort.renderY = FlowPort.y + IOSize / 2;

                        SDL_RenderCopy(rend, Flowtex, NULL, &FlowPort);
                        break;
                    }
                    case ActionNode::ActionType::RightOutput:
                    {
                        int topY = (renderable->topMargin - 12) * globalScaleFactor + drawY;
                        SDL_Rect FlowPort;
                        FlowPort.w = IOSize;
                        FlowPort.h = IOSize;
                        FlowPort.x = renderable->width * globalScaleFactor + drawX - 4 * globalScaleFactor;
                        FlowPort.y = topY;

                        eventNode->RightPort.renderX = FlowPort.x + IOSize / 2;
                        eventNode->RightPort.renderY = FlowPort.y + IOSize / 2;

                        SDL_RenderCopy(rend, Flowtex, NULL, &FlowPort);
                        break;
                    }
                }
            }

            for (int curInput = 0; curInput < inputcount; curInput++)
            {
                
                int topY = curInput * PortSize + renderable->topMargin * globalScaleFactor + drawY;

                

                //color the port
                switch (curNode->inputs[curInput]->port.PortType)
                {
                case Data::Data_Type::Boolean:
                    SDL_SetTextureColorMod(Porttex, 255, 0, 0);
                    break;
                case Data::Data_Type::Numeric:
                case Data::Data_Type::Float:
                    SDL_SetTextureColorMod(Porttex, 255, 255, 0);
                    break;
                case Data::Data_Type::Integer:
                    SDL_SetTextureColorMod(Porttex, 0, 255, 0);
                    break;
                case Data::Data_Type::String:
                    SDL_SetTextureColorMod(Porttex, 255, 0, 255);
                    break;
                case Data::Data_Type::Weird:
                    SDL_SetTextureColorMod(Porttex, 255, 147, 0);
                    break;
                }

                
                Portdest.w = IOSize;
                Portdest.h = IOSize;
                Portdest.x = drawX - 4 * globalScaleFactor;
                Portdest.y = topY;

                if (&curNode->inputs[curInput]->port != currentDragPort)
                {
                    curNode->inputs[curInput]->port.renderX = Portdest.x + IOSize / 2;
                    curNode->inputs[curInput]->port.renderY = Portdest.y + IOSize / 2;
                }
                

                //SDL_QueryTexture(Nodetex, NULL, NULL, &Portdest.w, &Portdest.h);

                SDL_RenderCopy(rend, Porttex, NULL, &Portdest);
                
            }

            for (int curOutput = 0; curOutput < outputcount; curOutput++)
            {
                int topY = curOutput * PortSize + renderable->topMargin * globalScaleFactor + drawY;

                

                //color the port
                switch (curNode->outputs[curOutput]->port.PortType)
                {
                case Data::Data_Type::Boolean:
                    SDL_SetTextureColorMod(Porttex, 255, 0, 0);
                    break;
                case Data::Data_Type::Numeric:
                case Data::Data_Type::Float:
                    SDL_SetTextureColorMod(Porttex, 255, 255, 0);
                    break;
                case Data::Data_Type::Integer:
                    SDL_SetTextureColorMod(Porttex, 0, 255, 0);
                    break;
                case Data::Data_Type::String:
                    SDL_SetTextureColorMod(Porttex, 255, 0, 255);
                    break;
                case Data::Data_Type::Weird:
                    SDL_SetTextureColorMod(Porttex, 255, 147, 0);
                    break;
                }

                
                Portdest.w = IOSize;
                Portdest.h = IOSize;
                Portdest.x = renderable->width * globalScaleFactor + drawX - 4 * globalScaleFactor;
                Portdest.y = topY;

                curNode->outputs[curOutput]->port.renderX = Portdest.x + IOSize / 2;
                curNode->outputs[curOutput]->port.renderY = Portdest.y + IOSize / 2;
                //SDL_QueryTexture(Nodetex, NULL, NULL, &Portdest.w, &Portdest.h);

                SDL_RenderCopy(rend, Porttex, NULL, &Portdest);
                
            }

            

        }

        //Draw Node List BG
        if (showNodeList)
        {
            SDL_Rect ListElement;
            ListElement.w = scrw;
            ListElement.h = scrh / 5;
            ListElement.x = 0;
            ListElement.y = scrh - ListElement.h;

            SDL_SetRenderDrawColor(rend, 40, 40, 50, 255);
            SDL_RenderFillRect(rend, &ListElement);
        }

        //Draw Node List FG
        //TBA

        //Draw Context Menu
        if (ContextMenuOpen)
        {
            //std::cout << "drawing context menu";
            
            int OptionSize = 28;
            int OptionMargin = 2;
            int OptionTextSize = 18;
            int CategoryCount = RightClickContext->Categories.size();

            int Overallheight = OptionSize * CategoryCount + OptionMargin * (CategoryCount + 1);
            //Draw main CM background
            SDL_Rect Background;
            Background.w = 140;
            Background.h = Overallheight;
            Background.x = ContextX;
            Background.y = ContextY;

            SDL_SetRenderDrawColor(rend, 70, 70, 70, 255);
            SDL_RenderFillRect(rend, &Background);

            //switch to iterators at some point
            for (int CategoryIndex = 0; CategoryIndex < CategoryCount; CategoryIndex++)
            {
                ContextCategory* CurCategory = RightClickContext->Categories[CategoryIndex];
                TTF_Font* Sans = TTF_OpenFont("C:/Users/riley/source/repos/SDLTests/x64/Debug/arial.ttf", OptionTextSize); //this opens a font style and sets a size
                SDL_Color White = { 255, 255, 255 };  // this is the color in rgb format, maxing out all would give you the color white, and it will be your text's color
                int TextMargin = 2;
                
                CurCategory->Renderable->w = Background.w;
                CurCategory->Renderable->h = OptionSize + OptionMargin * 2; // add option margin to help selection bounds
                CurCategory->Renderable->x = Background.x;
                CurCategory->Renderable->y = Background.y + (OptionSize + OptionMargin) * CategoryIndex;

                //draw category selected if shown
                if (CurCategory->Show)
                {
                    int CategoryOptionCount = CurCategory->Options.size();

                    SDL_Rect CategoryBackground;
                    CategoryBackground.w = CurCategory->Renderable->w;
                    CategoryBackground.h = CurCategory->Renderable->h - OptionMargin;
                    CategoryBackground.x = CurCategory->Renderable->x;
                    CategoryBackground.y = CurCategory->Renderable->y;

                    SDL_SetRenderDrawColor(rend, 40, 40, 40, 255);
                    SDL_RenderFillRect(rend, &CategoryBackground);

                    //Draw Submenu BG

                    int SubmenuHeight = OptionSize * CategoryOptionCount + OptionMargin * (CategoryOptionCount + 1);
                    SDL_Rect SubmenuBackground;
                    SubmenuBackground.w = Background.w;
                    SubmenuBackground.h = SubmenuHeight;
                    SubmenuBackground.x = Background.x + Background.w;
                    SubmenuBackground.y = Background.y + (OptionSize + OptionMargin) * CategoryIndex;

                    SDL_SetRenderDrawColor(rend, 70, 70, 70, 255);
                    SDL_RenderFillRect(rend, &SubmenuBackground);

                    //Draw text and selected bg again
                    for (int SelectionIndex = 0; SelectionIndex < CategoryOptionCount; SelectionIndex++)
                    {
                        ContextOption* CurOption = CurCategory->Options[SelectionIndex];

                        //Setup renderable
                        CurOption->Renderable->w = SubmenuBackground.w;
                        CurOption->Renderable->h = OptionSize + OptionMargin * 2;
                        CurOption->Renderable->x = SubmenuBackground.x;
                        CurOption->Renderable->y = SubmenuBackground.y + (OptionSize + OptionMargin) * SelectionIndex;

                        if (CurOption->Selected)
                        {
                            //draw selection bg
                            SDL_Rect OptionBackground;
                            OptionBackground.w = CurOption->Renderable->w;
                            OptionBackground.h = CurOption->Renderable->h - OptionMargin;
                            OptionBackground.x = CurOption->Renderable->x;
                            OptionBackground.y = CurOption->Renderable->y;

                            SDL_SetRenderDrawColor(rend, 40, 40, 40, 255);
                            SDL_RenderFillRect(rend, &OptionBackground);
                        }

                        const char* Selectiontext = CurOption->Name;
                        SDL_Surface* SelectionsurfMessage = TTF_RenderText_Solid(Sans, Selectiontext, White);
                        SDL_Texture* SelectionMessage = SDL_CreateTextureFromSurface(rend, SelectionsurfMessage);

                        SDL_Rect SelectionMessage_rect;
                        SelectionMessage_rect.x = SubmenuBackground.x + TextMargin;
                        SelectionMessage_rect.y = SubmenuBackground.y + OptionMargin + (OptionSize + OptionMargin) * SelectionIndex;

                        TTF_SizeText(Sans, Selectiontext, &SelectionMessage_rect.w, &SelectionMessage_rect.h);

                        SDL_RenderCopy(rend, SelectionMessage, NULL, &SelectionMessage_rect);

                        SDL_DestroyTexture(SelectionMessage);
                        SDL_FreeSurface(SelectionsurfMessage);

                    }
                }
                //draw category text
                

                const char* text = CurCategory->CategoryName;
                SDL_Surface* nodeMessage = TTF_RenderText_Solid(Sans, text, White);
                SDL_Texture* Message = SDL_CreateTextureFromSurface(rend, nodeMessage);

                SDL_Rect Message_rect;
                Message_rect.x = Background.x + TextMargin;
                Message_rect.y = Background.y + OptionMargin + (OptionSize + OptionMargin) * CategoryIndex;

                TTF_SizeText(Sans, text, &Message_rect.w, &Message_rect.h);

                SDL_RenderCopy(rend, Message, NULL, &Message_rect);

                SDL_DestroyTexture(Message);
                SDL_FreeSurface(nodeMessage);
                TTF_CloseFont(Sans);

            }
        }

        //Draw Console
        TTF_Font* ConsoleFont = TTF_OpenFont("C:/Users/riley/source/repos/SDLTests/x64/Debug/consola.ttf", 16);
        SDL_Color White = { 255, 255, 255 };
        int ConsoleLeftMargin = 4;
        int LineSize = 30;
        size_t MaxLines = 18;
        std::deque<ConsoleMessage> tempconsole = NodeDebug::console;
        for (int lineIndex = 0; lineIndex < std::min(tempconsole.size(), MaxLines); lineIndex++)
        {
            //int ConsoleTop = scrh - NodeDebug::console.size() * LineSize;
            //std::string lineText = "";
            if (tempconsole[lineIndex].message != nullptr)
            {
                std::string text = tempconsole[lineIndex].message;
                const char* convert = text.c_str();
                SDL_Surface* ConsolesurfMessage = TTF_RenderText_Solid(ConsoleFont, convert, White);
                SDL_Texture* ConsoleMessage = SDL_CreateTextureFromSurface(rend, ConsolesurfMessage);

                SDL_Rect ConsoleMessage_rect;
                ConsoleMessage_rect.x = ConsoleLeftMargin;
                ConsoleMessage_rect.y = scrh - LineSize - lineIndex * LineSize;

                TTF_SizeText(ConsoleFont, convert, &ConsoleMessage_rect.w, &ConsoleMessage_rect.h);

                SDL_RenderCopy(rend, ConsoleMessage, NULL, &ConsoleMessage_rect);

                SDL_DestroyTexture(ConsoleMessage);
                SDL_FreeSurface(ConsolesurfMessage);
                
                
            }
            //const char* text = lineText.c_str();
            
        }

        TTF_CloseFont(ConsoleFont);
        //End Draw
        // triggers the double buffers 
        // for multiple rendering 
        SDL_RenderPresent(rend);

        // calculates to 60 fps 
        SDL_Delay(1000 / 60);
    }
    
    // destroy texture 
    SDL_DestroyTexture(tex);

    SDL_DestroyTexture(Nodetex);
    SDL_DestroyTexture(Porttex);
    SDL_DestroyTexture(Flowtex);
    // destroy renderer 
    SDL_DestroyRenderer(rend);

    // destroy window 
    SDL_DestroyWindow(win);
    return 0;
}

