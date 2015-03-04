#include "AIE.h"
#include "Graph.h"
#include <iostream>
#include <string>
#include <fstream>

using namespace std;

int main( int argc, char* argv[] )
{	
	//initialize screen settings
	Initialise(720, 720, false, "Tanks");
	SetBackgroundColour(SColour(0x00, 0x00, 0x00, 0xFF));

	Graph myGraph(4, 4);	//	create graph
	std::vector<GraphNode*>::iterator beg = myGraph.m_aNodes.begin();
	beg++;
	std::vector<GraphNode*>::iterator end = myGraph.m_aNodes.end();
	end--;
	cout << myGraph.SearchDFSrec((*beg), (*end)) << endl;

	GraphNode display;
	display = myGraph.FindNode(1, 1);
	cout << display.m_iNodeNumber;

    //***********************Game Loop************************
	do
	{
		ClearScreen();

		

	} while(FrameworkUpdate() == false);
	//*******************************************************
		

    Shutdown();

    return 0;
}

