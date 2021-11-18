#include "matrix.h"

Matrix4x4::Matrix4x4(float diag) {
	memset(data_, 0, 16*sizeof(float));
	data_[0] =
	data_[5] =
	data_[10] =
	data_[15] = diag;
}

Matrix4x4::Matrix4x4(float data[]) {
	memcpy(data_, data, 16 * sizeof(float));
}

Matrix4x4::Matrix4x4(vector<float> data) {
	memcpy(data_, std::data(data), 16 * sizeof(float));
}

Matrix4x4::Matrix4x4(Vec4 col1, Vec4 col2, Vec4 col3, Vec4 col4) {
    data_[0]  = col1.x;
    data_[1]  = col1.y;
    data_[2]  = col1.z;
    data_[3]  = col1.w;
    data_[4]  = col2.x;
    data_[5]  = col2.y;
    data_[6]  = col2.z;
    data_[7]  = col2.w;
    data_[8]  = col3.x;
    data_[9]  = col3.y;
    data_[10] = col3.z;
    data_[11] = col3.w;
    data_[12] = col4.x;
    data_[13] = col4.y;
    data_[14] = col4.z;
    data_[15] = col4.w;
}

#define VALUE(x, y) 4 * x - 4 + y - 1

Matrix4x4 Matrix4x4::Invert(const Matrix4x4 &mat) {
    // Calculate the inverse using the Adjugate matrix.
    // We use a bottom up approach and first calculate all the 2x2 matrix
    // determinates to reduce computation cost.
    float zeros[16];
    memset(zeros, 0, 16 * sizeof(float));
    Matrix4x4 inverse(zeros);

    inverse.data_[VALUE(1, 1)] += mat.data_[VALUE(2, 2)] * mat.data_[VALUE(3, 3)] * mat.data_[VALUE(4, 4)] + mat.data_[VALUE(2, 3)] * mat.data_[VALUE(3, 4)] * mat.data_[VALUE(4, 2)] + mat.data_[VALUE(2, 4)] * mat.data_[VALUE(3, 2)] * mat.data_[VALUE(4, 3)];
    inverse.data_[VALUE(1, 1)] -= (mat.data_[VALUE(2, 4)] * mat.data_[VALUE(3, 3)] * mat.data_[VALUE(4, 2)] + mat.data_[VALUE(2, 3)] * mat.data_[VALUE(3, 2)] * mat.data_[VALUE(4, 4)] + mat.data_[VALUE(2, 2)] * mat.data_[VALUE(3, 4)] * mat.data_[VALUE(4, 3)]);

    inverse.data_[VALUE(1, 2)] += mat.data_[VALUE(2, 1)] * mat.data_[VALUE(3, 3)] * mat.data_[VALUE(4, 4)] + mat.data_[VALUE(2, 3)] * mat.data_[VALUE(3, 4)] * mat.data_[VALUE(4, 1)] + mat.data_[VALUE(2, 4)] * mat.data_[VALUE(3, 1)] * mat.data_[VALUE(4, 3)];
    inverse.data_[VALUE(1, 2)] -= (mat.data_[VALUE(2, 4)] * mat.data_[VALUE(3, 3)] * mat.data_[VALUE(4, 1)] + mat.data_[VALUE(2, 3)] * mat.data_[VALUE(3, 1)] * mat.data_[VALUE(4, 4)] + mat.data_[VALUE(2, 1)] * mat.data_[VALUE(3, 4)] * mat.data_[VALUE(4, 3)]);

    inverse.data_[VALUE(1, 3)] += mat.data_[VALUE(2, 2)] * mat.data_[VALUE(3, 3)] * mat.data_[VALUE(4, 4)] + mat.data_[VALUE(2, 3)] * mat.data_[VALUE(3, 4)] * mat.data_[VALUE(4, 2)] + mat.data_[VALUE(2, 4)] * mat.data_[VALUE(3, 2)] * mat.data_[VALUE(4, 3)];
    inverse.data_[VALUE(1, 3)] -= (mat.data_[VALUE(2, 4)] * mat.data_[VALUE(3, 3)] * mat.data_[VALUE(4, 2)] + mat.data_[VALUE(2, 3)] * mat.data_[VALUE(3, 2)] * mat.data_[VALUE(4, 4)] + mat.data_[VALUE(2, 2)] * mat.data_[VALUE(3, 4)] * mat.data_[VALUE(4, 3)]);

    inverse.data_[VALUE(1, 4)] += mat.data_[VALUE(2, 1)] * mat.data_[VALUE(3, 2)] * mat.data_[VALUE(4, 3)] + mat.data_[VALUE(2, 2)] * mat.data_[VALUE(3, 3)] * mat.data_[VALUE(4, 1)] + mat.data_[VALUE(2, 3)] * mat.data_[VALUE(3, 1)] * mat.data_[VALUE(4, 2)];
    inverse.data_[VALUE(1, 4)] -= (mat.data_[VALUE(2, 3)] * mat.data_[VALUE(3, 2)] * mat.data_[VALUE(4, 1)] + mat.data_[VALUE(2, 2)] * mat.data_[VALUE(3, 1)] * mat.data_[VALUE(4, 3)] + mat.data_[VALUE(2, 1)] * mat.data_[VALUE(3, 3)] * mat.data_[VALUE(4, 2)]);

    inverse.data_[VALUE(2, 1)] += mat.data_[VALUE(1, 2)] * mat.data_[VALUE(3, 3)] * mat.data_[VALUE(4, 4)] + mat.data_[VALUE(1, 3)] * mat.data_[VALUE(3, 4)] * mat.data_[VALUE(4, 2)] + mat.data_[VALUE(1, 4)] * mat.data_[VALUE(3, 2)] * mat.data_[VALUE(4, 3)];
    inverse.data_[VALUE(2, 1)] -= (mat.data_[VALUE(1, 4)] * mat.data_[VALUE(3, 3)] * mat.data_[VALUE(4, 2)] + mat.data_[VALUE(1, 3)] * mat.data_[VALUE(3, 2)] * mat.data_[VALUE(4, 4)] + mat.data_[VALUE(1, 2)] * mat.data_[VALUE(3, 4)] * mat.data_[VALUE(4, 3)]);

    inverse.data_[VALUE(2, 2)] += mat.data_[VALUE(1, 1)] * mat.data_[VALUE(3, 3)] * mat.data_[VALUE(4, 4)] + mat.data_[VALUE(1, 3)] * mat.data_[VALUE(3, 4)] * mat.data_[VALUE(4, 1)] + mat.data_[VALUE(1, 4)] * mat.data_[VALUE(3, 1)] * mat.data_[VALUE(4, 2)];
    inverse.data_[VALUE(2, 2)] -= (mat.data_[VALUE(1, 4)] * mat.data_[VALUE(3, 3)] * mat.data_[VALUE(4, 1)] + mat.data_[VALUE(1, 3)] * mat.data_[VALUE(3, 1)] * mat.data_[VALUE(4, 4)] + mat.data_[VALUE(1, 1)] * mat.data_[VALUE(3, 4)] * mat.data_[VALUE(4, 2)]);

    inverse.data_[VALUE(2, 3)] += mat.data_[VALUE(1, 1)] * mat.data_[VALUE(3, 2)] * mat.data_[VALUE(4, 4)] + mat.data_[VALUE(1, 2)] * mat.data_[VALUE(3, 4)] * mat.data_[VALUE(4, 1)] + mat.data_[VALUE(1, 4)] * mat.data_[VALUE(3, 1)] * mat.data_[VALUE(4, 2)];
    inverse.data_[VALUE(2, 3)] -= (mat.data_[VALUE(1, 4)] * mat.data_[VALUE(3, 2)] * mat.data_[VALUE(4, 1)] + mat.data_[VALUE(1, 2)] * mat.data_[VALUE(3, 1)] * mat.data_[VALUE(4, 4)] + mat.data_[VALUE(1, 1)] * mat.data_[VALUE(3, 4)] * mat.data_[VALUE(4, 2)]);

    inverse.data_[VALUE(2, 4)] += mat.data_[VALUE(1, 1)] * mat.data_[VALUE(3, 2)] * mat.data_[VALUE(4, 3)] + mat.data_[VALUE(1, 2)] * mat.data_[VALUE(3, 3)] * mat.data_[VALUE(4, 1)] + mat.data_[VALUE(1, 3)] * mat.data_[VALUE(3, 1)] * mat.data_[VALUE(4, 2)];
    inverse.data_[VALUE(2, 4)] -= (mat.data_[VALUE(1, 3)] * mat.data_[VALUE(3, 2)] * mat.data_[VALUE(4, 1)] + mat.data_[VALUE(1, 2)] * mat.data_[VALUE(3, 1)] * mat.data_[VALUE(4, 3)] + mat.data_[VALUE(1, 1)] * mat.data_[VALUE(3, 3)] * mat.data_[VALUE(4, 2)]);

    inverse.data_[VALUE(3, 1)] += mat.data_[VALUE(1, 2)] * mat.data_[VALUE(2, 3)] * mat.data_[VALUE(4, 4)] + mat.data_[VALUE(1, 3)] * mat.data_[VALUE(2, 4)] * mat.data_[VALUE(4, 2)] + mat.data_[VALUE(1, 4)] * mat.data_[VALUE(2, 2)] * mat.data_[VALUE(4, 3)];
    inverse.data_[VALUE(3, 1)] -= (mat.data_[VALUE(1, 4)] * mat.data_[VALUE(2, 3)] * mat.data_[VALUE(4, 2)] + mat.data_[VALUE(1, 3)] * mat.data_[VALUE(2, 2)] * mat.data_[VALUE(4, 4)] + mat.data_[VALUE(1, 2)] * mat.data_[VALUE(2, 4)] * mat.data_[VALUE(4, 3)]);

    inverse.data_[VALUE(3, 2)] += mat.data_[VALUE(1, 1)] * mat.data_[VALUE(2, 3)] * mat.data_[VALUE(4, 4)] + mat.data_[VALUE(1, 3)] * mat.data_[VALUE(2, 4)] * mat.data_[VALUE(4, 1)] + mat.data_[VALUE(1, 4)] * mat.data_[VALUE(2, 1)] * mat.data_[VALUE(4, 3)];
    inverse.data_[VALUE(3, 2)] -= (mat.data_[VALUE(1, 4)] * mat.data_[VALUE(2, 3)] * mat.data_[VALUE(4, 1)] + mat.data_[VALUE(1, 3)] * mat.data_[VALUE(2, 1)] * mat.data_[VALUE(4, 4)] + mat.data_[VALUE(1, 1)] * mat.data_[VALUE(2, 4)] * mat.data_[VALUE(4, 3)]);

    inverse.data_[VALUE(3, 3)] += mat.data_[VALUE(1, 1)] * mat.data_[VALUE(2, 2)] * mat.data_[VALUE(4, 4)] + mat.data_[VALUE(1, 2)] * mat.data_[VALUE(2, 3)] * mat.data_[VALUE(4, 1)] + mat.data_[VALUE(1, 4)] * mat.data_[VALUE(2, 1)] * mat.data_[VALUE(4, 2)];
    inverse.data_[VALUE(3, 3)] -= (mat.data_[VALUE(1, 4)] * mat.data_[VALUE(2, 2)] * mat.data_[VALUE(4, 1)] + mat.data_[VALUE(1, 2)] * mat.data_[VALUE(2, 1)] * mat.data_[VALUE(4, 3)] + mat.data_[VALUE(1, 1)] * mat.data_[VALUE(2, 4)] * mat.data_[VALUE(4, 2)]);

    inverse.data_[VALUE(3, 4)] += mat.data_[VALUE(2, 2)] * mat.data_[VALUE(3, 3)] * mat.data_[VALUE(4, 4)] + mat.data_[VALUE(2, 3)] * mat.data_[VALUE(3, 4)] * mat.data_[VALUE(4, 2)] + mat.data_[VALUE(2, 4)] * mat.data_[VALUE(3, 2)] * mat.data_[VALUE(4, 3)];
    inverse.data_[VALUE(3, 4)] -= (mat.data_[VALUE(2, 4)] * mat.data_[VALUE(3, 3)] * mat.data_[VALUE(4, 2)] + mat.data_[VALUE(2, 3)] * mat.data_[VALUE(3, 2)] * mat.data_[VALUE(4, 4)] + mat.data_[VALUE(2, 2)] * mat.data_[VALUE(3, 4)] * mat.data_[VALUE(4, 3)]);

    inverse.data_[VALUE(4, 1)] += mat.data_[VALUE(2, 3)] * mat.data_[VALUE(3, 2)] * mat.data_[VALUE(4, 1)] + mat.data_[VALUE(2, 2)] * mat.data_[VALUE(3, 1)] * mat.data_[VALUE(4, 3)] + mat.data_[VALUE(2, 1)] * mat.data_[VALUE(3, 3)] * mat.data_[VALUE(4, 2)];
    inverse.data_[VALUE(4, 1)] -= (mat.data_[VALUE(2, 1)] * mat.data_[VALUE(3, 2)] * mat.data_[VALUE(4, 3)] + mat.data_[VALUE(2, 2)] * mat.data_[VALUE(3, 3)] * mat.data_[VALUE(4, 1)] + mat.data_[VALUE(2, 3)] * mat.data_[VALUE(3, 1)] * mat.data_[VALUE(4, 2)]);

    inverse.data_[VALUE(4, 2)] += mat.data_[VALUE(1, 1)] * mat.data_[VALUE(3, 2)] * mat.data_[VALUE(4, 3)] + mat.data_[VALUE(1, 2)] * mat.data_[VALUE(3, 3)] * mat.data_[VALUE(4, 1)] + mat.data_[VALUE(1, 3)] * mat.data_[VALUE(3, 1)] * mat.data_[VALUE(4, 2)];
    inverse.data_[VALUE(4, 2)] -= (mat.data_[VALUE(1, 3)] * mat.data_[VALUE(3, 2)] * mat.data_[VALUE(4, 1)] + mat.data_[VALUE(1, 2)] * mat.data_[VALUE(3, 1)] * mat.data_[VALUE(4, 3)] + mat.data_[VALUE(1, 1)] * mat.data_[VALUE(3, 3)] * mat.data_[VALUE(4, 2)]);

    inverse.data_[VALUE(4, 3)] += mat.data_[VALUE(1, 3)] * mat.data_[VALUE(2, 2)] * mat.data_[VALUE(3, 1)] + mat.data_[VALUE(1, 2)] * mat.data_[VALUE(2, 1)] * mat.data_[VALUE(4, 3)] + mat.data_[VALUE(1, 1)] * mat.data_[VALUE(2, 3)] * mat.data_[VALUE(4, 2)];
    inverse.data_[VALUE(4, 3)] -= (mat.data_[VALUE(1, 1)] * mat.data_[VALUE(2, 2)] * mat.data_[VALUE(4, 3)] + mat.data_[VALUE(1, 2)] * mat.data_[VALUE(2, 3)] * mat.data_[VALUE(4, 1)] + mat.data_[VALUE(1, 3)] * mat.data_[VALUE(2, 1)] * mat.data_[VALUE(4, 2)]);

    inverse.data_[VALUE(4, 4)] += mat.data_[VALUE(1, 1)] * mat.data_[VALUE(2, 2)] * mat.data_[VALUE(3, 3)] + mat.data_[VALUE(1, 2)] * mat.data_[VALUE(2, 3)] * mat.data_[VALUE(3, 1)] + mat.data_[VALUE(1, 3)] * mat.data_[VALUE(2, 1)] * mat.data_[VALUE(3, 2)];
    inverse.data_[VALUE(4, 4)] -= (mat.data_[VALUE(1, 3)] * mat.data_[VALUE(2, 2)] * mat.data_[VALUE(3, 1)] + mat.data_[VALUE(1, 2)] * mat.data_[VALUE(2, 1)] * mat.data_[VALUE(3, 3)] + mat.data_[VALUE(1, 1)] * mat.data_[VALUE(2, 3)] * mat.data_[VALUE(3, 2)]);

    // Find the determinant
    float det = 0.f;
    det += (VALUE(2, 2) * VALUE(3, 3) * VALUE(4, 4) +
        VALUE(2, 3) * VALUE(3, 4) * VALUE(4, 2) +
        VALUE(2, 4) * VALUE(3, 2) * VALUE(4, 3));
    det -= (VALUE(2, 4) * VALUE(3, 3) * VALUE(4, 2) +
        VALUE(2, 3) * VALUE(3, 2) * VALUE(4, 4) +
        VALUE(2, 2) * VALUE(3, 4) * VALUE(4, 3));
    det *= VALUE(1, 1);

    det += (VALUE(1, 2) * VALUE(3, 3) * VALUE(4, 4) +
        VALUE(1, 3) * VALUE(3, 4) * VALUE(4, 2) +
        VALUE(1, 4) * VALUE(3, 2) * VALUE(4, 3));
    det -= (VALUE(1, 4) * VALUE(3, 3) * VALUE(4, 2) +
        VALUE(1, 3) * VALUE(3, 2) * VALUE(4, 4) +
        VALUE(1, 2) * VALUE(3, 4) * VALUE(4, 3));
    det *= -VALUE(2, 1);

    det += (VALUE(1, 2) * VALUE(2, 3) * VALUE(4, 4) +
        VALUE(1, 3) * VALUE(2, 4) * VALUE(4, 2) +
        VALUE(1, 4) * VALUE(2, 2) * VALUE(4, 3));
    det -= (VALUE(1, 4) * VALUE(2, 3) * VALUE(4, 2) +
        VALUE(1, 3) * VALUE(2, 2) * VALUE(4, 4) +
        VALUE(1, 2) * VALUE(2, 4) * VALUE(4, 3));
    det *= VALUE(3, 1);

    det += (VALUE(1, 2) * VALUE(2, 3) * VALUE(3, 4) +
        VALUE(1, 3) * VALUE(2, 4) * VALUE(3, 2) +
        VALUE(1, 4) * VALUE(2, 2) * VALUE(3, 3));
    det -= (VALUE(1, 4) * VALUE(2, 3) * VALUE(3, 2) +
        VALUE(1, 3) * VALUE(2, 2) * VALUE(3, 4) +
        VALUE(1, 2) * VALUE(2, 4) * VALUE(3, 3));
    det *= -VALUE(4, 1);

    for (int i = 0; i < 16; ++i) {
        inverse.data_[i] = inverse.data_[i] / det;
    }

    return inverse;
}

Vec4 operator *(const Matrix4x4 &lhs, const Vec4 &rhs) {
    Vec4 result;
    result.x = lhs.data_[0] * rhs.x + lhs.data_[4] * rhs.y + lhs.data_[8] * rhs.z + lhs.data_[12] * rhs.w;
    result.y = lhs.data_[1] * rhs.x + lhs.data_[5] * rhs.y + lhs.data_[9] * rhs.z + lhs.data_[13] * rhs.w;
    result.z = lhs.data_[2] * rhs.x + lhs.data_[6] * rhs.y + lhs.data_[10] * rhs.z + lhs.data_[14] * rhs.w;
    result.w = lhs.data_[3] * rhs.x + lhs.data_[7] * rhs.y + lhs.data_[11] * rhs.z + lhs.data_[15] * rhs.w;
    return result;
}