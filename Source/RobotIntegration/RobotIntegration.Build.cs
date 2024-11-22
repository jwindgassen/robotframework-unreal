using UnrealBuildTool;

public class RobotIntegration : ModuleRules {
	public RobotIntegration(ReadOnlyTargetRules Target) : base(Target) {
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new []{
			"Core", "RobotKeywords"
		});
		
		PrivateDependencyModuleNames.AddRange(new []{
			"CoreUObject",
			"Engine",
			"HTTPServer",
			"Json",
		});
	}
}