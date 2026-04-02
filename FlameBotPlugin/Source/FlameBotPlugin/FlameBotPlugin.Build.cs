using UnrealBuildTool;
using System.IO;

public class FlameBotPlugin : ModuleRules
{
    public FlameBotPlugin(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        // Engine kaynak kodları Private klasöründe
        PrivateIncludePaths.AddRange(new string[]
        {
            Path.Combine(ModuleDirectory, "Private"),
            Path.Combine(ModuleDirectory, "Private/Chess"),
            Path.Combine(ModuleDirectory, "Private/FlameBoth"),
            Path.Combine(ModuleDirectory, "Private/BoardHash"),
            Path.Combine(ModuleDirectory, "Private/OpeningBook"),
            Path.Combine(ModuleDirectory, "Private/Time"),
        });

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine"
        });
    }
}