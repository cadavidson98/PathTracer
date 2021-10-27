#ifndef SOBOL2D
#define SOBOL2D

#include <vector>

class Sobol2D {
public:
	Sobol2D(unsigned int x_0 = 0, unsigned int y_0 = 0);
	void SobolNext(float& x, float& y);
	void GetNum(int index, float& x, float& y);
private:
	unsigned int sobol_x_n = 0;
	unsigned int sobol_y_n = 0;
	unsigned int sobol_n = 0;
	std::vector<unsigned int> m_i_s;
	std::vector<unsigned int> sobol_x_s;
	std::vector<unsigned int> sobol_y_s;

};

#endif  // SOBOL2D

