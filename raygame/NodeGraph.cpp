#include "NodeGraph.h"
#include <raylib.h>
#include <xlocinfo>

DynamicArray<NodeGraph::Node*> reconstructPath(NodeGraph::Node* start, NodeGraph::Node* end)
{
	resetGraphScore(start);

	DynamicArray<NodeGraph::Node*> path;
	NodeGraph::Node* currentNode = end;

	while (currentNode != start->previous)
	{
		currentNode->color = 0xFF0000FF;
		path.insert(currentNode, 0);
		currentNode = currentNode->previous;
	}

	return path;
}

float diagonalDistance(NodeGraph::Node* node, NodeGraph::Node* goal, float cardinalCost, float diagonalCost)
{
	float displacementX = abs(node->position.x - goal->position.x);
	float displacementY = abs(node->position.y - goal->position.y);

	return cardinalCost * (displacementX + displacementY) + (diagonalCost - 2 * cardinalCost) * fmin(displacementX, displacementY);
}

void sortFScore(DynamicArray<NodeGraph::Node*>& nodes)
{
	NodeGraph::Node* key = nullptr;
	int j = 0;

	for (int i = 1; i < nodes.getLength(); i++) {
		key = nodes[i];
		j = i - 1;
		while (j >= 0 && nodes[j]->fScore > key->fScore) {
			nodes[j + 1] = nodes[j];
			j--;
		}

		nodes[j + 1] = key;
	}
}

DynamicArray<NodeGraph::Node*> NodeGraph::findPath(Node* start, Node* goal)
{
	// Create two lists and a currentNode that will hold the starting position.
	DynamicArray<NodeGraph::Node*> openSet = DynamicArray<NodeGraph::Node*>();
	DynamicArray<NodeGraph::Node*> closedSet = DynamicArray<NodeGraph::Node*>();
	Node* currentNode = start;

	// Adds start to the open list.
	openSet.addItem(start);

	// While there is an item in the open list, this will loop.
	while (openSet.getLength() > 0)
	{
		sortByGScore(openSet);
		currentNode = openSet[0];

		if (currentNode == goal)
			return reconstructPath(start, goal);

		openSet.remove(currentNode);

		if (!closedSet.contains(currentNode))
		{
			// For each of the nodes next to the current node set the g scores of each and set
			for (int i = 0; i < currentNode->edges.getLength(); i++)
			{
				NodeGraph::Node* targetNode = currentNode->edges[i].target;
				if (targetNode->gScore == 0 || targetNode->gScore > currentNode->gScore + currentNode->edges[i].cost)
				{
					targetNode->gScore = currentNode->gScore + currentNode->edges[i].cost;
					targetNode->previous = currentNode;
				}
				if(!openSet.contains(targetNode))
					openSet.addItem(targetNode);
			}
			closedSet.addItem(currentNode);
		}
	}
}

void NodeGraph::drawGraph(Node* start)
{
	DynamicArray<Node*> drawnList = DynamicArray<Node*>();
	drawConnectedNodes(start, drawnList);
}

void NodeGraph::drawNode(Node* node, float size)
{
	static char buffer[10];
	sprintf_s(buffer, "%.0f", node->gScore);

	//Draw the circle for the outline
	DrawCircle((int)node->position.x, (int)node->position.y, size + 1, GetColor(node->color));
	//Draw the inner circle
	DrawCircle((int)node->position.x, (int)node->position.y, size, GetColor(node->color));
	//Draw the text
	DrawText(buffer, (int)node->position.x, (int)node->position.y, .8f, BLACK);
}

void NodeGraph::drawConnectedNodes(Node* node, DynamicArray<Node*>& drawnList)
{
	drawnList.addItem(node);
	if (node->walkable)
		drawNode(node, 8);

	for (int i = 0; i < node->edges.getLength(); i++)
	{
		Edge e = node->edges[i];
		//Draw the Edge
		DrawLine((int)node->position.x, (int)node->position.y, (int)e.target->position.x, (int)e.target->position.y, WHITE);
		//Draw the cost
		MathLibrary::Vector2 costPos = { (node->position.x + e.target->position.x) / 2, (node->position.y + e.target->position.y) / 2 };
		static char buffer[10];
		sprintf_s(buffer, "%.0f", e.cost);
		DrawText(buffer, (int)costPos.x, (int)costPos.y, 16, RAYWHITE);
		//Draw the target node
		if (!drawnList.contains(e.target)) {
			drawConnectedNodes(e.target, drawnList);
		}
	}
}

void NodeGraph::sortByGScore(DynamicArray<Node*>& nodeList)
{
	NodeGraph::Node* key = nullptr;
	int j = 0;

	for (int i = 1; i < nodeList.getLength(); i++) {
		key = nodeList[i];
		j = i - 1;
		while (j >= 0 && nodeList[j]->gScore > key->gScore) {
			nodeList[j + 1] = nodeList[j];
			j--;
		}

		nodeList[j + 1] = key;
	}
}

void NodeGraph::resetGraphScore(Node * start)
{
	DynamicArray<Node*> resetList = DynamicArray<Node*>();
	resetConnectedNodes(start, resetList);
}

void NodeGraph::resetConnectedNodes(Node* node, DynamicArray<Node*>& resetList)
{
	resetList.addItem(node);

	for (int i = 0; i < node->edges.getLength(); i++)
	{
		node->edges[i].target->gScore = 0;
		node->edges[i].target->hScore = 0;
		node->edges[i].target->fScore = 0;
		node->edges[i].target->color = 0xFFFFFFFF;

		//Draw the target node
		if (!resetList.contains(node->edges[i].target)) {
			resetConnectedNodes(node->edges[i].target, resetList);
		}
	}
}