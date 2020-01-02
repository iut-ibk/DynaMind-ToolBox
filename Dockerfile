FROM ubuntu:18.04
MAINTAINER Christian Urich <christian.urich@monash.edu>

ENV PYTHONUNBUFFERED 1
ENV DEBIAN_FRONTEND=noninteractive 

RUN apt-get update && apt-get -y -q install wget unzip software-properties-common

RUN add-apt-repository ppa:ubuntugis/ppa -y

RUN apt-get -y update && apt-get -y install build-essential cmake git swig python3-dev python3-pip libgdal-dev libcgal-dev libboost-system-dev libboost-graph-dev python3-numpy python3-scipy python3-gdal python3-matplotlib libboost-all-dev libsfcgal-dev libqt5webkit5-dev

RUN apt-get -y update && apt-get -y install libcgal-dev build-essential libssl-dev libffi-dev python3-dev libsfcgal-dev libspatialite-dev libsqlite3-mod-spatialite


RUN useradd -ms /bin/bash node

USER root

# needed for gdal_wrap compile
ENV CPLUS_INCLUDE_PATH /usr/local/include/gdal
ENV C_INCLUDE_PATH /usr/local/include/gdal


RUN pip3 install netCDF4

RUN	pip3 install reimport

RUN pip3 install paramiko

RUN pip3 install dropbox

RUN pip3 install psycopg2

WORKDIR /home/node/

USER root

RUN pip3 install richdem

RUN pip3 install pyswmm

RUN pip3 install compiler

WORKDIR /home/node/

#RUN git clone --progress --verbose -b release/2.4  https://github.com/OSGeo/gdal.git
#WORKDIR /home/node/gdal/gdal/
#RUN	./configure --with-threads --disable-static --without-grass --with-jasper --with-pg --with-static-proj4 --with-curl 
#--with-spatialite --with-python
#RUN make -j 4 && make install


WORKDIR /home/node

RUN apt-get -y update && apt-get -y install default-jdk 

USER node

RUN git clone https://christianurich@bitbucket.org/mothlight/target_java.git

RUN	mkdir DynaMind-ToolBox

ADD . DynaMind-ToolBox


WORKDIR /home/node/DynaMind-ToolBox/

RUN	mkdir build

WORKDIR /home/node/DynaMind-ToolBox/build/

RUN cmake  -DWITH_GUI=FALSE -DWITH_PLUGIN_PERFORMANCE_ASSESSMENT=TRUE -DWITH_PLUGIN_GDALMODULE=TRUE  -DWITH_PLUGIN_GDALDRAINAGE=TRUE -DWITH_UNIT_TESTS=TRUE ..

RUN	make -j 4

ENV  PYTHONPATH {$PYTHONPATH}:/usr/local/lib

WORKDIR /home/node/DynaMind-ToolBox/build/output
RUN ./unit-test
# RUN ./unit-test_WaterBalance
# RUN ./unit-test_gdalmodules
