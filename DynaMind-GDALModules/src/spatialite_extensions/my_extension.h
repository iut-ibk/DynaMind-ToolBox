#ifndef MY_EXTENSION_H
#define MY_EXTENSION_H

#include <dmcompilersettings.h>
#include <QString>
#include <vector>


class DM_HELPER_DLL_EXPORT my_extension
{
public:
	static void multiply_vector(const std::vector<double> &vec1, double mutiplyer,  std::vector<double> &ress);
	static double vector_sum(const std::vector<double> &data);

	static void vector_addition(std::vector<double> &dvec, const std::vector<double> vec);
	static void addition_vector(const std::vector<double> & vec1, const std::vector<double> & vec2, std::vector<double> & ress);
	static void vector_multiply(const std::vector<double> & vec1, const std::vector<double> & vec2, std::vector<double> & ress);
    static std::string dm_vector_to_string(const std::vector<double> &data);
    static void print_vector(const std::vector<double> &data);
};

#endif // MY_EXTENSION_H
