#include "Interface.h"

int main(int argc, char** argv)
{
	wsp::Controller controller;
	if (!controller.start())
	{
		return 1;
	}

	wsp::Interface ui(controller); 
	ui.start();

	return 0;
}