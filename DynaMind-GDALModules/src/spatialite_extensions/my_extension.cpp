#include "my_extension.h"
#include "sstream"
#include <QStringList>

 std::string my_extension::mutiply_vector(const unsigned char * vec, double mutiplyer) {

	QString qvec= QString::fromStdString(reinterpret_cast<const char*>(vec));
	std::stringstream ss;
	foreach (QString s, qvec.split(" ")) {
		ss << s.toDouble() * mutiplyer;
		ss << " ";
	}

	return ss.str();
}
