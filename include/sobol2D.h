#ifndef SOBOL2D_H
#define SOBOL2D_H
#include "sampler.h"
#include <vector>

class Sobol2D : public Sampler2D {
public:
	Sobol2D(unsigned int x_0 = 0, unsigned int y_0 = 0);
	void NextSample(float& x, float& y);
private:
	unsigned long long int sobol_x_n_ = 0;
	unsigned long long int sobol_y_n_ = 0;
	unsigned long long int sobol_n_ = 0;
	std::vector<unsigned long long int> m_i_s_;
	std::vector<unsigned long long int> sobol_x_s_;
	std::vector<unsigned long long int> sobol_y_s_;

};

#endif  // SOBOL2D_H

