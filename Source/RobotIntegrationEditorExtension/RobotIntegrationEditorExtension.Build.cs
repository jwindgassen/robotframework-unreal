using UnrealBuildTool;

public class RobotIntegrationEditorExtension : ModuleRules
{
	public RobotIntegrationEditorExtension(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		OptimizeCode = CodeOptimization.Never;

		PublicDependencyModuleNames.Add("Core");
		PrivateDependencyModuleNames.AddRange(new[]
		{
			"CoreUObject", "Engine",
			"Slate", "SlateCore", "UMG", "UMGEditor"
		});
	}
}