#include "dmgdalhelper.h"
#include <QtGlobal>
#include <sstream>
#include <istream>

bool DM::DMFeature::SetDoubleList(OGRFeature *f, const std::string &name, const std::vector<double> &values)
{
	std::stringstream ss;
	foreach (double val, values) {
		ss << val;
		ss << " ";
	}

	f->SetField(name.c_str(), ss.str().c_str());
	return true;
}

bool DM::DMFeature::GetDoubleList(OGRFeature *f, const std::string &name, std::vector<double> &values)
{

	std::istringstream lineStream(f->GetFieldAsString(name.c_str()));

	double num;
	while (lineStream >> num) values.push_back(num);
	return true;
}


