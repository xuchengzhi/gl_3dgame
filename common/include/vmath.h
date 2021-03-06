#ifndef _VMATH_H_
#define _VMATH_H_
#include "def.h"

/*
0(0,0)	1(0,1)		2(0,2)		3(0,3)
4(1,0)	5(1,1)		6(1,2)		7(1,3)
8(2,0)	9(2,1)		10(2,2)		11(2,3)
12(3,0)	13(3,1)		14(3,2)		15(3,3)
*/
typedef float Matrix44f[16];

struct Vec2{
	float x,y;
};
/*
*	3D向量
*/
typedef struct Vec3{
	float x,y,z;
}Vec3;

struct Vec4
{
	float x,y,z,w;
};


/* Vectors */
typedef float Vec2_t[2];
typedef float Vec3_t[3];
typedef float Vec8_t[8];

enum {X, Y, Z, W};

/*
打印矩阵
*/
DLLIMPORT 
void mat4x4_printf(const char* name,Matrix44f M);
DLLIMPORT 
void mat4x4_double_printf(const char* name,double m[16]);
/*
矩阵归一化
*/
DLLIMPORT 
void mat4x4_identity(Matrix44f M);

/*
 *矩阵是否相等
 */
DLLIMPORT
int mat4x4_equal(Matrix44f a,Matrix44f b,int log);

/*
*反转矩阵(矩阵的逆)
*/
DLLIMPORT 
void mat4x4_invert(Matrix44f out,Matrix44f a);
DLLIMPORT 
void mat4x4_transformMat4(float out[4],float a[4],Matrix44f m);
/*
矩阵位移
*/
DLLIMPORT void mat4x4_translate(Matrix44f M, float x, float y, float z);
//sx,sy,sz	缩放
DLLIMPORT void mat4x4_scale(Matrix44f M, float x, float y, float z);
////初始化归一矩阵
//void mat4x4_identity(Matrix44f M);
/*
二维数组矩阵赋值
*/
DLLIMPORT void mat4x4_2t1(Matrix44f M,int m,int n,float value);
/*
将a矩阵的值copy到out中
*/
DLLIMPORT void mat4x4_copy(Matrix44f M,Matrix44f out);
/*
XYZ旋转
*/
DLLIMPORT void mat4x4_rotateX(Matrix44f M, float radian);

DLLIMPORT void mat4x4_rotateY(Matrix44f M, float radian);

DLLIMPORT void mat4x4_rotateZ(Matrix44f M, float radian);

DLLIMPORT void mat4x4_rotateXYZ(Matrix44f M, float radian, float x, float y, float z); 

DLLIMPORT void perspectiveFrustum(Matrix44f M, float left, float right, float top, float bottom, float near, float far); //透视投影
DLLIMPORT void mat4x4_orthoPerspect(Matrix44f M, float left, float right, float top, float bottom, float near, float far);  //正交投影

//绕x,y,z方向向量旋转r度
DLLIMPORT void mat4x4_rotate_vec(Matrix44f M,float r,float x,float y,float z);

//void matrix_m(float **a_matrix, const float **b_matrix, const float **c_matrix,
//			  int krow, int kline, int kmiddle, int ktrl);
/*
1维数组矩阵相乘,4x4 注意矩阵乘法有先后顺序
*/
DLLIMPORT void multi2(Matrix44f out,Matrix44f a, Matrix44f b);
/*
数组的每个选项都是0
*/
DLLIMPORT void mat4x4_zero(Matrix44f M);

/**
* 矩阵行列转置
* Transpose the values of a mat4
*
* @param {mat4} out the receiving matrix
* @param {mat4} a the source matrix
* @returns {mat4} out
*/
DLLIMPORT void mat4x4_transpose(Matrix44f a);
/*
*让矩阵朝向某个目标
*/
DLLIMPORT void mat4x4_lookAt(Matrix44f out,struct Vec3* eye,struct Vec3* center,struct Vec3* up);

//矩阵连乘
DLLIMPORT void mat4x4_mult(int c,Matrix44f o,...);
//构造透视矩阵
DLLIMPORT void 
mat4x4_perspective(Matrix44f M,float fov, float aspectRatio, float zNear, float zFar );

/*
	自偏移
*/
DLLIMPORT void tl_offset_vec(Vec3_t pos,float x,float y,float z);

/*vec缩放*/
DLLIMPORT void tl_scale_vec(Vec3_t vec,Vec3_t res,float scale);


/*
*	填充一个向量
*/
DLLIMPORT void vec3Set(struct Vec3* v,float x,float y,float z);
/*
*	向量相加 out = a + b
*/
DLLIMPORT void vec3Add(struct Vec3* a,struct Vec3* b,struct Vec3* out);
/*
*	向量相减 out = a - b
*/
DLLIMPORT void vec3Sub(struct Vec3* a,struct Vec3* b,struct Vec3* out);

/*
	计算a,b,c三个顶点的法线向量(面法线向量),输出到out
*/
DLLIMPORT void vec3CalculateNormal(struct Vec3* a,struct Vec3* b,struct Vec3* c,struct Vec3* out);
/*
*	向量叉乘(叉积)
*/
DLLIMPORT void vec3Cross(struct Vec3* a,struct Vec3* b,struct Vec3* out);
/*
*	求向量的长度
*/
DLLIMPORT float vec3Length(struct Vec3* v);
/*
 *	是否是0向量
 */
DLLIMPORT int vec3IsZero(struct Vec3* v);

/*
*	向量乘以一个标量
*/
DLLIMPORT void vec3Mult(struct Vec3* a,float value);

/*
*		向量点乘
*		
*		a·b>0    方向基本相同，夹角在0°到90°之间
*
*		a·b=0    正交，相互垂直  
*
*		a·b<0    方向基本相反，夹角在90°到180°之间 
*/
DLLIMPORT float vecDot(struct Vec3* a,struct Vec3* b);
/*
*向量取反
*/
DLLIMPORT void vec3Reverse(Vec3* v);
/*
*	求两向量的距离
*/
DLLIMPORT float vec3Distance(struct Vec3* a,struct Vec3* b);
/*
	规格化，单位化（Normalize）
	先要说矢量的长度：
	矢量Vector（x，y，z）
	矢量长度Length（Vector）= |Vector|=sqr（x*x+y*y+z*z）
	Normalize后：
	（x/Length（Vector），y/Length（Vector），z/Length（Vector））
	方向不变，长度为1个单位
*/
DLLIMPORT void vec3Normalize(struct Vec3* v);

/*
	向量除以一个float
*/
DLLIMPORT void vec3Div(struct Vec3* o,float v);

/*
	求两向量的夹角0~180之间
*/
//DLLIMPORT float vecToAngle(struct Vec3* a,struct Vec3* b);

/*
*	2d向量转化为角度
*/
DLLIMPORT double
vec_rotateAngle(double x1,double y1, double x2,double y2);

/**3d向量转化为角度*/
DLLIMPORT float vec_to_angle(float x1,float y1,float z1,float x2,float y2,float z2);
/*
*
*	射线与三角形的交点数据结构
*
*/
struct HitResultObject{
	//拾取的交点坐标
	float x,y,z;

	//是否已经拾取到,拾取到:TL_TRUE,未拾取到:TL_FALSE
	int isHit;

	//拾取的对象名字
	char name[G_BUFFER_32_SIZE];

	//射线与三角形的交点 距离 射线起点的距离
	float distance;

	//三角形的三个顶点
	//float pickTriangle[9];
};
/*
 *  射线拾取
 *  
 *  当拾取到多个交点的时候该方法只取离交点最近的交点
 *	tri			:三角形描述数据
 *	dataCount	:tri数组的长度 sizeof(float) * dataCount
 *	nearPoint	:最近点
 *	farPoint	:最远点
 *	fpos		:对象坐标
 */
DLLIMPORT void tl_pickTriangle(float* tri,int dataCount,struct Vec3* nearPoint,struct Vec3* farPoint,struct Vec3* fpos,struct HitResultObject* pResult);

/*
	功能描述:射线与三角形相交
	-----------------------------
	orig	:射线起始坐标
	dir		:方向向量			(射线向量-目标目标向量=方向向量)
	vert0~2	:三角形3个顶点坐标

	pHit	: 0不相交,1相交
	out		:x,y,z 交点坐标
	-----------------------------
	使用方法:
	void testHit(){
	double orig[3] = {0,-0.5,0.5};//射线坐标
	double target[3] = {0,-1,0};//目标点
	double dir[3] = {orig[0]-target[0],orig[1]-target[1],orig[2]-target[2]};//射线向量-目标目标向量=方向向量
	double vert0[3] = {0,0,0};
	double vert1[3] = {-1,-1,0};
	double vert2[3] = {1,-1,0};
	double out[3];
	int hit;
	tl_hitTriangle(orig,dir,vert0,vert1,vert2,out,&hit);
	printf("是否有交点 %d,交点坐标 x:%f y:%f z:%f\n",hit,out[0],out[1],out[2]);
}
*/
void tl_hitTriangle(float orig[3],float dir[3],float vert0[3],float vert1[3],float vert2[3],float* out,int* pHit);

//判断是逆时针还是顺时针返回 0:逆时针 1:顺时针
int vec3CheckDir(float x1,float y1,float z1,float x2,float y2,float z2,
				 float ax,float ay,float az );
#endif