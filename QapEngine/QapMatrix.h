/*------------------------------------------*/
/*   {<|author : Adler3D(Kashin Ivan) |>   }*/
/*   {<|e-mail : Adler3D@gmail.com    |>   }*/
/*   {<|site   : Adler3d.narod.ru     |>   }*/
/*------------------------------------------*/
#pragma once
#include <d3d9.h>
#include <math.h>
/************************************************************************/
/* Объявления                                                           */
/************************************************************************/
struct QapMat4:public D3DMATRIX{
	QapMat4(){}
	QapMat4(const float m00, const float m01, const float m02, const float m03,
		 const float m10, const float m11, const float m12, const float m13,
		 const float m20, const float m21, const float m22, const float m23,
		 const float m30, const float m31, const float m32, const float m33){
    #define f(a,b){m[a][b]=m##a##b;}
    #define F(a){f(a,3);f(a,1);f(a,0);f(a,2);};
      F(3);F(2);F(0);F(1);
    #undef F
    #undef f
     //D3DXMATRIX
	}
};
QapMat4 operator + (const QapMat4 &m, const QapMat4 &n);
QapMat4 operator - (const QapMat4 &m, const QapMat4 &n);
QapMat4 operator - (const QapMat4 &m);
QapMat4 operator * (const QapMat4 &m, const QapMat4 &n);
QapMat4 operator * (const QapMat4 &m, const float x);
QapMat4 Transpose(const QapMat4 &m);
/* --------------------------------------------------------------------------------- */
/*QapMat4 RotateX(const float angle);
QapMat4 RotateY(const float angle);*/
QapMat4 RotateZ(const float angle);/*
QapMat4 RotateXY(const float angleX, const float angleY);
QapMat4 RotateYX(const float angleX, const float angleY);
QapMat4 RotateZXY(const float angleX, const float angleY, const float angleZ);*/
QapMat4 Translate(const float x, const float y, const float z);
QapMat4 Scale(const float x, const float y, const float z);
QapMat4 Identity4();
/************************************************************************/
/* Конец                                                                */
/************************************************************************/
