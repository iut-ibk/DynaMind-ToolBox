#include "dm_microclimate.h"
#include <ogrsf_frmts.h>

#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>

DM_DECLARE_CUSTOM_NODE_NAME(DM_MicroClimate, Microclimate Extreme Heat, Performance Assessment)



DM_MicroClimate::DM_MicroClimate()
{
	GDALModule = true;

	grid = DM::ViewContainer("micro_climate_grid", DM::COMPONENT, DM::READ);
	grid.addAttribute("tree_cover_fraction", DM::Attribute::DOUBLE, DM::READ );
	grid.addAttribute("water_fraction", DM::Attribute::DOUBLE, DM::READ );
	grid.addAttribute("grass_fraction", DM::Attribute::DOUBLE, DM::READ);
	grid.addAttribute("irrigated_grass_fraction", DM::Attribute::DOUBLE, DM::READ);
	grid.addAttribute("roof_fraction", DM::Attribute::DOUBLE, DM::READ);
	grid.addAttribute("concrete_fraction", DM::Attribute::DOUBLE, DM::READ);
	grid.addAttribute("temperature", DM::Attribute::DOUBLE, DM::WRITE);

	this->addParameter("percentile",DM::INT, &this->percentile);
	percentile = 80;

	std::vector<DM::ViewContainer*> datastream;
	datastream.push_back(&grid);
	this->registerViewContainers(datastream);
}

void DM_MicroClimate::run()
{
	grid.resetReading();

	OGRFeature * f;
	while (f = grid.getNextFeature()) {
		double tree = f->GetFieldAsDouble("tree_cover_fraction") > 0 ? f->GetFieldAsDouble("tree_cover_fraction") : 0;
		double water = f->GetFieldAsDouble("water_fraction") > 0 ? f->GetFieldAsDouble("water_fraction") : 0;
		double grass = f->GetFieldAsDouble("grass_fraction") > 0 ? f->GetFieldAsDouble("grass_fraction") : 0;
		double irrGrass = f->GetFieldAsDouble("irrigated_grass_fraction") > 0 ? f->GetFieldAsDouble("irrigated_grass_fraction") : 0;
		double roof = f->GetFieldAsDouble("roof_fraction") > 0 ? f->GetFieldAsDouble("roof_fraction") : 0;
		double road = f->GetFieldAsDouble("road_fraction") > 0 ? f->GetFieldAsDouble("road_fraction") : 0;
		double concrete  = f->GetFieldAsDouble("concrete_fraction") > 0 ? f->GetFieldAsDouble("concrete_fraction") : 0;
		double scale = tree + water + grass + irrGrass + roof + road + concrete;
		f->SetField("temperature",
					tree/scale * getTempForSurface(1,this->percentile) +
					water/scale * getTempForSurface(2,this->percentile) +
					grass/scale * getTempForSurface(3,this->percentile) +
					irrGrass/scale * getTempForSurface(4,this->percentile) +
					roof/scale * getTempForSurface(5,this->percentile) +
					road/scale * getTempForSurface(6,this->percentile) +
					concrete/scale * getTempForSurface(7,this->percentile) );
	}
}


double DM_MicroClimate::getTempForSurface(int surface, int percentile)
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
