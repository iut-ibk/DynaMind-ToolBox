#include "microclimateassessment.h"
#include "math.h"

#include <iostream>
#include <fstream>
#include <string>
#include <QFile>
#include <QTextStream>
#include <QStringList>
//#include <QFileDialog>
#include <QSettings>
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>


DM_DECLARE_CUSTOM_NODE_NAME(MicroClimateAssessment,"Microclimate Extreme Heat", "Scenario Simulation and Assessment")
MicroClimateAssessment::MicroClimateAssessment()
{
	gridsize = 30;
	percentile = 80;
	mapPic = "";
	shapefile = "";
	landuse = "";
	techFile = "";
	QSettings settings;
	workingDir = settings.value("workPath").toString().toStdString();

	this->addParameter("Gridsize", DM::INT, &this->gridsize);
	this->addParameter("MapPic", DM::STRING, &this->mapPic);
	this->addParameter("Shapefile",DM::STRING,&this->shapefile);
	this->addParameter("Landuse",DM::STRING, &this->landuse);
	this->addParameter("Percentile",DM::INT, &this->percentile);
	this->addParameter("Techfile",DM::STRING, &this->techFile);

}

void MicroClimateAssessment::init()
{
	DM::View raster("Imp",DM::RASTERDATA,DM::READ);
	DM::View MCDs("MCDs",DM::COMPONENT,DM::READ);
	std::vector<DM::View> vdata;
	vdata.push_back(raster);
	vdata.push_back(MCDs);
	this->addData("City", vdata);
}

void MicroClimateAssessment::run()
{
	QSettings settings;
	workingDir = settings.value("workPath").toString().toStdString();

	//DM::View topo("Topology", DM::FACE, DM::READ);
	DM::View raster("Imp",DM::RASTERDATA,DM::READ);
	DM::View MCDs("MCDs",DM::COMPONENT,DM::READ);
	DM::System * data = this->getData("City");
	DM::RasterData * tmpimp = this->getRasterData("City",raster);
	DM::RasterData * imp = new DM::RasterData(tmpimp->getWidth(),tmpimp->getHeight(),0,0,tmpimp->getXOffset(),tmpimp->getYOffset());
	imp->setSize(tmpimp->getWidth(),tmpimp->getHeight(),tmpimp->getCellSizeX(),tmpimp->getCellSizeY(),tmpimp->getXOffset(),tmpimp->getYOffset());
	std::map<std::string, DM::Component*> cmps; //= data->getAllComponentsInView(MCDs);
	std::cout << cmps.size() << std::endl;
	DM::Component * comp = cmps.begin()->second;
	bool useMCD;
	if(comp->getAttribute("useMCD")->getString() == "false")
		useMCD = false;
	else
		useMCD = true;
	QList<QList<double> > input = readWsud(QString(this->workingDir.c_str()) + "/" + QString(comp->getAttribute("MCDFilename")->getString().c_str()));

	if(!useMCD)
	{
		for(int i = 0;i<imp->getHeight();i++)
		{
			for(int j = 0;j<imp->getWidth();j++)
			{
				imp->setCell(j,i,tmpimp->getCell(j,imp->getHeight()-1-i));
			}
		}
	}

	//getting x and y edges of the shapefile
	//with that information and the cellsize we can build the grid
	std::map<std::string,DM::Node*> cmp; //= data->getAllNodes();
	std::cout << "size: " << cmp.size() << endl;


	//topology map data
	/*
	for(map<std::string,DM::Node*>::iterator ii=cmp.begin(); ii!=cmp.end(); ++ii)
	{
		std::cout << (*ii).first << endl;
		DM::Node * n = (*ii).second;
		if(first)
		{
			Xmax = n->getX();
			Xmin = n->getX();
			Ymax = n->getY();
			Ymin = n->getY();
			first = false;
		}
		std::cout <<"max XY " << Xmax << " " << Ymax << endl;
		std::cout <<"min XY " << Xmin << " " << Ymin << endl;
		if(Xmax < x)
			Xmax = x;
		if(Xmin > x)
			Xmin = x;
		if(Ymax < y)
			Ymax = y;
		if(Ymin > y)
			Ymin = y;
		std::cout <<"nodeXY "<< n->getX() << " " << n->getY() << endl;


	}
	std::cout <<"max XY " << Xmax << " " << Ymax << endl;
	std::cout <<"min XY " << Xmin << " " << Ymin << endl;
	//topology map data
	*/


	// getting rasterdata values

	double impwidth = imp->getWidth();
	double impheight = imp->getHeight();
	double imptotalwidth = imp->getCellSizeX() * impwidth;
	double imptotalheight = imp->getCellSizeX() * impheight;


	//calculating values for our new grid
	double height = (double)imptotalheight / gridsize;
	double width = (double)imptotalwidth / gridsize;
	if (height - (int)height != 0)
	{
		height = (int) height + 1;
	}
	if(width - (int) width != 0)
	{
		width = (int)width + 1;
	}
	std::cout << "width " << width << endl;
	std::cout << "height " << height << endl;
	DM::RasterData * newgrid = new DM::RasterData(width,height,gridsize,gridsize,imp->getXOffset(),imp->getYOffset());
	newgrid->setSize(width,height,gridsize,gridsize,imp->getXOffset(),imp->getYOffset());
	fillZeros(newgrid);
	DM::RasterData * lst = new DM::RasterData(width,height,gridsize,gridsize,imp->getXOffset(),imp->getYOffset());
	lst->setSize(width,height,gridsize,gridsize,imp->getXOffset(),imp->getYOffset());
	fillZeros(lst);
	DM::RasterData * newlst = new DM::RasterData(width,height,gridsize,gridsize,imp->getXOffset(),imp->getYOffset());
	newlst->setSize(width,height,gridsize,gridsize,imp->getXOffset(),imp->getYOffset());
	fillZeros(newlst);
	DM::RasterData * lstReduction = new DM::RasterData(width,height,gridsize,gridsize,imp->getXOffset(),imp->getYOffset());
	lstReduction->setSize(width,height,gridsize,gridsize,imp->getXOffset(),imp->getYOffset());
	fillZeros(lstReduction);
	DM::RasterData * lstReductionAir = new DM::RasterData(width,height,gridsize,gridsize,imp->getXOffset(),imp->getYOffset());
	lstReductionAir->setSize(width,height,gridsize,gridsize,imp->getXOffset(),imp->getYOffset());
	fillZeros(lstReductionAir);
	DM::RasterData * lstAfterWsud = new DM::RasterData(width,height,gridsize,gridsize,imp->getXOffset(),imp->getYOffset());
	lstAfterWsud->setSize(width,height,gridsize,gridsize,imp->getXOffset(),imp->getYOffset());
	fillZeros(lstAfterWsud);
	DM::RasterData * impAreabeforeWSUD = new DM::RasterData(width,height,gridsize,gridsize,imp->getXOffset(),imp->getYOffset());
	impAreabeforeWSUD->setSize(width,height,gridsize,gridsize,imp->getXOffset(),imp->getYOffset());
	fillZeros(impAreabeforeWSUD);
	DM::RasterData * perAreabeforeWSUD = new DM::RasterData(width,height,gridsize,gridsize,imp->getXOffset(),imp->getYOffset());
	perAreabeforeWSUD->setSize(width,height,gridsize,gridsize,imp->getXOffset(),imp->getYOffset());
	fillZeros(perAreabeforeWSUD);
	DM::RasterData * newPervArea = new DM::RasterData(width,height,gridsize,gridsize,imp->getXOffset(),imp->getYOffset());
	newPervArea->setSize(width,height,gridsize,gridsize,imp->getXOffset(),imp->getYOffset());
	fillZeros(newPervArea);
	DM::RasterData * newPervFrac = new DM::RasterData(width,height,gridsize,gridsize,imp->getXOffset(),imp->getYOffset());
	newPervFrac->setSize(width,height,gridsize,gridsize,imp->getXOffset(),imp->getYOffset());
	fillZeros(newPervFrac);
	DM::RasterData * newImpPervArea = new DM::RasterData(width,height,gridsize,gridsize,imp->getXOffset(),imp->getYOffset());
	newImpPervArea->setSize(width,height,gridsize,gridsize,imp->getXOffset(),imp->getYOffset());
	fillZeros(newImpPervArea);
	DM::RasterData * newImpPervFrac = new DM::RasterData(width,height,gridsize,gridsize,imp->getXOffset(),imp->getYOffset());
	newImpPervFrac->setSize(width,height,gridsize,gridsize,imp->getXOffset(),imp->getYOffset());
	fillZeros(newImpPervFrac);
	DM::RasterData * testgrid = new DM::RasterData(width,height,gridsize,gridsize,imp->getXOffset(),imp->getYOffset());
	testgrid->setSize(width,height,gridsize,gridsize,imp->getXOffset(),imp->getYOffset());
	fillZeros(testgrid);

	double treeCover,waterCover,pondCover,wetlandCover,nonIrrGrassCover,swaleCover,grassCover,bioCover,infilCover,greenRoofCover,greenWallCover,roofCover,roadCover,pavementCover,concreteCover;
	double totalcounter,coverCounter;
	QList<double> wsudline;


	DM::RasterData * techs = readRasterFile(QString(workingDir.c_str()) + QString("/") + QString(this->techFile.c_str()));



	double tree,water,grass,irrGrass,roof,road,concrete;
	//variables for temp calculation
	//variables for tech % covering
	QList<QList<double> > readTechs;
	QList<QList<double> >WsudTech = readWsud(QString(this->workingDir.c_str()) + QString("/WSUDtech.mcd"));
	bool wsudempty;
	if(WsudTech.isEmpty())
		wsudempty = true;
	else
		wsudempty = false;

	// two for loops over the rasterdata array
	for(int i = 0;i<height;i++)
	{
		for(int j =0; j<width; j++)
		{

			//variables for temp calculation
			tree = 0;
			water = 0;
			grass = 0;
			irrGrass = 0;
			roof = 0;
			road = 0;
			concrete = 0;

			//variables for tech % covering
			treeCover = 0;
			waterCover = 0;
			pondCover = 0;
			wetlandCover = 0;
			nonIrrGrassCover = 0;
			swaleCover = 0;
			grassCover = 0;
			bioCover = 0;
			infilCover = 0;
			greenRoofCover = 0;
			greenWallCover = 0;
			roofCover = 0;
			roadCover = 0;
			pavementCover = 0;
			concreteCover = 0;

			if(!useMCD)
			{
				//get all smaller cells in the new big cell
				vector<QPointF> cells = getCoveringCells(j*gridsize,i*gridsize,gridsize,imp->getCellSizeX());

				//loop through all smaller cells and check what landcover it has
				while(!cells.empty())
				{
					QPointF p = cells.back();
					//std::cout << "input: "<< imp->getCell(p.x(),p.y()) << endl;
					switch((int)imp->getCell(p.x(),p.y()))
					{
						case 1:
							tree++;
							treeCover++;
							break;
						case 2:
							water++;
							waterCover++;
							break;
						case 3:
							water++;
							pondCover++;
							break;
						case 4:
							water++;
							wetlandCover++;
							break;
						case 5:
							grass++;
							nonIrrGrassCover++;
							break;
						case 6:
							grass++;
							swaleCover++;
							break;
						case 7:
							irrGrass++;
							grassCover++;
							break;
						case 8:
							irrGrass++;
							bioCover++;
							break;
						case 9:
							irrGrass++;
							infilCover++;
							break;
						case 10:
							irrGrass++;
							greenRoofCover++;
							break;
						case 11:
							irrGrass++;
							greenWallCover++;
							break;
						case 12:
							roof++;
							roofCover++;
							break;
						case 13:
							road++;
							roadCover++;
							break;
						case 14:
							road++;
							pavementCover++;
							break;
						case 15:
							concrete++;
							concreteCover++;
							break;
					}


					cells.pop_back();
				}
			}
			else
			{
				int linenr = imp->getCellSizeX() * i + j;
				linenr /= gridsize;
				QList<double> line = input[linenr+1];
				//add the new technologies and covers
				tree += line[1];
				treeCover += line[1];
				water += line[2];
				waterCover += line[2];
				water += line[3];
				pondCover += line[3];
				water += line[4];
				wetlandCover += line[4];
				grass += line[5];
				nonIrrGrassCover += line[5];
				grass += line[6];
				swaleCover += line[6];
				irrGrass += line[7];
				grassCover += line[7];
				irrGrass += line[8];
				bioCover += line[8];
				irrGrass += line[9];
				infilCover += line[9];
				irrGrass += line[10];
				greenRoofCover += line[10];
				irrGrass += line[11];
				greenWallCover += line[11];
				roof += line[12];
				roofCover += line[12];
				road += line[13];
				roadCover += line[13];
				road += line[14];
				pavementCover += line[14];
				concrete += line[15];
				concreteCover += line[15];

			}
			//get the total number
			totalcounter = tree + water + grass + irrGrass + roof + road + concrete;

			//calc temp for each technologies
			double treeTemp = (tree / totalcounter) * getTempForSurface(1,this->percentile);
			double waterTemp = (water/ totalcounter) * getTempForSurface(2,this->percentile);
			double grassTemp = (grass / totalcounter) * getTempForSurface(3,this->percentile);
			double irrGrassTemp = (irrGrass / totalcounter) * getTempForSurface(4,this->percentile);
			double roofTemp = (roof / totalcounter) * getTempForSurface(5,this->percentile);
			double roadTemp = (road / totalcounter) * getTempForSurface(6,this->percentile);
			double concreteTemp  = (concrete / totalcounter) * getTempForSurface(7,this->percentile);

			//calc temp of cell according to technologies
			double tmpInCurrentCell = (treeTemp + waterTemp + grassTemp + irrGrassTemp + roofTemp + roadTemp + concreteTemp);
			//save value
			lst->setCell(j,i,tmpInCurrentCell);

			/*
			//convert techcounters to percentages
			tree = tree * 100 / totalcounter;
			water = water * 100 / totalcounter;
			grass = grass * 100 / totalcounter;
			irrGrass = irrGrass * 100 / totalcounter;
			roof = roof * 100 / totalcounter;
			road = road * 100 / totalcounter;
			concrete = concrete * 100 / totalcounter;
			*/

			if(wsudempty)
			{


				if(this->techFile != "")
				{
					//variables for temp calculation
					tree = 0;
					water = 0;
					grass = 0;
					irrGrass = 0;
					roof = 0;
					road = 0;
					concrete = 0;

					treeCover = 0;
					waterCover = 0;
					pondCover = 0;
					wetlandCover = 0;
					nonIrrGrassCover = 0;
					swaleCover = 0;
					grassCover = 0;
					bioCover = 0;
					infilCover = 0;
					greenRoofCover = 0;
					greenWallCover = 0;
					roofCover = 0;
					roadCover = 0;
					pavementCover = 0;
					concreteCover = 0;
					vector<QPointF> cells = getCoveringCells(j*gridsize,i*gridsize,gridsize,techs->getCellSizeX());

					while(!cells.empty())
					{
						QPointF p = cells.back();
						std::cout <<"techfile "<< techs->getCell(p.x(),p.y()) << endl;
						switch((int)techs->getCell(p.x(),p.y()))
						{
						case 1:
							tree++;
							treeCover++;
							break;
						case 2:
							water++;
							waterCover++;
							break;
						case 3:
							water++;
							pondCover++;
							break;
						case 4:
							water++;
							wetlandCover++;
							break;
						case 5:
							grass++;
							nonIrrGrassCover++;
							break;
						case 6:
							grass++;
							swaleCover++;
							break;
						case 7:
							irrGrass++;
							grassCover++;
							break;
						case 8:
							irrGrass++;
							bioCover++;
							break;
						case 9:
							irrGrass++;
							infilCover++;
							break;
						case 10:
							irrGrass++;
							greenRoofCover++;
							break;
						case 11:
							irrGrass++;
							greenWallCover++;
							break;
						case 12:
							roof++;
							roofCover++;
							break;
						case 13:
							road++;
							roadCover++;
							break;
						case 14:
							road++;
							pavementCover++;
							break;
						case 15:
							concrete++;
							concreteCover++;
							break;
						}



						cells.pop_back();
					}
					coverCounter = treeCover + waterCover + pondCover + wetlandCover + nonIrrGrassCover + swaleCover + grassCover
							+ bioCover + infilCover + greenRoofCover + greenWallCover + roofCover + roadCover + pavementCover + concreteCover;
					wsudline.clear();
					wsudline << i*width+j << treeCover *100/coverCounter << waterCover*100/coverCounter << pondCover*100/coverCounter << wetlandCover*100/coverCounter
							 << nonIrrGrassCover*100/coverCounter << swaleCover*100/coverCounter << grassCover*100/coverCounter << bioCover*100/coverCounter
							 << infilCover*100/coverCounter << greenRoofCover*100/coverCounter << greenWallCover*100/coverCounter << roofCover*100/coverCounter
							 << roadCover*100/coverCounter << pavementCover*100/coverCounter << concreteCover*100/coverCounter;
					WsudTech.append(wsudline);
					wsudline.clear();

				}
			}else
			{

				wsudline = getTechAreasForCell(i,j,width,WsudTech);

				tree=0;water=0;grass=0;irrGrass=0;roof=0;road=0;concrete=0;
				treeCover = 0;waterCover = 0;pondCover = 0;wetlandCover = 0;nonIrrGrassCover = 0;swaleCover = 0;grassCover = 0;bioCover = 0;infilCover = 0;greenRoofCover = 0;greenWallCover = 0;roofCover = 0;roadCover = 0;pavementCover = 0;concreteCover = 0;

				//add the new technologies and covers
				tree += wsudline[1];
				treeCover += wsudline[1];
				water += wsudline[2];
				waterCover += wsudline[2];
				water += wsudline[3];
				pondCover += wsudline[3];
				water += wsudline[4];
				wetlandCover += wsudline[4];
				grass += wsudline[5];
				nonIrrGrassCover += wsudline[5];
				grass += wsudline[6];
				swaleCover += wsudline[6];
				irrGrass += wsudline[7];
				grassCover += wsudline[7];
				irrGrass += wsudline[8];
				bioCover += wsudline[8];
				irrGrass += wsudline[9];
				infilCover += wsudline[9];
				irrGrass += wsudline[10];
				greenRoofCover += wsudline[10];
				irrGrass += wsudline[11];
				greenWallCover += wsudline[11];
				roof += wsudline[12];
				roofCover += wsudline[12];
				road += wsudline[13];
				roadCover += wsudline[13];
				road += wsudline[14];
				pavementCover += wsudline[14];
				concrete += wsudline[15];
				concreteCover += wsudline[15];

			}
			// recalc total number
			totalcounter = tree + water + grass + irrGrass + roof + road + concrete;

			//recalc temperatures
			treeTemp = (tree / totalcounter) * getTempForSurface(1,this->percentile);
			waterTemp = (water/ totalcounter) * getTempForSurface(2,this->percentile);
			grassTemp = (grass / totalcounter) * getTempForSurface(3,this->percentile);
			irrGrassTemp = (irrGrass / totalcounter) * getTempForSurface(4,this->percentile);
			roofTemp = (roof / totalcounter) * getTempForSurface(5,this->percentile);
			roadTemp = (road / totalcounter) * getTempForSurface(6,this->percentile);
			concreteTemp  = (concrete / totalcounter) * getTempForSurface(7,this->percentile);

			if(totalcounter != 0)
			{
			//recalc temp of cell
			tmpInCurrentCell = (treeTemp + waterTemp + grassTemp + irrGrassTemp + roofTemp + roadTemp + concreteTemp);
			//save value
			lstAfterWsud->setCell(j,i,tmpInCurrentCell);

			//calc temp reduction
			double reduction = lst->getCell(j,i) - lstAfterWsud->getCell(j,i);
			reduction = QString::number(reduction,'f',2).toDouble();  // apparently the best way to round doubles ....
			lstReduction->setCell(j,i,reduction);
			}
			//calc percentages for cover
			coverCounter = treeCover + waterCover + pondCover + wetlandCover + nonIrrGrassCover + swaleCover + grassCover
					+ bioCover + infilCover + greenRoofCover + greenWallCover + roofCover + roadCover + pavementCover + concreteCover;
			wsudline.clear();
			if(coverCounter == 0)
			{
				wsudline << i*width+j << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0;
			}
			else
			{
				wsudline << i*width+j << treeCover *100/coverCounter << waterCover*100/coverCounter << pondCover*100/coverCounter << wetlandCover*100/coverCounter
						 << nonIrrGrassCover*100/coverCounter << swaleCover*100/coverCounter << grassCover*100/coverCounter << bioCover*100/coverCounter
						 << infilCover*100/coverCounter << greenRoofCover*100/coverCounter << greenWallCover*100/coverCounter << roofCover*100/coverCounter
						 << roadCover*100/coverCounter << pavementCover*100/coverCounter << concreteCover*100/coverCounter;
			}
			readTechs.append(wsudline);
			wsudline.clear();

		}
	}
	writeTechs(readTechs);
	/*
	lstReductionAir = calcReductionAirTemp(lstReductionAir);
	std::cout << "NEWGRID:" << endl;
	printRaster(newgrid);

	std::cout << "LST" << endl;
	printRaster(lst);

	std::cout << "imp area before wsud" << endl;
	printRaster(impAreabeforeWSUD);
	std::cout << "new perv Frac" << endl;
	printRaster(newPervFrac);
	std::cout << "new LST" << endl;
	printRaster(newlst);
	std::cout << "lstAFTER" << endl;
	printRaster(lstAfterWsud);
	std::cout << "lstReduction" << endl;
	printRaster(lstReduction);
	std::cout << "lstReductionAIR" << endl;
	printRaster(lstReductionAir);
	exportRasterData(imp,QString::fromStdString(workingDir)+"/Grid.txt");
	*/
	exportRasterData(lst,QString::fromStdString(workingDir)+"/LST before WSUD.txt",1);
	exportMCtemp(lst,QString::fromStdString(workingDir)+"/LST before WSUD.mcd",1);

	exportRasterData(lstAfterWsud,QString::fromStdString(workingDir)+"/LST after WSUD.txt",1);
	exportMCtemp(lstAfterWsud,QString::fromStdString(workingDir)+"/LST after WSUD.mcd",1);

	exportMCtemp(lstReduction,QString::fromStdString(workingDir)+"/Reduction in LST.mcd",-1);
	exportRasterData(lstReduction,QString::fromStdString(workingDir)+"/Reduction in LST.TXT",-1);

	/*exportRasterData(lstReductionAir,QString::fromStdString(workingDir)+"/Reduction in Air Temperature.txt");
	exportMCtemp(lstReductionAir,QString::fromStdString(workingDir)+"/Reduction in Air Temperature.mcd",1);
	*/
}

void MicroClimateAssessment::printRaster(DM::RasterData * r)
{

	for(int i = 0;i<r->getHeight();i++)
	{
		for(int j = 0; j<r->getWidth();j++)
		{
			std::cout<<r->getCell(j,i)<< "\t\t";
		}
		std::cout<<endl;
	}
}

void MicroClimateAssessment::fillZeros(DM::RasterData * r)
{
	for(int i = 0;i<r->getWidth();i++)
	{
		for(int j = 0; j<r->getHeight();j++)
		{
			r->setCell(i,j,0);
		}
	}
}

double MicroClimateAssessment::chooseTab(double perc)
{
	QList<QList<double> > tif02;
	QList<double> zeile1,zeile2;
	zeile1 << 27.59 << 28.54 << 29.49 << 30.44 << 31.40 << 32.36;
	zeile2 <<  1.52 <<  6.99 << 37.36 << 88.70 << 99.33 << 99.94;
	tif02 << zeile1 << zeile2;

	QList<QList<double> > tif03;
	zeile1.clear();zeile2.clear();
	zeile1 << 27.99 << 28.76 << 29.53 << 30.30 << 31.07 << 31.85 << 32.62;
	zeile2 <<  0.58 <<  2.92 << 15.49 << 48.22 << 92.35 << 99.36 << 99.95;
	tif03 << zeile1 << zeile2;

	QList<QList<double> > tif04;
	zeile1.clear();zeile2.clear();
	zeile1 << 28.61 << 29.24 << 29.87 << 30.51 << 31.15 << 31.78 << 32.41;
	zeile2 <<  0.91 <<  5.17 << 16.71 << 43.75 << 85.68 << 98.75 << 99.96;
	tif04 << zeile1 << zeile2;

	QList<QList<double> > tif05;
	zeile1.clear();zeile2.clear();
	zeile1 << 28.50 << 29.23 << 29.96 << 30.71 << 31.45 << 32.18 << 32.92;
	zeile2 <<  0.59 <<  2.67 << 13.35 << 43.91 << 90.48 << 99.38 << 99.98;
	tif05 << zeile1 << zeile2;

	QList<QList<double> > tif06;
	zeile1.clear();zeile2.clear();
	zeile1 << 29.53 << 30.30 << 31.10 << 31.90 << 32.68;
	zeile2 <<  2.01 << 15.18 << 57.01 << 95.11 << 99.98;
	tif06 << zeile1 << zeile2;

	QList<QList<double> > tif07;
	zeile1.clear();zeile2.clear();
	zeile1 << 26.14 << 28.03 << 29.45 << 30.38 << 31.34 << 32.30 << 33.23;
	zeile2 <<  0.64 <<  1.29 <<  4.50 << 34.08 << 89.38 << 99.67 << 99.99;
	tif07 << zeile1 << zeile2;

	QList<QList<double> > tif08;
	zeile1.clear();zeile2.clear();
	zeile1 << 29.10 << 30.04 << 30.97 << 31.92 << 32.84 << 33.77;
	zeile2 <<  0.63 <<  2.84 << 27.76 << 83.27 << 99.04 << 99.99;
	tif08 << zeile1 << zeile2;

	QList<QList<double> > tif09;
	zeile1.clear();zeile2.clear();
	zeile1 << 26.48 << 28.36 << 29.61 << 30.88 << 32.15 << 33.41 << 35.94;
	zeile2 <<  0.58 <<  1.16 <<  2.61 << 24.05 << 87.81 << 99.40 << 99.98;
	tif09 << zeile1 << zeile2;

	QList<QList<double> > tif1;
	zeile1.clear();zeile2.clear();
	zeile1 << 27.31 << 28.73 << 30.11 << 31.52 << 32.95 << 34.37 << 35.78;
	zeile2 <<  0.93 <<  2.48 <<  6.50 << 45.19 << 89.76 << 98.42 << 99.97;
	tif1 << zeile1 << zeile2;

	if(perc < 20)
	{
		return calcLST(tif02);
	}
	else if(perc < 30)
	{
		return calcLST(tif03);
	}
	else if(perc < 40)
	{
		return calcLST(tif04);
	}
	else if(perc < 50)
	{
		return calcLST(tif05);
	}
	else if(perc < 60)
	{
		return calcLST(tif06);
	}
	else if(perc < 70)
	{
		return calcLST(tif07);
	}
	else if(perc < 80)
	{
		return calcLST(tif08);
	}
	else if(perc < 90)
	{
		return calcLST(tif09);
	}
	else
	{
		return calcLST(tif1);
	}


}

double MicroClimateAssessment::calcLST(QList<QList<double> > t)
{
	int pointer;
	double res;
	for(int i = 0;i<t[1].size();i++)
	{
		if(this->percentile < t[1][i])

		{
			pointer = i;
			break;
		}
	}
	// interpolating the perc value of the cell to the values in the table
	res = t[0][pointer-1] + ( (double)this->percentile - t[1][pointer-1])/(t[1][pointer] - t[1][pointer-1])*(t[0][pointer] - t[0][pointer-1]);
	return res;
}

QList<QList<double> > MicroClimateAssessment::readWsud(QString filename)
{
	QList<QList<double> > res;
	QList<double> line;
	QFile file;
	file.setFileName(filename);
	file.open(QIODevice::Text|QIODevice::ReadOnly);
	QTextStream stream;
	stream.setDevice(&file);

	while(!stream.atEnd())
	{
		QString input=stream.readLine();
		QStringList list=input.split(",",QString::KeepEmptyParts);
		while(!list.isEmpty())
			line.append(list.takeFirst().toDouble());
		res.append(line);
		line.clear();
	}
	file.close();
	return res;
}

QList<double> MicroClimateAssessment::getTechAreasForCell(int x, int y,double w, QList<QList<double> > table)
{
	QList<double>zero;
	zero.append(0);
	//double res = 0;
	double cellarea = this->gridsize * this->gridsize;
	int pos = x * w + y;
	/*for(int i = 1;i<table[0].size();i++)
	{
		res += table[pos][i]/100 * cellarea;
	}*/
	if (pos > table.size()-1)
		return zero;
	return table[pos + 1]; // +1 because of the header line in the mcd file
}

double MicroClimateAssessment::calcDeltaLst(QList<double> t, double frac)
{
	int counter = 0;
	double res = 0;
	if(t[1] >= 20)// swales
	{
		counter++;
		res += t[1] /100;
	}
	if(t[2] >= 20)//bioretention
	{
		counter++;
		res += ((t[2] /100) -0.2)*0.5;
	}
	if(t[3] >= 20)//infiltration system
	{
		counter++;
		res += ((t[3] /100) -0.2)*0.5;
	}
	if(t[4] >= 20)//surface wetland
	{
		counter++;
		res += t[4] /100 - 0.2;
	}
	if(t[5] >= 20)//pond & basin
	{
		counter++;
		res += t[5] /100 - 0.2;
	}
	if(t[6] >= 20)//tree
	{
		counter++;
		res += ((t[6] /100) - 0.2) * 2.75;
	}
	if(t[7] >= 20)//grass
	{
		counter++;
		res += (t[7] /100);
	}
	if(t[8] >= 20)
	{
		counter++;
		res += (t[8] /100); // impervious area ... dont know ....
	}
	if(t[9] >= 20)// green wall
	{
		counter++;
		res += t[9] /100;
	}
	if(t[10] >= 20)// green roof
	{
		counter++;
		res += t[10] /100;
	}
	if(t[11] >= 20)// user defiend coefficient
	{
		double coef;
		coef = getCoef();
		counter++;
		res += ((t[11] /100) - 0.2) * coef;
	}
	if(res != 0)
	{
		res = res / (double)counter;
	}
	return res;
}

void MicroClimateAssessment::exportRasterData(DM::RasterData *r, QString filename,double scale)
{
	QFile file (filename);
	if(file.open(QIODevice::WriteOnly))
	{
		QTextStream outstream (&file);
		outstream << "ncols" << "\t" << r->getWidth() << endl;
		outstream << "nrows" << "\t" << r->getHeight() << endl;
		outstream << "xllcorner" << "\t" << r->getXOffset() << endl;
		outstream << "yllcorner" << "\t" << r->getYOffset() << endl;
		outstream << "cellsize" << "\t" << r->getCellSizeX() << endl;
		outstream << "NODATA_value" << "\t" << r->getNoValue() << endl;

		for(int i = 0; i<r->getHeight();i++)
		{
			for(int j = 0; j< r->getWidth(); j++)
			{
				outstream << r->getCell(j,i)*scale << " ";
			}
			outstream << endl;
		}
	}
	file.close();
}
void MicroClimateAssessment::exportMCtemp(DM::RasterData *r, QString filename, double scale)
{
	int counter = 0;
	QFile file (filename);
	QFileInfo finfo = QFileInfo(file);
	QString description = finfo.fileName(); // take before/after wsud etc from file name
	description.chop(4);
	if(file.open(QIODevice::WriteOnly))
	{
		QTextStream outstream(&file);
		//headers
		outstream << "Block, "<< description.toStdString().c_str() << endl;
		for(int i = 0; i<r->getHeight(); i++)
		{
			for(int j = 0; j<r->getWidth();j++)
			{
				outstream << counter << "," << r->getCell(j,i)*scale << endl;
				counter ++;
			}
		}
	}
}

double MicroClimateAssessment::getCoef()
{
	double res=0;
	QFile file;
	file.setFileName(QString(this->workingDir.c_str()) + QString("/userdefcoef.txt"));
	file.open(QIODevice::Text|QIODevice::ReadOnly);
	QTextStream stream;
	stream.setDevice(&file);
	/*
	while(!stream.atEnd())
	{
		QString input=stream.readLine();
		QStringList list=input.split(",",QString::KeepEmptyParts);
		while(!list.isEmpty())
			res = list.takeFirst().toDouble();
	}*/
	if (!stream.atEnd())
		res=stream.readLine().toDouble();
	cout << "Res:"<<res<<endl;
	file.close();
	return res;
}
double MicroClimateAssessment::getTempForSurface(int surface, int percentile)
{
	QList<double> temps10;
	temps10 << 36.8573641414 << 25.6397036706 << 51.7954010152 << 34.4497549263 << 51.9899343183 << 51.7804839049 << 45.8813874516;

	QList<double> temps20;
	temps20 << 37.1278526115 << 25.809094656 << 53.1523905731 << 34.8175474363 << 53.7844889243 << 54.1454705114 << 46.9649870516;

	QList<double> temps30;
	temps30 << 37.4102257336 << 25.9712013589 << 53.9004466744 << 35.0630756865 << 55.36840078 << 54.6859472176 << 47.3038528044;

	QList<double> temps40;
	temps40 << 37.711369073 << 26.5457269186 << 54.3352334037 << 35.5859990169 << 56.6602507341 << 55.5096123239 << 47.5379579664;

	QList<double> temps50;
	temps50 << 37.7998565571 << 27.9692036594 << 55.3523103008 << 35.8858329712 << 58.0237663265 << 55.7530830685 << 47.8958665519;

	QList<double> temps60;
	temps60 << 37.9258020587 << 29.1686798204 << 57.1767368824 << 36.2410729831 << 58.8706742642 << 56.2135073307 << 48.5954112801;

	QList<double> temps70;
	temps70 << 38.1896837015 << 29.679948214 << 58.0200711928 << 36.8607996264 << 60.728960356 << 56.7127437062 << 48.9845970901;

	QList<double> temps80;
	temps80 << 38.4460002208 << 30.1869363438 << 58.9104927717 << 37.4528198439 << 61.9964420703 << 57.4144121447 << 49.7828650018;

	QList<double> temps90;
	temps90 << 38.7733364651 << 31.9040294748 << 59.7076862736 << 38.2019399788 << 63.5975124149 << 57.8973530304 << 51.4188583462;

	QList<double> temps100;
	temps100 << 39.2841448328 << 35.2505707055 << 61.7341270296 << 39.8018041569 << 66.5963175456 << 59.4802032752 << 52.8484158041;

	QList<QList <double> > alltemps;
	alltemps << temps10 << temps20 << temps30 << temps40 << temps50 << temps60 << temps70 << temps80 << temps90 << temps100;

	if(percentile == 20)
		return alltemps[1][surface-1];
	if(percentile == 50)
		return alltemps[4][surface-1];
	if(percentile == 80)
		return alltemps[7][surface-1];
	//when this code reached user selected the 4th percentile option
	double median = (alltemps[4][surface-1] + alltemps[5][surface-1])/2; // since the numbers are already sorted and the amount is even.
																		// take 5th and 6th and calc average
	double variance;
	double diff;
	for(int i = 0; i < 10;i++)
	{
		diff = alltemps[i][surface-1] - median;
		if(diff < 0)
			diff *= -1;
		variance += diff * diff;
	}
	variance /= 10;
	double stdev = sqrt(variance);
	//todo

	boost::mt19937 *rng = new boost::mt19937();
	rng->seed(time(NULL));

	boost::normal_distribution<> distribution(median, stdev);
	boost::variate_generator< boost::mt19937, boost::normal_distribution<> > dist(*rng, distribution);
	double number = dist();
	//std::cout << "Super random number: ";
	//std::cout << number << std::endl;
//    double number=0;

	return number;

}

void MicroClimateAssessment::writeTechs(QList<QList<double> > techs)
{
	QFile file (QString(workingDir.c_str()) + "/WSUDtech.mcd");
	if(file.open(QIODevice::WriteOnly))
	{
		QTextStream outstream (&file);
		//headers
		outstream << "Block, Tree, Water, Pond and basin, Wetland, Dry Grass, Swale, Irrigated Grass, Biofilter, Inf system, Green roof, Green wall, Roof, Road, Porous Pav, Concrete" << endl;
		for(int i = 0; i<techs.size();i++)
		{
			for(int j = 0; j< techs[i].size(); j++)
			{
				if(j==techs[i].size()-1)
					outstream << techs[i][j];
				else
					outstream << techs[i][j] << ",";
			}
			outstream << endl;
		}
	}
	file.close();
}

void MicroClimateAssessment::writeTechsToRaster(QList<QList<double> > techs,QString filename, double xoffset, double yoffset, double noDataValue)
{
	QFile file (filename);
	if(file.open(QIODevice::WriteOnly))
	{
		QTextStream outstream (&file);
		outstream << "ncols" << "\t" << techs[0].size() << endl;
		outstream << "nrows" << "\t" << techs.size() << endl;
		outstream << "xllcorner" << "\t" << xoffset << endl;
		outstream << "yllcorner" << "\t" << yoffset << endl;
		outstream << "cellsize" << "\t" << this->gridsize << endl;
		outstream << "NODATA_value" << "\t" << noDataValue << endl;

		for(int i = 0; i<techs.size();i++)
		{
			for(int j = 0; j< techs[0].size(); j++)
			{
				outstream << techs[j][i] << " ";
			}
			outstream << endl;
		}
	}
	file.close();
}

DM::RasterData * MicroClimateAssessment::readRasterFile(QString FileName)
{

	DM::RasterData *r = new DM::RasterData();
	QFile file(FileName);


	QTextStream stream(&file);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return r;
	}

	QString line("NULL");

	int LineCounter  = 0;
	int rowCounter = 0;
	int ncols = 0;
	int nrows = 0;
	double xoffset = 0;
	double yoffset = 0;
	double cellsize = 0;
	double NoDataValue = -9999; //default

	//Read Header
	while (!line.isNull() && LineCounter < 6 ) {
		LineCounter++;
		line =stream.readLine();
		if (LineCounter == 1) {
			QStringList list = line.split(QRegExp("\\s+"));
			QString s = QString(list[1]);
			s.replace(",", ".");
			ncols = s.toInt();
		}
		if (LineCounter == 2) {
			QStringList list = line.split(QRegExp("\\s+"));
			QString s = QString(list[1]);
			s.replace(",", ".");
			nrows = s.toInt();
		}
		if (LineCounter == 3) {
			QStringList list = line.split(QRegExp("\\s+"));
			QString s = QString(list[1]);
			s.replace(",", ".");
			xoffset = s.toDouble();
		}
		if (LineCounter == 4) {
			QStringList list = line.split(QRegExp("\\s+"));
			QString s = QString(list[1]);
			s.replace(",", ".");
			yoffset = s.toDouble();
		}
		if (LineCounter == 5) {
			QStringList list = line.split(QRegExp("\\s+"));
			QString s = QString(list[1]);
			s.replace(",", ".");
			cellsize = s.toDouble();
		}
		if (LineCounter == 6) {
			QStringList list = line.split(QRegExp("\\s+"));
			QString s = QString(list[1]);
			s.replace(",", ".");
			NoDataValue = s.toDouble();
		}
	}
	std::cout <<" Cols " << ncols << std::endl;
	std::cout <<" Rows " << nrows << std::endl;
	std::cout <<" Cellsize " << cellsize << std::endl;
	r->setNoValue(NoDataValue);

	r->setSize(ncols, nrows, cellsize,cellsize,xoffset,yoffset);

	while (!line.isNull()) {
		LineCounter++;
		line =stream.readLine();
		if (LineCounter >= 6 && rowCounter < nrows) {
			QStringList list = line.split(QRegExp("\\s+"));
			for ( int i = 0; i < list.size(); i++ ) {
				QString s = QString(list[i]);
				s.replace(",", ".");
				r->setCell(i, rowCounter, s.toDouble());
			}
			rowCounter++;

		}
	}
	file.close();
	return r;
}


bool MicroClimateAssessment::isleft(DM::Node a, DM::Node b, DM::Node c)
{
	return ((b.getX() - a.getX()) * (c.getY() - a.getY()) - (b.getY() - a.getY()) * (c.getX() - a.getX())) > 0;
}

DM::RasterData * MicroClimateAssessment::calcReductionAirTemp(DM::RasterData *r)
{
	int counter;
	double value;
	DM::RasterData * res = new DM::RasterData(r->getWidth(),r->getHeight(),r->getCellSizeX(),r->getCellSizeY(),r->getXOffset(),r->getYOffset());
	res->setSize(r->getWidth(),r->getHeight(),r->getCellSizeX(),r->getCellSizeY(),r->getXOffset(),r->getYOffset());
	for(int i = 0; i<res->getHeight();i++)
	{
		for(int j = 0; j< res->getWidth(); j++)
		{
			counter = 0;
			value = 0;
			if(j == 0)  //top row
			{
				value += r->getCell(j+1,i);
				counter++;
			}else if(j == res->getWidth()-1) //bot row
			{
				value += r->getCell(j-1,i);
				counter++;
			}
			else
			{
				value +=  r->getCell(j+1,i) + r->getCell(j-1,i);
				counter++;
				counter++;
			}
			if(i == 0)  //first column
			{
				value += r->getCell(j,i+1);
				counter++;
			}else if(i == res->getHeight()-1) // last column
			{
				value += r->getCell(j,i-1);
				counter++;
			}
			else
			{
				value +=  r->getCell(j,i+1) + r->getCell(j,i-1);
				counter++;
				counter++;
			}
			res->setCell(j,i,value/(double)counter);
		}
	}
	return res;
}




double MicroClimateAssessment::calcOverlay(double x1,double y1,double g1,double x2,double y2, double g2)
{

	QRectF * cell1 = new QRectF(x1,y1, g1,g1);
	QRectF * cell2 = new QRectF(x2,y2,g2,g2);

	//check if overlapping
	if (cell1->topLeft().x() > cell2->topRight().x())
		return 0;
	if (cell1->topLeft().y() > cell2->bottomLeft().y())
		return 0;
	//adjust boundrys of cell1
	if(cell1->left() < cell2->left())
		cell1->setLeft(cell2->left());
	if(cell1->right() > cell2->right())
		cell1->setRight(cell2->right());
	if(cell1->top()< cell2->top())
		cell1->setTop(cell2->top());
	if(cell1->bottom() > cell2->bottom())
		cell1->setBottom(cell2->bottom());

	return (calcA(cell1)/calcA(cell2))*100;
}

double MicroClimateAssessment::calcA(QRectF *r)
{
	return r->width() * r->height();
}

std::vector<QPointF> MicroClimateAssessment::getCoveringCells(double x, double y, double g1,double g2)
{
	std::vector<QPointF> res;
	QRectF * cell = new QRectF(x,y,g1,g1);

	int leftX = cell->left();
	int topY = cell->top();
	int rightX = cell->right();
	int botY = cell->bottom();

	int columns = (rightX - leftX) / g2;
	int rows = (botY - topY) / g2;
	int correction = 2;
	if (fmod(cell->right(),g2) == 0)
	{
		correction --;
	}
	if (fmod(cell->left(),g2) == 0)
	{
		correction --;
	}
	for(int i = 0; i<columns + correction;i++)
	{
		for(int j = 0; j < rows + correction;j++)
		{
			QPointF p = QPointF(leftX/g2 + i,topY/g2 + j);
			res.push_back(p);
		}
	}
	return res;
}


