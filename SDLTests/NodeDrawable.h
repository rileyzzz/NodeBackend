#pragma once
class NodeDrawable
{
public:
	int x, y;
	int topMargin, bottomMargin;
	int width;
	bool Collapsed;
	NodeDrawable(int givenX = 0, int givenY = 0)
	{
		Collapsed = false;

		x = givenX;
		y = givenY;
		topMargin = 20;
		bottomMargin = 20;
		width = 120;
	}
};

