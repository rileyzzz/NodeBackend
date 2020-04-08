#pragma once
class NodeDrawable
{
public:
	int x, y;
	int renderX, renderY;
	int topMargin, bottomMargin;
	int width;
	int currentHeight;
	bool Collapsed;

	//for dragging
	int mouseStartX, mouseStartY;
	int StartX, StartY;
	NodeDrawable(int givenX = 0, int givenY = 0)
	{
		Collapsed = false;

		x = givenX;
		y = givenY;
		topMargin = 20;
		bottomMargin = 4;
		width = 120;
		currentHeight = 0;
		renderX = 0;
		renderY = 0;

		mouseStartX = 0;
		mouseStartY = 0;
		StartX = 0;
		StartY = 0;
	}
};

