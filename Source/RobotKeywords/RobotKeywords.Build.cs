using System.IO;
using System.Linq;
using EpicGames.Core;
using UnrealBuildTool;

public class RobotKeywords : ModuleRules {
    public RobotKeywords(ReadOnlyTargetRules Target) : base(Target) {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new []{
            "Core",
            "XmlRpc",
            // "AutomationDriver",  see below
        });
        
        PrivateDependencyModuleNames.AddRange(new []{
            "CoreUObject",
            "Engine",
            
            // AutomationDriver Dependencies:
            "ApplicationCore",
            "InputCore",
            "Json",
            "Slate",
            "SlateCore",
        });
        
        /*
         * We need the AutomationDriver Module. Unfortunately, the Module is in the `Engine/Source/Developer`
         * Directory and can usually not be included in a packaged build. Do circumvent that, we copy the Source
         * Files into this Module here and just build them into this Module.
         * We also need to throw out a few bits to make them actually compilable.
         *
         * WARNING: This might cause a licensing problem. Do not distribute any packaged game with this Module enabled!
         */
        CopyAutomationDriver();
        PrivateDefinitions.Add("AUTOMATIONDRIVER_API=ROBOTKEYWORDS_API");
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "AutomationDriver", "Public")); 
        PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "AutomationDriver", "Private"));
    }

    private void CopyAutomationDriver() {
        string Source = Path.Combine(EngineDirectory, "Source", "Developer", "AutomationDriver");
        string Destination = Path.Combine(ModuleDirectory, "AutomationDriver");

        if (Directory.Exists(Destination)) {
            return;
        }
        
        Log.TraceInformationOnce("Copying AutomationDriver from '{0}' to '{1}'", Source, Destination);
        
        // Create all Directories
        foreach (string DirectoryPath in Directory.EnumerateDirectories(Source, "*", SearchOption.AllDirectories)) {
            Directory.CreateDirectory(DirectoryPath.Replace(Source, Destination));
        }
        
        // Copy all Files and make them editable
        foreach (string FilePath in Directory.EnumerateFiles(Source, "*.*", SearchOption.AllDirectories)) {
            File.Copy(FilePath, FilePath.Replace(Source, Destination));
            new FileInfo(FilePath.Replace(Source, Destination)).IsReadOnly = false;
        }
        
        // Delete AutomationDriver.Build.cs
        File.Delete(Path.Combine(Destination, "AutomationDriver.Build.cs"));
        
        // Remove contents in AutomationDriverModule.h / .cpp.
        // We still need the files so includes aren't broken, but we cannot use the Module System
        // We could keep the contents, but just making sure they aren't used accidentally
        File.WriteAllText(Path.Combine(Destination, "Public", "IAutomationDriverModule.h"), "");
        File.WriteAllText(Path.Combine(Destination, "Private", "AutomationDriverModule.cpp"), "");
    }
}