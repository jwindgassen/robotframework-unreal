# RobotIntegration Examples

This directory contains an example Robot Framework Test Suite for showcasing
the basic functionality of the `RobotIntegration` Plugin.

### How it works

The Example Suite utilizes the Assets from the Content of the Plugin, which contains
a small test environment. After the Plugin has been added to a project, we can override
the Level we open by passing its name to the open command and open the `ExampleLevel` instead.
This level will add a HUD to the Viewport, which we can then use to run some tests.

### How to run the Example Test Suite

To correctly run the Test Suite, you need to provide the path to the Engine Installation
(required to find the `RunUAT.bat`) and the path to the Application's uproject file as
global variables to the Suite:

`--variable UNREAL_ENGINE_PATH:/path/to/Engine/ --variable UNREAL_APPLICATION_PATH:/path/to/Application.uproject`

Additionally, its recommended to also provide an output directory: `--outputdir=output`

To run the Suite, execute the following command, replacing the paths with their correct location:

```shell
cd Python/examples

robot \
  --variable UNREAL_ENGINE_PATH:/path/to/Engine/ \
  --variable UNREAL_APPLICATION_PATH:/path/to/Application.uproject \
  --outputdir=output \
  Example.robot
```
