// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using UnrealBuildTool.Rules;

public class NeverGU : ModuleRules
{
	public NeverGU(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[]
		{	"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
            "AIModule",
            "GameplayTasks",
            "NavigationSystem",
            "Paper2D",
            "HeadMountedDisplay",
            "EnhancedInput",
			"UMG",
			"SlateCore",
			"Slate",
			"Niagara"});

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
