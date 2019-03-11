//#pragma warning(noce:4996) //仅显示一个

//#pragma warning(disable:4996) 
//#pragma warning(disable:4244) 
//#define BUILDING_DLL

#include <stdio.h>     
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <stdarg.h>

#define BUILDING_DLL

#include "common.h"
#include "tools.h"
#include "gettime.h"
#include "tl_malloc.h"
#include "vmath.h"
#include "str.h"

//#define DEBUG

#define DEFINE_BASE_64_STRING  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/="


#define G_SIGN_AITA '@'

///*
//*	opengl数据结构
//*/
//struct VerNorUvStruct
//{
//	float* pvs;			
//	float* pnormals;	
//	float* puvs;		
//};
/*======================================================================

切割字符串+回调

//void  tl_split(const char* dest,const char sign,CallBackFun pCallBack);

	多参数传递切割
	const char* input:		输入字符串
	const char sign:		切割标示
	CallBackFun pCallBack:	回调接口
======================================================================*/
static void 
f_split_s(const char* input,const char sign,int (*CallBackFun)(char*),	int* parms,int parmsLen);

struct LStackFindNode{
	/*
	*	当前节点指针
	*/
	void* node;
	/*
	*	输入参数指针
	*/
	void* parms;

};

/*
根据类型,切割字符串
*/
typedef struct SplitObj{
	int index;
	int type;
	char* cur;
}SplitObj;

void* MALLOC(int size){
	void* p = (void*)malloc(size);
	if(p == NULL){
		printf("===========================MALLOC内存申请失败,需要%d字节!\n",size);
		assert(0);
	}
	return p;
}
void FREE(void*p){
	if(p!=NULL){
		free(p);
	}else{
		printf("释放了一个地址为0的数据\n");
		assert(0);
	}
}



void tl_replace(char *pInput, char *pOutput, char *pSrc, char *pDst)
{
	char    *pi, *po, *p;
	int     nSrcLen, nDstLen, nLen;

	// 指向输入字符串的游动指针.
	pi = pInput;    
	// 指向输出字符串的游动指针.
	po = pOutput;
	// 计算被替换串和替换串的长度.
	nSrcLen = (int)strlen(pSrc);
	nDstLen = (int)strlen(pDst);

	// 查找pi指向字符串中第一次出现替换串的位置,并返回指针(找不到则返回null).   
	p = strstr(pi, pSrc);
	if(p)
	{
		// 找到.
		while(p)
		{
			// 计算被替换串前边字符串的长度.
			nLen = (int)(p - pi);
			// 复制到输出字符串.
			memcpy(po, pi, nLen);
			memcpy(po + nLen, pDst, nDstLen);
			// 跳过被替换串.
			pi = p + nSrcLen;
			// 调整指向输出串的指针位置.
			po = po + nLen + nDstLen;
			// 继续查找.
			p = strstr(pi, pSrc);
		}
		// 复制剩余字符串.
		strcpy_s(po,strlen(pi)+1,pi);
	}
	else
	{
		// 没有找到则原样复制.
		strcpy_s(po,strlen(pi)+1, pi);
	}
}
//###########################################################
//#include <setjmp.h>
//#include <stdio.h>
//
//jmp_buf env;
//int count = 0;
//
//#define Try     if ((count = setjmp(env)) == 0)
//
//#define Catch(e)    else if (count == (e))
//
//#define Finally    ;
//
//#define Throw(idx)    longjmp(env, (idx))
//###########################################################
char* tl_loadfile(const char* xfile,int* outLengthPtr){
	unsigned int len;
	char* buffer = 0;
	FILE *stream;
	const char* file=0;

//#ifdef _MAIN_PATH_
//	char tempPath[G_BUFFER_256_SIZE];
//	//若定义了该宏 使用局部路径
//	memset(tempPath,0,G_BUFFER_256_SIZE);
//	sprintf(tempPath,"%s%s",_MAIN_PATH_,xfile);
//	file = tempPath;
//	if((int)strlen(xfile)+(int)strlen(_MAIN_PATH_)>G_BUFFER_256_SIZE){
//		printf("路径太长\n");
//		assert(0);
//	}
//#else
//	//未定义这个宏的时候 使用绝对路径
//	file = xfile;
//#endif
#ifdef DEBUG
	long time = get_longTime();
#endif

	char outStr[G_BUFFER_256_SIZE];
	errno_t err;
	if(tl_convert_path((char*)xfile,outStr,G_BUFFER_256_SIZE))
		file = outStr;
	else
		file = xfile;
	/*
	_CRT_SECURE_NO_WARNINGS
	1>e:/project/htt/ishow/functions.cpp(156) : warning C4996: 'fopen': This function or variable may be unsafe. 
	Consider using fopen_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
	解决方法：
	右击工程 - 属性 - 配置属性 - C/C++  - 命令行 
	命令行增加 /D _CRT_SECURE_NO_WARNINGS
	*/
	err = fopen_s(&stream, file, "rb+" );	//rb+ 读写打开一个二进制文件，允许读写数据，文件必须存在。
	if( err == 0 ){
		fseek(stream,0,SEEK_END);
		len = ftell(stream);
		//Try{
			buffer = (char*)tl_malloc((unsigned int)(len+1));
		//} Catch(1) {
			//printf("count:%s\n", xfile);
			//assert(0);
		//} Finally {
		//	//printf("other count\n");
		//}
		memset(buffer,0,len+1);
		fseek(stream,0,SEEK_SET);
		fread( buffer, sizeof( char ), len, stream);
		fclose( stream );
		if(outLengthPtr!=NULL){
			//pLength指针不为空的时候为其赋值
			*outLengthPtr = len;
		}
	}else{
		/* 找不到文件 此处报异常 */		
		log_code(ERROR_NOT_EXIST_FILE);
		printf("找不到文件%s\n",file);
		//assert(stream);
		assert(0);
	}
	//printf("%s\n",(char*)buffer);

	#ifdef DEBUG
		log_color(0xff00ff,"tl_loadfile 加载文件 %s (%d字节) 耗时 %ld 毫秒\n",xfile,len,get_longTime()-time);
	#endif

	return (char*)buffer;
}
/*


警告c4047：msdn给的解释是：

“operator”:“identifier1”与“identifier2”的间接寻址级别不同

指针可指向变量（一级间接寻址），指向另一个指向变量的指针（二级间接寻址）等。
其实这里的间接级别不同，就是间接寻址级别不同：
关于间接寻址寻址级别不同：
例子：




#include<stdio.h>
// C4047.c
// compile with: /W1
int main() {
char **p = 0;   // two levels of indirection
char *q = 0;   // one level of indirection

char *p2 = 0;   // one level of indirection
char *q2 = 0;   // one level of indirection

p = q;   // C4047
p2 = q2;
return 0;
}


这个问题如果有时不注意就可能造成错误，如error C2040。

*/
char *tl_strsep(char **stringp, const char *delim) {
	char *s;    
	const char *spanp;   
	int c, sc;     
	char *tok;     
	if ((s = *stringp)== NULL)   
		return (NULL);     
	for (tok = s;;){         
		c = *s++;      
		spanp = delim;    
		do {           
			if ((sc =*spanp++) == c) {         
				if (c == 0)           
					s = NULL;             
				else                  
					s[-1] = 0;          
				*stringp = s;      
				return (tok);
			}
		} while (sc != 0);
	}     /* NOTREACHED */
}

void tl_double2str(double d,char* out)
{
	char str[G_BUFFER_64_SIZE];
	int i,len,pos,cut=0,charLength=G_BUFFER_64_SIZE;

	memset(str,0,charLength);
	sprintf_s(str,G_BUFFER_256_SIZE,"%f",d);
	len = (int)strlen(str);
	pos = len;
	for(i=len;i>0;i--){
		char ch = str[i-1];
		if(cut!=1 && ch=='0'){
			pos = i;
		}else{
			cut = 1;
		}
	}
	memset(out,0,charLength);
	memcpy(out,str,pos-1);
}

int tl_isNum(char* s){
	
	int i;
	int flag = 1;
	char temp;
	
	char str[G_BUFFER_64_SIZE];
	int __length = (int)strlen(s);

	if(__length>G_BUFFER_64_SIZE)
	{
		printf("检测的字符文本太长!\n");
		assert(0);
	}
	if(__length<=0)
	{
		flag = 0;
	}
	else
	{
		memset(str,0,G_BUFFER_64_SIZE);
		//str = (char*)tl_malloc((int)strlen(s));
		memcpy(str,s,strlen(s));
		for(i = 0; i < (int)strlen(str); i++ ){
			temp = str[i];
			if( temp >= '0' && temp <='9' || temp=='-' || temp=='+' || temp == '.'){
				continue;
			}else{
				flag = 0;
				break;
			}
		}
		//tl_free(str);
	}
	return flag;
}



void tl_strsub(const char * dest,char* str,int start,int end){
	memcpy(str,dest+start,end-start+1);
}
//----------------------------------------------------------------
#define _SIGN_LEN_ 1//标示的长度
static void f_splitstr(char* str,const char* dest,int start,int i,int (*pCallBack)(char*),int* parms,int parmsLen)
{
	tl_strsub(dest,str,start,i-_SIGN_LEN_);
	parms[parmsLen] = (int)str;
	pCallBack((char*)parms);
}

void 
f_split_s(const char* dest,const char sign,int (*pCallBack)(char*),int* parms,int parmsLen){

	int i,start=0;
	char _ch;
	int len = (int)strlen(dest);
	char* str = (char*)MALLOC(len+1);
	for(i = 0;i < len;i++){
		_ch = dest[i];
		if(_ch==sign){
			memset(str,0,len+1);
			f_splitstr(str,dest,start,i,pCallBack,parms,parmsLen);
			start = i+_SIGN_LEN_;
		}
	}
	memset(str,0,len+1);
	f_splitstr(str,dest,start,i,pCallBack,parms,parmsLen);
	FREE(str);
}




#undef _SIGN_LEN_


int tl_charIndex(char* str,char c){
	char* ptr = strchr(str,c);
	if(ptr!=NULL)
		return (int)(ptr-str);

	return -1;
}
#define G_SIGN_SWAY '\\'		/*斜杆/ */

void tl_getPathByFile(char* path,char* dest){
	int i,len;
	char c;
	len = (int)strlen(path);
	for(i = len; i >= 0;i--){
		c = path[i];
		if(c == G_SIGN_SWAY){
			memcpy(dest,path,i+1);
			break;
		}
	}
}
/*
void tl_getLastString(const char* path,char* dest,const char sign){
	int i,len;
	char c;
	len = (int)strlen(path);
	for(i = len; i >= 0;i--){
		c = path[i];
		if(c == sign){
			memcpy(dest,path+i+1,len-i-1);
			break;
		}
	}
}
*/
int tl_getIndexByKey(const char* str,const char* name,const char d)
{
	int index = -1;
	char* res =	strstr((char*)str,(char*)name);//str_stri(str,name);
	if(res == NULL)
	{
		//未找到字符串	
	}
	else
	{
		int length = (int)strlen(str);		
		int i = 0;
		int start = 0;
		int cur = 0;
		char cstr[G_BUFFER_512_SIZE];
		int k = 0;
		while(i<length)
		{
			char c = str[i];
			if(c==d)
			{
				int t =0;
				memset(cstr,0,G_BUFFER_512_SIZE);
				if(k > 0)
				{
					t = 1;	
				}
				memcpy(cstr,str+start+t,cur);

				start = i;
				cur = 0;
				if(!strcmp(cstr,name))
				{
					//找到匹配字符串,返回索引
					index = k;
					break;	
				}
				k++;
			}
			else
			{
				cur++;
			}
			i++;
		}
	}
	return index;
}
static	int 
f_getFileNameSplice(char* str,void* parms){

	#define _BufferSize_ G_BUFFER_128_SIZE

	//struct StrCutSplit* p = (struct StrCutSplit*)point;
	//char* str = p->str;
	char* outStr = (char*)parms;//p->parms;
	char* i=strstr(str,".");
	int len;
	//printf("[%s,%s]",str,i);
	if(i!=NULL){
		len = (int)strlen(str);
		if(len>_BufferSize_){
			printf("切割字符串长度溢出:%d\n",len);
			assert(0);
		}
		memset(outStr,0,_BufferSize_);
		memcpy(outStr,str,len-strlen(i));
	}
	return 0;
}
void tl_getPathName(const char* str,char* fileName){
	str_split_cut(str,'\\',f_getFileNameSplice,(void*)fileName);
}
#define G_SIGN_POINT '.'		/*符号.*/
void tl_getSuffixByPath(char* path,char* _out,int _outBufferSize){
	int i,len;
	char c;

	memset(_out,0,_outBufferSize);
	len = (int)strlen(path);
	for(i = len; i >= 0;i--){
		c = path[i];
		if(c == G_SIGN_POINT){
			memcpy(_out,path+i+1,len-i-1);
			break;
		}
	}
}
int tl_big_endian (void){
	union{
		long l;
		char c[sizeof(long)];
	}u;
	u.l = 1;
	return  (u.c[sizeof(long) - 1] == 1);
}



/******************************************************************************/

void tl_setString(const char* dest,const char* key,char* out){
	int len = (int)strlen(key);
	if(strncmp(dest,key,len)== 0){
		char _str[G_BUFFER_256_SIZE];
		memset(_str,0,G_BUFFER_256_SIZE);
		//char* _str = (char*)tl_malloc(len+1);
		//memset(_str,0,len+1);
		memcpy(_str,dest+len+1,strlen(dest)-len-1);
		sscanf_s(_str,"%s",out,strlen(_str));
		//tl_free(_str);
	}
}
void tl_setInt(const char* dest,const char* key,int* out){
	int len = (int)strlen(key);
	if(strncmp(dest,key,len)== 0){
		char _str[G_CHAR_SIZE_];
		memset(_str,0,G_CHAR_SIZE_);
		memcpy(_str,dest+len+1,strlen(dest)-len-1);
		sscanf_s(_str,"%d",out);
	}
}

void tl_get_exe_path(char *argv[],char* out){
	int i; 
	char *name; 
	name = strrchr(argv[0], 0x5c);  
	if(name != NULL){
		for(i=0; i<(int)strlen(name); i++) {
			name[i] = name[i+1];  
		}
		memcpy(out,argv[0],strlen(argv[0])-strlen(name));
		sprintf_s(out,128,"%s\\",out);
	}
}
/*
void tl_http_get_query_string(char* result){
	char* data = getenv("QUERY_STRING");
	if(data != NULL){
		memcpy(result,data,strlen(data));
	}else{
		//未找到数据
		printf("tl_http_get_query_string(...) didn`t get data!");
	}
}
*/


/* */ 
char f_find_pos(char ch){ 
	const char BASE_64_STRING[] = DEFINE_BASE_64_STRING;
	char *ptr = (char*)strrchr(BASE_64_STRING, ch);//the last position (the only) in base[] 
	return (char)(ptr - BASE_64_STRING); 
}
//include <stdio.h> 

//char* base64_encode(const char* data, int data_len); 
//char* base64_decode(const char* data, int data_len); 
//static char find_pos(char ch); 
/*
int main(int argc, char* argv[]) 
{ 
char *t = "那个abcd你好吗，哈哈，ANMOL"; 
int i = 0; 
int j = strlen(t); 
char *enc = base64_encode(t, j); 
int len = strlen(enc); 
char *dec = base64_decode(enc, len); 
printf("\noriginal: %s\n", t); 
printf("\nencoded : %s\n", enc); 
printf("\ndecoded : %s\n", dec); 
tl_free(enc); 
tl_free(dec); 
return 0; 
}*/
/* */ 
char* tl_base64_encode(const char* data, int data_len) { 
	//int data_len = strlen(data); 
	int prepare = 0; 
	int ret_len; 
	int temp = 0; 
	char *ret = NULL; 
	char *f = NULL; 
	int tmp = 0; 
	char changed[4]; 
	int i = 0; 
	const char* BASE_64_STRING= DEFINE_BASE_64_STRING;
	ret_len = data_len / 3; 
	temp = data_len % 3; 
	if (temp > 0) { 
		ret_len += 1; 
	} 
	ret_len = ret_len*4 + 1; 
	ret = (char *)tl_malloc(ret_len); 

	if ( ret == NULL) { 
		printf("No enough memory.\n"); 
		exit(0); 
	} 
	memset(ret, 0, ret_len); 
	f = ret; 
	while (tmp < data_len) { 
		temp = 0; 
		prepare = 0; 
		memset(changed, '\0', 4); 
		while (temp < 3) { 
			//printf("tmp = %d\n", tmp); 
			if (tmp >= data_len) 
			{ 
				break; 
			} 
			prepare = ((prepare << 8) | (data[tmp] & 0xFF)); 
			tmp++; 
			temp++; 
		} 
		prepare = (prepare<<((3-temp)*8)); 
		//printf("before for : temp = %d, prepare = %d\n", temp, prepare); 
		for (i = 0; i < 4 ;i++ ) { 
			if (temp < i) { 
				changed[i] = 0x40; 
			} else { 
				changed[i] = (prepare>>((3-i)*6)) & 0x3F; 
			} 
			*f = BASE_64_STRING[changed[i]]; 
			//printf("%.2X", changed[i]); 
			f++; 
		} 
	} 
	*f = '\0'; 
	return ret; 
} 
/* */ 
char* tl_base64_decode(const char *data, int data_len) { 
	int ret_len = (data_len / 4) * 3; 
	int equal_count = 0; 
	char *ret = NULL; 
	char *f = NULL; 
	int tmp = 0; 
	int temp = 0; 
	char need[3]; 
	int prepare = 0; 
	int i = 0; 
	if (*(data + data_len - 1) == '=') { 
		equal_count += 1; 
	} 
	if (*(data + data_len - 2) == '=') { 
		equal_count += 1; 
	} 
	if (*(data + data_len - 3) == '=') {//seems impossible 
		equal_count += 1; 
	} 
	switch (equal_count) { 
	case 0: 
		ret_len += 4;//3 + 1 [1 for NULL] 
		break; 
	case 1: 
		ret_len += 4;//Ceil((6*3)/8)+1 
		break; 
	case 2: 
		ret_len += 3;//Ceil((6*2)/8)+1 
		break; 
	case 3:
		ret_len += 2;//Ceil((6*1)/8)+1 
		break; 
	} 
	ret = (char *)tl_malloc(ret_len); 
	if (ret == NULL) { 
		printf("No enough memory.\n"); 
		exit(0); 
	} 
	memset(ret, 0, ret_len); 
	f = ret; 
	while (tmp < (data_len - equal_count)) { 
		temp = 0; 
		prepare = 0; 
		memset(need, 0, 4); 
		while (temp < 4) { 
			if (tmp >= (data_len - equal_count)) { 
				break; 
			} 
			prepare = (prepare << 6) | (f_find_pos(data[tmp])); 
			temp++; 
			tmp++; 
		} 
		prepare = prepare << ((4-temp) * 6); 
		for (i=0; i<3 ;i++ )  { 
			if (i == temp)  { 
				break; 
			} 
			*f = (char)((prepare>>((2-i)*8)) & 0xFF); 
			f++; 
		} 
	} 
	*f = '\0'; 
	return ret; 
}

int 
tl_strpos_s(const char *haystack,const char *needle){
	//int k=tl_strpos(haystack,needle,1);
	int pos = -1;
	char* _position=strstr(haystack,needle);
	if(_position){
		pos = (int)(_position-haystack);
	}
	//if(k!=pos){
	//	assert(0);
	//}
	return pos;	
}

int tl_strlen(const char* str){
	return (int)strlen(str);
}
//==============================================================
/*
*	Function:	equalToLStack
*	--------------------------
*	交换两栈的数据
*/
static void equalToLStack(struct LStackNode* dest,struct LStackNode* s);

struct LStackNode* LStack_create(){
	struct LStackNode *top = (struct LStackNode*)MALLOC(sizeof(struct LStackNode));
	memset(top,0,sizeof(struct LStackNode));
	if(top==0)
		return 0;//printf("LStack_init error");
	top->next=0;
	top->pre =0;
	return top;
}
void LStack_delete(struct LStackNode* top){
	struct LStackNode *p,*q;
	p=top;
	while(p!=0){
		q=p;
		p=p->next;
		if(q!=0){
			FREE(q);
		}
		q =0;
	}
}
//void LStack_delete_S(struct LStackNode* top,CallBackFun callBack){
//	struct LStackNode *p,*q;
//	p=top;
//	while(p!=0){
//		q=p;
//		callBack((char*)p->data);
//		p=p->next;
//		if(q!=0){
//			FREE(q);
//		}
//		q =0;
//	}
//}

void LStack_clear(struct LStackNode* top){
	int c;
	while(LStack_length(top)!=0){	
		LStack_pop(top,&c);
	}
}

//删除链栈指定的节点	1:删除操作成功	0:删除操作失败
int LStack_delNode(struct LStackNode *p,int node){
	//用双端链表实现,前驱和后驱的的节点处理
	//struct LStackNode *p = dest;
	while(p!=0&&p->next!=0){
		if(p!=0){
			struct LStackNode* pre = p;
			p=p->next;
			if(p->data == node){
				struct LStackNode* next = p->next;
				if(next){
					next->pre = pre;
				}
				pre->next = next;
				p->next = 0;
				FREE(p);
				p = 0;
				return 1;
			}
		}
	}
	return 0;
}
static
int LStack_isEmpty(struct LStackNode* top){
	if(top->next==0){			/*判断链栈头结点的指针域是否为空*/
		return 1;			/*当链栈为空时，返回1；否则返回0*/
	}
	return 0;
}
int LStack_push(void* plt,void* nodePoint){
	struct LStackNode* lt = (struct LStackNode*)plt;
	struct LStackNode* top = (struct LStackNode*)lt;
	
	//为当前的节点分配一块内存
	struct LStackNode *p=(struct LStackNode*)MALLOC(sizeof(struct LStackNode));
	
	if(!p){
		printf("链栈节点,内存分配失败!\n");
		assert(0);
		return -1;
	}
	p->data = (int)nodePoint;
	
	p->next=top->next;
	p->pre = top;
	top->next=p;
	return 1;
}
int 
LStatk_getNodeByIndex(struct LStackNode* top,int index,int* out)
{
	struct LStackNode *p;
	int count=0;
	int listCount;
	p=top;

	listCount = LStack_length(top);

	if(index-1 > listCount)
	{
		return 0;
	}

	while(p!=0&&p->next!=0)
	{
		if(p!=0)
		{
			p=p->next;
			if(count == (listCount-index -1))
			{
				*out =(int)p->data;
				return 1;
			}
			count++;
		}
	}
	return 0;
}

int LStatk_getAddressByIndex(struct LStackNode* top,int index)
{
	struct LStackNode *p;
	int count=0;
	int listCount;
	p=top;

	listCount = LStack_length(top);

	if(index-1 > listCount)
	{
		return 0;
	}

	while(p!=0&&p->next!=0)
	{
		if(p!=0)
		{
			p=p->next;
			if(count == (listCount-index -1))
			{
				//*out =(float)p->data;
				//return 1;
				return p->data;
			}
			count++;
		}
	}
	return 0;
}

int LStack_pop(struct LStackNode* top,int *e){
	struct LStackNode* pNode,*p;
	p=top->next;
	if(!p){
		return 0;
	}
	pNode = p->next;
	p->pre = top;
	
	/*将栈顶结点与链表断开，即出栈*/
	top->next=p->next;				
	
	*e=p->data;
	
	//释放这个节点的引用
	FREE(p);

	return 1;
}
int LStack_length(struct LStackNode* top){
	struct LStackNode *p;
	int count=0;
	p=top;
	while(p!=0&&p->next!=0){
		if(p!=0){
			p=p->next;
			count++;
		}
	}
	return count;
}
int LStack_top(struct LStackNode* top,int *e){
	struct LStackNode *p=top->next;
	if(!p){						/*判断链栈是否为空*/
		return 0;//printf("top链栈已空");
	}
	*e=p->data;					/*将出栈元素赋值给e*/
	return 1;
}
void LStack_rev(struct LStackNode* dest){
	struct LStackNode* s = (struct LStackNode*)LStack_create();
	struct LStackNode* p = dest;
	while(LStack_length(dest)!=0){
		int v;
		LStack_pop(dest,&v);
		LStack_push(s,(void*)v);
	}
	equalToLStack(dest,s);
}
void LStack_shift(struct LStackNode* dest,int *e){
	if(LStack_length(dest)==1){
		LStack_pop(dest,e);
	}else{
		if(LStack_length(dest)){
			struct LStackNode* s = (struct LStackNode*)LStack_create();
			int v;
			while(LStack_length(dest)){
				LStack_pop(dest,&v);
				LStack_push(s,(void*)v);
			}
			LStack_rev(s);
			LStack_pop(s,&v);
			*e = v;
			equalToLStack(dest,s);
		}
	}
}
static void equalToLStack(struct LStackNode* dest,struct LStackNode* s){
	struct LStackNode* node = s->next;
	node->pre = dest;
	dest->next = node;
	s->next = 0;
	LStack_delete(s);
}
struct ICompare {
	int field;
};
#define T ICompare
/*冒泡排序*/
static void sortBubbleNodes(int* list,int len,int sortType){
	int i;
	for(i=0;i<len;i++){
		int j;
		for(j=i+1;j<len;j++){
			int temp;
			struct T* ai=(struct T*)list[i];
			struct T* aj=(struct T*)list[j];
			if(	sortType == SORT_ASCEND	&&	ai->field < aj->field	||
				sortType == SORT_DROP	&&	ai->field >= aj->field
				){
					//交换数据
					temp=(int)ai;
					list[i]=(int)aj;
					list[j]=temp;
			}
		}
	}
}
/*赋值*/
static void f_tl_fillData(int* list,struct LStackNode* s){
	int index=0;
	struct LStackNode *p;
	p=s;
	while(p!=0 && p->next!=0){
		if(p!=0){
			p=p->next;
			list[index]=(int)p->data;
			index++;
		}
	}
}
/*
比较的字段,第一个字段可以赋值,对第一个字段进行排序即可排序链表
案例可以查看ExampleLStackNodeSort		
*/
void LStack_sort(struct LStackNode* p,int type){

	int length = LStack_length(p);

	int i;

	int* pList = (int*)MALLOC(sizeof(int)*length);

	f_tl_fillData(pList,p);

	sortBubbleNodes(pList,length,type);

	LStack_clear(p);
	for(i=0;i<length;i++){
		LStack_push(p,(void*)pList[i]);
	}
	FREE(pList);
}
#undef T

void ExampleLStackNodeSort(){
#define SIZE 3
	/*MyStruct可以是任何结构体,结构体的第一个字段f为排序值*/
	struct MyStruct{
		int f;	/*保证第一个字段为序号字段*/
		char* name;
		int type;
	};
#define T MyStruct
	struct LStackNode* s = (struct LStackNode*)LStack_create();
	int a[SIZE]={67,78,72};

	int i;
	for(i=SIZE-1;i>=0;i--){
		struct T* st=(struct T*)MALLOC(sizeof(struct T));

		st->f = a[i];
		LStack_push(s,st);
	}	    

	printf("length:	%d\n",LStack_length(s));

	LStack_sort(s,SORT_ASCEND);

	while(LStack_length(s)!=0){
		int va;
		struct	T* ts = NULL;
		LStack_shift(s,&va);
		ts = (struct T*)va;
		printf("0x%d  %d\n",(int)ts,ts->f);
	}
#undef SIZE
#undef T
}

void* LStack_next(void* s)
{
	struct LStackNode* _s = (struct LStackNode*)s;
	return (void*)(_s->next);
}
void* LStack_pre(struct LStackNode* s){
	return s->pre;
}
int LStack_data(void* s)
{
	struct LStackNode* _s = (struct LStackNode* )s;
	return _s->data;
}

void LStack_ergodic(struct LStackNode* list,int (*callBack)(int,int),int parm){
	struct LStackNode* s = (struct LStackNode* )list;
	void* top,*p;
	top = s;
	p=top;
	while((int)LStack_next(p)){
		int data;
		p=(void*)LStack_next(p);
		data = LStack_data(p);
		if(!callBack(data,parm)){
			//callBack返回0的时候中断遍历
			return;
		}
	}
}

void LStack_ergodic_t(struct LStackNode* list,void (*callBack)(int,int),int parm){
	struct LStackNode* s = (struct LStackNode* )list;
	void* top,*p;
	top = s;
	p=top;
	while((int)LStack_next(p)){
		int data;
		p=(void*)LStack_next(p);
		data = LStack_data(p);
		callBack(data,parm);
	}
}

void LStack_IErgodic_S(void* _s,int (*callBack)(char*),int* parms){
	struct LStackNode* s = (struct LStackNode* )_s;
	void* top,*p;
	top = s;
	p=top;
	while((int)LStack_next(p)){
		int data;
		p=(void*)LStack_next(p);
		data = LStack_data(p);
		parms[0] = data;
		//callBack_Param((void*)data,in);//回调
		callBack((char*)parms);
	}
}


//=============================================================================
static char dec2HexChar(short int n) {
	char NUM = '0';
	char CH = 'A';
	if ( 0 <= n && n <= 9 ) {
		return NUM + n;
	} else if ( 10 <= n && n <= 15 ) {
		return ( CH + n - 10 );
	} 
	return 0;
}
static short int hexChar2Dec(char c){
	char NUM_0 = '0';
	char CH_a = 'a';
	char CH_A = 'A';
	char CH_f = 'f';
	char CH_F = 'F';
	if ( '0'<=c && c<='9' ) {
		return (c-NUM_0);
	} else if ( 'a'<=c && c<=CH_f ) {
		return (c-CH_a) + 10;
	} else if ( 'A'<=c && c<=CH_F ) {
		return (c-CH_A) + 10;
	}
	return -1;
}
void tl_urlEncode(const char* URL,char* strResult){
	unsigned int i,length,pos=0;
	int i1,i0,j;
	char _ch;
	memset(strResult,0,G_BUFFER_2048_SIZE);
	length = (unsigned int)strlen(URL);
	for (i=0; i<length; i++ ){
		char c = URL[i];
		printf("%d ",c);
		if (
			( '0'<=c && c<='9' ) ||
			( 'a'<=c && c<='z' ) ||
			( 'A'<=c && c<='Z' ) ||
			c=='/' || c=='.'
			) {
				memcpy(strResult+pos,&c,1);
				pos++;
		} else {
			j = (short int)c;
			if ( j < 0 ){
				j += 256;
			}
			i1 = j / 16;
			i0 = j - i1*16;
			memcpy(strResult+pos,"%",1);
			_ch = dec2HexChar((short int)i1);
			memcpy(strResult+pos+1,&_ch,1);
			_ch = dec2HexChar((short int)i0);
			memcpy(strResult+pos+2,&_ch,1);
			pos+=3;
		}
	}
}
void tl_urlDecode(const char* URL,char* result){
	int length,pos = 0,num = 0;
	char c,c0,c1,c2;
	int i;
	length = (int)strlen(URL);
	for (i=0; i<length; i++ ) {
		c = URL[i];
		if ( c != '%' ) {
			memcpy(result+pos,&c,1);
			pos++;
			//printf("%d ",c);
		} else{
			c1 = URL[++i];
			c0 = URL[++i];
			num = 0;
			num += hexChar2Dec(c1) * 16 + hexChar2Dec(c0);
			c2 = num;
			memcpy(result+pos,&c2,1);
			//printf("%d ",c2);
			pos++;
		}
	}
}


static void f_quick(float* pData,int left,int right) {
	int i,j; 
	float middle,iTemp; 
	i = left; 
	j = right; 
	middle = pData[(left+right)/2]; //求中间值 
	do{ 
		while((pData[i]<middle) && (i<right)){//从左扫描大于中值的数 
			i++; 
		}
		while((pData[j]>middle) && (j>left)){//从右扫描大于中值的数 
			j--; 
		}
		if(i<=j)//找到了一对值 
		{ 
			//交换 
			iTemp = pData[i]; 
			pData[i] = pData[j]; 
			pData[j] = iTemp; 
			i++; 
			j--; 
		} 
	}while(i<=j);//如果两边扫描的下标交错，就停止（完成一次）

	//当左边部分有值(left<j)，递归左半边 
	if(left<j){ 
		f_quick(pData,left,j); 
	}
	//当右边部分有值(right>i)，递归右半边 
	if(right>i) {
		f_quick(pData,i,right); 
	}
} 
//快速排序(升序)
void tl_quickSort(float* pData,int Count) 
{ 
	f_quick(pData,0,Count-1); 
}

void tl_bubbleSort(float* _array,int _length)
{
	int i,j;
	for(i=0;i<_length-1;i++) 
	{ 
		for(j=i;j<_length;j++ ){ 
			if(_array[i]>_array[j]){ 
				float temp = _array[i]; 
				_array[i]=_array[j]; 
				_array[j] = temp;   
			} 
		} 
	} 
}
void tl_bubbleSortInt(int* _array,int _length)
{
	int i,j;
	for(i=0;i<_length-1;i++) 
	{ 
		for(j=i;j<_length;j++ ){ 
			if(_array[i]>_array[j]){ 
				int temp = _array[i]; 
				_array[i]=_array[j]; 
				_array[j] = temp;   
			} 
		} 
	} 
}









//=======================================================================================
//射线与三角形相交
//http://wenku.baidu.com/link?url=bU7DdIlje395HIuaJLRkCc-mjhaKGD6m9vV4N_141yUV4TwieoQ94rKBZOl83yvIaDwDaV0XNAz04P-WouAaiNC-6HnbFYsK279aX2n8i6y


#define CROSS(dest,v1,v2)\
	dest[0]=v1[1]*v2[2]-v1[2]*v2[1];\
	dest[1]=v1[2]*v2[0]-v1[0]*v2[2];\
	dest[2]=v1[0]*v2[1]-v1[1]*v2[0];
#define DOT(v1,v2)(v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2])
#define SUB(dest,v1,v2)\
	dest[0]=v1[0]-v2[0];\
	dest[1]=v1[1]-v2[1];\
	dest[2]=v1[2]-v2[2];
/*
三角形上的顶点方程
T=(1-u-v)V0+uV1+vV2(0≤u≤1，0≤v≤1，0≤u+v≤1).

向量加减法
A(X1,Y1) B(X2,Y2)，则A+B=（X1+X2，Y1+Y2），A-B=（X1-X2，Y1-Y2） 
*/
static int f_interTriangle(float orig[3],float dir[3],float vert0[3],float vert1[3],float vert2[3],float*t,float*u,float*v)
{
	float edge1[3],edge2[3],tvec[3],pvec[3],qvec[3];
	float det,inv_det;
	/*find vectors for two edges sharing vert0*/
	SUB(edge1,vert1,vert0);
	SUB(edge2,vert2,vert0);
	/*begin calculating determinant-also used to calculate U parameter*/
	CROSS(pvec,dir,edge2);
	/*if determinant is near zero,ray lies in plane of triangle*/
	det=DOT(edge1,pvec);
	//a·b>0    方向基本相同，夹角在0°到90°之间 
	//a·b=0    正交  
	//a·b<0    方向基本相反，夹角在90°到180°之间

#ifdef TEST_CULL//是否,剔除背面三角形

	/*define TEST_CULL if culling is desired*/
	if(det<EPSILON){
		return 0;
	}

	/*calculate distance from vert0 to ray origin*/
	SUB(tvec,orig,vert0);

	/*calculateUparameterandtestbounds*/
	*u=DOT(tvec,pvec);
	if(*u<0.0||*u>det){
		return 0;
	}
	/*prepare to test V parameter*/
	CROSS(qvec,tvec,edge1);

	/*calculate V parameter and test bounds*/
	*v=DOT(dir,qvec);
	if(*v<0.0||*u+*v>det){
		return 0;
	}

	/*calculate t,scale parameters,ray intersects triangle*/
	*t=DOT(edge2,qvec);
	inv_det=1.0/det;
	*t*=inv_det;
	*u*=inv_det;
	*t*=inv_det;
#else
	/*the non-culling branch*/
	if(det>-EPSILON&&det<EPSILON){
		return 0;
	}
	inv_det=1.0f/det;

	/*calculate distance from vert0 to ray origin*/
	SUB(tvec,orig,vert0);

	/*calculate U parameter and test bounds*/
	*u=DOT(tvec,pvec)*inv_det;
	if(*u<0.0||*u>1.0){
		return 0;
	}

	/*prepare to test V parameter*/
	CROSS(qvec,tvec,edge1);

	/*calculate V parameter and test bounds*/
	*v=DOT(dir,qvec)*inv_det;
	if(*v<0.0||*u+*v>1.0){
		return 0;
	}

	/*calculatet,ray intersects triangle*/
	*t=DOT(edge2,qvec)*inv_det;
#endif
	return 1;
}

void 
tl_hitTriangle(float orig[3],float dir[3],float vert0[3],float vert1[3],float vert2[3],float* out,int* pHit){
	float t,u,v,r;
	int i;

	for(i = 0;i <3;i++)
		out[i]=0;

	*pHit = f_interTriangle(orig,dir,vert0,vert1,vert2,&t,&u,&v);
	if(*pHit){
		for(i = 0;i < 3;i++){
			//out[i] = (1-u-v)*vert0[i] + u*vert1[i] + v*vert2[i];//计算射线在三角形内的交点坐标
			r = (1-u-v)*vert0[i] + u*vert1[i] + v*vert2[i];
			out[i]=r;
		}
	}
}
//#undef EPSILON
#undef CROSS
#undef DOT
#undef SUB

//=============================================================


//uvs normals vs
/*
struct VerNorUvStruct* tl_parseVerts2(float* verts,int len){
	struct VerNorUvStruct* pv = (struct VerNorUvStruct*)tl_malloc(sizeof(struct VerNorUvStruct));
	int i,k = 0,u = 0,n = 0,ver_span = 3,uv_spane=2,normal_span = 3;
	int all_span = ver_span + normal_span + uv_spane;
	
	float* vs =			(float*)tl_malloc(sizeof(float)*(len*ver_span+1));
	float* normals =	(float*)tl_malloc(sizeof(float)*(len*normal_span+1));
	float* uvs =		(float*)tl_malloc(sizeof(float)*(len*uv_spane+1));

	memset(vs,0,len*ver_span+1);
	memset(normals,0,len*normal_span+1);
	memset(uvs,0,len*uv_spane+1);

	memset(pv,0,sizeof(struct VerNorUvStruct));
	
	for(i = 0;i<len*all_span;i+=all_span){
		uvs[u] = verts[i];		
		uvs[u+1] = verts[i+1];

		normals[n]= verts[i+2];	
		normals[n+1]= verts[i+3];	
		normals[n+2]= verts[i+4];

		vs[k] = verts[i+5];		
		vs[k+1] = verts[i+6];		
		vs[k+2] = verts[i+7];

		u+=uv_spane;k+=ver_span;n+=normal_span;
	}
	pv->pvs = vs;
	pv->pnormals = normals;
	pv->puvs = uvs;

	return pv;
}
*/
/*
char tl_str2ch(const char* str){
	if(strlen(str)>=1)
	{
		return str[0];
	}
	return (char)NULL;
}
*/
//int animsSplit(char* point)
//{
//	int* parms = (int*)point;
//	struct SplitObj* p = (struct SplitObj*)parms[0];
//	char* str =  (char*)parms[1];
//
//	if(p->index == p->type){
//		memcpy(p->cur,str,strlen(str));
//	}
//	p->index++;
//	return 1;
//}
//void tl_strType(const char* str,int type,char* out,int wordSize)
//{
//	struct SplitObj pp;
//	struct SplitObj* p = &pp;
//
//	int parms[2];
//
//	p->index = 0;
//	p->type  = type;
//	p->cur = (char*)tl_malloc(wordSize);
//	memset(p->cur,0,wordSize);
//
//	parms[0] = (int)p;
//	f_split_s((const char*)str,',',animsSplit,parms,1);
//
//	memset(out,0,wordSize);
//	if(strlen(p->cur)>0){
//		memcpy(out,p->cur,strlen(p->cur));
//	}else{
//		printf("didn`t find type:%d\n",type);
//	}
//	tl_free(p->cur);
//	p->cur = NULL;
//}
static int typeSplit(char* point)
{
	int* parms = (int*)point;
	struct SplitObj* p = (struct SplitObj*)parms[0];
	char* str =  (char*)parms[1];
	if(strcmp((const char*)p->cur,(const char*)str) == 0){
		p->type = p->index;
	}
	p->index++;
	return 1;
}
/*
//废弃
int
tl_typeStr(const char* str,const char* target){
	struct SplitObj pp;
	struct SplitObj* p = &pp;

	int parms[2];
	int len = (int)strlen(target)+1;
	p->index = 0;
	p->type = -1;
	p->cur = (char*)tl_malloc(len);
	memset(p->cur,0,len);
	memcpy(p->cur,target,strlen(target));
	parms[0] = (int)p;
	f_split_s((const char*)str,',',typeSplit,parms,1);
	tl_free(p->cur);
	return p->type;
}
*/
/*
	复制一个字符串,返回一个堆上的引用
*/
/*
char* tl_strCopy(const char* str){
	int len = (int)strlen(str);
	char* s = NULL;
	len++;
	s = (char*)tl_malloc(len);
	memset(s,0,len);
	memcpy(s,str,len);
	return s;
}
*/
//==========================================================================


/*
	return	负数	逆时针
			正数	顺时针
*/
static float getAngleState(struct Vec3* a,struct Vec3* b){
	return (a->x*b->x)-(a->y*b->y)-(a->z*b->z);
}

static float tmp;
static int plusMinusVar=0;

/*
*	判断两个向量之间夹角是顺时针还是逆时针
*	<x1,y1>是一个方向向量一般是<0,1>
*	return 顺时针方向就返回1,逆时针方向就返回-1
*
*
*	利用平面向量的叉乘
*
*	a = (x1,y1)    b = (x2,y2)
*
*	a×b = x1y2 - x2y1
*
*	若结果为正，则向量b在a的顺时针方向
*
*	否则，在a的逆时针方向
*
*	若结果为0，则a与b共线
*/
static int checkPlusMinus(float x1,float y1,float x2,float y2){
	return ( x1*y2 - x2*y1) > 0 ? 1 : -1;
}

float vecToAngle(struct Vec3* a,struct Vec3* b){
	float t,kk,sum,dis;
	
	//vec3Normalize(a);
	//vec3Normalize(b);

	//顺时针为1,逆时针-1
	int _st;
	
	_st = checkPlusMinus(0.0f,1.0f,b->x,b->y);
	if(_st!=plusMinusVar){
		plusMinusVar = _st;
		printf("%d\n",_st);
	}

	t=	a->x*a->x+
		a->y*a->y+
		a->z*a->z;

	kk=	b->x*b->x+
		b->y*b->y+
		b->z*b->z;
	
	dis=(float)sqrt(t+kk);//两向量的距离

	t=a->x*b->x+a->y*b->y+a->z*b->z; 
	
	sum=t/dis; 
	
	sum=(float)acos(sum);
	
	t=sum/PI;
	
	if(tmp!=t){
	//	printf("%.2f\n",t*180.0f);
	}

	tmp = t;

	//t:顺时针,-t逆时针

	t = t * 180.0f;

	if(_st>0){
		t = 360.0f-t;
	}

	return t;
}

int tl_split_getlen(const char* str,char key){
	int _k = 0;
	int start = 0;
	int end;
	int index = tl_charIndex((char*)str,key);
	while (index!=-1){
		end = index+start;
		start=end+1;//偏移一个
		index = tl_charIndex((char*)(str + start),key);
		_k++;
	}
	_k++;
	return _k;
}

float vecDot( struct Vec3* a,struct Vec3* b )
{
	return a->x * b->x + a->y * b->y + a->z * a->z;
}

static int 
tl_bintree(int t){
	int x = 0,a=t;
	while(a!=1){
		a=a/2;	x++;
	}
	return x;
}
/*


void matrix_t(double **a_matrix, const double **b_matrix, int krow, int kline)
////////////////////////////////////////////////////////////////////////////
//	a_matrix:转置后的矩阵
//	b_matrix:转置前的矩阵
//	krow    :行数
//	kline   :列数
////////////////////////////////////////////////////////////////////////////
{
	int k, k2;   

	for (k = 0; k < krow; k++)
	{
		for(k2 = 0; k2 < kline; k2++)
		{
			a_matrix[k2][k] = b_matrix[k][k2];
		}
	}
}
void matrix_a(double **a_matrix, const double **b_matrix, const double **c_matrix, 
			  int krow, int kline, int ktrl)
			  ////////////////////////////////////////////////////////////////////////////
			  //	a_matrix=b_matrix+c_matrix
			  //	 krow   :行数
			  //	 kline  :列数
			  //	 ktrl   :大于0: 加法  不大于0:减法
			  ////////////////////////////////////////////////////////////////////////////
{
	int k, k2;

	for (k = 0; k < krow; k++)
	{
		for(k2 = 0; k2 < kline; k2++)
		{
			a_matrix[k][k2] = b_matrix[k][k2]
			+ ((ktrl > 0) ? c_matrix[k][k2] : -c_matrix[k][k2]); 
		}
	}
}

void matrix_m(double **a_matrix, const double **b_matrix, const double **c_matrix,
			  int krow, int kline, int kmiddle, int ktrl)
			  ////////////////////////////////////////////////////////////////////////////
			  //	a_matrix=b_matrix*c_matrix
			  //	krow  :行数
			  //	kline :列数
			  //	ktrl  :	大于0:两个正数矩阵相乘 不大于0:正数矩阵乘以负数矩阵
			  ////////////////////////////////////////////////////////////////////////////
{
	int k, k2, k4;
	double stmp;

	for (k = 0; k < krow; k++)     
	{
		for (k2 = 0; k2 < kline; k2++)   
		{
			stmp = 0.0;
			for (k4 = 0; k4 < kmiddle; k4++)  
			{
				stmp += b_matrix[k][k4] * c_matrix[k4][k2];
			}
			a_matrix[k][k2] = stmp;
		}
	}
	if (ktrl <= 0)   
	{
		for (k = 0; k < krow; k++)
		{
			for (k2 = 0; k2 < kline; k2++)
			{
				a_matrix[k][k2] = -a_matrix[k][k2];
			}
		}
	}
}


int  matrix_inv(double **a_matrix, int ndimen)
////////////////////////////////////////////////////////////////////////////
//	a_matrix:矩阵
//	ndimen :维数
////////////////////////////////////////////////////////////////////////////
{
	double tmp, tmp2, b_tmp[20], c_tmp[20];
	int k, k1, k2, k3, j, i, j2, i2, kme[20], kmf[20];
	i2 = j2 = 0;

	for (k = 0; k < ndimen; k++)  
	{
		tmp2 = 0.0;
		for (i = k; i < ndimen; i++)  
		{
			for (j = k; j < ndimen; j++)  
			{
				if (fabs(a_matrix[i][j] ) <= fabs(tmp2)) 
					continue;
				tmp2 = a_matrix[i][j];
				i2 = i;
				j2 = j;
			}  
		}
		if (i2 != k) 
		{
			for (j = 0; j < ndimen; j++)   
			{
				tmp = a_matrix[i2][j];
				a_matrix[i2][j] = a_matrix[k][j];
				a_matrix[k][j] = tmp;
			}
		}
		if (j2 != k) 
		{
			for (i = 0; i < ndimen; i++)  
			{
				tmp = a_matrix[i][j2];
				a_matrix[i][j2] = a_matrix[i][k];
				a_matrix[i][k] = tmp;
			}    
		}
		kme[k] = i2;
		kmf[k] = j2;
		for (j = 0; j < ndimen; j++)  
		{
			if (j == k)   
			{
				b_tmp[j] = 1.0 / tmp2;
				c_tmp[j] = 1.0;
			}
			else 
			{
				b_tmp[j] = -a_matrix[k][j] / tmp2;
				c_tmp[j] = a_matrix[j][k];
			}
			a_matrix[k][j] = 0.0;
			a_matrix[j][k] = 0.0;
		}
		for (i = 0; i < ndimen; i++)  
		{
			for (j = 0; j < ndimen; j++)  
			{
				a_matrix[i][j] = a_matrix[i][j] + c_tmp[i] * b_tmp[j];
			}  
		}
	}
	for (k3 = 0; k3 < ndimen;  k3++)   
	{
		k  = ndimen - k3 - 1;
		k1 = kme[k];
		k2 = kmf[k];
		if (k1 != k)   
		{
			for (i = 0; i < ndimen; i++)  
			{
				tmp = a_matrix[i][k1];
				a_matrix[i][k1] = a_matrix[i][k];
				a_matrix[i][k] = tmp;
			}  
		}
		if (k2 != k)   
		{
			for(j = 0; j < ndimen; j++)  
			{
				tmp = a_matrix[k2][j];
				a_matrix[k2][j] = a_matrix[k][j];
				a_matrix[k][j] = tmp;
			}
		}
	}
	return (0);
}


void chol(double **a_matrix, const double **b_matrix, int ndimen)
////////////////////////////////////////////////////////////////////////////
//	输入参数:
//		b_matrix:  对称正定方阵    ndimen: 矩阵维数
//	返回值:
//		a_matrix: 下三角矩阵
////////////////////////////////////////////////////////////////////////////
{
	int i, j, r;
	double m = 0;   
	static double **c_matrix;
	static int flag = 0;

	if (flag == 0)
	{
		flag = 1;
		c_matrix = (double **)malloc(ndimen * sizeof(double *));

		for (i = 0; i < ndimen; i++)
			c_matrix[i] = (double *)malloc(ndimen * sizeof(double));
	}

	for (i = 0; i < ndimen; i++)
	{
		for (j = 0; j < ndimen; j++) 
			c_matrix[i][j] = 0;
	}

	c_matrix[0][0] = sqrt(b_matrix[0][0]);

	for (i = 1; i < ndimen; i++)
	{
		if (c_matrix[0][0] != 0) 
			c_matrix[i][0] = b_matrix[i][0] / c_matrix[0][0];
	}

	for (i = 1; i < ndimen; i++)
	{
		for (r = 0; r < i; r++)		m = m + c_matrix[i][r] * c_matrix[i][r];

		c_matrix[i][i] = sqrt(b_matrix[i][i] - m);
		m = 0.0;

		for (j = i + 1; j < ndimen; j++)
		{
			for (r = 0; r < i; r++)		m = m + c_matrix[i][r] * c_matrix[j][r];
			c_matrix[j][i] = (b_matrix[i][j] - m) / c_matrix[i][i];
			m = 0;
		}
	}

	for (i = 0; i < ndimen; i++)
	{
		for (j = 0; j < ndimen; j++)	
			a_matrix[i][j] = c_matrix[i][j];
	}
}
*/


//
//#include <time.h>
//int main()
//{
//int i;
//
//my_srandom((unsigned)(time(NULL)));
//for(i=0;i<100;i++)
//{
//if(i % 10 == 0)
//printf("\n");
//printf("%d\t",my_random()%99+1);
//}
//system("pause");
//return 0;
//}




#define _ULONG_MAX  ((unsigned long)(~0L)) /* 0xFFFFFFFF*/
#define _LONG_MAX   ((long)(_ULONG_MAX >> 1))/* 0x7FFFFFFF*/

//#define NULL (void *) 0

static long int RandomTable[32] ={ 3,
0x9a319039, 0x32d9c024, 0x9b663182, 0x5da1f342,
0xde3b81e0, 0xdf0a6fb5, 0xf103bc02, 0x48f340fb,
0x7449e56b, 0xbeb1dbb0, 0xab5c5918, 0x946554fd,
0x8c2e680f, 0xeb3d799f, 0xb11ee0b7, 0x2d436b86,
0xda672e2a, 0x1588ca88, 0xe369735d, 0x904f35f7,
0xd7158fd6, 0x6fa6f051, 0x616e6b96, 0xac94efdc,
0x36413f93, 0xc622c298, 0xf5a42ab8, 0x8a88d77b,
0xf5ad9d0e, 0x8999220b, 0x27fb47b9
};

static long int *fptr = &RandomTable[4];
static long int *rptr = &RandomTable[1];
static long int *state = &RandomTable[1];
static long int *end_ptr = &RandomTable[sizeof(RandomTable) / sizeof(RandomTable[0])];

long int my_random ()
{
#define __LONG_MAX 2147483647L
	long int i;
	*fptr += *rptr;

	i = (*fptr >> 1) & __LONG_MAX;
	fptr++;
	if (fptr >= end_ptr)
	{
		fptr = state;
		rptr++;
	}
	else
	{
		rptr++;
		if (rptr >= end_ptr)
			rptr = state;
	}
	return i;
}
void my_srandom (unsigned int seed)
{
	register long int i;
	state[0] = seed;
	for (i = 1; i < 31; ++i)
		state[i] = (1103515145 * state[i - 1]) + 12345;
	fptr = &state[3];
	rptr = &state[0];
	for (i = 0; i < 10 * 31; ++i)
		my_random();
}

//==============================================================================

//=================================================================================


/************************************************************************/
/*      内存池                                                  */
/************************************************************************/
#define MEM_USED 0//已经使用
#define MEM_UN_USED 1//没有使用

#define MEM_IS_FOUND 0		//找到
#define MEM_NOT_FOUNT -1	//找不到匹配的尺寸
#define MEM_NEED_MORE -2	//需要更大的size

struct MemNode{
	char* p;
	int len;
	char uesd;
};

struct FNode{
	//需要的尺寸
	int needSize;

	//返回结果
	struct MemNode* out;
};

//struct MemCutNode{
//	int len;
//};

struct MemHandle{
	struct LStackNode* list;	
	int maxSize;
	int pos;
	void* buffer;
	char pool[G_BUFFER_256_SIZE];

	int bufferList[G_BUFFER_64_SIZE];
	int bufferCount;

	/*
	内存池检测开关状态(TRUE,FALSE)
	*/
	int stat;
};

//#define struct MemHandle struct MemHandle
#define N struct MemNode

struct MemHandle* _memInstance;

//内存池句柄
static struct MemHandle* GetInstance(){
	if(_memInstance==NULL)
	{
		printf("内存池未初始化!!!\n");
		assert(0);
	}
	return _memInstance;
}

static struct MemNode* 
ex_addNode(struct MemHandle * p, int len){
	struct MemNode * m = NULL;
	if(p->pos+len > p->maxSize){
		printf("大于最大内存尺寸\n");
		return NULL;
	}
	m = (struct MemNode * )MALLOC(sizeof(struct MemNode));
	m->len = len;
	m->uesd = MEM_UN_USED;
	m->p = (char*)p->buffer + p->pos + len;

	//printf("pos:%d,len:%d字节,内存块地址 0x%x\n",p->pos,len,m->p);

	LStack_push((void*)p->list,m);
	p->pos+=len;   
	return m;
}

static int findNodeCallBack(int data,int parm){
	struct MemNode* n = (struct MemNode*)data;
	struct FNode* _pNode = (struct FNode*)parm;
	if(n->uesd == MEM_UN_USED && n->len == _pNode->needSize){
		_pNode->out = n;
	}
	return 1;
}

/*
*
*	搜索可以使用的内存节点
*
*/
static struct MemNode* findNode(struct MemHandle* p,int len){
 	struct FNode _node;
	memset(&_node,0,sizeof(struct FNode));
	
	//需要的字节长度
	_node.needSize = len;

	LStack_ergodic(p->list,findNodeCallBack,(int)&_node);

	//搜索到的字节结点
	return _node.out;
}

static int memSizeCallBack(int data,int parm){
	int *p = (int*)parm;
	struct MemNode* _node = (struct MemNode*)data;
	if(_node->uesd == MEM_USED){
		*p +=_node->len;
	}
	return 1;
}

//int tl_memGetStat()
//{
//	return GetInstance()->stat;
//}
/*
int tl_memByteSize(){
	if(tl_memGetStat()){
		int size = 0;
		LStack_ergodic(GetInstance()->list,memSizeCallBack,(int)&size);

		//printf("size\t%d\n",size);
		return size;
	}else{
		log_color(0xff0000,"未使用内存池模式,取出来的数据有问题 请设置GetInstance()->stat\n");
	}
	return -1;
}
*/


int Split(char* p){
	int* parms = (int*)p;
	int i;
	struct MemHandle * instance=(struct MemHandle *)parms[0];
	char* str = (char*)parms[2];
	int buffer,bufferSize;
	sscanf_s(str,"%d,%d",&buffer,&bufferSize);

	for(i = 0;i < bufferSize;i++){
		ex_addNode(instance,buffer);//生成bufferSize个相同尺寸的内存块节点
	}
	return 1;
}

static int 
f_MemSplitAdd(char* p){
	int* parms = (int*)p;
	struct MemHandle * _instance=(struct MemHandle *)parms[0];
	char* str = (char*)parms[2];
	int buffer,bufferSize;
	sscanf_s(str,"%d,%d",&buffer,&bufferSize);

	{
		int _size = bufferSize * buffer;
		_instance->maxSize+=_size;

#ifdef DEBUG
		printf("开辟内存块 %.3f mb,内存节点大小:%.3fkb,内存节点个数:%d\n",(float)_size/1024/1024,(float)buffer / 1024.0f,bufferSize);
#endif

	} 

	_instance->bufferList[_instance->bufferCount] = buffer;
	_instance->bufferCount++;
	//printf("%d ",buffer);
	return 1;
}
/*
获取需要的缓存大小
*/
static void 
f_GetNeedSize(struct MemHandle * p,const char* pool)
{
	int parms[3];
	p->bufferCount = 0;
	p->maxSize = 0;
	parms[0] = (int)p;
	parms[1] = 0;
	//获取需要的缓存大小
	f_split_s((const char*)pool,G_SIGN_AITA,f_MemSplitAdd,parms,2);
}
//
//void tl_memInit(const char* pool){
//	int i = 0;
//	int parms[3];
//
//	struct MemHandle * p = (struct MemHandle *)MALLOC(sizeof(struct MemHandle));
//	_memInstance = p;
//	
//	printf("内存池配置:(%s)\n",pool);
//
//	p->maxSize = 0;
//
//	f_GetNeedSize(p,pool);
//	p->pos = 0;
//	p->list = LStack_create();
//	p->buffer = (void*)MALLOC(p->maxSize);
//
//	parms[0] = (int)p;
//	parms[1] = 0;
//	f_split_s((const char*)pool,G_SIGN_AITA,Split,parms,2);
//	tl_bubbleSortInt(p->bufferList,p->bufferCount);
//
//	printf("内存池总大小%f mb,内存池已分配大小:%f mb\n",(float)(p->maxSize)/1024/1024,(float)p->pos/1024/1024);
//}

/*
返回-1,表示未找到可使用的尺寸

return		-2	需要更大的size
return		-1	找不到匹配的尺寸

*/
static int getNextSize(int size,int* list,int len,int* outSize){
	int i;
	int _stat=MEM_NOT_FOUNT;
	//tl_bubbleSortInt(list,len);
	if(len > 0 && size < list[0]){
		_stat = MEM_IS_FOUND;
		*outSize = list[0];
	}
	else if(len > 0 && size > list[len-1]){
		//需要一个更大的内存块
		_stat = MEM_NEED_MORE;
	}else{
		for(i = 0;i<len-1;i++){
			int _l = list[i];
			int _r = list[i+1];
			if(size >= _l && size < _r ){
				_stat = MEM_IS_FOUND;
				*outSize = _r;
				break;;
			}
		}
	}
	return _stat;
}
/*
int CallBackFindDel(char* parms){
	int* point = (int*)parms;
	N* n = (N*)point[0];
	char* target = (char*)point[1];
	if(n->p ==target && n->uesd == MEM_USED){
		point[3] = TRUE;
		memset(n->p,0,n->len);//可以注释,性能较好
		n->uesd = MEM_UN_USED;
		point[2] = (int)n;
	}
	return 0;
}
*/
struct MemDelNode{
	void* target;//	target
	struct MemNode* result;//储存结果
	int stat;//	resulg is OK	是否删除成功 TL_TRUE:成功 TL_FALSE:失败
};

/*
	删除内存结点
*/
static int freeDelNode(int data,int parm){
	struct MemNode* n = (struct MemNode*)data;
	struct MemDelNode* delNode = (struct MemDelNode*)parm;
	char* target =	(char*)delNode->target;
	if(n->p ==target && n->uesd == MEM_USED){
		delNode->stat = TRUE;
		memset(n->p,0,n->len);//可以注释,性能较好
		n->uesd = MEM_UN_USED;
		delNode->result = n;
	}
	return 1;
}

//删除内存池
void tl_memdel(){
	struct MemHandle* t = GetInstance();
	FREE(t->buffer);
	t->buffer = NULL;
	FREE(t);
	t = NULL;
}

//void tl_memState(int _stat){
//	struct MemHandle* t = GetInstance();
//	t->stat=_stat;
//}
////=================================================================================
////ramp链表
//struct LStackNode* rampList = NULL;
//
//void f_rampMove(struct BaseLoopVO* p);
//
///*
//*	获取一个节点
//*/
//struct BaseLoopVO* f_rampget();
///*
//	v1:目标向量
//	ms:需要消耗的毫秒数
//*/
//void f_ramp_vec(struct BaseLoopVO* move,float* px,float* py, float* pz,struct Vec3* v1,float ms,int fps){
//	//int fps = tl_get_fps();
//
//	//out
//	struct Vec3 o;
//
//	struct Vec3 v0;
//
//	int count;
//
//	vec3Set(&v0,*px,*py,*pz);
//
//	
//	vec3Sub(v1,&v0,&o);
//
//	//	回调的次数,float n = 1000/fps;//1帧 需要n毫秒
//	
//	count = (int)(ms/(1000.0f/(float)fps));
//
//	move->tx = v1->x;			move->ty = v1->y;			move->tz = v1->z;
//	move->px = px;				move->py = py;				move->pz = pz;
//	move->fps = fps;
//	if(count<=0){
//		//printf("=====================>直接结束 ms:%.3f,fps:%d,count:%d\n",ms,fps,count);
//		//直接结束,直接赋值
//		f_rampMove(move);
//	}else{
//		//有回调次数
//		//struct Vec3 ev;
//		vec3Div(&o,(float)count);
//
//		move->start=get_longTime();
//		move->count = count;
//
//		move->o.x = o.x;		move->o.y = o.y;		move->o.z = o.z;
//		move->usems = (long)ms;
//		move->callBack = f_rampMove;
//	}
//}
///*
//	实现步骤思路:
//	1.加一个回调到渲染主循环中,然后获取每一帧间隔的毫秒数every_ms
//	2.用callbackCount =	ms / every_ms获取到需要回调的次数
//	3.计算从[向量v0]移动到[向量v1]每次偏移的向量数
//	4.如果到达目标向量,则停止回调
//*/
//void f_rampMove(struct BaseLoopVO* p){
//	struct Vec3 v0,v1;
//	p->count--;
//	
//	vec3Set(&v0,*p->px ,*p->py ,*p->pz );
//	vec3Add(&v0,&p->o,&v1);
//
//	*p->px = v1.x;	*p->py = v1.y; *p->pz = v1.z;
//
//	if(p->count<=0){
//		p->callBack = NULL;
//		*p->px = p->tx;		*p->py = p->ty;	*p->pz = p->tz;
//		//printf(" count: %d   消耗时间%ld毫秒, 开始移动 %.3f,%.3f,%.3f\n",p->count,			get_longTime() - tmpms,v1.x ,v1.y ,v1.z);		
//		//printf("end\n");
//		return;
//	}else{
//		struct Vec3 v1;
//		v1.x = p->tx;
//		v1.y = p->ty;
//		v1.z = p->tz;
//
//		f_ramp_vec(p,
//			p->px,p->py,p->pz,
//			&v1, (float)(p->usems- (get_longTime()-p->start)),p->fps);
//	}
//}
//
//void ramp_callBack(){
//	struct LStackNode* s = (struct LStackNode* )rampList;
//	void* top,*p;
//	int isFind = 0;
//	if(!s){
//		return;
//	}
//	top = s;
//	p=top;
//	while((int)LStack_next(p)){
//		int data;
//		struct BaseLoopVO* b;
//
//		p=(void*)LStack_next(p);
//		data = LStack_data(p);
//
//		b = (struct BaseLoopVO*)data;
//
//		//回调函数引用==NULL的时候 说明该节点未使用
//		if(b->callBack!=NULL){
//			b->callBack(b);
//		}
//	}
//}
//struct BaseLoopVO* f_rampget(){
//
//	struct LStackNode* s;
//	void* top,*p;
//	int isFind = 0;
//	struct BaseLoopVO* node;
//
//	if(!rampList){
//		rampList =LStack_create();
//	}
//	s = (struct LStackNode* )rampList;
//	
//	top = s;
//	p=top;
//	while((int)LStack_next(p)){
//		int data;
//		struct BaseLoopVO* b;
//
//		p=(void*)LStack_next(p);
//		data = LStack_data(p);
//		
//		b = (struct BaseLoopVO*)data;
//		
//		//回调函数引用==NULL的时候 说明该节点未使用
//		if(!b->callBack){
//			memset(b,0,sizeof(struct BaseLoopVO));
//			return b;
//		}
//	}
//	node = (struct BaseLoopVO*)tl_malloc(sizeof(struct BaseLoopVO));
//	memset(node,0,sizeof(struct BaseLoopVO));
//	LStack_push((void*)s,(int)node);
//
//	printf("创建节点[struct BaseLoopVO*],ramp_list.length:%d\n",LStack_length(s));
//
//	return node;
//}
//
//struct BaseLoopVO* 
//ramp_vec_trace(float* px,float* py,float* pz,float tx,float ty,float tz,float ms,int fps){
//	struct Vec3 v1;
//	struct BaseLoopVO* n = f_rampget();
//	vec3Set(&v1,tx,ty,tz);
//	f_ramp_vec(n,px,py,pz,&v1,ms,fps);
//	return n;
//}
//
//static void 
//f_ramp_delay_callBack(struct BaseLoopVO* p){
//	//long n = ;
//	//printf("已经使用了%ld ms\n",get_longTime());
//	if(get_longTime() - p->start>=p->usems){
//		p->delayCallBack(p->delayParms);
//		p->callBack=NULL;
//	}
//}
//
//struct BaseLoopVO* ramp_delay(void (*callBack)(void*),void* parms,float ms){
//	struct BaseLoopVO* n = f_rampget();
//	n->start = get_longTime();
//	n->usems = (long)ms;
//	n->delayCallBack = callBack;
//	n->delayParms = parms;
//	n->callBack=f_ramp_delay_callBack;
//	return n;
//}
//
//int ramp_isPlaying(struct BaseLoopVO* node){
//	if(!node){
//		return 0;
//	}
//	return node->callBack==NULL ? 0 : 1;
//}
//
//void ramp_stopPlaying(struct BaseLoopVO* node){
//	if(node && ramp_isPlaying(node))
//		node->callBack=NULL;
//}






//一维数组存储矩阵乘法  d = a x b
void multi2(Matrix44f out,Matrix44f a, Matrix44f b){
	//int i,j,k;
	////clearMat(d);
	//for (i=0;i<4;i++)            //i代表a矩阵的行数  
	//{  
	//	for (j=0;j<4;j++)        //j代表b矩阵的列数  
	//	{  
	//		for (k=0;k<4;k++)    //k代表a矩阵的列数和b数组的行数  
	//		{  
	//			//一个矩阵用一维数组存储时(假设该矩阵为m*n的)，则  
	//			//a[i][j]在这个一维数组中的位置为array[n*i+j]
	//			/*if(4*i+j==15){
	//				float v = a[4*i+k]*b[4*k+j];
	//			}*/

	//			out[4*i+j]+=a[4*i+k]*b[4*k+j];  
	//			//printf("%d,",4*i+j,a[4*i+k]*b[4*k+j]);
	//		} 
	//	}  
	//}  


		int i,j,k;
	//clearMat(d);
	for (i=0;i<4;i++)            //i代表a矩阵的行数  
	{  
		for (j=0;j<4;j++)        //j代表b矩阵的列数  
		{  
			for (k=0;k<4;k++)    //k代表a矩阵的列数和b数组的行数  
			{  
				//一个矩阵用一维数组存储时(假设该矩阵为m*n的)，则  
				//a[i][j]在这个一维数组中的位置为array[n*i+j]
				int index = 4*i + j;
				int row = 4*i+k;
				int col = 4*k+j;
				float value = a[row]*b[col];
				out[index]+=value;
				//if(index==15)
				//{
				//	printf("[%d][%d] * [%d][%d]= %f\n",i+1,k+1,k+1,j+1,value);
				//}
			} 
		}  
	}
}
//====================================================================================
//#include "matrix4.hpp"

void setv(int* v,int flag)
{
	*v = (*v) | flag;
}
void resetv(int* v,int t)
{
	*v = (~t)&(*v); 
}
int getv(int* v,int t)
{
	return (*v & t)>>tl_bintree(t);
}

