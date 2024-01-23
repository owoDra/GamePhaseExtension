// Copyright (C) 2024 owoDra

using UnrealBuildTool;

public class GEPhase : ModuleRules
{
	public GEPhase(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
            new string[]
            {
                ModuleDirectory,
                ModuleDirectory + "/GEPhase",
            }
        );


        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core", "CoreUObject", "Engine",

                "GameplayTags", 
                
                "GFCore",
            }
        );


        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "GameFeatures", "ModularGameplay", "NetCore",
            }
        );
    }
}
