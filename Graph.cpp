#include "graph.h"

Graph::Graph(int a_x, int a_y)
{
	// create and set x&y of nodes
	for (int i = 0; i < a_y; i++)
	{
		for (int j = 0; j < a_x; j++)
		{
			GraphNode *ptr = new GraphNode;
			ptr->posX = j;
			ptr->posY = i;
			ptr->m_iNodeNumber = (i*a_x) + j;
			this->AddNode(ptr);
		}
	}

	//	add edges
	std::vector<GraphNode*>::iterator it = this->m_aNodes.begin();
	for (int i = 0; i < a_y; i++)
	{
		for (int j = 0; j < a_x; j++)
		{
			float node = (i*a_x) + j;
			//	edge above
			if (i < a_y - 1)
				this->m_aNodes[node]->AddEdge(this->m_aNodes[node + a_x], 1);
			// edge to the right
			if (j < a_x - 1)
				this->m_aNodes[node]->AddEdge(this->m_aNodes[node + 1], 1);
			//	edge down
			if (i > 0)
				this->m_aNodes[node]->AddEdge(this->m_aNodes[node - a_x], 1);
			//	edge to the left
			if (j > 0)
				this->m_aNodes[node]->AddEdge(this->m_aNodes[node - 1], 1);
		}
	}
}

GraphNode::GraphNode(int a_iNum)
{
	this->m_iNodeNumber = a_iNum;
}

void GraphNode::DisplayEdges()
{
	std::vector<Edge>::iterator it;
	cout << "Node " << this->m_iNodeNumber << " connects to: ";
	for (it = this->m_aEdges.begin(); it != this->m_aEdges.end(); ++it) {
		cout << it->m_pEnd->m_iNodeNumber << ", "; /*<< ". cost = " << it->m_fCost << endl;*/
	}
	cout << endl;
}

void Graph::AddNode(GraphNode* a_pNode)
{
	m_aNodes.push_back(a_pNode);
}

void Graph::RemoveNode(GraphNode* a_pNode)
{
	std::vector<GraphNode*>::iterator pNode = this->m_aNodes.begin();
	for (pNode; pNode != m_aNodes.end(); pNode++)
	{
		(*pNode)->RemoveEdges(a_pNode);
	}
}

void GraphNode::RemoveEdges(GraphNode *n)
{
	std::vector<Edge>::iterator it = this->m_aEdges.begin();
	while (it != this->m_aEdges.end())
	{
		if (it->m_pEnd == n)
		{
			//	erase returns random iterator to next element (have to assign it to it)
			it = m_aEdges.erase(it);
		}
		else
		{
			if (it != this->m_aEdges.end())
				it++;
		}
	}
}

void GraphNode::AddEdge(GraphNode *endNode, float cost)
{
	Edge temp;
	temp.m_pEnd = endNode;
	temp.m_pStart = this;
	temp.m_fCost = cost;
	this->m_aEdges.push_back(temp);
}

void Graph::DisplayNodes()
{
	std::vector<GraphNode*>::iterator it;
	for (it = this->m_aNodes.begin(); it != this->m_aNodes.end(); ++it) {
		cout << "Node: " << (*it)->posX << ", " << (*it)->posY << endl;
	}
}

void Graph::ResetVisited()
{
	std::vector<GraphNode*>::iterator it;
	for (it = this->m_aNodes.begin(); it != this->m_aNodes.end(); ++it) {
		(*it)->m_bVisited = false;
	}
}

bool Graph::SearchDFS(GraphNode* a_pStart, GraphNode* a_pEnd)
{
	std::stack<GraphNode*> oNodeStack;
	oNodeStack.push(a_pStart);

	while (!oNodeStack.empty())
	{
		GraphNode* pCurrent = oNodeStack.top();
		oNodeStack.pop();

		//	error checking if visisted
		if (pCurrent->m_bVisited == true)
		{
			continue;
		}

		//	set visited 
		pCurrent->m_bVisited = true;

		//	exit when found end
		if (pCurrent == a_pEnd)
		{
			return true;
		}

		// add all the neighbours of the current node onto the stack
		for (int i = 0; i < pCurrent->m_aEdges.size(); ++i)
		{
			if (pCurrent->m_aEdges[i].m_pEnd->m_bVisited != true)
			{
				oNodeStack.push(pCurrent->m_aEdges[i].m_pEnd);
			}
		}
	}

	//return false if we didn't find a_pEnd
	return false;
}

bool Graph::SearchDFSrec(GraphNode* a_pStart, GraphNode* a_pEnd)
{
	if (a_pStart->m_iNodeNumber != a_pEnd->m_iNodeNumber)
	{
		for (int i = 0; i < a_pStart->m_aEdges.size(); i++)
		{
			if (SearchDFSrec(a_pStart->m_aEdges[i].m_pEnd, a_pEnd))
				return true;
		}
	}
	else
		return true;
	return false;
}

bool Graph::SearchBFS(GraphNode* a_pStart, GraphNode* a_pEnd)
{
	std::queue<GraphNode*> oNodeQueue;
	oNodeQueue.push(a_pStart);

	while (!oNodeQueue.empty())
	{
		GraphNode* pCurrent = oNodeQueue.front();
		oNodeQueue.pop();

		//	error checking if visisted
		if (pCurrent->m_bVisited == true)
		{
			continue;
		}

		//	set visited 
		pCurrent->m_bVisited = true;

		//	exit when found end
		if (pCurrent == a_pEnd)
		{
			return true;
		}

		// add all the neighbours of the current node onto the queue
		for (int i = 0; i < pCurrent->m_aEdges.size(); ++i)
		{
			if (pCurrent->m_aEdges[i].m_pEnd->m_bVisited != true)
			{
				oNodeQueue.push(pCurrent->m_aEdges[i].m_pEnd);
			}
			/*if (pCurrent->m_aEdges[i].m_pEnd == a_pEnd)
			return true;*/
		}
	}

	//return false if we didn't find a_pEnd
	return false;
}

GraphNode Graph::FindNode(float a_x, float a_y)
{
	unsigned int closestX = INFINITY, closestY = INFINITY;
	std::vector<GraphNode*>::iterator it = this->m_aNodes.begin();
	GraphNode* returnNode = NULL;
	for (it; it != this->m_aNodes.end(); it++)
	{
		//	return the exact position: FIX if needed
		unsigned int tempX = (*it)->posX - a_x;
		unsigned int tempY = (*it)->posY - a_y;
		if (tempX == 0 && tempY == 0)
			returnNode = *it;
	}
	return (*returnNode);
}