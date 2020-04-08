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
extern "C" { FILE __iob_func[3] = { *stdin,*stdout,*stderr }; }

//each programmable object will contain a nodestack and id count that persist
int IDcount = 0;
std::vector<Node*> NodeStack;
std::vector<Link*> LinkStack;
int gridoffsetX = 0;
int gridoffsetY = 0;
int MMoffsetX = 0;
int MMoffsetY = 0;
int gridParallaxAmount = 4;

DataNode* CreateNode(Node::Node_Type type, std::vector<Input*> inputs, std::vector<Output*> outputs, const char* title, int x = 0, int y = 0, Data* (*f)(std::vector<Data*>) = nullptr)
{
    DataNode* NewNode = new DataNode(IDcount, type, inputs, outputs, title, x, y, f);
    IDcount++;
    NodeStack.push_back(NewNode);
    return NewNode;
}

static void DestroyNode(int id)
{
    NodeStack.erase(NodeStack.begin() + id);
}

ActionNode* CreateActionNode(Node::Node_Type type, std::vector<Input*> inputs, std::vector<Output*> outputs, const char* title, int x = 0, int y = 0, bool (*f)(std::vector<Data*>) = nullptr)
{
    ActionNode* NewNode = new ActionNode(IDcount, type, inputs, outputs, title, x, y, f);
    IDcount++;
    NodeStack.push_back(NewNode);
    return NewNode;
}


void GetScreenCoordinates(int x, int y, int* sendX, int* sendY)
{
    //int returnarr[2];
    *sendX = (gridoffsetX + MMoffsetX) + x;
    *sendY = (gridoffsetY + MMoffsetY) + y;
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
    

    

    // controls annimation loop 
    int close = 0;

    // speed of box 
    int speed = 300;

    bool MiddleMouse = false;
    int startX, startY;
    

    int zoomLevel = 1;
    int zoomMax = 5;
    int zoomMin = 0;

    

    bool showNodeList = false;
    bool draggingNode = false;
    Node* currentDrag = nullptr;

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

    Data* outputresult = ExampleNode3->CalculateInputs();
    NodeFloat* calcvalue = (NodeFloat*)outputresult;

    std::cout << "Output result is: ";
    if (outputresult->type == Data::Data_Type::Float)
    {
        std::cout << "success - value is: ";
        std::cout << calcvalue->value;
    }
    else {
        std::cout << "fail";
    }
    std::cout << "\n";


    //Example Action Graph
    std::vector<DataPort> EventinPorts{  };
    std::vector<DataPort> EventoutPorts{ DataPort(Data::Data_Type::String) };
    std::vector<Input*> EventnodeInputs;
    std::vector<Output*> EventnodeOutputs = CreateOutputs(EventoutPorts);

    ActionNode* EventExampleNode = CreateActionNode(Node::Node_Type::Node_Event, EventnodeInputs, EventnodeOutputs, "Event Tick", 0, 200);


    std::vector<DataPort> PrintinPorts{ DataPort(Data::Data_Type::String) };
    std::vector<DataPort> PrintoutPorts{  };
    std::vector<Input*> PrintnodeInputs = CreateInputs(PrintinPorts);
    std::vector<Output*> PrintnodeOutputs;

    ActionNode* PrintExampleNode = CreateActionNode(Node::Node_Type::Node_Action, PrintnodeInputs, PrintnodeOutputs, "Print", 150, 200, NodeDebug::Print);

    //Link the flow between event and print
    EventExampleNode->Next = PrintExampleNode;

    //Link the IO string
    //PrintExampleNode->inputs[0]->link = EventExampleNode->outputs[0];
    LinkStack.push_back(new Link(PrintExampleNode, 0, EventExampleNode, 0));

    //Once again, event input data is managed through the InputData variable, since an event can also act as an input - completely optional
    EventExampleNode->InputData = new NodeString("Testing from the Event node!");

    EventExampleNode->Run();



    //Node texture
    SDL_Surface* Nodesurface;

    Nodesurface = IMG_Load("C:/Users/riley/Pictures/H2x1_SNES_SuperMarioWorld_image1600w.jpg");
    SDL_Texture* Nodetex = SDL_CreateTextureFromSurface(rend, Nodesurface);
    SDL_FreeSurface(Nodesurface);

    SDL_Rect Nodedest;
    SDL_QueryTexture(tex, NULL, NULL, &Nodedest.w, &Nodedest.h);

    

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
                case SDL_SCANCODE_SPACE:
                    showNodeList = !showNodeList;
                }
            case SDL_MOUSEBUTTONDOWN:
                switch (event.button.button) {
                case SDL_BUTTON_LEFT:
                    //std::cout << "Left mouse pressed\n";

                    //drag hit test
                    int mouseX, mouseY;
                    SDL_GetMouseState(&mouseX, &mouseY);

                    for (int Nodecount = 0; Nodecount < NodeStack.size(); Nodecount++)
                    {
                        Node* checkNode = NodeStack[Nodecount];
                        NodeDrawable* renderable = checkNode->renderable;
                        
                        if (mouseX > renderable->renderX && mouseX < renderable->renderX + renderable->width)
                        {
                            if (mouseY > renderable->renderY && mouseY < renderable->renderY + renderable->currentHeight)
                            {
                                //std::cout << "clicked a node!";
                                renderable->mouseStartX = mouseX;
                                renderable->mouseStartY = mouseY;
                                renderable->StartX = renderable->x;
                                renderable->StartY = renderable->y;

                                currentDrag = checkNode;
                                draggingNode = true;
                                
                            }
                        }
                    }

                    break;
                case SDL_BUTTON_RIGHT:
                    //std::cout << "Right mouse pressed\n";
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
                    //std::cout << "Left mouse released\n";
                    draggingNode = false;
                    currentDrag = nullptr;
                    break;
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
                }
                else if (event.wheel.y < 0) // scroll down
                {
                    if (zoomLevel > zoomMin) zoomLevel--;
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
            NodeDrawable* renderable = currentDrag->renderable;

            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            int deltaMouseX = mouseX - renderable->mouseStartX;
            int deltaMouseY = mouseY - renderable->mouseStartY;

            renderable->x = renderable->StartX + deltaMouseX;
            renderable->y = renderable->StartY + deltaMouseY;
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

        //Draw Node List BG
        if (showNodeList)
        {
            //temp for profiling
            EventExampleNode->Run();
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

        //Draw Nodes
        for (int NodeIter = 0; NodeIter < NodeStack.size(); NodeIter++)
        {
            
            Node* curNode = NodeStack[NodeIter];
            NodeDrawable* renderable = curNode->renderable;
            int drawX, drawY;
            GetScreenCoordinates(renderable->x, renderable->y, &drawX, &drawY);

            //Nodedest.x = drawX;
            //Nodedest.y = drawY;

            //SDL_RenderCopy(rend, Nodetex, NULL, &Nodedest);

            int inputcount = curNode->inputs.size();
            int outputcount = curNode->outputs.size();

            //With margin
            int PortSize = 24;
            int midheight = std::fmax(inputcount, outputcount) * PortSize;

            SDL_Rect NodeElement;
            NodeElement.w = renderable->width;
            NodeElement.h = renderable->topMargin + midheight + renderable->bottomMargin;
            renderable->currentHeight = renderable->topMargin + midheight + renderable->bottomMargin;
            NodeElement.x = drawX;
            NodeElement.y = drawY;
            renderable->renderX = drawX;
            renderable->renderY = drawY;

            SDL_SetRenderDrawColor(rend, 40, 40, 50, 255);
            SDL_RenderFillRect(rend, &NodeElement);

            //Render text
            TTF_Init();

            TTF_Font* Sans = TTF_OpenFont("C:/Users/riley/source/repos/SDLTests/x64/Debug/arial.ttf", 12); //this opens a font style and sets a size

            //if (Sans == nullptr)
            //{
            //    std::cout << "font failed\n";
            //}
            //else
            //{
            //    std::cout << "font success\n";
            //}

            SDL_Color White = { 255, 255, 255 };  // this is the color in rgb format, maxing out all would give you the color white, and it will be your text's color

            const char* text = curNode->title;
            SDL_Surface* nodeMessage = TTF_RenderText_Solid(Sans, text, White); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first

            SDL_Texture* Message = SDL_CreateTextureFromSurface(rend, nodeMessage); //now you can convert it into a texture

            SDL_Rect Message_rect; //create a rect
            Message_rect.x = drawX + 2;  //controls the rect's x coordinate 
            Message_rect.y = drawY + 2; // controls the rect's y coordinte

            TTF_SizeText(Sans, text, &Message_rect.w, &Message_rect.h);


            SDL_RenderCopy(rend, Message, NULL, &Message_rect);

            
            SDL_DestroyTexture(Message);
            SDL_FreeSurface(nodeMessage);








            //draw ports
            
            //actual circle
            int IOSize = 8;

            for (int curInput = 0; curInput < inputcount; curInput++)
            {
                
                int topY = curInput * PortSize + renderable->topMargin + drawY;

                SDL_Surface* Portsurface;

                Portsurface = IMG_Load("C:/Users/riley/source/repos/SDLTests/x64/Debug/io.png");
                SDL_Texture* Nodetex = SDL_CreateTextureFromSurface(rend, Portsurface);
                SDL_FreeSurface(Portsurface);

                //color the port
                switch (curNode->inputs[curInput]->port.PortType)
                {
                case Data::Data_Type::Boolean:
                    SDL_SetTextureColorMod(Nodetex, 255, 0, 0);
                    break;
                case Data::Data_Type::Numeric:
                case Data::Data_Type::Float:
                    SDL_SetTextureColorMod(Nodetex, 255, 255, 0);
                    break;
                case Data::Data_Type::Integer:
                    SDL_SetTextureColorMod(Nodetex, 0, 255, 0);
                    break;
                case Data::Data_Type::String:
                    SDL_SetTextureColorMod(Nodetex, 255, 0, 255);
                    break;
                case Data::Data_Type::Weird:
                    SDL_SetTextureColorMod(Nodetex, 255, 147, 0);
                    break;
                }

                SDL_Rect Portdest;
                Portdest.w = IOSize;
                Portdest.h = IOSize;
                Portdest.x = drawX - 4;
                Portdest.y = topY;

                curNode->inputs[curInput]->port.renderX = Portdest.x + IOSize / 2;
                curNode->inputs[curInput]->port.renderY = Portdest.y + IOSize / 2;
                //SDL_QueryTexture(Nodetex, NULL, NULL, &Portdest.w, &Portdest.h);

                SDL_RenderCopy(rend, Nodetex, NULL, &Portdest);
                SDL_DestroyTexture(Nodetex);
            }

            for (int curOutput = 0; curOutput < outputcount; curOutput++)
            {
                int topY = curOutput * PortSize + renderable->topMargin + drawY;

                SDL_Surface* Portsurface;

                Portsurface = IMG_Load("C:/Users/riley/source/repos/SDLTests/x64/Debug/io.png");
                SDL_Texture* Nodetex = SDL_CreateTextureFromSurface(rend, Portsurface);
                SDL_FreeSurface(Portsurface);

                //color the port
                switch (curNode->outputs[curOutput]->port.PortType)
                {
                case Data::Data_Type::Boolean:
                    SDL_SetTextureColorMod(Nodetex, 255, 0, 0);
                    break;
                case Data::Data_Type::Numeric:
                case Data::Data_Type::Float:
                    SDL_SetTextureColorMod(Nodetex, 255, 255, 0);
                    break;
                case Data::Data_Type::Integer:
                    SDL_SetTextureColorMod(Nodetex, 0, 255, 0);
                    break;
                case Data::Data_Type::String:
                    SDL_SetTextureColorMod(Nodetex, 255, 0, 255);
                    break;
                case Data::Data_Type::Weird:
                    SDL_SetTextureColorMod(Nodetex, 255, 147, 0);
                    break;
                }

                SDL_Rect Portdest;
                Portdest.w = IOSize;
                Portdest.h = IOSize;
                Portdest.x = renderable->width + drawX - 4;
                Portdest.y = topY;

                curNode->outputs[curOutput]->port.renderX = Portdest.x + IOSize / 2;
                curNode->outputs[curOutput]->port.renderY = Portdest.y + IOSize / 2;
                //SDL_QueryTexture(Nodetex, NULL, NULL, &Portdest.w, &Portdest.h);

                SDL_RenderCopy(rend, Nodetex, NULL, &Portdest);
                SDL_DestroyTexture(Nodetex);
            }
        }

        //Draw Links
        for (int curLink = 0; curLink < LinkStack.size(); curLink++)
        {
            Link* current = LinkStack[curLink];

            int InX = current->LinkInput->port.renderX;
            int InY = current->LinkInput->port.renderY;
            int OutX = current->LinkOutput->port.renderX;
            int OutY = current->LinkOutput->port.renderY;

            SDL_SetRenderDrawColor(rend, 255, 255, 255, SDL_ALPHA_OPAQUE);
            SDL_RenderDrawLine(rend, InX, InY, OutX, OutY);
        }


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

    // destroy renderer 
    SDL_DestroyRenderer(rend);

    // destroy window 
    SDL_DestroyWindow(win);
    return 0;
}

