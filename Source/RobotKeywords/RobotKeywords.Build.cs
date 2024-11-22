using UnrealBuildTool;

public class RobotKeywords : ModuleRules {
    public RobotKeywords(ReadOnlyTargetRules Target) : base(Target) {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.Add("Core");
        
        PrivateDependencyModuleNames.AddRange(new []{
            "CoreUObject",
            "Engine",
            "AutomationDriver"
        });
    }
}