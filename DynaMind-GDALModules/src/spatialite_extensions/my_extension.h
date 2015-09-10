#ifndef MY_EXTENSION_H
#define MY_EXTENSION_H

#include <dmcompilersettings.h>
#include <QString>
#include <vector>


class DM_HELPER_DLL_EXPORT my_extension
{
public:
	static  std::string  multiply_vector(const unsigned char *vec, double mutiplyer);
	static	double vector_sum(const unsigned char *vec);

	static void vector_addition(std::vector<double> &dvec, const unsigned char *vec);
	static std::string addition_vector(const unsigned char *vec1, const unsigned char *vec2);
};

#endif // MY_EXTENSION_H
