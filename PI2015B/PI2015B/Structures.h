#pragma once
#include "stdafx.h"
struct VECTOR4D
{
	union
	{
		struct
		{
			float x, y, z, w;
		};
		float vector[4];
	};
};

struct MATRIX4D
{
	union
	{
		struct
		{
			float m00, m01, m02, m03;
			float m10, m11, m12, m13;
			float m20, m21, m22, m23;
			float m30, m31, m32, m33;
		};
		float m[4][4];
		float v[16];
	};
};

// OPERACIONES A IMPLEMENTAR
float Dot( VECTOR4D &A, VECTOR4D &B );
VECTOR4D Cross3( VECTOR4D &A, VECTOR4D &B ); //w=0
VECTOR4D operator*( VECTOR4D &A, VECTOR4D &B ); 
VECTOR4D operator*( VECTOR4D &A, float s ); 
VECTOR4D operator*( float s, VECTOR4D &A ); 
VECTOR4D operator+( VECTOR4D &A, VECTOR4D &B ); 
float SqMag( VECTOR4D &A ); // Dot(A,A);

// OPERACIONES CON MATRICES Y VECTORES A IMPLEMENTAR
MATRIX4D Identy();
MATRIX4D Zero();
//MATRIX4D RotationAxis( VECTOR4D Axis, float theta); //Cuaternios (Opcional)
MATRIX4D RotationX( float theta ); //Left Handled
MATRIX4D RotationY( float theta ); //Left Handled
MATRIX4D RotationZ( float theta ); //Left Handled
MATRIX4D operator*( MATRIX4D &A, MATRIX4D &B );
//MATRIX4D operator*( VECTOR4D &V, MATRIX4D &M );
//MATRIX4D operator*( MATRIX4D &M, VECTOR4D &V );
VECTOR4D operator*( VECTOR4D &V, MATRIX4D &M );
VECTOR4D operator*( MATRIX4D &M, VECTOR4D &V );
MATRIX4D Transpose( MATRIX4D &M );
float Det( MATRIX4D &M ); //Determinante (Opcional)
MATRIX4D Inverse( MATRIX4D &M );

MATRIX4D Translation( float dx, float dy, float dz);
MATRIX4D Scaling( float sx, float sy, float sz);

MATRIX4D KernelIdentity(); // C=0 o cualquier valor
MATRIX4D KernelInverse(); // C=1
MATRIX4D KernelSoft();  // C=0
MATRIX4D KernelLaplace();  // C=0.5
MATRIX4D KernelEmbossVert();  // C=0.5
MATRIX4D KernelEmbossHoriz();  // C=0.5
MATRIX4D KernelSharp( float c );  // C=0.5