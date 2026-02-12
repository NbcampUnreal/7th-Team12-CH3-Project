// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TheSeventhbullet : ModuleRules
{
	public TheSeventhbullet(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput","NavigationSystem","AIModule", "Niagara", "GameplayTasks", });
	}
}
