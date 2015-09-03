#ifndef MY_EXTENSION_H
#define MY_EXTENSION_H

#include <QString>
#include <vector>

class my_extension
{
public:
	static  std::string  multiply_vector(const unsigned char *vec, double mutiplyer);
	static	double vector_sum(const unsigned char *vec);

	static void vector_addition(std::vector<double> &dvec, const unsigned char *vec);
};

#endif // MY_EXTENSION_H
