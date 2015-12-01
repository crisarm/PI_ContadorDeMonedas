
#include "stdafx.h"
#include "Structures.h"
#include <math.h>

// OPERACIONES A IMPLEMENTAR
float Dot( VECTOR4D &A, VECTOR4D &B )
{
	float pd = 0;
	for( int i=0; i<4; i++ ) 
		pd += A.vector[i] * B.vector[i];
	return pd;
}
VECTOR4D Cross3( VECTOR4D &A, VECTOR4D &B ) //w=0
{
	VECTOR4D cp;
	for( int i=0; i<3; i++ ) 
		cp.vector[i] = A.vector[ i+1%3 ] * B.vector[ i+2%3 ] -
					   A.vector[ i+2%3 ] * B.vector[ i+1%3 ];
	return cp;
}
VECTOR4D operator*( VECTOR4D &A, VECTOR4D &B )
{
	VECTOR4D v;
	for( int i=0; i<4; i++ ) 
		v.vector[i] = A.vector[ i ] * B.vector[ i ];
	return v;
}
VECTOR4D operator*( VECTOR4D &A, float s )
{
	VECTOR4D v;
	for( int i=0; i<4; i++ ) 
		v.vector[i] = A.vector[ i ] * s;
	return v;
}
VECTOR4D operator*( float s, VECTOR4D &A )
{
	VECTOR4D v;
	for( int i=0; i<4; i++ ) 
		v.vector[i] = A.vector[ i ] * s;
	return v;
}
VECTOR4D operator+( VECTOR4D &A, VECTOR4D &B )
{
	VECTOR4D v;
	for( int i=0; i<4; i++ ) 
		v.vector[i] = A.vector[ i ] + B.vector[ i ];
	return v;
}
float SqMag( VECTOR4D &A ) // Dot(A,A);
{
	return Dot( A, A );
}

// OPERACIONES CON MATRICES Y VECTORES A IMPLEMENTAR
MATRIX4D Identy(){
	MATRIX4D im=					  {  1,  0,  0,  0,
										 0,  1,  0,  0,
										 0,  0,  1,  0,
										 0,  0,  0,  1  };
	return im;
}
MATRIX4D Zero(){
	MATRIX4D im=					  {  0,  0,  0,  0,
										 0,  0,  0,  0,
										 0,  0,  0,  0,
										 0,  0,  0,  0  };
	return im;
}
//MATRIX4D RotationAxis( VECTOR4D Axis, float theta); //Cuaternios (Opcional)
MATRIX4D RotationX( float theta ) //Left Handled
{
	MATRIX4D rm = Identy();
	rm.m11 = cos(theta);	rm.m12 = sin(theta);
	rm.m21 = -rm.m12;		rm.m22 = rm.m11;
	return rm;
}
MATRIX4D RotationY( float theta ) //Left Handled
{
	MATRIX4D rm = Identy();
	rm.m00 = cos(theta);	rm.m02 = -sin(theta);
	rm.m20 = -rm.m02;		rm.m22 = rm.m00;
	return rm;
}
MATRIX4D RotationZ( float theta ) //Left Handled
{
	MATRIX4D rm = Identy();
	rm.m00 = cos(theta);	rm.m01 = sin(theta);
	rm.m10 = -rm.m01;		rm.m11 = rm.m00;
	return rm;
}
MATRIX4D operator*( MATRIX4D &A, MATRIX4D &B ){
	MATRIX4D nm = Zero();
	for (int j = 0; j < 4; j++)
	for (int i = 0; i < 4; i++)
	for (int k = 0; k < 4; k++)
		nm.m[j][i] += A.m[j][k] * B.m[k][i];
	return nm;
}
VECTOR4D operator*( VECTOR4D &V, MATRIX4D &M )
{
	VECTOR4D product;

	for ( int i = 0; i < 4; i++ ) {
		product.vector[i] = 0;
		for ( int j = 0; j < 4; j++ )
			product.vector[i] += M.m[i][j] * V.vector[j];
	}
  return product;
}
VECTOR4D operator*( MATRIX4D &M, VECTOR4D &V )
{
	VECTOR4D product;

	for ( int i = 0; i < 4; i++ ) {
		product.vector[i] = 0;
		for ( int j = 0; j < 4; j++ )
			product.vector[i] += M.m[i][j] * V.vector[j];
	}
  return product;
}
MATRIX4D Transpose( MATRIX4D &M ){
	MATRIX4D nm;
	for (int j = 0; j < 4; j++)
	for (int i = 0; i < 4; i++)
		nm.m[j][i] = M.m[i][j];
	return nm;
}

MATRIX4D mul(MATRIX4D& M, float s){
	MATRIX4D nm;
	for( int i=0; i<16; i++ )
		nm.v[i] = M.v[i] * s;
	return nm;
}

MATRIX4D Inverse( MATRIX4D &M ){
		double inv[16], det;
    int i;

    inv[0] = M.v[5]  * M.v[10] * M.v[15] - 
             M.v[5]  * M.v[11] * M.v[14] - 
             M.v[9]  * M.v[6]  * M.v[15] + 
             M.v[9]  * M.v[7]  * M.v[14] +
             M.v[13] * M.v[6]  * M.v[11] - 
             M.v[13] * M.v[7]  * M.v[10];

    inv[4] = -M.v[4]  * M.v[10] * M.v[15] + 
              M.v[4]  * M.v[11] * M.v[14] + 
              M.v[8]  * M.v[6]  * M.v[15] - 
              M.v[8]  * M.v[7]  * M.v[14] - 
              M.v[12] * M.v[6]  * M.v[11] + 
              M.v[12] * M.v[7]  * M.v[10];

    inv[8] = M.v[4]  * M.v[9] * M.v[15] - 
             M.v[4]  * M.v[11] * M.v[13] - 
             M.v[8]  * M.v[5] * M.v[15] + 
             M.v[8]  * M.v[7] * M.v[13] + 
             M.v[12] * M.v[5] * M.v[11] - 
             M.v[12] * M.v[7] * M.v[9];

    inv[12] = -M.v[4]  * M.v[9] * M.v[14] + 
               M.v[4]  * M.v[10] * M.v[13] +
               M.v[8]  * M.v[5] * M.v[14] - 
               M.v[8]  * M.v[6] * M.v[13] - 
               M.v[12] * M.v[5] * M.v[10] + 
               M.v[12] * M.v[6] * M.v[9];

    inv[1] = -M.v[1]  * M.v[10] * M.v[15] + 
              M.v[1]  * M.v[11] * M.v[14] + 
              M.v[9]  * M.v[2] * M.v[15] - 
              M.v[9]  * M.v[3] * M.v[14] - 
              M.v[13] * M.v[2] * M.v[11] + 
              M.v[13] * M.v[3] * M.v[10];

    inv[5] = M.v[0]  * M.v[10] * M.v[15] - 
             M.v[0]  * M.v[11] * M.v[14] - 
             M.v[8]  * M.v[2] * M.v[15] + 
             M.v[8]  * M.v[3] * M.v[14] + 
             M.v[12] * M.v[2] * M.v[11] - 
             M.v[12] * M.v[3] * M.v[10];

    inv[9] = -M.v[0]  * M.v[9] * M.v[15] + 
              M.v[0]  * M.v[11] * M.v[13] + 
              M.v[8]  * M.v[1] * M.v[15] - 
              M.v[8]  * M.v[3] * M.v[13] - 
              M.v[12] * M.v[1] * M.v[11] + 
              M.v[12] * M.v[3] * M.v[9];

    inv[13] = M.v[0]  * M.v[9] * M.v[14] - 
              M.v[0]  * M.v[10] * M.v[13] - 
              M.v[8]  * M.v[1] * M.v[14] + 
              M.v[8]  * M.v[2] * M.v[13] + 
              M.v[12] * M.v[1] * M.v[10] - 
              M.v[12] * M.v[2] * M.v[9];

    inv[2] = M.v[1]  * M.v[6] * M.v[15] - 
             M.v[1]  * M.v[7] * M.v[14] - 
             M.v[5]  * M.v[2] * M.v[15] + 
             M.v[5]  * M.v[3] * M.v[14] + 
             M.v[13] * M.v[2] * M.v[7] - 
             M.v[13] * M.v[3] * M.v[6];

    inv[6] = -M.v[0]  * M.v[6] * M.v[15] + 
              M.v[0]  * M.v[7] * M.v[14] + 
              M.v[4]  * M.v[2] * M.v[15] - 
              M.v[4]  * M.v[3] * M.v[14] - 
              M.v[12] * M.v[2] * M.v[7] + 
              M.v[12] * M.v[3] * M.v[6];

    inv[10] = M.v[0]  * M.v[5] * M.v[15] - 
              M.v[0]  * M.v[7] * M.v[13] - 
              M.v[4]  * M.v[1] * M.v[15] + 
              M.v[4]  * M.v[3] * M.v[13] + 
              M.v[12] * M.v[1] * M.v[7] - 
              M.v[12] * M.v[3] * M.v[5];

    inv[14] = -M.v[0]  * M.v[5] * M.v[14] + 
               M.v[0]  * M.v[6] * M.v[13] + 
               M.v[4]  * M.v[1] * M.v[14] - 
               M.v[4]  * M.v[2] * M.v[13] - 
               M.v[12] * M.v[1] * M.v[6] + 
               M.v[12] * M.v[2] * M.v[5];

    inv[3] = -M.v[1] * M.v[6] * M.v[11] + 
              M.v[1] * M.v[7] * M.v[10] + 
              M.v[5] * M.v[2] * M.v[11] - 
              M.v[5] * M.v[3] * M.v[10] - 
              M.v[9] * M.v[2] * M.v[7] + 
              M.v[9] * M.v[3] * M.v[6];

    inv[7] = M.v[0] * M.v[6] * M.v[11] - 
             M.v[0] * M.v[7] * M.v[10] - 
             M.v[4] * M.v[2] * M.v[11] + 
             M.v[4] * M.v[3] * M.v[10] + 
             M.v[8] * M.v[2] * M.v[7] - 
             M.v[8] * M.v[3] * M.v[6];

    inv[11] = -M.v[0] * M.v[5] * M.v[11] + 
               M.v[0] * M.v[7] * M.v[9] + 
               M.v[4] * M.v[1] * M.v[11] - 
               M.v[4] * M.v[3] * M.v[9] - 
               M.v[8] * M.v[1] * M.v[7] + 
               M.v[8] * M.v[3] * M.v[5];

    inv[15] = M.v[0] * M.v[5] * M.v[10] - 
              M.v[0] * M.v[6] * M.v[9] - 
              M.v[4] * M.v[1] * M.v[10] + 
              M.v[4] * M.v[2] * M.v[9] + 
              M.v[8] * M.v[1] * M.v[6] - 
              M.v[8] * M.v[2] * M.v[5];

    det = M.v[0] * inv[0] + M.v[1] * inv[4] + M.v[2] * inv[8] + M.v[3] * inv[12];

    if (det == 0)
        return Zero();

    det = 1.0 / det;
	MATRIX4D I;
    for (i = 0; i < 16; i++)
        I.v[i] = inv[i] * det;

    return I;
}

MATRIX4D Translation( float dx, float dy, float dz){
		MATRIX4D newM = Identy();
		newM.m30 = dx;		newM.m31 = dy;		newM.m32 = dz;
		return newM;
}
MATRIX4D Scaling( float sx, float sy, float sz){
		MATRIX4D newM = Zero();
		newM.m00 = sx;		newM.m11 = sy;		newM.m22 = sz;		newM.m33 = 1;
		return newM;
}


MATRIX4D KernelIdentity()
{
	MATRIX4D ki =					  {  0,  0,  0,  0,
										 0,  1,  0,  0,
										 0,  0,  0,  0,
										 0,  0,  0,  0  };
	return ki;
}
MATRIX4D KernelInverse()
{
	MATRIX4D ki =					  {  0,  0,  0,  0,
										 0, -1,  0,  0,
										 0,  0,  0,  0,
										 0,  0,  0,  0  };
	return ki;
}
MATRIX4D KernelSoft()
{
	MATRIX4D ks =					  {  1/9.0f,  1/9.0f,  1/9.0f,  0,
										 1/9.0f,  1/9.0f,  1/9.0f,  0,
										 1/9.0f,  1/9.0f,  1/9.0f,  0,
										      0,       0,       0,  0  };
	return ks;
}
MATRIX4D KernelLaplace()
{
	MATRIX4D kl =					  {       0, -1/8.0f,       0,  0,
										-1/8.0f,  1/2.0f, -1/8.0f,  0,
										      0, -1/8.0f,       0,  0,
										      0,       0,       0,  0  };
	return kl;
}
MATRIX4D KernelEmbossVert()
{
	MATRIX4D ke =					  { -1, -1, -1,  0,
										 0,  0,  0,  0,
										 1,  1,  1,  0,
										 0,  0,  0,  0  };
	return ke;
}
MATRIX4D KernelEmbossHoriz()
{
	MATRIX4D ke =					  { -1,  0,  1,  0,
										-1,  0,  1,  0,
										-1,  0,  1,  0,
										 0,  0,  0,  0  };
	return ke;
}
MATRIX4D KernelSharp( float c )
{
	MATRIX4D ks =					  {  0,  c,  0,  0,
										 c,  1, -c,  0,
										 0, -c,  0,  0,
										 0,  0,  0,  0  };
	return ks;
}