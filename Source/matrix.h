#ifndef matrix_H
#define matrix_H

#include <iostream>
using std::cout;
using std::endl;
#include "Vec3D.h"
#include <vector>

/// Matrice identite
const GLdouble identity[] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};


/// Affichage
void printMatrix( const GLdouble* m )
{
	for( int i=0; i<16; ++i ) cout<<m[i]<<" ";
}


/// Projection q=Mp
void project( const GLdouble* m, const GLdouble* p, GLdouble* q )
{
	for( int i=0; i<3; ++i ){
		q[i] = 0;
		for( int j=0; j<4; ++j )
			q[i] += m[i+4*j]*p[j];
	}
	q[3] = p[3];
}

void project(const GLdouble* m, const GLdouble* p, GLfloat* q)
{
	for (int i = 0; i<3; ++i){
		q[i] = 0;
		for (int j = 0; j<4; ++j)
			q[i] += m[i + 4 * j] * p[j];
	}
	q[3] = p[3];
}


/// Inversion
void inverse( const double *m, double *p )
{
	{   // transposition de la rotation
		for(int i=0; i<3; ++i )
			for(int j=0; j<3; ++j )
				p[i+4*j] = m[j+4*i];
	}

	{   // calcul de la translation
		double q[3]={0,0,0};
		for(int i=0; i<3; ++i ){
			for(int j=0; j<3; ++j )
				q[i] -= p[i+4*j]*m[12+j];
		}
		p[12]=q[0]; p[13]=q[1]; p[14]=q[2];
	}

	// derniere ligne
	p[3] = p[7] = p[11] = 0; p[15] = 1;
}

Vec3Df calculateMatrix(std::vector<std::vector<float>> mat, Vec3Df vec) {
	Vec3Df res;
	res[0] = mat[0][0] * vec[0] + mat[0][1] * vec[1] + mat[0][2] * vec[2];
	res[1] = mat[1][0] * vec[0] + mat[1][1] * vec[1] + mat[1][2] * vec[2];
	res[2] = mat[2][0] * vec[0] + mat[2][1] * vec[1] + mat[2][2] * vec[2];

	return res;
}

std::vector<std::vector<float>> rotateMatrixX(float angle) {
	std::vector<float> r1 = { 1, 0, 0 };
	std::vector<float> r2 = { 0, std::cos(angle), -1 * std::sin(angle) };
	std::vector<float> r3 = { 0, std::sin(angle), std::cos(angle) };

	std::vector<std::vector<float>> mat = { r1, r2, r3 };

	return mat;
}

std::vector<std::vector<float>> rotateMatrixY(float angle) {
	std::vector<float> r1 = { std::cos(angle), 0, std::sin(angle) };
	std::vector<float> r2 = { 0, 1, 0 };
	std::vector<float> r3 = { -1 * std::sin(angle), 0, std::cos(angle) };

	std::vector<std::vector<float>> mat = { r1, r2, r3 };

	return mat;
}

std::vector<std::vector<float>> rotateMatrixZ(float angle) {
	std::vector<float> r1 = { std::cos(angle), -1 * std::sin(angle), 0 };
	std::vector<float> r2 = { std::sin(angle), std::cos(angle), 0 };
	std::vector<float> r3 = { 0, 0, 1 };

	std::vector<std::vector<float>> mat = { r1, r2, r3 };

	return mat;
}

std::vector<std::vector<float>> matrixMultiplication(std::vector<std::vector<float>> mat1, std::vector<std::vector<float>> mat2) {
	float res11 = Vec3Df::dotProduct(Vec3Df(mat1[0][0], mat1[0][1], mat1[0][2]), Vec3Df(mat2[0][0], mat2[1][0], mat2[2][0]));
	float res12 = Vec3Df::dotProduct(Vec3Df(mat1[0][0], mat1[0][1], mat1[0][2]), Vec3Df(mat2[0][1], mat2[1][1], mat2[2][1]));
	float res13 = Vec3Df::dotProduct(Vec3Df(mat1[0][0], mat1[0][1], mat1[0][2]), Vec3Df(mat2[0][2], mat2[1][2], mat2[2][2]));

	float res21 = Vec3Df::dotProduct(Vec3Df(mat1[1][0], mat1[1][1], mat1[1][2]), Vec3Df(mat2[0][0], mat2[1][0], mat2[2][0]));
	float res22 = Vec3Df::dotProduct(Vec3Df(mat1[1][0], mat1[1][1], mat1[1][2]), Vec3Df(mat2[0][1], mat2[1][1], mat2[2][1]));
	float res23 = Vec3Df::dotProduct(Vec3Df(mat1[1][0], mat1[1][1], mat1[1][2]), Vec3Df(mat2[0][2], mat2[1][2], mat2[2][2]));

	float res31 = Vec3Df::dotProduct(Vec3Df(mat1[2][0], mat1[2][1], mat1[2][2]), Vec3Df(mat2[0][0], mat2[1][0], mat2[2][0]));
	float res32 = Vec3Df::dotProduct(Vec3Df(mat1[2][0], mat1[2][1], mat1[2][2]), Vec3Df(mat2[0][1], mat2[1][1], mat2[2][1]));
	float res33 = Vec3Df::dotProduct(Vec3Df(mat1[2][0], mat1[2][1], mat1[2][2]), Vec3Df(mat2[0][2], mat2[1][2], mat2[2][2]));

	std::vector<float> r1 = { res11, res12, res13 };
	std::vector<float> r2 = { res21, res22, res23 };
	std::vector<float> r3 = { res31, res32, res33 };
	
	std::vector<std::vector<float>> mat = { r1, r2, r3 };

	return mat;
}
#endif
;