#include "drainagehelper.h"
#include <fstream>

void DrainageHelper::CreateEulerRainFile(double duration, double deltaT, double return_period, double cf, std::string rfile) {

	QDateTime date(QDate(2000,1,1), QTime(0,0));

	QString fileName = QString::fromStdString(rfile);
	std::fstream out;
	out.open(fileName.toAscii(),ios::out);

	std::vector<double> rainseries_tmp;
	int steps = duration/deltaT;
	double D = 0;
	DM::Logger(DM::Debug) << "Rain D U W r";
	for (int s = 0; s < steps; s++) {
		D=D+deltaT;
		double Uapprox = exp(1.4462+0.3396*log(D));
		double Wapprox = -0.4689+2.9227*log(D)-0.0815*pow(log(D),2);
		double rain = (Uapprox+Wapprox*log(return_period))*cf;

		DM::Logger(DM::Debug) << D <<"/"<< Uapprox <<"/"<< Wapprox << "/" << rain;
		rainseries_tmp.push_back(rain);
	}

	std::vector<double> rainseries;
	rainseries.push_back(rainseries_tmp[0]);
	for (uint i = 1; i < rainseries_tmp.size(); i++)
		rainseries.push_back(rainseries_tmp[i] - rainseries_tmp[i-1]);


	//flip first 3rd of the rain
	std::vector<double> rainseries_flipped(rainseries);
	std::vector<double> flipped_values;
	for (int s = 0; s < steps/3; s++) {
		flipped_values.push_back(rainseries[s]);
	}
	std::reverse(flipped_values.begin(), flipped_values.end());

	for (uint s = 0; s < flipped_values.size(); s++) rainseries_flipped[s] = flipped_values[s];

	for (int s = 0; s < steps; s++) {
		date = date.addSecs(deltaT*60);
		out << "STA01 " <<date.toString("yyyy M d HH mm").toStdString() << " " << rainseries_flipped[s];
		out << "\n";
	}

	out.close();
}

void DrainageHelper::WriteOutputFiles(std::string filename, DM::System * sys, SWMMWriteAndRead &swmmreeadfile,  std::map<std::string, std::string> additional)
{

	typedef std::pair<int, double > rainnode;

	std::map<int, double> flooded = swmmreeadfile.getFloodedNodes();
	std::map<int, double> surcharge = swmmreeadfile.getNodeDepthSummery();

	std::fstream inp;
	double effective_runoff = swmmreeadfile.getVSurfaceRunoff() + swmmreeadfile.getVSurfaceStorage();




	inp.open(filename.c_str(),ios::out);

	for (std::map<std::string, std::string>::const_iterator it =  additional.begin();
		 it != additional.end();
		 it++) {
		inp << it->first;
		inp << "\t";
		inp << it->second;
		inp << "\n";
	}

	inp << "ImperviousTotal\t" << swmmreeadfile.getTotalImpervious()<< "\n";
	inp << "ImperviousInfitration\t" << swmmreeadfile.getImperiousInfiltration() / 10000.<< "\n";
	inp << "Vr\t" << effective_runoff << "\n";
	inp << "Vp\t" << swmmreeadfile.getVp()<< "\n";
	inp << "Vwwtp\t" << swmmreeadfile.getVwwtp()<< "\n";
	inp << "Vout\t" << swmmreeadfile.getVout()<< "\n";
	inp << "AverageCapacity\t" << swmmreeadfile.getAverageCapacity() << "\n";
	inp << "below_0\t" << swmmreeadfile.getWaterLeveleBelow0()<< "\n";
	inp << "below_10\t" << swmmreeadfile.getWaterLeveleBelow10()<< "\n";
	inp << "below_20\t" << swmmreeadfile.getWaterLeveleBelow20()<< "\n";
	inp << "error\t" << swmmreeadfile.getContinuityError()<< "\n";
	inp << "swmm_path\t"<< swmmreeadfile.getSWMMUUIDPath() << "\n";
	inp << "floodednodes\t" << flooded.size() << "\n";
	inp << "Vstorage\t" << swmmreeadfile.getVSurfaceStorage()<< "\n";
	inp << "END\t"  << "\n";
	inp << fixed;
	inp << "FLOODSECTION\t" << "\n";
	foreach (rainnode  fn, flooded) {
		inp << fn.first;
		inp << "\t";
		inp << fn.second;
		inp << "\n";
	}
	inp << "END\t" << "\n";

//	inp << "SURCHARGE" << "\n";
//	foreach (rainnode  fn, surcharge) {
//		DM::Component * n = fn.first;
//		if (!n) continue;
//		inp << fn.first;
//		inp << "\t";
//		inp << n->getAttribute("D")->getDouble();
//		inp << "\t";
//		inp <<fn.second;
//		inp << "\n";
//	}

//	inp << "END" << "\n";

	inp.close();

}
