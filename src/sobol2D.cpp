#include "sobol2D.h"
#include <iostream>
#include <algorithm>

Sobol2D::Sobol2D(unsigned int x_0, unsigned int y_0) : 
    sobol_x_n_(x_0), sobol_y_n_(y_0), sobol_n_(0) {
	// push back the initial values for the m_i recurrence relation
	m_i_s_.push_back(0);
	m_i_s_.push_back(1 << 31);
    sobol_x_s_.push_back(sobol_x_n_);
    sobol_y_s_.push_back(sobol_y_n_);
}

void Sobol2D::NextSample(float &x, float &y) {
    // find c, the index of the rightmost 0 in the binary representation
    // of index (1 indexed)
    unsigned long long int temp = sobol_n_;
    unsigned long long int c = 1;
    while (temp & 0x01) {
        c++;
        temp >>= 1;
    }
    // now calculate m
    // Note: in the paper, Kuo defines M_c as 2^c. However,
    // this calculation stores the sobol values in fractional
    // binary. Here we use 2^(32 - c), so it is easier to convert the
    // integer representation into a floating point value.
    unsigned long long int m_c = 1 << (64 - c);  // m_i == 1 in one dimension
    // finally XOR to get the final value
    sobol_x_n_ ^= m_c;
    // calculate y - finding m requires a little more work since we are one dimension higher
    while (c > m_i_s_.size() - 1) {
        // build all the m_i terms up to the current m_i, as they are needed for the recurrence relation
        unsigned long long int m_i_minus_1 = m_i_s_.back();
        unsigned long long int m_i = m_i_minus_1 ^ (m_i_minus_1 >> 1);
        m_i_s_.push_back(m_i);
    }
    sobol_y_n_ ^= m_i_s_[c];
    // increment the index
    sobol_n_++;
    // bring it back to a useful float
    float max = pow(2.f, 64);
    if (sobol_n_ >= max) {
        std::cout << "OUT OF SOBOL" << std::endl;
        exit(1);
    }
    x = sobol_x_n_ / max;
    y = sobol_y_n_ / max;
}