#include "MainPlayerController.h"

AMainPlayerController::AMainPlayerController()
	: InputMappingContext(nullptr),
	MoveAction(nullptr),
	LookAction(nullptr),
	SprintAction(nullptr),
	DodgeAction(nullptr),
	InteractAction(nullptr),
	SkillAction(nullptr),
	OpenInventoryAction(nullptr),
	AimAction(nullptr),
	FireAction(nullptr)
{
}
