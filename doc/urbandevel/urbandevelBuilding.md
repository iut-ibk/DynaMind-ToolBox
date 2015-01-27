*Author:* Christian Mikovits  
*Version:* 0.1

# urbandevelBuilding

## Overview

This module generates buildings on existing parcels.

## Description

The buildings (FACE) are created on existing parcels (FACE).
It consists of the following options to be defined in the module configuration:
- on Signal \[BOOL\]: if TRUE  at a given 'ratio' [0-1] defined in the module settings.

## Usage

### Options

- \'on Signal\': BOOLEAN \[0/1\]  
	built houses only if indicated by PARCEL.status equals the needed buildingtype which is defined by the SUPERBLOCK
- \'Parameters from City\': BOOLEAN \[0/1\]  
	defines if the parameters (description below) should be taken from the CITY view.
- \'create Population\': BOOLEAN \[0/1\]  
	should population values be taken from CITY.cyclepopdiff and distributed
- \'offset\': DOUBLE \[0.1-0.9\]
- \'stories\': INT \[>1\]
- \'year\': INT
- \'space per person\': INT
- \'check type (ignore if empty)\': STRING

### Input

Needed Views:

- CITY
	- cyclepopdiff (optional)
	- required_space (optional)
	- year (optional)
	- offset (optional)

- PARCEL
	- status

- BUILDING (optional)

### Output

- PARCEL
	- status
	- BUILDING (link)

- BUILDING
	- centroid_x
	- centroid_y
	- year
	- stories
	- A_roof
	- Aeff_roof
	- A_road
	- Aeff_road
	- A_imp
	- Aeff_imp
	- PARCEL (link)

The user has to provide 

## Example
