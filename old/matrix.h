#ifndef MATRIX_H
#define MATRIX_H

#include "vec.h"
#include <vector>

using namespace std;

class Matrix4x4 {
    public:
    // Constructors assume matrix is column major
    Matrix4x4(float diag = 1.0);
    Matrix4x4(float data[]);
    Matrix4x4(Vec4 col1, Vec4 col2, Vec4 col3, Vec4 col4);
    Matrix4x4(vector<float> data);

    static Matrix4x4 Invert(const Matrix4x4 &mat);
    friend Vec4 operator *(const Matrix4x4 &lhs, const Vec4 &rhs);
    friend Matrix4x4 operator *(const Matrix4x4 &lhs, const Matrix4x4 &rhs);
    private:
    // Column Major
    float data_[16];
};

#endif  // MATRIX_H