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

std::string my_extension::addition_vector(const unsigned char * vec1, const unsigned char * vec2) {

	QString qvec1= QString::fromStdString(reinterpret_cast<const char*>(vec1));
	QString qvec2= QString::fromStdString(reinterpret_cast<const char*>(vec2));
	std::stringstream ss;
	bool first = true;

	QStringList dqvec2 = qvec2.split(" ");
	int counter = -1;
	foreach (QString s, qvec1.split(" ")) {
		counter++;
		if (!first)
			ss << " ";
		if (s.isEmpty())
			continue;
		ss << s.toDouble() + dqvec2[counter].toDouble();
		first = false;

	}

	return ss.str();
}

void my_extension::vector_addition(std::vector<double> & dvec, const unsigned char * vec) {
	QString qvec= QString::fromStdString(reinterpret_cast<const char*>(vec));
	int size = dvec.size();
	int counter = 0;
	foreach (QString s, qvec.split(" ")) {
		if (s.isEmpty() || s == " ")
			continue;
		if (size <= counter) {
			dvec.push_back(s.toDouble());
		} else
			dvec[counter] = dvec[counter]+s.toDouble();
		counter++;
	}
}



double my_extension::vector_sum(const unsigned char * vec) {
	if (!vec)
		return 0;
	QString qvec= QString::fromStdString(reinterpret_cast<const char*>(vec));
	double d = 0;
	foreach (QString s, qvec.split(" ")) {
		d+=s.toDouble();
	}

	return d;
}

