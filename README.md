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

Documentation can be found [here](http://iut-ibk.github.io/DynaMind-ToolBox)
## Install on Mac with Homebrew

```
brew tap christianurich/dynamind
brew install dynamind
```

## Compile DynaMind
- [Compiling-DynaMind-Toolbox-on-Windows](https://github.com/iut-ibk/DynaMind-ToolBox/wiki/Compiling-DynaMind-Toolbox-on-Windows)
- [Compiling-DynaMind-Toolbox-on-Linux](https://github.com/iut-ibk/DynaMind-ToolBox/wiki/Compiling-DynaMind-Toolbox-on-Linux)



