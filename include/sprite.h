#ifndef _SPRITE_H_
#define _SPRITE_H_
#include "vmath.h"
//#define USE_VBO_SPRITE	//是否使用VBO模式的Sprite

//2d触发界面
struct Sprite
{
	void* base;
	float pos_x;		//表示在正交投影坐标系(左下角为坐标原点)的坐标，
	float pos_y;
	float pos_z;		//用此值来标识sprite是否在前面一层s
	
	//z轴的缩放
	float zScale;
	//鼠标相对于面板的点击局部坐标
	int mouseLocalX,mouseLocalY;

	//屏幕坐标,从左上角开始计算0,0
	float screenX;
	float screenY;

	float mWidth;		//sprite所屏幕像素单位
	float mHeight;
	int m_bPressed;

	int mouseDownX,mouseDownY;//鼠标左键点击下的坐标

	void* vertexs;//GLfloat*顶点缓存数据,使用完之后要删除tl_free
	int	vertLen;

	//void* material;//按钮材质

	struct Atals* atals;//图集
	
	/************************************************************************/
	/* 材质引用                                                    */
	/************************************************************************/
	void* material;
	
	Matrix44f mat4x4;
	
	/*
		做射线拾取的时候使用
	*/
	float *hitTriangle;
	//float *triangle2;
	
	/*
		Sprite点击回调
	*/
	void (*clickCallBack)(struct Sprite* ptrSprite,int localX,int localY);

	/*
		以Sprire相对的0,0坐标设置的拖拽范围,如果设置的是0,没有范围限制
	*/
	int	dragX,dragY,dragWidth,dragHeight,oldx,oldy;

	/*
		--0:横向拖动滑块,1纵向拖动滑块
	*/
	int	dragDirection;

	//char* luaTablePtr;
	int parseType;
	int useVBO;//是否使用VBO模式
	struct Obj_vbo_model* vbo;


	//子对象列表
	struct LStackNode *childs;
	//父对象
	void* parent;
	//局部坐标,相对于父对象,如果是在stage上就为0,0
	int localx,localy;
	
	//是否可以穿透界面,拾取到3d层
	//int open_through;
	
	//点击到局部坐标
	//int click_local_x,click_local_y;
	
	//是否使其可以被渲染
	//int _renderState;
};

/*
 *为spr添加一个子对象
 **/
void sprite_addChild(void* spr,void* child);
/*移除子对象*/
void sprite_removeChild(void* spr,void* child);

//设置坐标，相对于局部坐标
//void sprite_setLocalPos(void* ptr,int x,int y);

/*
 *获取全局坐标
 */
void sprite_getGlobalPos(void* spr,int* px,int *py);

/*
 *在addChild之后设置其相对最表
 *如果此sprite是其他的sprite的子对象的时候,使用该接口设置其相对于父对象的坐标
 */
void sprite_set_self_pos(void* p,int x,int y);

/* 
	初始化按钮

	void (*clickCallBack)(struct Sprite* ,int ,int )	点击回调函数,如果未设置回调函数是不会启用点击拾取事件的
*/
struct Sprite* sprite_create(
	char* _spriteName,
	int x,int y,
	int width,int height,
	void (*clickCallBack)(struct Sprite* ,int ,int ));

/*
	设置sprite的UV
*/
void sprite_setUV(struct Sprite* sprite,float x,float y,float w,float h);

/*
	绘制一个sprite 
*/
void sprite_drawRender(int data);

/*
	按钮事件特效
*/
void sprite_action(const int data);

/*
	渲染更新ui节点
*/
void sprite_updatePos(int data);
/*
	删除sprite节点
*/
void sprite_dipose(struct Sprite* spr);

/*
	根据图集中的key设置位图,atlas其实是一组配置
*/
void sprite_texName(struct Sprite* spr,const char* texName,struct AtlasNode* ptr);
/*
	更新坐标
*/
void sprite_setpos(struct Sprite* spr,int x,int y);

/*
	鼠标左键点击后拖拽只用该接口更新sprite坐标
*/
void sprite_mouseMove(int data);

/*
	是否可以渲染
*/
int sprite_isEnable(int data);

/*
	设置sprite可拖动范围
	x,y:可拖拽的坐标
	w:可拖拽的宽度
	h:可拖拽的高度
*/
void sprite_setDragScope(struct Sprite* pSpr,int x,int y,int w,int h);

/*
 *旋转rz轴 PI的n倍数,只变换渲染轴向
 */
void sprite_rotateZ(struct Sprite* ptr,float rz);
/*
	重置sprite的尺寸
*/
void sprite_resize(struct Sprite* spr,int w,int h);
/*设置Z缩放  */
void sprite_set_scale_z(struct Sprite* spr,float v);
//开启点击此界面的之后可以穿透界面层
//void
//sprite_open_through(void* spr);
/*
	是否有鼠标事件
*/
int 
sprite_isCanClick(void* p);
//使其可以被渲染,显示出来
//void 
//sprite_set_render(void* p,int v);

//为Sprite设置一个默认的图集
void
sprite_set_default_tex(void* p);
#endif