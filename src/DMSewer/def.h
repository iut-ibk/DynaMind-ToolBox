#include "dmcompilersettings.h"

#ifndef __def
#define __def

#include <iostream>
#include <QString>


#define PI 3.141592654;
#define ON 1;
#define OFF 0;

namespace DM{

enum DM_HELPER_DLL_EXPORT elayers
{
	/*
	topoStatus:
		defines water and land cells
			mainRiver: 10
			minorRiver: 9
			meanderArea: 6
			else: 0

	topoElevation:
		digital Elevation map
		in (m)

	topoGradProb:
		slope < 20%
		fabs(20%)<slope: 0
		0%: 1
		between 0-fabs(20) linear interpolation

	cityCenter:
		cityCenter : 1
		close to cityCenter >0.001
		between exponent function

	RoadNetwork:
		status layer
		for main road: 10
		else: 0

	RoadConnectivity:
		Connectivity for main road
		for main road: 1
		distance to main road
		exponent function for distance

	WWTPlant
		for WasteWaterTreatmentPlant : 1
		exponent function for distance

	SewerNetwork:
		status layer
		for main road: 10
		else: 0

	SewerConnectivity:
		Connectivity for main sewer
		for  main sewer: 1
		distance to sewer road
		exponent function for distance

	Landuse:
		enum elandUse

	Population:
		population density for cell
		hotspots
		persons per hectar
		maxVal=150 EW/ha

	PopulationConnectivity
		population density for cell
		exponet function for hotspots
		persons per hectar
		maxVal=150 EW/ha

	Industry:

	Agent01:

	Agent02:
		erosion agent

	Agent03:

	Agent04:

	MaxDataLAyer:
		draw entire City

	*/
	topology,
		topoStatus,
		topoElevation,
		topoGradient,
		topoGradProb,
	initalCity,
		cityCenters,
		initRoadNetwork,
		initRoadConnectivity,
		WWTPlant,
		initSewerNetwork,
		initSewerConnectivity,
		initLandUse,
		initPopulation,
		initPopConnectivity,
		initIndustry,
		initWaterDemand,
	city,
		RoadNetwork,
		RoadConnectivity,
		SewerNetwork,
		SewerConnectivity,
		LandUse,
		SumPE,
		PercImpArea,
		DryWeatherFlow,
	population,
		Population,
		PopConnectivity,
		PopRoadAccess,
		PopEquivalent,
	commercial,
		Commercial,
		ComPopEquivalent,
	industry,
		Industry,
		IndPopEquivalent,
	waterdistribution,
		PopDemand,
		ComDemand,
		IndDemand,
	agents,
		Agent01,
		Agent02,
		Agent03,
		Agent04,
		Agent_LEA,
		Agent_LEAmod,
	csg,
		//DataLayer For CSG_Sewer
		Csg_s,
		csg_s_gradientField,
		csg_s_runOffModel,
		csg_s_sewer,
		csg_s_WWTPlant,
		csg_s_testField,
		csg_s_Sewer_R,
		csg_s_Sewer_R_Depth,	
		csg_s_Sewer_R_Depth_1,
		csg_s_Sewer_R_PlayField,
		csg_s_Buffer,
		Sewer_G0,
		Sewer_F_G0,
		Sewer_G1,
		Sewer_F_G1,
		Sewer_G2,
		Sewer_F_G2,
		Sewer_G3,
		Sewer_F_G3,
		Sewer_G4,
		Sewer_F_G4,
                sewer_h,
		InitSewer,
		MAX_dataLayer

};
//static QVector<int> topLayerPos={0,5,13,16,18,23};
static const QString slayers[]=
{	"topology",
		"topoStatus",
		"topoElevation",
		"topoGradient",
		"topoGradProb",
	"initalCity",
		"cityCenters",
		"initRoadNetwork",
		"initRoadConnectivity",
		"WWTPlant",
		"initSewerNetwork",
		"initSewerConnectivity",
		"initLandUse",
		"initPopulation",
		"initPopConnectivity",
		"initIndustry",
		"initWaterDemand",
	"city",
		"RoadNetwork",
		"RoadConnectivity",
		"SewerNetwork",
		"SewerConnectivity",
		"LandUse",
		"SumPE",
		"PercImpArea",
		"DryWeatherFlow",
	"population",
		"Population",
		"PopConnectivity",
		"PopRoadAccess",
		"PopEquivalent",
	"commercial",
		"Commercial",
		"ComPopEquivalent",
	"industry",
		"Industry",
		"IndPopEquivalent",
	"waterdistribution",
		"PopDemand",
		"ComDemand",
		"IndDemand",
	"agents",
		"Agent01",
		"Agent02",
		"Agent03",
		"Agent04",
		"Agent_LEA",
		"Agent_LEAmod",
	"csg",
		//DataLayer For CSG_Sewer
		"Csg_s",
		"csg_s_gradientField",
		"csg_s_runOffModel",
		"csg_s_sewer",
		"csg_s_WWTPlant",
		"csg_s_testField",
		"csg_s_Sewer_R",
		"csg_s_Sewer_R_Depth",	
		"csg_s_Sewer_R_Depth_1",
		"csg_s_Sewer_R_PlayField",	
		"csg_s_Buffer",
		"Sewer_G0",
		"Sewer_F_G0",
		"Sewer_G1",
		"Sewer_F_G1",
		"Sewer_G2",
		"Sewer_F_G2",
		"Sewer_G3",
		"Sewer_F_G3",
		"Sewer_G4",
		"Sewer_F_G4",
                "sewer_h",
		"InitSewer",
		"MAX_dataLayer"
};
/*enum DM_HELPER_DLL_EXPORT elandUse
{
	//LEVEL 1
		//LEVEL 2
			//LEVEL 3
	//-------------------------//
	ArtificialSurfaces,				//1.
		UrbanFabric,				 //1.1
			ContUrbanFabric,		  //1.1.1
			DisContUrbanFabric,		  //1.1.2
		IndustrialCommercialTrans,	 //1.2
			IndustrialCommercial,	  //1.2.1
			RoadRailNetwork,		  //1.2.2
		MineDumpConstruction,		 //1.3
		ArtificialVegetated,		 //1.4
	NonArtificialSurface,			//Control
	//-------------------------//

	//-------------------------//
	AgriculturalAreas,				//2.
		ArableLand,					 //2.1
		PermanentCrops,				 //2.2
		Pastures,					 //2.3
		HetAgriculture,				 //2.4
	NonAgriculturalAreas,			//ControlValue
	//-------------------------//

	//-------------------------//
	ForestsSemiNatural,				//3.
		Forests,					 //3.1
		Shrub,						 //3.2
		littleVeg,					 //3.3
	NonForest,						//ControlValue
	//-------------------------//

	//-------------------------//
	Wetlands,						//4.
		InlandWetLand,				 //4.1
		CoastalWetLand,				 //4.2
	NoWetlands,						//ControlValue
	//-------------------------//

	//-------------------------//
	WaterBodies,					//5.
		InlandWaters,				 //5.1
		MarineWaters,				 //5.2
	NoWaterBodies,					//ControlValue
	//-------------------------//
	LandUseEnd


};*/
};





//BIND_ENUM(elayers)
//{
//	{topoStatus, "topoStatus"},
//	{topoElevation, "topoElevation"},
//	{Population, "Population"},
//	{Industry, "Industry"},
//	{Agent01, "Agent01"},
//	{MAX_dataLayer, "Maximum"}
//
//};




#endif
