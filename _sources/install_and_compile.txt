===============
Install and Run
===============

Windows
=======


- `DynaMind-Toolbox-0.8.0-win32 (GDAL only) <https://www.dropbox.com/s/daq4tskt59ucvdm/DynaMind-Toolbox-0.8.0-win32.exe?dl=0>`_
- `DynaMind-Toolbox-0.8.0-win32 (includes DynaVIBe) <https://www.dropbox.com/s/1ccsb6sjr31ij2y/DynaMind-Toolbox_legacy-0.8.0-win32.exe?dl=0>`_


Old Release
-----------

- `DynaMind-Toolbox-0.8.0-beta.4-win32 (GDAL only) <https://www.dropbox.com/s/frwnpwx2lae494j/DynaMind-Toolbox-0.8.0-beta.4_1-win32.exe?dl=0>`_
- `DynaMind-Toolbox-0.8.0-beta.4-win32 (includes DynaVIBe) <https://www.dropbox.com/s/ae9a6vakc8re35w/DynaMind-Toolbox_legacy-0.8.0-beta.4_1-win32.exe?dl=0>`_


OSX
===

For OSX the DynaMind-ToolBox can be installed with the highly recommended `Homebrew <http://brew.sh>`_ build system


.. code-block:: bash

    brew tap christianurich/dynamind
    brew install dynamind
..


To start DynaMind open a console and run ``dynamind`` or ``dynamind-gui``


Linux (Ubuntu)
==============

.. code-block:: bash

    sudo apt-add-repository -y ppa:ubuntugis/ubuntugis-unstable
    sudo apt-get update
    sudo apt-get install build-essential cmake git swig qt4-dev-tools python-dev python-pip libgdal1-dev libcgal-dev libqglviewer-dev libboost-system-dev libboost-graph-dev python-numpy python-scipy python-gdal python-matplotlib python-netcdf libboost-all-dev

    sudo pip install reimport
    sudo pip install netCDF4
    sudo pip install reimport
    sudo pip install sphinx
    sudo pip install sphinx_rtd_theme


    #install SFCGAL
    wget https://github.com/Oslandia/SFCGAL/archive/v1.0.5.zip
    unzip v1.0.5.zip
    cd SFCGAL-1.0.5/
    cmake . && make && sudo make install
    cd ..
    #rm -rf SFCGAL-1.0/
    rm v1.0.5.zip

    git clone git://github.com/iut-ibk/DynaMind-ToolBox.git

    cd DynaMind-ToolBox/


    git submodule update --init --recursive

    mkdir build
    
    cd build
    
    cmake ../ -DWITH_PLUGIN_GDALMODULE=TRUE -DWITH_DOC=True -DWITH_PLUGIN_PERFORMANCE_ASSESSMENT=True -DWITH_PLUGIN_GDALDRAINAGE=True
    
    make
    
    make install

..


To start DynaMind open a console and run ``dynamind`` or ``dynamind-gui``




