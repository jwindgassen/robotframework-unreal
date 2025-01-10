using UnrealBuildTool;

public class RobotIntegration : ModuleRules {
	public RobotIntegration(ReadOnlyTargetRules Target) : base(Target) {
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		if (Target.Configuration == UnrealTargetConfiguration.Shipping) {
			throw new BuildException(
				"Cannot use 'RobotIntegration' in a Shipping build, as the 'Automation Driver' module is not configured " +
				"for production use. Please make sure to only enable 'RobotIntegration' in Debug and Development builds!"
			);
		}

		PublicDependencyModuleNames.AddRange(new []{
			"Core",
			"XmlRpc",
			"RobotKeywords",
			"SlateCore",
			"Slate",
			"UMG",
		});

		PrivateDependencyModuleNames.AddRange(new []{
			"CoreUObject",
			"Engine",
		});
	}
}