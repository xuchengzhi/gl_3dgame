#ifndef _SHADER_H_
#define _SHADER_H_


//font1文本着色器上传数据到GPU的回调
void 
font1_updateVarCallback(void* material,Matrix44f M,void* param);

//9宫格着色器回调
void
grid9CallBack(void* material,Matrix44f M,void* param);

#endif