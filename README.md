# Visibility_Study

## Introduction

This repository contains code related to research methodologies related to quantifying driver blind spots and fields of view for various vehicles.

x64 Windows is currently the only platform support available.

## Getting Started

The app is a C++ application that can be built in Visual Studio, but you will need the C++ desktop tools to do so. You can install the Desktop Development with C++ tools (including cmake) from the Tools > Get tools and features menu in Visual Studio.

## Build and Test Locally

* Clone this repo to your local system.
* Open the `pgrid.sln` solution file in Visual Studio.
* Set the **pgrid** project as the start up project.
* Build the solution. 
* Start the debugger.


## Using "markerless" mode

The app has several available grid alignment methods but we're aiming to use the "marklerless" mode. This mode uses a single camera to capture images of the vehicle and the environment. The user will be prompted to select a region of interest (ROI) in the image. The app will then use the ROI to detect the vehicle and calculate the blind spot and field of view. Other modes are expected to be buggy and are mostly just proof of concept.

To use "markerless" mode:
1. Open the app.
2. Click "Load camera profile" and select the relevant camera profile for the image. Each camera should have it's own specific profile & calibration.
3. Use file > Import image to import the image.
4. Select the "markerless" mode.
5. Input the various camera position values for the image (X, Y, and Z offsets + camera rotation).
6. Annotate the visible region in the image using the Paint mode.
7. Export the results by selecting a file location, giving the file a name (don't forget the .csv extension!), adding the dummy settings, and clicking Write Output. Use append to add to an existing file.

## Deployment from Visual Studio on developer machine

0. In order to remove the hardcoded "_Test" at the end of published MSIX directory names, Microsoft requires you to edit the `Microsoft.AppxPackage.Targets` file, which for VS 2022 can be found at `C:\Program Files\Microsoft Visual Studio\2022\Enterprise\MSBuild\Microsoft\VisualStudio\v17.0\AppxPackage`. 

    Remove the "_Test" suffix from the AppxPackageTestDir and AppxPackageTestExternalPackagesDir conditions. The edited code should look like this:

    ```xml
        <PropertyGroup Condition="'$(AppxPackageTestDir)' == ''">
        <AppxPackageTestDir Condition="'$(ProduceAppxBundle)' == 'false'">$(AppxPackageDir)$(AppxPackageName)\</AppxPackageTestDir>
        <AppxPackageTestDir Condition="'$(ProduceAppxBundle)' == 'true'">$(AppxPackageDir)$(AppxPackageNameNeutral)$(_AppxPackageConfiguration)\</AppxPackageTestDir>
        </PropertyGroup>

        <PropertyGroup Condition="'$(AppxPackageTestExternalPackagesDir)' == ''">
        <AppxPackageTestExternalPackagesDir Condition="'$(ProduceAppxBundle)' == 'false'">$(AppxPackageDir)$(AppxPackageName)\$(ExternalPackagesDir)</AppxPackageTestExternalPackagesDir>
        <AppxPackageTestExternalPackagesDir Condition="'$(ProduceAppxBundle)' == 'true'">$(AppxPackageDir)$(AppxPackageNameNeutral)$(_AppxPackageConfiguration)\$(ExternalPackagesDir)</AppxPackageTestExternalPackagesDir>
        </PropertyGroup>
    ```

1. Make sure you build in Release for x64.
2. Right click on the pgridPackagingProject project and select Publish > Create App Packages ...
3. Select sideloading with automatic updates, and click Next.
4. Select a software signing certificate to sign the app package, with http://timestamp.digicert.com timestamp server and SHA256 algorithm. Click Next.
5. Use the following settings, and then click Next:
    * Output location: < *path to local folder where install files will be copied* >
    * Auto increment version number
    * Generate app bundle: never
    * Packages to create: x64 Release
6. Use the following settings, and then click Create:
    * Installer location: < *url or path to installer location after copy* >
    * Check for updates: every time the application runs
7. If successful, you will get a pop up with details of the deployment and the installation page should be available at the installer location you specified above (which can be used for both installation and updates).

## Licensing

This software is licensed under the terms of the Apache License, version 2.0. 

This software was built with appreciation using the following open source projects, without modification to their original source code:

* [GLEW](https://glew.sourceforge.net/) [(licensing)](https://github.com/nigels-com/glew#copyright-and-licensing)
* [GLFW](www.glfw.org) [(licensing)](https://www.glfw.org/license.html)
* [GLM](https://github.com/g-truc/glm)  [(licensing)](https://github.com/g-truc/glm/blob/master/copying.txt)
* [ImGui](https://www.dearimgui.com/)  [(licensing)](https://github.com/ocornut/imgui/blob/master/LICENSE.txt)
* [OpenCV](https://opencv.org/) [(licensing for 4.8.0)](https://opencv.org/license/)
