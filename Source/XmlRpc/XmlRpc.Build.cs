using UnrealBuildTool;

public class XmlRpc : ModuleRules {
    public XmlRpc(ReadOnlyTargetRules Target) : base(Target) {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new [] {
            "Core",
            "HTTPServer",
            "XmlParser"
        });
        
        PrivateDependencyModuleNames.AddRange(new []{
            "CoreUObject", 
            "Engine"
        });
    }
}