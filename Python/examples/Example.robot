*** Settings ***
Documentation    Execute some Example Tests to showcase the RobotIntegration Plugin. We start the Application and 
...              override the Startup Level to the ExampleLevel from the Plugin Content.
...              To work correctly, please provide the Path to the Application and the Engine Installation via a global 
...              variable by adding `--variable UNREAL_APPLICATION_PATH:<path/to/application.uproject>` and
...              `--variable UNREAL_ENGINE_PATH:<path/to/engine>` to the commandline.

Library    UnrealLibrary

Suite Setup    Start Application
Suite Teardown    Stop Unreal Application


*** Test Cases ***
Check HUD is visible
    Element Should Be Visible    \#MessageInput
    Element Should Be Visible    \#OpenLevel
    Element Should Not Be Visible     \#SpawnCube

Show a Toast
    Type Text On Element    A simple Message    \#MessageInput
    Click Element    \#ShowMessage
    Element Should Be Visible    \#Toast
    Sleep    6s    Wait until the Toast disappeares
    Element Should Not Be Visible    \#Toast
    
Open the other Panel
    Click Element    <SCheckbox>
    Element Should Be Visible    \#SpawnCube

Spawn a Cube
    Click Element    \#SpawnCube
    Level Has Actor Of Class    /RobotIntegration/SimpleCube.SimpleCube_C

Switch to another Level and back
    Click Element    \#OpenLevel
    ${Level} =     Get Level Name
    Should Be Equal    ${Level}    OtherLevel


*** Keywords ***
Start Application
    Build Unreal Application

    ${Args} =     Create List    /RobotIntegration/ExampleLevel.ExampleLevel
    Start Unreal Application    run_args=${Args}
    
Element should be visible
    [Arguments]    ${Locator}
    ${Visible} =     Element Is Visible    ${Locator}
    Should Be True    ${Visible}

Element should not be visible
    [Arguments]    ${Locator}
    TRY
        ${Visible} =     Element Is Visible    ${Locator}
    EXCEPT    Could not locate Element
        RETURN
    END
    
    Should Not Be True    ${Visible}
