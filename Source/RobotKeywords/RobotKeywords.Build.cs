using UnrealBuildTool;

public class RobotKeywords : ModuleRules {
    public RobotKeywords(ReadOnlyTargetRules Target) : base(Target) {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new []{
            "Core",
        });
        
        PrivateDependencyModuleNames.AddRange(new []{
            "CoreUObject",
            "Engine",
            "InputCore",
            "XmlRpc",
            "AutomationDriver"
        });
    }
}