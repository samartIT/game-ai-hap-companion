// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class HAPBook : ModuleRules
{
	public HAPBook(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"NavigationSystem",
			"GameplayTasks",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"HAPBook",
			"HAPBook/Variant_Platforming",
			"HAPBook/Variant_Platforming/Animation",
			"HAPBook/Variant_Combat",
			"HAPBook/Variant_Combat/AI",
			"HAPBook/Variant_Combat/Animation",
			"HAPBook/Variant_Combat/Gameplay",
			"HAPBook/Variant_Combat/Interfaces",
			"HAPBook/Variant_Combat/UI",
			"HAPBook/Variant_SideScrolling",
			"HAPBook/Variant_SideScrolling/AI",
			"HAPBook/Variant_SideScrolling/Gameplay",
			"HAPBook/Variant_SideScrolling/Interfaces",
			"HAPBook/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
