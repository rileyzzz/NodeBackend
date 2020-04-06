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
extern "C" { FILE __iob_func[3] = { *stdin,*stdout,*stderr }; }


int IDcount = 0;
std::vector<Node*> NodeStack;

int gridoffsetX = 0;
int gridoffsetY = 0;
int MMoffsetX = 0;
int MMoffsetY = 0;
int gridParallaxAmount = 1;

Node* CreateNode(Node::Node_Type type, std::vector<Input*> inputs, std::vector<Output*> outputs, const char* title, int x = 0, int y = 0, Data* (*f)(std::vector<Data*>) = nullptr)
{
    Node* NewNode = new Node(IDcount, type, inputs, outputs, title, x, y, f);
    IDcount++;
    NodeStack.push_back(NewNode);
    return NewNode;
}

static void DestroyNode(int id)
{
    NodeStack.erase(NodeStack.begin() + id);
}

void GetScreenCoordinates(int x, int y, int* sendX, int* sendY)
{
    //int returnarr[2];
    *sendX = (gridoffsetX + MMoffsetX / gridParallaxAmount) + x;
    *sendY = (gridoffsetY + MMoffsetY / gridParallaxAmount) + y;
}

Data* CalculateLinkChain(Output* srcLink)
{
    Node* ParentNode = srcLink->ParentNode;
    /*std::cout << "Calculating link chain for node: ";
    std::cout << ParentNode->title;
    std::cout << "\n";*/
    if (ParentNode)
    {
        Data* returnval = nullptr;
        switch (ParentNode->type)
        {
        case Node::Node_Type::Node_Input:
            std::cout << "Input node found\n";
            returnval = new NodeInteger(5);
            break;

        case Node::Node_Type::Node_Calculation:
            if (ParentNode->inputs.size())
            {
                for (int dependency = 0; dependency < ParentNode->inputs.size(); dependency++)
                {
                    ParentNode->CalculatedInputs.push_back(CalculateLinkChain(ParentNode->inputs[dependency]->link));
                }
                returnval = ParentNode->Calculate(ParentNode->CalculatedInputs);
            }
            break;

        }
        return returnval;
    }
}

Data* CalculateOutput(Node* output)
{
    if (output->type != Node::Node_Type::Node_Output)
    {
        std::cout << "This node is not of type output.";
        return new NodeNone();
    }

    //Loop through every input required for our output node.
    //std::cout << "output input amount is:";
    //std::cout << output->inputs.size();
    //std::cout << "\n";

    for (int nodeInputID = 0; nodeInputID < output->inputs.size(); nodeInputID++)
    {
        Input* nodeInput = output->inputs[nodeInputID];

        //Recursion through entire link chain
        output->CalculatedInputs.push_back(CalculateLinkChain(nodeInput->link));
    }
    return output->CalculatedInputs[0];
}

Data* ExampleMultiply(std::vector<Data*> calcInputsMoved)
{
    NodeInteger* input1 = (NodeInteger*)calcInputsMoved[0];
    int value1 = input1->value;

    NodeInteger* returndata = new NodeInteger(value1*5);
    return returndata;
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
    

    //Create example node
    std::vector<DataPort> inPorts{  };
    std::vector<DataPort> outPorts{ DataPort(Data::Data_Type::Integer) };
    std::vector<Input*> nodeInputs;
    std::vector<Output*> nodeOutputs = CreateOutputs(outPorts);

    Node* ExampleNode = CreateNode(Node::Node_Type::Node_Input, nodeInputs, nodeOutputs, "Example Node", 0, 0);


    std::vector<DataPort> inPorts2{ DataPort(Data::Data_Type::Integer) };
    std::vector<DataPort> outPorts2{ DataPort(Data::Data_Type::Integer) };
    std::vector<Input*> nodeInputs2 = CreateInputs(inPorts2);
    std::vector<Output*> nodeOutputs2 = CreateOutputs(outPorts2);

    Node* ExampleNode2 = CreateNode(Node::Node_Type::Node_Calculation, nodeInputs2, nodeOutputs2, "Example Node 2", 150, 0, ExampleMultiply);
    
    //ExampleNode2->Calculate


    std::vector<DataPort> inPorts3{ DataPort(Data::Data_Type::Integer) };
    std::vector<DataPort> outPorts3{ };
    std::vector<Input*> nodeInputs3 = CreateInputs(inPorts3);
    std::vector<Output*> nodeOutputs3;

    Node* ExampleNode3 = CreateNode(Node::Node_Type::Node_Output, nodeInputs3, nodeOutputs3, "Output", 300, 0);

    //Setup example connections
    ExampleNode3->inputs[0]->link = ExampleNode2->outputs[0];
    ExampleNode2->inputs[0]->link = ExampleNode->outputs[0];

  
    Data* outputresult = CalculateOutput(ExampleNode3);
    NodeInteger* calcvalue = (NodeInteger*)outputresult;
    //NodeInteger& cval = static_cast<NodeInteger&>(outputresult);
    std::cout << "Output result is: ";
    if (outputresult->type == Data::Data_Type::Integer)
    {
        std::cout << "success - value is: ";
        std::cout << calcvalue->value;
    }
    else {
        std::cout << "fail";
    }
    std::cout << "\n";


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
                    std::cout << "Left mouse pressed\n";
                    break;
                case SDL_BUTTON_RIGHT:
                    std::cout << "Right mouse pressed\n";
                    break;
                case SDL_BUTTON_MIDDLE:
                    std::cout << "Middle mouse pressed\n";
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
                    std::cout << "Left mouse released\n";
                    break;
                case SDL_BUTTON_RIGHT:
                    std::cout << "Right mouse released\n";
                    break;
                case SDL_BUTTON_MIDDLE:
                    std::cout << "Middle mouse released\n";
                    MiddleMouse = false;
                    gridoffsetX += (MMoffsetX / gridParallaxAmount);
                    gridoffsetY += (MMoffsetY / gridParallaxAmount);
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
        //std::cout << zoomLevel;
        //std::cout << "\n";
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
                dest.x = (myX - 1) * dest.w + ((gridoffsetX + MMoffsetX / gridParallaxAmount) % dest.w);

                // sets initial y-position of object 
                dest.y = (myY - 1) * dest.h + ((gridoffsetY + MMoffsetY / gridParallaxAmount) % dest.h);

                SDL_RenderCopy(rend, tex, NULL, &dest);
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

        //Draw Nodes
        for (int NodeIter = 0; NodeIter < NodeStack.size(); NodeIter++)
        {
            
            Node* curNode = NodeStack[NodeIter];
            NodeDrawable* renderable = curNode->renderable;
            int drawX, drawY;
            GetScreenCoordinates(renderable->x, renderable->y, &drawX, &drawY);
            //std::cout << "X: ";
            //std::cout << drawX;
            //std::cout << "\n";
            //std::cout << "Y: ";
            //std::cout << drawY;
            //std::cout << "\n";
            

            Nodedest.x = drawX;
            Nodedest.y = drawY;

            //Nodedest.x = 500;
            //Nodedest.y = 500;

            SDL_RenderCopy(rend, Nodetex, NULL, &Nodedest);
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



// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
