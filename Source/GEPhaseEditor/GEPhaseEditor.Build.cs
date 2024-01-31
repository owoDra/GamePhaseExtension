// Copyright (C) 2024 owoDra

using UnrealBuildTool;

public class GEPhaseEditor : ModuleRules
{
	public GEPhaseEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;	

		PublicIncludePaths.AddRange(
            new string[] 
			{
                ModuleDirectory,
                ModuleDirectory + "/GEPhaseEditor",
            }
        );			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
			}
		);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
                "Core", "CoreUObject", "Engine", "UnrealEd",

                "ClassViewer", "AssetTools", "ToolMenus",

                "InputCore", "Slate", "SlateCore",

                "Kismet", "KismetCompiler",

                "GEPhase",
            }
		);
	}
}
