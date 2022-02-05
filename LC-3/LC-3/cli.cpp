#include <iostream>
#include <fstream>
#include <bit>

#include <signal.h> // SIGINT
#ifdef WIN32
#include <Windows.h>
#include <conio.h>  // _kbhit

HANDLE hStdin = INVALID_HANDLE_VALUE;
DWORD fdwMode, fdwOldMode;

void disable_input_buffering()
{
	hStdin = GetStdHandle(STD_INPUT_HANDLE);
	GetConsoleMode(hStdin, &fdwOldMode); /* save old mode */
	fdwMode = fdwOldMode
		^ ENABLE_ECHO_INPUT  /* no input echo */
		^ ENABLE_LINE_INPUT; /* return when one or
								more characters are available */
	SetConsoleMode(hStdin, fdwMode); /* set new mode */
	FlushConsoleInputBuffer(hStdin); /* clear buffer */
}

void restore_input_buffering()
{
	SetConsoleMode(hStdin, fdwOldMode);
}

void handle_interrupt(int signal)
{
	restore_input_buffering();
	printf("\n");
	exit(-2);
}
#endif

#include "LC-3.h"

int main(int argc, char** argv)
{

	signal(SIGINT, handle_interrupt);
	disable_input_buffering();

	argc--;
	argv++;

	if (argc < 1)
	{
		std::cout << "Only one argument is supported - the filename (object file) e.g. \"my_src.obj\"" << std::endl;
		//return EXIT_FAILURE;
	}


	VirtualMachine lc3(false);
	//lc3.LoadObj(argv[1]);
	lc3.LoadObj("2048.obj");
	lc3.Run();

	restore_input_buffering();

	return EXIT_SUCCESS;
}