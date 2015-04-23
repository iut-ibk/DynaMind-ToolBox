===============
Install and Run
===============


OSX
===

For OSX the DynaMind-ToolBox can be installed with the highly recommended `Homebrew <http://brew.sh>`_ build system


.. code-block:: bash

    brew tap christianurich/dynamind
    brew install dynamind
..


To start DynaMind open a console and run `dynamind`_ or `dynamind-gui`_


Linux (Ubuntu)
==============

.. code-block:: bash

    sudo apt-add-repository -y ppa:ubuntugis/ubuntugis-unstable
    sudo apt-get update
    sudo apt-get install build-essential cmake git swig qt4-dev-tools python-dev python-pip libgdal1-dev libcgal-dev libqglviewer-dev libboost-system-dev libboost-graph-dev python-numpy python-scipy python-gdal python-matplotlib python-netcdf

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


    cmake . -DWITH_PLUGIN_GDALMODULE=TRUE -DWITH_DOC=True && make && make install

..


To start DynaMind open a console and run ``dynamind`` or ``dynamind-gui``