#ifndef matrix_H
#define matrix_H

#include <iostream>
using std::cout;
using std::endl;
#include "Vec3D.h"

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

Vec3Df calculateMatrix(float mat[3][3], Vec3Df vec) {
	Vec3Df res;
	res[0] = mat[0][0] * vec[0] + mat[0][1] * vec[1] + mat[0][2] * vec[2];
	res[1] = mat[1][0] * vec[0] + mat[1][1] * vec[1] + mat[1][2] * vec[2];
	res[2] = mat[2][0] * vec[0] + mat[2][1] * vec[1] + mat[2][2] * vec[2];

	return res;
}

Vec3Df rotateMatrixX(float angle, Vec3Df vec) {
	float mat[3][3] = {
		{ 1, 0, 0 },
		{ 0, std::cos(angle), -1 * std::sin(angle) },
		{ 0, std::sin(angle), std::cos(angle) }
	};

	return calculateMatrix(mat, vec);
}

Vec3Df rotateMatrixY(float angle, Vec3Df vec) {
	float mat[3][3] = {
		{ std::cos(angle), 0, std::sin(angle) },
		{ 0, 1, 0 },
		{ -1 * std::sin(angle), 0, std::cos(angle) }
	};

	return calculateMatrix(mat, vec);
}

Vec3Df rotateMatrixZ(float angle, Vec3Df vec) {
	float mat[3][3] = {
		{ std::cos(angle), -1 * std::sin(angle), 0 },
		{ std::sin(angle), std::cos(angle), 0 },
		{ 0, 0, 1 }
	};

	return calculateMatrix(mat, vec);
}
#endif
