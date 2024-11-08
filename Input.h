#pragma once



class Input
{
public:
	Input() = default;

	void Down();
	void Up();

	void ResetForNextFrame();

	bool down = false;
	bool up = false;
	bool held = false;
};



