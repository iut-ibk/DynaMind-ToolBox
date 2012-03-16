//
// C++ Interface: csg_s_def
//
// Description: 
//
//
// Author: Christian Urich <christian@christian-laptop>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CSG_SCSG_S_DEF_H
#define CSG_SCSG_S_DEF_H

namespace csg_s {

/**
	@author Christian Urich <christian@christian-laptop>
*/
enum var
{
	ALL,
	FIELD,
	SEWER,
	SEWERWITHSEGMENT,
	ALIVE,
	DEATH_BY_GRADIENT,
	DEATH_BY_MAPLIMIT,
	DEATH_BY_SEWER,
	DEATH_BY_RIVER,
	DEATH_BY_ERROR,
	DEATH_BY_HEIGHT,
	DEATH_BY_CULVERT,
        DEATH_BY_LENGHT,
        FIELD_EROSION,
        FIELD_REPLACE,
        FIELD_FUNCTION_U,
        FIELD_FUNCTION_V,
        LINK_SEWER,
        LINK_OUTFALL,
        STORAGE
};

enum layer
{
    TOPO_ELEVATION,
    TOPO_STATUS,
    INIT_SEWER_NETWORK,
    POP_EQUIVALENT,
    LANDUSE,
    AREA,
    PERCIMPAREA,

    WWTP,
    ENVIRONMENT,
    BUFFER,


    SEWER_H,
    SEWER_G0,
    SEWER_F_G0,
    SEWER_F_G3,
    SEWER_G4,
    CSG_S,    

    NUMBER_OF_LAYER    


};

}

#endif
