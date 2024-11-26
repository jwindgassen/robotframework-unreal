using UnrealBuildTool;

public class XmlRpc : ModuleRules {
    public XmlRpc(ReadOnlyTargetRules Target) : base(Target) {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new []{
            "Core", 
            "HttpServer",
        });

        PrivateDependencyModuleNames.AddRange(new []{
            "CoreUObject",
            "Engine",
            "HttpServer",
            "XmlParser",
        });
    }
}