#include "ChaserPathfindingComponent.h"
#include "Ghost.h"
#include "Transform2D.h"
#include "MazeScene.h"

MathLibrary::Vector2 ChaserPathfindComponent::findDestination()
{
	if(dynamic_cast<Ghost*>(getOwner())->getIsChaser())
		return getTarget()->getTransform()->getWorldPosition();
	else
		return getOwner()->getTransform()->getWorldPosition() + getOwner()->getTransform()->getForward();
}
