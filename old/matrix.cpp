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

    inverse.data_[0]  =  mat.data_[5] * mat.data_[10] * mat.data_[15] - mat.data_[5] * mat.data_[11] * mat.data_[14] - mat.data_[9] * mat.data_[6] * mat.data_[15] + mat.data_[9] * mat.data_[7] * mat.data_[14] + mat.data_[13] * mat.data_[6] * mat.data_[11] - mat.data_[13] * mat.data_[7] * mat.data_[10];
    inverse.data_[4]  = -mat.data_[4] * mat.data_[10] * mat.data_[15] + mat.data_[4] * mat.data_[11] * mat.data_[14] + mat.data_[8] * mat.data_[6] * mat.data_[15] - mat.data_[8] * mat.data_[7] * mat.data_[14] - mat.data_[12] * mat.data_[6] * mat.data_[11] + mat.data_[12] * mat.data_[7] * mat.data_[10];
    inverse.data_[8]  =  mat.data_[4] * mat.data_[9]  * mat.data_[15] - mat.data_[4] * mat.data_[11] * mat.data_[13] - mat.data_[8] * mat.data_[5] * mat.data_[15] + mat.data_[8] * mat.data_[7] * mat.data_[13] + mat.data_[12] * mat.data_[5] * mat.data_[11] - mat.data_[12] * mat.data_[7] * mat.data_[9];
    inverse.data_[12] = -mat.data_[4] * mat.data_[9]  * mat.data_[14] + mat.data_[4] * mat.data_[10] * mat.data_[13] + mat.data_[8] * mat.data_[5] * mat.data_[14] - mat.data_[8] * mat.data_[6] * mat.data_[13] - mat.data_[12] * mat.data_[5] * mat.data_[10] + mat.data_[12] * mat.data_[6] * mat.data_[9];
    inverse.data_[1]  = -mat.data_[1] * mat.data_[10] * mat.data_[15] + mat.data_[1] * mat.data_[11] * mat.data_[14] + mat.data_[9] * mat.data_[2] * mat.data_[15] - mat.data_[9] * mat.data_[3] * mat.data_[14] - mat.data_[13] * mat.data_[2] * mat.data_[11] + mat.data_[13] * mat.data_[3] * mat.data_[10];
    inverse.data_[5]  =  mat.data_[0] * mat.data_[10] * mat.data_[15] - mat.data_[0] * mat.data_[11] * mat.data_[14] - mat.data_[8] * mat.data_[2] * mat.data_[15] + mat.data_[8] * mat.data_[3] * mat.data_[14] + mat.data_[12] * mat.data_[2] * mat.data_[11] - mat.data_[12] * mat.data_[3] * mat.data_[10];
    inverse.data_[9]  = -mat.data_[0] * mat.data_[9]  * mat.data_[15] + mat.data_[0] * mat.data_[11] * mat.data_[13] + mat.data_[8] * mat.data_[1] * mat.data_[15] - mat.data_[8] * mat.data_[3] * mat.data_[13] - mat.data_[12] * mat.data_[1] * mat.data_[11] + mat.data_[12] * mat.data_[3] * mat.data_[9];
    inverse.data_[13] =  mat.data_[0] * mat.data_[9]  * mat.data_[14] - mat.data_[0] * mat.data_[10] * mat.data_[13] - mat.data_[8] * mat.data_[1] * mat.data_[14] + mat.data_[8] * mat.data_[2] * mat.data_[13] + mat.data_[12] * mat.data_[1] * mat.data_[10] - mat.data_[12] * mat.data_[2] * mat.data_[9];
    inverse.data_[2]  =  mat.data_[1] * mat.data_[6]  * mat.data_[15] - mat.data_[1] * mat.data_[7]  * mat.data_[14] - mat.data_[5] * mat.data_[2] * mat.data_[15] + mat.data_[5] * mat.data_[3] * mat.data_[14] + mat.data_[13] * mat.data_[2] * mat.data_[7]  - mat.data_[13] * mat.data_[3] * mat.data_[6];
    inverse.data_[6]  = -mat.data_[0] * mat.data_[6]  * mat.data_[15] + mat.data_[0] * mat.data_[7]  * mat.data_[14] + mat.data_[4] * mat.data_[2] * mat.data_[15] - mat.data_[4] * mat.data_[3] * mat.data_[14] - mat.data_[12] * mat.data_[2] * mat.data_[7]  + mat.data_[12] * mat.data_[3] * mat.data_[6];
    inverse.data_[10] =  mat.data_[0] * mat.data_[5]  * mat.data_[15] - mat.data_[0] * mat.data_[7]  * mat.data_[13] - mat.data_[4] * mat.data_[1] * mat.data_[15] + mat.data_[4] * mat.data_[3] * mat.data_[13] + mat.data_[12] * mat.data_[1] * mat.data_[7]  - mat.data_[12] * mat.data_[3] * mat.data_[5];
    inverse.data_[14] = -mat.data_[0] * mat.data_[5]  * mat.data_[14] + mat.data_[0] * mat.data_[6]  * mat.data_[13] + mat.data_[4] * mat.data_[1] * mat.data_[14] - mat.data_[4] * mat.data_[2] * mat.data_[13] - mat.data_[12] * mat.data_[1] * mat.data_[6]  + mat.data_[12] * mat.data_[2] * mat.data_[5];
    inverse.data_[3]  = -mat.data_[1] * mat.data_[6]  * mat.data_[11] + mat.data_[1] * mat.data_[7]  * mat.data_[10] + mat.data_[5] * mat.data_[2] * mat.data_[11] - mat.data_[5] * mat.data_[3] * mat.data_[10] - mat.data_[9]  * mat.data_[2] * mat.data_[7]  + mat.data_[9]  * mat.data_[3] * mat.data_[6];
    inverse.data_[7]  =  mat.data_[0] * mat.data_[6]  * mat.data_[11] - mat.data_[0] * mat.data_[7]  * mat.data_[10] - mat.data_[4] * mat.data_[2] * mat.data_[11] + mat.data_[4] * mat.data_[3] * mat.data_[10] + mat.data_[8]  * mat.data_[2] * mat.data_[7]  - mat.data_[8]  * mat.data_[3] * mat.data_[6];
    inverse.data_[11] = -mat.data_[0] * mat.data_[5]  * mat.data_[11] + mat.data_[0] * mat.data_[7]  * mat.data_[9]  + mat.data_[4] * mat.data_[1] * mat.data_[11] - mat.data_[4] * mat.data_[3] * mat.data_[9]  - mat.data_[8]  * mat.data_[1] * mat.data_[7]  + mat.data_[8]  * mat.data_[3] * mat.data_[5];
    inverse.data_[15] =  mat.data_[0] * mat.data_[5]  * mat.data_[10] - mat.data_[0] * mat.data_[6]  * mat.data_[9]  - mat.data_[4] * mat.data_[1] * mat.data_[10] + mat.data_[4] * mat.data_[2] * mat.data_[9]  + mat.data_[8]  * mat.data_[1] * mat.data_[6]  - mat.data_[8]  * mat.data_[2] * mat.data_[5];

    float det = mat.data_[0] * inverse.data_[0] + mat.data_[1] * inverse.data_[4] + mat.data_[2] * inverse.data_[8] + mat.data_[3] * inverse.data_[12];

    for (int i = 0; i < 16; ++i) {
        inverse.data_[i] = inverse.data_[i] / det;
    }

    return inverse;
}

Matrix4x4 operator *(const Matrix4x4 &lhs, const Matrix4x4 &rhs) {
    Matrix4x4 result;
    result.data_[0] = lhs.data_[0] * rhs.data_[0] + lhs.data_[4] * rhs.data_[1] 
                    + lhs.data_[8] * rhs.data_[2] + lhs.data_[12] * rhs.data_[3];
    result.data_[1] = lhs.data_[1] * rhs.data_[0] + lhs.data_[5] * rhs.data_[1] 
                    + lhs.data_[9] * rhs.data_[2] + lhs.data_[13] * rhs.data_[3];
    result.data_[2] = lhs.data_[2] * rhs.data_[0] + lhs.data_[6] * rhs.data_[1] 
                    + lhs.data_[10] * rhs.data_[2] + lhs.data_[14] * rhs.data_[3];
    result.data_[3] = lhs.data_[3] * rhs.data_[0] + lhs.data_[7] * rhs.data_[1] 
                    + lhs.data_[9] * rhs.data_[2] + lhs.data_[15] * rhs.data_[3];

    result.data_[4] = lhs.data_[0] * rhs.data_[4] + lhs.data_[4] * rhs.data_[5] 
                    + lhs.data_[8] * rhs.data_[6] + lhs.data_[12] * rhs.data_[7];
    result.data_[5] = lhs.data_[1] * rhs.data_[4] + lhs.data_[5] * rhs.data_[5] 
                    + lhs.data_[9] * rhs.data_[6] + lhs.data_[13] * rhs.data_[7];
    result.data_[6] = lhs.data_[2] * rhs.data_[4] + lhs.data_[6] * rhs.data_[5] 
                    + lhs.data_[10] * rhs.data_[6] + lhs.data_[14] * rhs.data_[7];
    result.data_[7] = lhs.data_[3] * rhs.data_[4] + lhs.data_[7] * rhs.data_[5] 
                    + lhs.data_[9] * rhs.data_[6] + lhs.data_[15] * rhs.data_[7];

    result.data_[8] = lhs.data_[0] * rhs.data_[8] + lhs.data_[4] * rhs.data_[9] 
                    + lhs.data_[8] * rhs.data_[10] + lhs.data_[12] * rhs.data_[11];
    result.data_[9] = lhs.data_[1] * rhs.data_[8] + lhs.data_[5] * rhs.data_[9] 
                    + lhs.data_[9] * rhs.data_[10] + lhs.data_[13] * rhs.data_[11];
    result.data_[10] = lhs.data_[2] * rhs.data_[8] + lhs.data_[6] * rhs.data_[9] 
                    + lhs.data_[10] * rhs.data_[10] + lhs.data_[13] * rhs.data_[11];
    result.data_[11] = lhs.data_[3] * rhs.data_[8] + lhs.data_[7] * rhs.data_[9] 
                    + lhs.data_[11] * rhs.data_[10] + lhs.data_[15] * rhs.data_[11];

    result.data_[12] = lhs.data_[0] * rhs.data_[12] + lhs.data_[4] * rhs.data_[13] 
                    + lhs.data_[8] * rhs.data_[14] + lhs.data_[12] * rhs.data_[15];
    result.data_[13] = lhs.data_[1] * rhs.data_[12] + lhs.data_[5] * rhs.data_[13] 
                    + lhs.data_[9] * rhs.data_[14] + lhs.data_[13] * rhs.data_[15];
    result.data_[14] = lhs.data_[2] * rhs.data_[12] + lhs.data_[6] * rhs.data_[13] 
                    + lhs.data_[10] * rhs.data_[14] + lhs.data_[13] * rhs.data_[15];
    result.data_[15] = lhs.data_[3] * rhs.data_[12] + lhs.data_[7] * rhs.data_[13] 
                    + lhs.data_[11] * rhs.data_[14] + lhs.data_[15] * rhs.data_[15];
    return result;
}

Vec4 operator *(const Matrix4x4 &lhs, const Vec4 &rhs) {
    Vec4 result;
    result.x = lhs.data_[0] * rhs.x + lhs.data_[4] * rhs.y + lhs.data_[8] * rhs.z + lhs.data_[12] * rhs.w;
    result.y = lhs.data_[1] * rhs.x + lhs.data_[5] * rhs.y + lhs.data_[9] * rhs.z + lhs.data_[13] * rhs.w;
    result.z = lhs.data_[2] * rhs.x + lhs.data_[6] * rhs.y + lhs.data_[10] * rhs.z + lhs.data_[14] * rhs.w;
    result.w = lhs.data_[3] * rhs.x + lhs.data_[7] * rhs.y + lhs.data_[11] * rhs.z + lhs.data_[15] * rhs.w;
    return result;
}