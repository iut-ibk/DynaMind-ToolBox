#ifndef MICROCLIMATEASSESSMENT_H
#define MICROCLIMATEASSESSMENT_H

#include <dmmodule.h>
#include <dm.h>
#include <qrect.h>
class DM_HELPER_DLL_EXPORT MicroClimateAssessment : public DM::Module
{
	DM_DECLARE_NODE(MicroClimateAssessment)

private:


public:

	int gridsize;
	int percentile;
	std::string mapPic;
	std::string shapefile;
	std::string landuse;
	std::string wsudTech;
	std::string workingDir;
	std::string techFile;
	QList<QList<double> > tec;


	MicroClimateAssessment();
	void init();
	void run();
	//virtual bool createInputDialog();
	void printRaster(DM::RasterData * r);
	double calcOverlay(double x1, double y1, double gridsize1, double x2, double y2, double gridsize2);
	double calcA(QRectF * r);
	std::vector<QPointF>  getCoveringCells(double x,double y,double g1,double g2);
	void fillZeros(DM::RasterData * r);
	double chooseTab(double perc);
	double calcLST(QList<QList<double> > t);

	QList<QList<double> > readWsud(QString filename);
	QList<double> getTechAreasForCell(int x, int y,double width, QList<QList<double> >table);
	double calcDeltaLst(QList<double> t, double frac);
	void exportRasterData(DM::RasterData * r, QString filename,double scale);
	bool isleft(DM::Node a,DM::Node b,DM::Node c);
	DM::RasterData * calcReductionAirTemp(DM::RasterData * r);
	void exportMCtemp(DM::RasterData *r, QString filename, double scale);
	double getCoef();
	double getTempForSurface(int surface, int percentile);
	void writeTechs(QList<QList<double> > techs);
	void writeTechsToRaster(QList<QList<double> > techs,QString filename,double xoffset,double yoffset, double noDataValue);
	DM::RasterData * readRasterFile(QString FileName);

};

#endif // MICROCLIMATEASSESSMENT_H
