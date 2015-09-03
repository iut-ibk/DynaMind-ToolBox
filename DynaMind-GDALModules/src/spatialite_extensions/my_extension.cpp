#include "my_extension.h"
#include "sstream"
#include <QStringList>
#include <iostream>
std::string my_extension::multiply_vector(const unsigned char * vec, double mutiplyer) {

	QString qvec= QString::fromStdString(reinterpret_cast<const char*>(vec));
	std::stringstream ss;
	bool first = true;
	foreach (QString s, qvec.split(" ")) {
		if (!first)
			ss << " ";
		if (s.isEmpty())
			continue;
		ss << s.toDouble() * mutiplyer;
		first = false;
	}

	return ss.str();
}

void my_extension::vector_addition(std::vector<double> & dvec, const unsigned char * vec) {

	QString qvec= QString::fromStdString(reinterpret_cast<const char*>(vec));

	if (dvec.size() == 0) {
		foreach (QString s, qvec.split(" ")) {
			if (s.isEmpty())
				continue;
			dvec.push_back(s.toDouble());
		}
		return;
	}
	int counter = 0;
	foreach (QString s, qvec.split(" ")) {
		if (s.isEmpty())
			continue;
		dvec[counter] = dvec[counter]+s.toDouble();
		counter++;
	}
}

double my_extension::vector_sum(const unsigned char * vec) {

	QString qvec= QString::fromStdString(reinterpret_cast<const char*>(vec));
	double d = 0;
	foreach (QString s, qvec.split(" ")) {
		d+=s.toDouble();
	}

	return d;
}

