#ifndef _EX_H_
#define _EX_H_
#include "tlgl.h"



#include "lua.h"
//#include "lualib.h"
//#include "lauxlib.h"

#define  SPRITE_TRIANGLE_COUNT	18	//一个sprite 2d面片需要的三角形(2 * 3 * 3)数据长度

/************************************************************************/
/*  EX->flags(32位,存储32个状态)										*/
/************************************************************************/
/*
	EX_FLAGS_RAY:
	射线拾取开关枚举,这是一个总开关
	设置静态碰撞盒子,设置成TRUE就可以射线检测,不检测射线拾取,这是一个整体开关,单个模型可以单独设置
*/
//#define EX_FLAGS_RAY 0x01

//#define EX_FLAGS_DRAW_DEBUG_TEXT 0x02	/*是否显示调试信息的文本在标题处*/
#define EX_FLAGS_RAY_TO_UI 0x04			/*是否点击到了界面元素*/
#define EX_FLAGS_LEFT_DOWN 0x08			/*鼠标是否左键常按着*/

//##############################################################
#define	E_RenderModeNormal	1			//			固定管线渲染模式
#define	E_RenderModeVBO		2			//			VBO渲染模式

//文件类型定义
#define TYPE_OBJ_FILE 0	//obj类型
#define TYPE_MD5_FILE 1 //md5类型
#define TYPE_MD2_FILE 2 //md2类型
#define TYPE_SPRITE_FLIE 3//UI界面类型中的sprite
#define TYPE_TEXT_FILE	4	//文本类型
#define TYPE_OBJ_VBO_FILE	5//VBO文件数据
#define TYPE_FONT_TXT  6	//freetype文本
#define TYPE_LINENODE 7 //线

//键值枚举
#define KEY_ESC 27
#define KEY_1 49
#define KEY_2 50
#define KEY_A 97
#define KEY_B 98
#define KEY_C 99
#define KEY_D 100
#define KEY_I 105

/************************************************************************/
/* 引擎事件定义                                                         */
/************************************************************************/
#define EVENT_ENGINE_RENDER_3D 100				//引擎渲染回调
#define EVENT_ENGINE_KEYBOARD  101				//全局键盘事件
#define EVENT_ENGINE_RESIZE	   102				//resize事件

#define EVENT_ENGINE_BASE_UPDATE	   102				//base更新事件
#define EVENT_ENGINE_BASE_END		   103				//base结束事件

#define EVENT_ENGINE_SPRITE_CLICK		   104//click
#define EVENT_ENGINE_SPRITE_CLICK_DOWN	   105//click down
#define EVENT_ENGINE_SPRITE_CLICK_MOVE	   106//click move
#define EVENT_ENGINE_SPRITE_FOCUS_CHANGE   107//焦点发生变化
#define EVENT_ENGINE_TEX_LOAD_COMPLETE	   108//贴图加载完成了
#define EVENT_ENGINE_COMPLETE			   109//完成事件,此事件是可以共享的


//#define EVENT_ENGINE_SPRITE_FOCUS_OUT   108	//2d焦点离开的时候触发
//#define EVENT_ENGINE_SPRITE_FOCUS_IN    109	//2d焦点进入的时候触发

#define CUST_LUA_EVENT_SPRITE_FOCUS_CHANGE 110 //向lua层发送焦点变化
#define CUST_LUA_EVENT_INPUT_CHANGE		   111 //向lua层发送input输入变化
/*
*引擎初始化完成(stage2d,设备,lua函数注册完成,可以执行上层业务)
*/
#define EVENT_ENGING_INIT 200	

/**
 * 计时器事件
 */
#define EVENT_TIMER 201

//键盘事件状态
struct E_KeyBoard
{
	unsigned char key;
	int x,y;
};

//鼠标状态
struct MouseState
{
	GLint button;
	GLint action;

	GLint xMouse,yMouse;//点击下的鼠标状态

	int moveX,moveY;//当前移动的位置
	// 中键方向 -1:向上,1:向下 0:的时候 说明没有滚动
	int midDirection;
};
//=============================================================================================================
struct EX
{	 
	//public
	/*
		事件列表
	*/
	void* evtList;
	/*
	*	摄影机坐标
	*/
	//float camx,camy,camz;
	//float camRotateX,camRotateY,camRotateZ;
	//struct ECamera cam;//camera
	/*
		各种枚举定义,二进制位存储
	*/
	int flags;
	
	/************************************************************************/
	/* 字体设备句柄                                                                     */
	/************************************************************************/
	void* ft;

	/*
	渲染节点对象列表
	*/
	struct LStackNode *renderList;

	/*
	*	背景填充色
	*/
	float bg_r,bg_g,bg_b;

	//public
	/*
	*	景深
	*/
	GLdouble zFar;


	/*
		屏幕尺寸
	*/
	float _screenWidth,_screenHeight;

	/*
		当前鼠标状态
	*/
	struct MouseState mouseState;
	struct HitResultObject* hit;
	lua_State* mylua;

	/*
		UI 视角的z深度
	*/
	//int zBuffer;

	//当前的界面层的ui z轴的坐标
	float ui_pos_z;

	/*
		总共的Z深度
	*/
	int allzBuffer;

	//2d舞台
	void* stage2d;
	
	//临时点击sprite获取的信息
	void* clickInfo;

	//当前鼠标的焦点(struct Sprite*)
	void* curFocus;
	
	//3d Camera引用
	void* _3dcam;

	//2d Camera引用
	void* _2dcam;

	//当前的camera
	void* _3dCurCam;
	
	//当前的2dCam
	void* _2dCurCam;
	
	//fbo对象管理器
	struct LStackNode *fboList;

	void* pro3d;//着色器管理器句柄

	//键值对象句柄
	void* mapptr;

	struct LStackNode* timelaterList;

};

//接口定义
//=====================================================
/*
*	渲染循环
*/
void ex_render(void);

//渲染renderlist所有的3d节点
void ex_renderAll3dNode();
//void f_renderlistCall(void _callBack(int));

/*
*	初始化
*/
void ex_init(struct EX* p,GLdouble zfar,float sw,float sh);

//重绘回调
void ex_reshape(int w,int h);

/*
*	销毁引擎
*/
void ex_dispose(struct EX* p);

/*
gluUnProject -- map window coordinates to object coordinates 
将屏幕坐标映射到世界坐标

gluUnProject maps the specified window coordinates into object coordinates using modelMatrix, 
projMatrix, and viewport. The result is stored in objx, objy, and objz. 
A return value of GL_TRUE indicates success, and GL_FALSE indicates failure


二维三维坐标转换函数： gluUnProject()
此函数的具体用途是将一个OpenGL视区内的二维点转换为与其对应的场景中的三维坐标。
转换过程如下图所示（由点P在窗口中的XY坐标得到其在三维空间中的世界坐标）：
*/
void mousePlot(GLint button, GLint action, GLint xMouse, GLint yMouse);

/*
	鼠标移动
*/
void mouseMove(int x,int y);

/*
普通按键控制a,1,2,3
*/
void onKeyboardCallBack(unsigned char key, int x, int y);



/*
	添加一个渲染节点
*/
void ex_add(void* ptr);
/*
	获取引擎句柄
*/
struct EX* ex_getIns();
/*
	加载一个模型
	E_RenderMode mode:	管线使用的渲染模式
*/
void* 
ex_load_model(char* name,const char* url,int mode);

/*
	设置动作
*/
void ex_set_anim(void* ptr,const char* animKey);

/*
	在屏幕左上角显示日志文本
*/
void ex_showLog(const char* buffer);

/*
	回调lua函数

	调用一个参数的lua函数
	const char* luaFunName	:lua中的函数名
	int parm				:参数引用


	lua_pcall(lua_State *L, int nargs, int nresults, int errfunc)
	nargs:参数个数，nresults：返回值个数，errFunc：错误处理函数，0表示无，表示错误处理函数在栈中的索引


	lua:
	===================================
	function btnCallBackEvt(spriteName)
	end
*/
void ex_callParmLuaFun(const char* luaFunName,const char* parm);
/*
	调用lua方法luaFunName(lua函数必须是lua全局函数),传递一个int型的参数
*/
void ex_callIntLuaFun(const char* luaFunName,int value);
/*
*	根据name获取HeadInfo;
*/
struct HeadInfo* 
ex_find_headinfo(struct EX* p,const char* name);

/************************************************************************/
/* 搜索节点                                                                     */
/************************************************************************/
void* 
ex_find(const char* name);
/*
	删除引擎中的一个对象
*/
void ex_ptr_remove(void* ptr);

/************************************************************************/
/* 重命名                                                               */
/************************************************************************/
int ex_rename(void* p,const char* name);

/*
	设置材质
*/
void 
ex_set_material(void* ptr,void* material);

/************************************************************************
	当前的FPS                                                                  
	frame per second 每一关键帧需要的毫秒
************************************************************************/
extern int g_fps;

/************************************************************************
	每一关键帧的延迟毫秒数                                                                    
************************************************************************/
extern long g_delayTime;
/************************************************************************
是否让全局的Sprite显示线框
#define _SPRITE_RENDER_LINE_//渲染线框,用于调试,此方式可以在低配置电脑上执行,因为使用的是功耗最小的模式
************************************************************************/
//extern int g_sprite_line;

/*
 *获取偏移值
 */
int ex_get_gap(int type);


//切换到当前camera,场景中可能会有多个camera,我们可以用用这个函数进行切换到当前的cam
//比如从默认的cam切换到fbo的cam,其实cam就是一个齐次透视矩阵和一个模型矩阵
void ex_switch3dCam(void* cam);

//加载VBO模式的模型
void* ex_loadVBO(char* name,const char* url);

//切换到2d camera
void ex_switch2dCam(void* cam);

//添加一个fbo对象到引擎
void ex_add_fbo(void* fbo);

//从引擎列表中移除fbo
void ex_remove_fbo(void* fbo);

//根据shader类型获取一个
GLuint ex_getProgrom3D(const char* glslType);

//向指定的事件发送一个浮点型数据
void ex_lua_evt_dispatch_f(void* obj,int evtid,float data);

//向lua发送一个事件
void ex_lua_evt_dispatch(void* obj,int evtid,const char* data);
//*******************************************************************
/*
	加载动作配置,设置
*/
int ex_load_anim_config(void* ptr,char* animConf,long fps);

/*
 *	设置动作
 *	playend:当前动作关键帧播放完成之后回调该函数引用
 */
int 
ex_animtor_ptr_setcur(void* ptr,const char* animKey,void (*playend)(void*));

/*
设置摄影机的坐标
*/
void ex_cam_set_pos(float x,float y,float z);

/*
设置渲染的背景颜色
*/
void setBgColor(float r,float g,float b);

/*
	打印引擎当前信息
*/
void ex_get_info();
#endif