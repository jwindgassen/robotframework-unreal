using UnrealBuildTool;

public class XmlRpc : ModuleRules {
    public XmlRpc(ReadOnlyTargetRules Target) : base(Target) {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new [] {
            "Core",
            "XmlParser"
        });
        
        PrivateDependencyModuleNames.AddRange(new []{
            "CoreUObject", 
            "Engine",
            "Sockets",
            "Networking"
        });
    }
}