#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>

#include "tools.h"
#include "tl_malloc.h"
#include "tmat.h"
#include "tlgl.h"
#include "jgl.h"
#include "ex.h"
#include "gettime.h"
#include "camera.h"
#include "atlas.h"
#include "shader.h"
#include "map.h"
//#define _DEBUG_

//=======================================================================================
void 
tmat_setFlatColor(struct GMaterial* ptr,float r,float g,float b)
{
	//ptr->diffuseOutLine = 1;
	ptr->_diffuseOutLineColor.x = r;
	ptr->_diffuseOutLineColor.y = g;
	ptr->_diffuseOutLineColor.z = b;
	ptr->_diffuseOutLineColor.w = 1.0f;
}
//void tmat_getShader(void* pvoid,char* _out,int _outBufferSize)
//{
//	struct GMaterial* _mat = pvoid;
//	//memset(_out,0,_outBufferSize);
//	//memcpy(_out,_mat->glslType,strlen(_mat->glslType));
//	return _mat->glslType;
//}

//播放的时间戳
static float playTime = 0;

//这里预制贴图数量
enum{_TEXTURE_NUM_=2};
/*
	着色器更新贴图数据
	检查有无纹理引用
*/
void 
tmat_updateTexture(GLuint program3D,struct GMaterial* mat){
	const char *_textureKeyArray[_TEXTURE_NUM_] = {"texture1","texture2"};
	const int _textureIndexArray[_TEXTURE_NUM_] = {GL_TEXTURE0,GL_TEXTURE1};
	int _texIndex;

	//if(mat->id==1){
	//	int a;
	//}
	for(_texIndex = 0;_texIndex < _TEXTURE_NUM_;_texIndex++)
	{
		int _texture1 = glGetUniformLocation(program3D,_textureKeyArray[_texIndex]);

		if(_texture1!=-1){
			glUniform1i(_texture1,_texIndex);
			
			glActiveTexture(_textureIndexArray[_texIndex]);

			glBindTexture(GL_TEXTURE_2D,mat->texs[_texIndex]);
			////作者的bug
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
		}
	}
}

//static float testValue = 1.0;
/*
 *
 *	上传模型变换矩阵到顶点着色器
 ****
	变换
	自缩放,旋转
	缩放==> 平移到(0,0,0) ==>缩放 ==>恢复到原来坐标
	旋转==> 平移到(0,0,0) ==>旋转 ==>恢复到原来坐标

*/
void 
tmat_uploadMat4x4(GLint location_mat4x4,Matrix44f _out_mat4x4){
	glUniformMatrix4fv(location_mat4x4,1,GL_TRUE,_out_mat4x4);
}

/*****************************************************************
漫射着色器,上载数据
顶点着色器每次计算一次,GPU阵列集群计算
******************************************************************/

static void 
f_updateShaderVar(GLuint program3D,struct GMaterial* _material, Matrix44f M){

	//轮廓线颜色
	int _outlineColor = glGetUniformLocation(program3D,"_outlineColor");
	//传递camera的坐标
	int _camPos = glGetUniformLocation(program3D,"_camPos");

	//Alpha	-->vboDiffuse.ps
	int _Alpha = glGetUniformLocation(program3D,"_Alpha");

	//获取第1个矩阵引用
	GLint mat1 = glGetUniformLocation(program3D,"_mat1");

	//透视矩阵
	GLint _perspectivePtr = glGetUniformLocation(program3D,"_PerspectiveMatrix4x4");

	//视图矩阵
	GLint _modelViewPtr = glGetUniformLocation(program3D,"_ModelViewMatrix4x4");

	GLint ui_perspectivePtr = glGetUniformLocation(program3D,"ui_PerspectiveMatrix4x4");
	GLint ui_modelViewPtr = glGetUniformLocation(program3D,"ui_ModelViewMatrix4x4");

	//获取第2个矩阵引用
	int mat2 = glGetUniformLocation(program3D,"_mat2");

	//高光开关
	int bSpecular = glGetUniformLocation(program3D,"bSpecular");
	//剔除alpha的开关
	int _DiscardAlpha = glGetUniformLocation(program3D,"_DiscardAlpha");
	//时间线
	int iGlobalTime = glGetUniformLocation(program3D,"_iGlobalTime");

	//颜色
	int _lineColor = glGetUniformLocation(program3D,"_lineColor");
	
	//使用flat渲染
	int _useFlat = glGetUniformLocation(program3D,"_useFlat");
	
	//线宽度(line.vs)
	int _LineWidth = glGetUniformLocation(program3D,"_LineWidth");

	//uv缩放值
	int _uvScale =glGetUniformLocation(program3D,"_uvScale");

	//更新位图
	tmat_updateTexture(program3D,_material);

	

	if(iGlobalTime!=-1)
	{
		playTime+= 1.0f / (g_fps*10);
		glUniform1f(iGlobalTime,playTime);
	}

	//if(pos!=-1){
	//	glUniform3f(pos,0,0,0);//默认传入坐标0,0,0的时候,不使用外部传入的价值
	//}else{
	//	//printf("着色器中未找到传入变量pos\n");
	//}

	if(_outlineColor!=-1)
	{
		struct Vec4 c = _material->_outlineColor;
		glUniform4f(_outlineColor,c.x,c.y,c.z,0.0);
	}
	
	if(_useFlat!=-1)
	{
		int _stat = getv(&_material->flags,SHADER_FLAG_FLAT_COLOR);
		glUniform1i(_useFlat,_stat);
		if(_stat)
		{
			int _diffuseFlatColor = glGetUniformLocation(program3D,"_diffuseFlatColor");
			struct Vec4 c = _material->_diffuseOutLineColor;
			glUniform4f(_diffuseFlatColor,c.x,c.y,c.z,0.0);
		}
	}

	if(_lineColor!=-1)
	{
		struct Vec4 c = _material->_lineColor;
		glUniform4f(_lineColor,c.x,c.y,c.z,0.0);
	}
	if(_LineWidth!=-1)
	{
		if(_material->lineWidth <= 0)
		{
			log_code(ERROR_BAD_VALUE);
			assert(0);
		}
		glUniform1f(_LineWidth,_material->lineWidth);
	}
	else
	{
/*
#ifdef _DEBUG_
		printf("didn`t exist _LineWidth");
#endif
*/
	}

	if(_uvScale!=-1){
		if(_material->uvScale != 0){
			glUniform1f(_uvScale,_material->uvScale);
		}else{
			glUniform1f(_uvScale,1.0f);
		}
	}

	if(mat1!=-1){//模型变换矩阵

		tmat_uploadMat4x4(mat1,M);

	}else{
		//printf("着色器中未找到传入矩阵变量mat1\n");
	}

	//透视变换矩阵
	if(_perspectivePtr!=-1)
	{
		void* perspect = cam_getPerctive(ex_getIns()->_3dCurCam);
		tmat_uploadMat4x4(_perspectivePtr,perspect);
	}

	if(_camPos!=-1){
		void* cam = ex_getIns()->_3dCurCam;
		//printf("%.3f,%.3f,%.3f\n",cam_getX(cam),cam_getY(cam),cam_getZ(cam));
		glUniform3f(_camPos,cam_getX(cam),cam_getY(cam),cam_getZ(cam));
	}

	if(_modelViewPtr!=-1)
	{
		//mat4x4_transpose(ex_getInstance()->modelViewMatrix);//转置矩阵
		tmat_uploadMat4x4(_modelViewPtr,cam_getModel(ex_getIns()->_3dCurCam)/*ex_getIns()->modelViewMatrix*/);
	}

	if(ui_perspectivePtr!=-1)
	{
		//上传2d齐次坐标矩阵
		
		void* per = cam_getPerctive(ex_getIns()->_2dCurCam);
		tmat_uploadMat4x4(ui_perspectivePtr,per);
	}
	if(ui_modelViewPtr!=-1)
	{
		void* per = cam_getModel(ex_getIns()->_2dCurCam);
		//mat4x4_printf("2dmodelcam",per);
		//上传2d模型矩阵
		tmat_uploadMat4x4(ui_modelViewPtr,per);
	}

	if(mat2!=-1){
		Matrix44f _scaleMat;
		float _s = 1.0;
		mat4x4_zero(_scaleMat);
		mat4x4_identity(_scaleMat);
		mat4x4_scale(_scaleMat,_s,_s,_s);//缩放
		//mat4x4_rotateX(_scaleMat,90);//旋转
		glUniformMatrix4fv(mat2,1,GL_TRUE,_scaleMat);
	}

	//是否启用高光1.0开启,0.0关闭
	if(bSpecular!=-1){
		GLfloat _openSpecular = 1.0f;
		glUniform1f(bSpecular,_openSpecular);
	}
	/*if(_DiscardAlpha!=-1){
		glUniform1i(_DiscardAlpha,_material->_DiscardAlpha);
	}*/
	if(_Alpha!=-1)
	{
		glUniform1f(_Alpha,_material->_Alpha);
	}
}
//struct GMaterial *tmat_create()
//{
//	struct GMaterial* mat = (struct GMaterial*)tl_malloc(sizeof(struct GMaterial));
//	memset(mat,0,sizeof(struct GMaterial));
//
//	return mat;
//}
/*
	往材质中压入一个贴图路径,curTexIndex会自+1(curTexIndex从0开始)
*/
static void 
f_tmat_pushTex(struct GMaterial* p,const char* path){
	if(path!=0){
		int len = strlen(path);
		char* _s;
		p->texArray[p->curTexIndex]=(int)tl_malloc(len+1);
		_s = (char*)p->texArray[p->curTexIndex];
		memset(_s,0,len+1);
		memcpy(_s,path,len);
		
		//printf("%s\n",p->texarray[p->curTexIndex]);

		p->curTexIndex++;
	}else
	{
		//assert(0);
	}
}

void 
tmat_print_tex(void* mat){
	struct GMaterial* m = (struct GMaterial*)mat;
	int i;
	for (i = 0;i < m->curTexIndex;i++){
		printf("索引%d贴图句柄:%d",i,m->texs[i]);
	}
}

void 
tmat_pushTex(void* mat,GLuint tex){
	struct GMaterial* m = (struct GMaterial*)mat;
	if(m->curTexIndex+1>=MATERIAL_TEXTURE_COUNT){
		printf("贴图索引溢出\n");
		assert(0);
	}
	m->texs[m->curTexIndex] = tex;
	m->curTexIndex++;
}

/*
*	存储在到GPU显存中一张贴图
*	int texIndex		:		纹理索引
*	GLuint tex			:		纹理句柄引用
*/
void 
tmat_setTexFromGPU(struct GMaterial* mat,int texIndex,GLuint tex){
	//创建纹理数据
	if(texIndex >= mat->curTexIndex){
		printf("纹理索引不能溢出!\n");
		assert(0);
		return;
	}
	if(mat->texs[texIndex]){
		//需要删除该纹理数据后再创建
		printf("创建异常,该索引上已经有了纹理\n");
		assert(0);
		return;
	}
	mat->texs[texIndex] = tex;
	//mat->texs[texIndex]=jgl_loadTex("//resource//texture//2x2.tga");//测试图像
}

GLuint 
tmat_getTextureByIndex(struct GMaterial* pmat,int index)
{
	return (GLuint)pmat->texs[index];
}

/*
	struct GMaterial* mat:	材质引用
	float x,float y,float z	坐标
*/
void tmat_render(void* pvoid,const char* shader,Matrix44f M)
	 //float x,float y,float z,float scale,
{
	GLuint program3D;
	struct GMaterial* material = pvoid;

	if(material==NULL)
	{
		printf("模型（%s）没有设置材质\n");
		assert(0);
		return;
	}

	if(shader==NULL)
	{
		printf("传递的Shader字符串为null\n");
		assert(0);
	}

	if(!material->glslType){
		printf("无着色器类型指定,需要设置着色器\n");
		assert(0);
		return;
	}

	if(material->updateVarCallback){
		material->updateVarCallback(material,M,material->params);
	}else{
		program3D = ex_getProgrom3D(shader);//mat->glslType

		//切换到当前的着色器引用
		glUseProgram(program3D);

		//向着色器上传相关数据
		f_updateShaderVar(program3D,material,M);
	}
	//设置自定义的着色器回调传值
}

/*
	清理纹理路径,在使用完成之后就释放
*/
static void 
f_clearTexArray(struct GMaterial *mat)
{
	int i = 0;
	
	for(i = 0;i< MATERIAL_TEXTURE_COUNT;i++){
		int _p = mat->texArray[i];
		if(_p){
			tl_free((void*)_p);
			mat->texArray[i] = 0;
		}
	}
}
/*
	删除GPU贴图
*/
static void f_deleteGPU_texture(struct GMaterial* mat)
{
	int i;
	for(i = 0;i< MATERIAL_TEXTURE_COUNT;i++)
	{
		GLuint tex = mat->texs[i];
		if(tex!=0)
		{
			//如果要删除纹理缓冲区数据	请这样使用下面方式;
			void* node = map_getNodeByValue(ex_getIns()->mapptr,(void*)tex);
			
			if(node){
				printf("使用texmap = %0x,tex=%d,不使用glDeleteTextures删除贴图\n",node,tex);
			}else{
				printf("销毁纹理,纹理句柄=%d\n",tex);
				glDeleteTextures(1,&(mat->texs[i]));
			}
			mat->texs[i] = 0;
		}
	}
}
void tmat_disposeTextureByIndex(struct GMaterial* mat,int index)
{
	int i;
	for(i = 0;i< MATERIAL_TEXTURE_COUNT;i++)
	{
		if(mat->texs[i]!=0 && i == index)
		{
			//如果要删除纹理缓冲区数据	请这样使用下面方式;
			glDeleteTextures(1,&(mat->texs[i]));
			mat->texs[i] = 0;
		}
	}
}
//销毁一个材质
void tmat_dispose(void* pvoid)
{
	struct GMaterial* mat = pvoid;
	if (mat->params){
		tl_free(mat->params);
	}
	log_color(0,"销毁材质%0x\n",pvoid);

	f_deleteGPU_texture(mat);	
	
	if(mat->shaderVars){
		tl_free(mat->shaderVars);
	}
	if(mat->glslType){
		tl_free(mat->glslType);
	}

	tl_free((void*)mat);
}
/*
	为材质创建贴图
*/
static void f_createMaterialTexture(struct GMaterial *p)
{
	int i;
	for(i = 0;i< p->curTexIndex;i++)
	{
		GLuint tex = jgl_loadTex((const char*)p->texArray[i]);
		tmat_setTexFromGPU(p,i,tex);
	}
}
/*
	赋着色器
*/
static void 
f_assignShader(struct GMaterial* tmat,const char* glslType)
{
#ifdef _DEBUG_
	printf("设置着色器%s\n",glslType);
#endif
	//memset(tmat->glslType,0,G_BUFFER_32_SIZE);
	//memcpy(tmat->glslType,glslType,strlen(glslType));
	
	if(tmat->glslType){
		tl_free(tmat->glslType);
	}
	tmat->glslType = (char*)tl_malloc(strlen(glslType)+1);
	memset(tmat->glslType,0,strlen(glslType)+1);
	memcpy(tmat->glslType,glslType,strlen(glslType));
	//printf("[%s]\n",tmat->glslType);
}

void tmat_setID(void* m,int id){
	struct GMaterial* tmat = (struct GMaterial*)m;
	tmat->id = id;
}

int tmat_getID(void* m){
	struct GMaterial* tmat = (struct GMaterial*)m;
	return tmat->id;
}

static void
f_initMaterial(struct GMaterial* tmat){
	//设置了alpha的值 = 1.0
	tmat->_Alpha = 1.0f;

	//设置线框颜色
	tmat->_outlineColor.x = 0;
	tmat->_outlineColor.y = 1;
	tmat->_outlineColor.z = 1;

	//设置flat颜色
	tmat_setFlatColor(tmat,0.43,0.51,0.96);

	tmat->rendermode = GL_TRIANGLES;
}

void* 
tmat_create(const char* glslType,int texCnt,...){
	
	struct GMaterial* tmat = (struct GMaterial*)tl_malloc(sizeof(struct GMaterial));
	memset(tmat,0,sizeof(struct GMaterial));
	
	if(texCnt > MATERIAL_TEXTURE_COUNT){
		log_code(ERROR_BAD_VALUE);//超出预留的贴图数量
		assert(0);
	}
	else{
		int i;
		va_list ap;
		va_start(ap, texCnt);
		for(i = 0; i < texCnt; i++){
			int p = va_arg(ap, int);			
			if(p && strlen((char*)p) > 0) {
				f_tmat_pushTex(tmat,(char*)p);
			}
		}
		va_end(ap);
	}

	f_assignShader(tmat,glslType);

	//开始构造贴图
	f_createMaterialTexture(tmat);

	//清理纹理路径列表需要的堆内存数据
	f_clearTexArray(tmat);

	f_initMaterial(tmat);
	log_color(0,"构建材质%0x,shader=%s\n",tmat,tmat->glslType);
	return tmat;
}

//创建一个空贴图的材质
void* 
tmat_createTex(const char* glslType,GLint width,GLint height){
	struct GMaterial* tmat = (struct GMaterial*)tl_malloc(sizeof(struct GMaterial));
	memset(tmat,0,sizeof(struct GMaterial));
	
	{
		//创建一张指定的宽高位图纹理
		GLuint tex = jgl_createRGB_Texture(width,height,GL_RGB);
		
		//指定0号索引位置的位图
		tmat->curTexIndex++;
		tmat_setTexFromGPU(tmat,0,tex);
	}
	f_assignShader(tmat,glslType);
	f_initMaterial(tmat);
	
	return tmat;
}

void* 
tmat_create_empty(const char* glslType){
	struct GMaterial* tmat = (struct GMaterial*)tl_malloc(sizeof(struct GMaterial));
	memset(tmat,0,sizeof(struct GMaterial));
	f_assignShader(tmat,glslType);
	f_initMaterial(tmat);
	return tmat;
}

void* 
tmat_create_rgba(const char* glslType,GLint width,GLint height,GLenum rgbaType){
	struct GMaterial* tmat = (struct GMaterial*)tl_malloc(sizeof(struct GMaterial));
	GLuint tex;
	memset(tmat,0,sizeof(struct GMaterial));
	
	tex=jgl_create_opengl_RGBA_Tex(width,height,rgbaType);
	tmat->curTexIndex++;
	tmat_setTexFromGPU(tmat,0,tex);
	f_assignShader(tmat,glslType);
	f_initMaterial(tmat);
	return tmat;
}
//
//void
//tmat_set_discardAlpha(void* p,int value){
//	struct GMaterial* tmat = (struct GMaterial*)p;
//	//tmat->_DiscardAlpha = value;
//}

void tmat_renderSprite(struct GMaterial *_material,const char* shader,Matrix44f mat4x4,GLfloat* vertexs,int vertLen,int format,int mode)
{
	tmat_render(_material,shader,mat4x4);	
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);//剔除前面
	glPolygonMode (GL_BACK,mode);
	glInterleavedArrays(format,0,vertexs);
	glDrawArrays(GL_TRIANGLES,0,(GLsizei)vertLen);
	glDisable(GL_CULL_FACE);
}
//创建一个九宫格材质
void* tmat_create_9grid(struct Atals* atals,const char* icon){
	void* _mater = tmat_create_empty("grid9vbo");
	struct GMaterial* _matarial = (struct GMaterial*)_mater;
	GLuint tex =  atals_new_tex(atals,icon,0,0);
	_matarial->updateVarCallback = grid9CallBack;
	tmat_pushTex(_matarial,tex);
	return _mater;
}