#include "sobol2D.h"
#include <iostream>
#include <algorithm>

Sobol2D::Sobol2D(unsigned int x_0, unsigned int y_0) : 
    sobol_x_n(x_0), sobol_y_n(y_0), sobol_n(0) {
	// push back the initial values for the m_i recurrence relation
	m_i_s.push_back(0);
	m_i_s.push_back(1 << 31);
    sobol_x_s.push_back(sobol_x_n);
    sobol_y_s.push_back(sobol_y_n);
}

void Sobol2D::SobolNext(float &x, float &y) {
    // find c, the index of the rightmost 0 in the binary representation
    // of index (1 indexed)
    unsigned int temp = sobol_n;
    unsigned int c = 1;
    while (temp & 0x01) {
        c++;
        temp >>= 1;
    }
    // now calculate m
    // Note: in the paper, Kuo defines M_c as 2^c. However,
    // this calculation stores the sobol values in fractional
    // binary. Here we use 2^(32 - c), so it is easier to convert the
    // integer representation into a floating point value.
    unsigned int m_c = 1 << (32 - c);  // m_i == 1 in one dimension
    // finally XOR to get the final value
    sobol_x_n ^= m_c;
    // calculate y - finding m requires a little more work since we are one dimension higher
    while (c > m_i_s.size() - 1) {
        // build all the m_i terms up to the current m_i, as they are needed for the recurrence relation
        unsigned int m_i_minus_1 = m_i_s.back();
        unsigned int m_i = m_i_minus_1 ^ (m_i_minus_1 >> 1);
        m_i_s.push_back(m_i);
    }
    sobol_y_n ^= m_i_s[c];
    // increment the index
    sobol_n++;
    // bring it back to a useful float
    float max = pow(2.f, 32);
    if (sobol_n >= max) {
        std::cout << "OUT OF SOBOL" << std::endl;
        exit(1);
    }
    x = sobol_x_n / max;
    y = sobol_y_n / max;
}

void Sobol2D::GetNum(int index, float& x, float& y) {
    // Keep generating sobol numbers until we reach the specified index
    while (sobol_x_s.size() <= index) {
        float next_x, next_y;
        SobolNext(next_x, next_y);
        sobol_x_s.push_back(sobol_x_n);
        sobol_y_s.push_back(sobol_y_n);
    }
    index = sobol_x_s.size() - 1;
    x = sobol_x_s[index]/pow(2.f,32);
    y = sobol_y_s[index]/pow(2.f,32);
}