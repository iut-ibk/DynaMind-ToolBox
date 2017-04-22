#include "dmgdalhelper.h"
#include <QtGlobal>
#include <sstream>
#include <istream>
#include <iostream>

bool DM::DMFeature::SetDoubleList(OGRFeature *f, const std::string &name, const std::vector<double> &values)
{
	if (values.size() == 0)
		return true;
	//convert to bytestream
	std::stringbuf bytebuffer;
	for (size_t i = 0; i < values.size(); i++) {
		bytebuffer.sputn(reinterpret_cast<const char *>(&values[i]), 8);
	}
	GByte * val= reinterpret_cast<unsigned char*>(const_cast<char*>(bytebuffer.str().c_str()));
	f->SetField(f->GetFieldIndex(name.c_str()),values.size()*8,val);
	return true;
}

bool DM::DMFeature::GetDoubleList(OGRFeature *f, const std::string &name, std::vector<double> &values)
{
	int bytes;
	double * buffer =  reinterpret_cast<double*>(f->GetFieldAsBinary(f->GetFieldIndex(name.c_str()), & bytes));
	if (!buffer) {
		return false;
	}
	values = std::vector<double>(buffer, buffer + bytes/8);
	return true;
}

bool DM::DMFeature::GetDoubleListFromString(OGRFeature *f, const std::string &name, std::vector<double> &values)
{
	std::istringstream lineStream(f->GetFieldAsString(name.c_str()));

	double num;
	while (lineStream >> num) values.push_back(num);
	return true;
}


