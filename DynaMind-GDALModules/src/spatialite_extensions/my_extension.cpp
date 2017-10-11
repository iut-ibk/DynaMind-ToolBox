#include "my_extension.h"
#include "sstream"
#include <iostream>
#include <numeric>
#include <iostream>



void my_extension::multiply_vector(const std::vector<double> & vec, double mutiplyer, std::vector<double> & ress) {

	for (int i = 0; i < vec.size(); i++) {
		ress[i] = vec[i] * mutiplyer;
	}

}

void my_extension::addition_vector(const std::vector<double> & vec1, const std::vector<double> & vec2, std::vector<double> & ress) {

	for (int i = 0; i < vec1.size(); i++) {
		ress[i] = vec1[i] + vec2[i];
	}
}

void my_extension::vector_addition(std::vector<double> & dvec, const std::vector<double> vec) {
	for (int i = 0; i < dvec.size(); i++) {
		dvec[i] = dvec[i]+vec[i];
	}
}



double my_extension::vector_sum(const std::vector<double> & data) {
	double sum = std::accumulate(data.begin(), data.end(), 0.0);
	return sum;
}




std::string my_extension::dm_vector_to_string(const std::vector<double> & vec) {
	std::stringstream ss;
	bool first = true;
	foreach (double s, vec) {
		if (!first)
			ss << " ";
		ss << s;
		first  =false;
	}
	std::string ress = ss.str();

    return ress;
}

void my_extension::print_vector(const std::vector<double> &data)
{
    std::cout << my_extension::dm_vector_to_string(data) << std::endl;
}


