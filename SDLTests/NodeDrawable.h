#pragma once
class NodeDrawable
{
public:
	int x, y;

	bool Collapsed;
	NodeDrawable(int givenX = 0, int givenY = 0)
	{
		Collapsed = false;

		x = givenX;
		y = givenY;
	}
};

