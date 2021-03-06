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
	if(p == 0){
		printf("===========================MALLOC内存申请失败,需要%d字节!\n",size);
		assert(0);
	}
	return p;
}
void FREE(void*p){
	if(p!=0){
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
	if(top==0){
		assert(0);//printf("LStack_init error");
		return 0;
	}
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
		//if(p!=0){
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
		//}
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

struct LStackNode*
LStatk_getByIndex(struct LStackNode* top,int index){

	struct LStackNode *p;
	int count=0;
	int listCount;
	p=top;

	listCount = LStack_length(top);

	if(index-1 > listCount){
		return 0;
	}
	while(p!=0&&p->next!=0){
		if(p!=0){
			p=p->next;
			if(count == (listCount-index -1)){
				return p;
			}
			count++;
		}
	}
	return 0;
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
	if(e){
		*e=p->data;
	}
	//释放这个节点的引用
	FREE(p);

	return 1;
}
int LStack_length(struct LStackNode* top){
	struct LStackNode *p;
	int count=0;
	p=top;
	while(p!=0&&p->next!=0){
		//if(p!=0){
			p=p->next;
			count++;
		//}
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
void* 
LStack_findNodeByData(struct LStackNode* list,int _data){
	struct LStackNode* s = (struct LStackNode* )list;
	void* top,*p;
	top = s;
	p=top;
	while((int)LStack_next(p)){
		int data;
		p=(void*)LStack_next(p);
		data = LStack_data(p);
		if(data == _data){
			return p;
		}
	}
	return 0;
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
//static int typeSplit(char* point)
//{
//	int* parms = (int*)point;
//	struct SplitObj* p = (struct SplitObj*)parms[0];
//	char* str =  (char*)parms[1];
//	if(strcmp((const char*)p->cur,(const char*)str) == 0){
//		p->type = p->index;
//	}
//	p->index++;
//	return 1;
//}
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


///*
//	return	负数	逆时针
//			正数	顺时针
//*/
//static float getAngleState(struct Vec3* a,struct Vec3* b){
//	return (a->x*b->x)-(a->y*b->y)-(a->z*b->z);
//}
//
//static float tmp;
//static int plusMinusVar=0;

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
//static int checkPlusMinus(float x1,float y1,float x2,float y2){
//	return ( x1*y2 - x2*y1) > 0 ? 1 : -1;
//}
//
//float vecToAngle(struct Vec3* a,struct Vec3* b){
//	float t,kk,sum,dis;
//	
//	//vec3Normalize(a);
//	//vec3Normalize(b);
//
//	//顺时针为1,逆时针-1
//	int _st;
//	
//	_st = checkPlusMinus(0.0f,1.0f,b->x,b->y);
//	if(_st!=plusMinusVar){
//		plusMinusVar = _st;
//		printf("%d\n",_st);
//	}
//
//	t=	a->x*a->x+
//		a->y*a->y+
//		a->z*a->z;
//
//	kk=	b->x*b->x+
//		b->y*b->y+
//		b->z*b->z;
//	
//	dis=(float)sqrt(t+kk);//两向量的距离
//
//	t=a->x*b->x+a->y*b->y+a->z*b->z; 
//	
//	sum=t/dis; 
//	
//	sum=(float)acos(sum);
//	
//	t=sum/PI;
//	
//	if(tmp!=t){
//	//	printf("%.2f\n",t*180.0f);
//	}
//
//	tmp = t;
//
//	//t:顺时针,-t逆时针
//
//	t = t * 180.0f;
//
//	if(_st>0){
//		t = 360.0f-t;
//	}
//
//	return t;
//}

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


static int 
tl_bintree(int t){
	int x = 0,a=t;
	while(a!=1){
		a=a/2;	x++;
		if(a==0){
			break;
		}
	}
	return x;
}

//一维数组存储矩阵乘法  d = a x b
void multi2(Matrix44f out,Matrix44f a, Matrix44f b){
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
static int gid;
int newid(){
	gid++;
	return gid;
}



//**********************************************************
//链栈排序 


	//f_sort(l.cnt,l,func,newlist);


static void f_sort(int cnt,struct LStackNode* l,
				   int (*sortHander)(void* pre,void* next),
					struct LStackNode* newlist)
{
	int i;
	int len;
	for(i = 0;i < cnt;i++){
		//struct LStackNode* pre =
		struct LStackNode* pre = LStatk_getByIndex(l,i);
		if(i < cnt - 1){
			struct LStackNode* next = LStatk_getByIndex(l,i+1);
			if(next && sortHander((void*)pre->data,(void*)next->data)==1){
				int t = next->data;
				next->data = pre->data;
				pre->data = t;
			}
		}
	}

	len = LStack_length(l);
	if(len > 0){
		int data;
		LStack_pop(l,&data);
		LStack_push((void*)newlist,(void*)data);
	}
	i = 0;
	cnt = len;
	for(i = 0;i < cnt-1;i++){
		f_sort(cnt,l,sortHander,newlist);
	}
}

void LStack_sort_func(struct LStackNode* s,int (*sortHander)(void* pre,void*next)){
	struct LStackNode* newlist = LStack_create();
	int len = LStack_length(s);
	int i;
	f_sort(len,s,sortHander,newlist);
	LStack_clear(s);

	//清理链表
	for(i = 0;i < len;i++){
		int e;
		LStack_pop(newlist,&e);
		printf("%d = %d\n",i,e);
		LStack_push((void*)s,(void*)e);
	}
	LStack_delete(newlist);
}