#include "stdafx.h"
#include "QapMatrix.h"

/*
void QapMat4::Translate(const vec3 &v){
	rows[0].w += dot(rows[0].xyz(), v);
	rows[1].w += dot(rows[1].xyz(), v);
	rows[2].w += dot(rows[2].xyz(), v);
	rows[3].w += dot(rows[3].xyz(), v);
}*/

QapMat4 operator + (const QapMat4 &m, const QapMat4 &n){
  QapMat4 O; for(int i=0;i<4;i++)for(int j=0;j<4;j++)O.m[i][j]=m.m[i][j]+n.m[i][j]; return O;
}

QapMat4 operator - (const QapMat4 &m, const QapMat4 &n){
  QapMat4 O; for(int i=0;i<4;i++)for(int j=0;j<4;j++)O.m[i][j]=m.m[i][j]-n.m[i][j]; return O;
}

QapMat4 operator - (const QapMat4 &m){
	QapMat4 O; for(int i=0;i<4;i++)for(int j=0;j<4;j++)O.m[i][j]=-m.m[i][j]; return O;
}

QapMat4 operator * (const QapMat4 &m, const QapMat4 &n){
	#define rcDot4(r,c)(m.m[r][0]*n.m[0][c]+m.m[r][1]*n.m[1][c]+m.m[r][2]*n.m[2][c]+m.m[r][3]*n.m[3][c])
	return QapMat4(
		rcDot4(0,0),rcDot4(0,1),rcDot4(0,2),rcDot4(0,3),
		rcDot4(1,0),rcDot4(1,1),rcDot4(1,2),rcDot4(1,3),
		rcDot4(2,0),rcDot4(2,1),rcDot4(2,2),rcDot4(2,3),
		rcDot4(3,0),rcDot4(3,1),rcDot4(3,2),rcDot4(3,3)
  );
	#undef rcDot
}

QapMat4 operator * (const QapMat4 &m, const float x){
  QapMat4 O; for(int i=0;i<4;i++)for(int j=0;j<4;j++)O.m[i][j]=x*m.m[i][j]; return O;
}

QapMat4 Transpose(const QapMat4 &m){
	return QapMat4(
		m.m[0][0],m.m[1][0],m.m[2][0],m.m[3][0],
		m.m[0][1],m.m[1][1],m.m[2][1],m.m[3][1],
		m.m[0][2],m.m[1][2],m.m[2][2],m.m[3][2],
		m.m[0][3],m.m[1][3],m.m[2][3],m.m[3][3]
  );
}

/* --------------------------------------------------------------------------------- */
/*
QapMat4 RotateX(const float angle){
	float cosA = cosf(angle), sinA = sinf(angle);
	
	return QapMat4(
		1, 0,     0,    0,
		0, cosA, -sinA, 0,
		0, sinA,  cosA, 0,
		0, 0,     0,    1);
}

QapMat4 RotateY(const float angle){
	float cosA = cosf(angle), sinA = sinf(angle);

	return QapMat4(
		cosA, 0, -sinA, 0,
		0,    1,  0,    0,
		sinA, 0,  cosA, 0,
		0,    0,  0,    1);
}
*/
QapMat4 RotateZ(const float angle){
  float c=cosf(angle),s=sinf(angle);
  return QapMat4(
    +c,+s,+0,+0,
    -s,+c,+0,+0,
    +0,+0,+1,+0,
    +0,+0,+0,+1
  );
}
/*
QapMat4 RotateXY(const float angleX, const float angleY){
	float cosX = cosf(angleX), sinX = sinf(angleX), 
		  cosY = cosf(angleY), sinY = sinf(angleY);

	return QapMat4(
		 cosY,        0,    -sinY,        0,
		-sinX * sinY, cosX, -sinX * cosY, 0,
		 cosX * sinY, sinX,  cosX * cosY, 0,
		 0,           0,     0,           1);
}

QapMat4 RotateYX(const float angleX, const float angleY){
	float cosX = cosf(angleX), sinX = sinf(angleX), 
		  cosY = cosf(angleY), sinY = sinf(angleY);

	return QapMat4(
		cosY, -sinX * sinY, -cosX * sinY, 0,
		0,     cosX,        -sinX,        0,
		sinY,  sinX * cosY,  cosX * cosY, 0,
		0,     0,            0,           1);
}

QapMat4 RotateZXY(const float angleX, const float angleY, const float angleZ){
	float cosX = cosf(angleX), sinX = sinf(angleX), 
		  cosY = cosf(angleY), sinY = sinf(angleY),
		  cosZ = cosf(angleZ), sinZ = sinf(angleZ);

	return QapMat4(
		cosY * cosZ + sinX * sinY * sinZ,   -cosX * sinZ,    sinX * cosY * sinZ - sinY * cosZ,  0,
		cosY * sinZ - sinX * sinY * cosZ,    cosX * cosZ,   -sinY * sinZ - sinX * cosY * cosZ,  0,
		cosX * sinY,                         sinX,           cosX * cosY,                       0,
		0,                                   0,              0,                                 1);
}*/

QapMat4 Translate(const float x,const float y,const float z){
  return QapMat4(
    1,0,0,0,
    0,1,0,0,
    0,0,1,0,
    x,y,z,1
  );
}

QapMat4 Scale(const float x,const float y,const float z){
  return QapMat4(
    x,0,0,0,
    0,y,0,0,
    0,0,z,0,
    0,0,0,1
  );
}

QapMat4 Identity4(){
	return QapMat4(
    1,0,0,0,
    0,1,0,0,
    0,0,1,0,
    0,0,0,1
  );
}