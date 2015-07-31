[![Build Status](https://travis-ci.org/iut-ibk/DynaMind-ToolBox.svg?branch=master)](https://travis-ci.org/iut-ibk/DynaMind-ToolBox)
DynaMind-ToolBox 
================

DynaMind is an open-source GIS modelling toolbox similar to the ArcGIS - model builder. The idea is that small reusable modules can be linked together to describe a complex workflow in an urban environment. E.g. the evolution of a small city

<a href="http://htmlpreview.github.io/?https://github.com/christianurich/DynaMind-Analysis/blob/master/viewer/viewer.html"><img src="https://github.com/christianurich/DynaMind-Analysis/blob/master/viewer.png?raw=true" border="0"></a> 

The heart of the DynaMind-Toolbox is [DynaMind](https://github.com/iut-ibk/DynaMind) a small efficient core written in C++. The core provides easy to use interfaces to develop new modules in C++ or Python and to access the data in the data stream. 

The DyanMind-Toolbox comes with:


- a model build to set up and edit simulations

<img src="https://github.com/iut-ibk/DynaMind-Gui/blob/master/doc/images/gui_with_view.png?raw=true" border="0">

- a set of modules to import, edit, create and export data
- an easy to use Python interface to create new simulation or build new modules.

## [Documentation](http://iut-ibk.github.io/DynaMind-ToolBox)

## Windows Installer
- [DynaMind-Toolbox-0.8.0-beta.4-win32 (includes DynaVIBe)](https://www.dropbox.com/s/ae9a6vakc8re35w/DynaMind-Toolbox_legacy-0.8.0-beta.4_1-win32.exe?dl=0)
- [DynaMind-Toolbox-0.8.0-beta.4-win32 (GDAL only)] (https://www.dropbox.com/s/frwnpwx2lae494j/DynaMind-Toolbox-0.8.0-beta.4_1-win32.exe?dl=0)

## Install on Mac using the Dynamind [tap](https://github.com/christianurich/homebrew-dynamind)

```
brew tap christianurich/dynamind
brew install dynamind --HEAD
```

## [Compile DynaMind](http://iut-ibk.github.io/DynaMind-ToolBox/install_and_compile.html#windows)



