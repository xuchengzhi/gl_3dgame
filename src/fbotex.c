#include <gl/glew.h>
#include <gl/glut.h>
#include <assert.h>
#include <stdio.h>

#include "common.h"
#include "tl_malloc.h"
#include "jgl.h"

#include "fbotex.h"
#include "tmat.h"
#include "base.h"
#include "sprite.h"
#include "camera.h"
#include "ex.h"
struct FBOTexNode
{
	struct Sprite*		_2dspr;				//2dSprite,绑定fbo的2dSprite
	void* _3dcam;


	GLuint              fboName;			//FBO命名对象	
	GLuint				mirrorTexture;		//镜像贴图
	GLuint              depthBufferName;	//深度缓冲区
	int					texw,texh;			//贴图的宽高
	

	// 渲染节点的回调
	void (*callBack)();

	//渲染的节点列表
	void* nodelist;
};
static void
f_renderList(int data,int parms){
	ex_render3dNode(data);
}
void
fbo_render(void* ptr){
	struct FBOTexNode* fbo = (struct FBOTexNode*)ptr;
	const GLenum fboBuffs[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo->fboName);
	glDrawBuffers(1, fboBuffs);

	glViewport(0, 0, fbo->texw, fbo->texh);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//清除当前的缓冲区

	ex_switch3dCam(fbo->_3dcam);
	if(fbo->callBack){
		fbo->callBack();
	}
	LStack_ergodic_t(fbo->nodelist,f_renderList,0);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void fbo_pushNode(void* p,void* _node){
	struct FBOTexNode* fbo = (struct FBOTexNode*)p;
	if(_node){
		struct HeadInfo* b;
		b = base_get(_node);
		if( base_findByName(fbo->nodelist,b->name) != NULL){
			printf("fbo_pushNode,error! 重名的对象_engine :%s\n",b->name);
			assert(0);
		}else{
			LStack_push(fbo->nodelist,_node);
		}
	}

}

void fbo_bind(void* ptr,void (*callBack)()){
	struct FBOTexNode* fbo = (struct FBOTexNode*)ptr;
	fbo->callBack = callBack;
}
//#define _TestTex_
void* 
fbo_init(int texW,int texH){
	struct FBOTexNode* fbo = (struct FBOTexNode*)tl_malloc(sizeof(struct FBOTexNode));
	
	GLint mirrorTexWidth  = texW;
	GLint mirrorTexHeight = texH;

	GLuint              fboName;
	//GLuint				textures[1];//大理石贴图
	GLuint				mirrorTexture;//镜像贴图
	GLuint              depthBufferName; //深度缓冲区
	
	memset(fbo,0,sizeof(struct FBOTexNode));

	fbo->nodelist = LStack_create();

	fbo->texw = mirrorTexWidth;
	fbo->texh = mirrorTexHeight;

	// Create and bind an FBO(创建,绑定帧缓存对象fbo)
	glGenFramebuffers(1,&fboName);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboName);

	// Create depth renderbuffer(创建深度缓存)
	glGenRenderbuffers(1, &depthBufferName);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBufferName);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, mirrorTexWidth, mirrorTexHeight);

	// Create the reflection texture(创建映射贴图)
	glGenTextures(1, &mirrorTexture);
	glBindTexture(GL_TEXTURE_2D, mirrorTexture);
	//作者的bug
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, mirrorTexWidth, mirrorTexHeight, 0, GL_RGBA, GL_FLOAT, 0);
  
	// Attach texture to first color attachment and the depth RBO(将texture绑定到RBO)
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mirrorTexture, 0);
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBufferName);

	fbo->depthBufferName = depthBufferName;
	fbo->fboName = fboName;
	fbo->mirrorTexture = mirrorTexture;

	// Make sure all went well
	//gltCheckErrors();

	
	//创建2dSprite
	/*fbo->_2dspr = sprite_create("fbo1",0,0,256,256,0);
	*/
	{
		struct Sprite* spr = 0;
#ifdef _TestTex_
		GLuint tex =jgl_loadTex("//resource//texture//1.tga");
#endif
		void* mat= tmat_create_empty("_spritevbo");
		
		char buffer[64];
		tl_newName(buffer,64);
		//tmat_setTexFromGPU(mat,0,mirrorTexture);
		tmat_setID(mat,1);
		
#ifdef _TestTex_
		tmat_pushTex(mat,tex);//mirrorTexture
#else
		tmat_pushTex(mat,mirrorTexture);//mirrorTexture
#endif
		//printf("mirrorTexture:%d\n",mirrorTexture);
		//fbo->mat = mat;
		
		spr = sprite_create(buffer,0,0,texW,texH,0);
		sprite_rotateZ(spr,-PI/2);//sprite旋转90度
		sprite_rotateX(spr,PI);
		base_setv(spr,FLAGS_REVERSE_FACE);

		spr->material = mat;
		fbo->_2dspr=spr;

	}
	
	// Reset framebuffer binding
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	//###################################
	fbo->_3dcam = cam_create();
	cam_setPerspect(fbo->_3dcam,45.0,fbo->texw / fbo->texh , 0.1, 10000);
	return fbo;
}

//销毁fbo对象
void
fbo_dispose(void* p){
	struct FBOTexNode* fbo = (struct FBOTexNode*)p;

	//释放之前要销毁那些push进去的3d节点
	LStack_delete(fbo->nodelist);
	fbo->nodelist = 0;
	
	glDeleteTextures(1, &fbo->mirrorTexture);

	sprite_dipose(fbo->_2dspr);
	//glDeleteTextures(1, textures);
	cam_dispose(fbo->_3dcam);

	// Cleanup RBOs
	glDeleteRenderbuffers(1, &fbo->depthBufferName);

	// Cleanup FBOs
	glDeleteFramebuffers(1, &fbo->fboName);
}


void* fbo_get_spr(void* p){
	struct FBOTexNode* fbo = (struct FBOTexNode*)p;
	return fbo->_2dspr;
}
void* fbo_get_cam(void* p){
	struct FBOTexNode* fbo = (struct FBOTexNode*)p;
	return fbo->_3dcam;
}