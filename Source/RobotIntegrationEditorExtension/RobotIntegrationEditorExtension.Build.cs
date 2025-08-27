using UnrealBuildTool;

public class RobotIntegrationEditorExtension : ModuleRules {
	public RobotIntegrationEditorExtension(ReadOnlyTargetRules Target) : base(Target) {
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		OptimizeCode = CodeOptimization.Never;

		PublicDependencyModuleNames.AddRange(new []{
			"Core", "UMGEditor",
		});
		
		PrivateDependencyModuleNames.AddRange(new []{
			"CoreUObject", "Engine", "Slate", "SlateCore",
			"UMG", "UnrealEd", "Kismet",
			"RobotIntegration",
		});
	}
}