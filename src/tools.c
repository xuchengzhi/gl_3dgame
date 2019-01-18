//#pragma warning(noce:4996) //仅显示一个

//#pragma warning(disable:4996) 
//#pragma warning(disable:4244) 

#include <stdio.h>     
#include <stdlib.h>     
//#include <time.h>
//#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <stdarg.h>

#include "gettime.h"

#include "tools.h"
#include "tmat.h"
#include "tl_malloc.h"



//相对路径
static char* localPath[G_BUFFER_256_SIZE];

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
		strcpy(po, pi);
	}
	else
	{
		// 没有找到则原样复制.
		strcpy(po, pi);
	}
}

int tl_isFileExist(const char* file){
	FILE *stream = fopen( file, "r" );
	if(stream!=NULL){
		fclose( stream );
		stream = NULL;
		return 1;
	}
	return 0;
}

void tl_setLocalPath(const char* path){
	char* p = localPath;
	if(strlen(path)>G_BUFFER_256_SIZE){
		printf("设置的路径长度大于缓冲区长度!\n");
		assert(0);
	}	
	if(!path){
		memset(p,0,G_BUFFER_256_SIZE);
	}else{
		memset(p,0,G_BUFFER_256_SIZE);
		memcpy(p,path,strlen(path));
	}
}

int tl_convert_path(char* in,char* out,int outBufferSize)
{
	char* p = localPath;
	if((int)strlen((const char*)p)>0){
		memset(out,0,outBufferSize);
		sprintf_s(out,outBufferSize,"%s%s",p,in);
		return 1;
	}
	return 0;
	//printf("%s\n%s\n",in,out);
}

char* tl_loadfile(const char* xfile,int* outLengthPtr){
	unsigned int len;
	char* buffer = NULL;
	FILE *stream;
	const char* file=NULL;

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
#ifdef _DEBUG_MODE_
	long time = get_longTime();
#endif

	char outStr[G_BUFFER_256_SIZE];

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
	stream = fopen( file, "rb+" );	//rb+ 读写打开一个二进制文件，允许读写数据，文件必须存在。
	if( stream != NULL ){
		fseek(stream,0,SEEK_END);
		len = ftell(stream);
		buffer = (char*)tl_malloc((unsigned int)(len+1));
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

	#ifdef _DEBUG_MODE_
	{

		log_color(0xff00ff,"tl_loadfile 加载文件 %s (%d字节) 耗时 %ld 毫秒\n",xfile,len,get_longTime()-time);
	}
	#endif

	return (char*)buffer;
}

void tl_writeAppandFile(const char* filePath,char* buffer){
	FILE * pFile;
	pFile = fopen ( filePath , "ab+" );
	if(pFile == NULL){
		printf("file %s is not exist!\n",filePath);
		return;
	}
	fwrite (buffer , strlen(buffer), 1, pFile );
	fclose (pFile);
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

void tl_set_vec(Vec3_t vec,float x,float y,float z)
{
	vec[X] = x;
	vec[Y] = y;
	vec[Z] = z;
}
void tl_add_vec(Vec3_t a,Vec3_t b,Vec3_t vec)
{
	vec[X] = a[X]+b[X];
	vec[Y] = a[Y]+b[Y];
	vec[Z] = a[Z]+b[Z];
}

void tl_offset_vec(Vec3_t pos,float x,float y,float z)
{
	pos[X] += x;
	pos[Y] += y;
	pos[Z] += z;
}

void tl_strsub(const char * dest,char* str,int start,int end){
	memcpy(str,dest+start,end-start+1);
}
//----------------------------------------------------------------
#define _SIGN_LEN_ 1//标示的长度
static void f_splitstr(char* str,const char* dest,int start,int i,CallBackFun pCallBack,int* parms,int parmsLen)
{
	tl_strsub(dest,str,start,i-_SIGN_LEN_);
	parms[parmsLen] = (int)str;
	pCallBack((char*)parms);
}

void tl_split_s(const char* dest,const char sign,CallBackFun pCallBack,int* parms,int parmsLen)
{
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
static void f_splitByStrNode(char* str,const char* dest,int signStrLength,int start,int i,void (*pCallBack)(int*,char*),int* parms)
{
	tl_strsub(dest,str,start,i-signStrLength);
	pCallBack(parms,str);
}
void tl_splitByStr(const char* dest,const char sign,void (*pCallBack)(int*,char*),int* parms)
{
	int i,start=0;
	char _ch;
	int len = (int)strlen(dest);
	int length = len+1;
	
	int signLen = _SIGN_LEN_;

	char* str = (char*)MALLOC(length);
	for(i = 0;i < len;i++){
		_ch = dest[i];
		if(_ch==sign){
			memset(str,0,length);
			f_splitByStrNode(str,dest,signLen,start,i,pCallBack,parms);
			start = i+signLen;
		}
	}
	memset(str,0,len+1);
	f_splitByStrNode(str,dest,signLen,start,i,pCallBack,parms);
	FREE(str);
}


static void fSplitString(char* str,int strLength,const char* dest,int* pStart,int i,
				  int (*_callBack)(struct StrCutSplit*),struct StrCutSplit* pCut){
	memset(str,0,strLength);
	tl_strsub(dest,str,*pStart,i-_SIGN_LEN_);
	pCut->str = str;
	_callBack(pCut);
	*pStart = i+_SIGN_LEN_;
}

void tl_split_cut(const char* dest,const char sign,
				  int (*pCallBack)(struct StrCutSplit*),int* parms){
#define _TEMP_SIZE_ G_BUFFER_512_SIZE
	if(strlen(dest)+1>=_TEMP_SIZE_){
		printf("检测dest长度超过函数处理长度 当前dest的长度为%d\n",(int)strlen(dest));
		assert(0);
	}else{
		int i,start=0;
		char _ch;
		int len = (int)strlen(dest);
		int strLength = len + 1;
		
		//将堆内存申请修改为栈内存申请,使速度更快
		char str[_TEMP_SIZE_];

		struct StrCutSplit _sc;
		//struct StrCutSplit* _ps = &_sc;
		memset(&_sc,0,sizeof(struct StrCutSplit));

		//未参数引用赋值
		_sc.parms = parms;
		for(i = 0;i < len;i++){
			_ch = dest[i];
			if(_ch==sign){
				fSplitString(str,_TEMP_SIZE_,dest,&start,i,pCallBack,&_sc);
			}
		}
		fSplitString(str,_TEMP_SIZE_,dest,&start,i,pCallBack,&_sc);
	}
#undef _TEMP_SIZE_
}

#undef _SIGN_LEN_
//----------------------------------------------------------------
void tl_strSplitLoop(struct StringSplit* sp)
{
	const char* dest = sp->dest;
	const char* sign = sp->sign;
	int *pos = &sp->pos;
	char* pOutstr = sp->pOutstr;

	int _l = (int)strlen(sign);
	int len,i,_sta = 0;
	char* __dest;
	int outStrLength = G_BUFFER_MAX_SIZE;	//单个输出的字符长度
	int cl = G_BUFFER_16_SIZE;				//标识符的长度
	int cut = 0;
	//============================================
	__dest = (char*)dest + *pos; 
	len = (int)strlen(__dest);

	if((int)strlen(__dest) >= _l)
	{
		char tmp[G_BUFFER_16_SIZE];
		memset(tmp,0,G_BUFFER_16_SIZE);
		tl_strsub(__dest,tmp,len-_l,len-1);
		if(strcmp(tmp,sign) == 0)
		{
			//去掉最后一个字符串
			len -= _l;
			cut = 1;
		}
	}

	if(strlen(__dest) < strlen(sign) || (int)strlen(sign) > cl)
	{
		printf("cmp is little than %d __dest:(%s) \n",cl,__dest);
		sp->isEnd = -1;
		return;
	}

	for(i = 0;i < len;i++)
	{
		char cmp[G_BUFFER_16_SIZE];
		if(len-i<_l)
		{
			break;
		}
		memset(cmp,0,cl);
		tl_strsub((const char*)__dest,cmp,i,i+_l-1);
		if(strcmp((const char*)cmp,sign) == 0)
		{
			if(i-_l>=outStrLength)
			{
				printf("error, one line is too long!\n");
				sp->isEnd = -1;
				return;
			}
			memset(pOutstr,0,outStrLength);
			tl_strsub(__dest,pOutstr,_sta,i-_sta-1);
			_sta = i+_l;
			*pos += _sta;
			sp->isEnd = 0;
			return;
		}
	}
	memset(pOutstr,0,outStrLength);
	if(cut == 0)
	{
		tl_strsub(__dest,pOutstr,_sta,_sta+len);
	}
	else
	{
		tl_strsub(__dest,pOutstr,_sta,_sta+len-_l);
	}
	sp->isEnd = -1;
}

void tl_strSplitInit(struct StringSplit* p,const char* dest,const char* sign,char* pOutstr)
{
	p->dest = (char*)dest;
	p->sign = (char*)sign;
	p->pos = 0;
	p->isEnd = 0;
	p->pOutstr = pOutstr;
}

int tl_charIndex(char* str,char c){
	char* ptr = strchr(str,c);
	if(ptr!=NULL)
		return (int)(ptr-str);

	return -1;
}

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
static	int f_getFileNameSplice(struct StrCutSplit* p){
#define _BufferSize_ G_BUFFER_128_SIZE
	//struct StrCutSplit* p = (struct StrCutSplit*)point;
	char* str = p->str;
	char* outStr = (char*)p->parms;
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
	tl_split_cut(str,'\\',f_getFileNameSplice,(int*)fileName);
}

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
void tl_delchar(char *str,char ch){
	//char ts[G_BUFFER_256_SIZE];
	int len,i,j=0;
	char* ts = NULL;
	len = (int)strlen(str);
	ts = (char*)tl_malloc(len+1);
	memset(ts,0,len+1);
	for(i = 0;i< len;i++){
		if(str[i]!=ch){
			ts[j] = str[i];
			j++;
		}
	}
	memset(str,0,strlen(str));
	memcpy(str,ts,strlen(ts));
	tl_free(ts);
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
		sscanf(_str,"%s",out);
		//tl_free(_str);
	}
}
void tl_setInt(const char* dest,const char* key,int* out){
	int len = (int)strlen(key);
	if(strncmp(dest,key,len)== 0){
		char _str[G_CHAR_SIZE_];
		memset(_str,0,G_CHAR_SIZE_);
		memcpy(_str,dest+len+1,strlen(dest)-len-1);
		sscanf(_str,"%d",out);
	}
}
void tl_get_files(const char* path,struct LStackNode* stack) {
	/*
	long _handle;
	struct _finddata_t _fileInfo;
	char _path[G_BUFFER_128_SIZE];
	memset(_path,0,G_BUFFER_128_SIZE);
	sprintf(_path,"%s*",path);
	if((_handle=_findfirst(_path,&_fileInfo))==-1L){//D:\cgi		D:\\*.txt		"d:\\cgi\\*"
	printf("there is not exist files\n");
	}else{
	//printf("[%s]\n",_fileInfo.name);
	while(_findnext(_handle,&_fileInfo)==0){
	//printf("%s <br>",_fileInfo.name);
	struct tl_strcut_char* sc = 
	(struct tl_strcut_char*)tl_malloc(sizeof(struct tl_strcut_char));

	memset(sc->str,0,G_BUFFER_128_SIZE);
	memcpy(sc->str,_fileInfo.name,strlen(_fileInfo.name));
	LStack_push(stack,(int)sc);
	}
	_findclose(_handle);
	}
	*/
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
void tl_http_get_query_string(char* result){
	char* data = getenv("QUERY_STRING");
	if(data != NULL){
		memcpy(result,data,strlen(data));
	}else{
		//未找到数据
		printf("tl_http_get_query_string(...) didn`t get data!");
	}
}



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
int tl_strpos(const char *haystack,const char *needle,int ignorecase)  {  
	register unsigned char c, needc;
	unsigned char const *from, *end;
	const char *findreset;
	int i;
	int len = (int)strlen(haystack);
	int needlen = (int)strlen(needle);
	from = (unsigned char *)haystack;
	end = (unsigned char *)haystack + len;
	findreset = needle;

	for ( i = 0; from < end; ++i) {
		c = *from++;
		needc = *needle;
		if (ignorecase) {
			if (c >= 65 && c < 97)
				c += 32;
			if (needc >= 65 && needc < 97)
				needc += 32;
		}
		if(c == needc) {
			++needle;
			if(*needle == '\0') {
				if (len == needlen) 
					return 0;
				else
					return i - needlen+1;
			}
		}  
		else {  
			if(*needle == '\0' && needlen > 0)
				return i - needlen +1;
			needle = findreset;  
		}
	}  
	return  -1;  
}  
int 
tl_strpos_s(const char *haystack,const char *needle){
	//int k=tl_strpos(haystack,needle,1);
	int pos = -1;
	char* _position=strstr(haystack,needle);
	if(_position){
		pos = _position-haystack;
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
				FREE(p);
				return 1;
			}
		}
	}
	return 0;
}
int LStack_isEmpty(struct LStackNode* top){
	if(top->next==0){			/*判断链栈头结点的指针域是否为空*/
		return 1;			/*当链栈为空时，返回1；否则返回0*/
	}
	return 0;
}
int LStack_push(struct LStackNode* lt,int nodePoint){
	struct LStackNode* top = (struct LStackNode*)lt;
	
	//为当前的节点分配一块内存
	struct LStackNode *p=(struct LStackNode*)MALLOC(sizeof(struct LStackNode));
	
	if(!p){
		printf("链栈节点,内存分配失败!\n");
		assert(0);
		return -1;
	}
	p->data = nodePoint;
	
	p->next=top->next;
	p->pre = top;
	top->next=p;
	return 1;
}
int LStatk_getNodeByIndex(struct LStackNode* top,int index,int* out)
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
		LStack_push(s,v);
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
				LStack_push(s,v);
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
		LStack_push(p,pList[i]);
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
		LStack_push(s,(int)st);
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
//====================================================================================================
/*
*	可变长列表实现
*/
void list_create(struct List* _l){
	_l->length = 0;
	_l->list = NULL;
}
void list_add(struct List* _l,float value){
	int i;
	float* newList = (float*)tl_malloc((_l->length+1)*sizeof(float));
	for(i = 0;i <_l->length;i++)
	{
		newList[i] = _l->list[i];
	}
	newList[i] = value;
	_l->length++;
	if(_l->list!=NULL)
	{
		tl_free(_l->list);
	}
	_l->list = newList;
}
void list_dispose(struct List* _l){
	_l->length = 0;
	tl_free(_l->list);
}
//====================================================================================================

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

#define X_min 0
#define X_max 1

#define Y_min 2
#define Y_max 3

#define Z_min 4
#define Z_max 5

#define _LENGTH_ BOX_SIZE
int tl_boxArray(float* in,float* out){

	float triData[_LENGTH_] = 
	{
		in[X_min], in[Y_min], in[Z_max],		in[X_max], in[Y_min], in[Z_max],		in[X_min],  in[Y_max], in[Z_max],

		in[X_max], in[Y_min], in[Z_max],in[X_max], in[Y_max], in[Z_max],in[X_min], in[Y_max], in[Z_max],

		in[X_min], in[Y_max], in[Z_max],in[X_max], in[Y_max], in[Z_max],in[X_min], in[Y_max], in[Z_min],

		in[X_max], in[Y_max], in[Z_max],in[X_max], in[Y_max], in[Z_min],in[X_min], in[Y_max], in[Z_min],

		in[X_min], in[Y_max], in[Z_min],in[X_max], in[Y_max], in[Z_min],in[X_min], in[Y_min], in[Z_min],

		in[X_max], in[Y_max], in[Z_min],in[X_max], in[Y_min], in[Z_min],in[X_min], in[Y_min], in[Z_min],

		in[X_min], in[Y_min], in[Z_min],in[X_max], in[Y_min], in[Z_min],in[X_min], in[Y_min], in[Z_max],

		in[X_max], in[Y_min], in[Z_min],in[X_max], in[Y_min], in[Z_max],in[X_min], in[Y_min], in[Z_max],

		in[X_max], in[Y_min], in[Z_max],in[X_max], in[Y_min], in[Z_min],in[X_max], in[Y_max], in[Z_max],

		in[X_max], in[Y_min], in[Z_min],in[X_max], in[Y_max], in[Z_min],in[X_max], in[Y_max], in[Z_max],

		in[X_min], in[Y_min], in[Z_min],in[X_min], in[Y_min], in[Z_max],in[X_min], in[Y_max], in[Z_min],

		in[X_min], in[Y_min], in[Z_max],in[X_min], in[Y_max], in[Z_max],in[X_min], in[Y_max], in[Z_min]

	};
	memcpy(out,triData,(int)sizeof(float)*_LENGTH_);
	return _LENGTH_;
}
//float* tl_createBoxArray(float* in,int* pLength){
//	float* out = tl_malloc(sizeof(float)*_LENGTH_);
//	float triData[_LENGTH_] = 
//	{
//		in[X_min], in[Y_min], in[Z_max],		in[X_max], in[Y_min], in[Z_max],		in[X_min],  in[Y_max], in[Z_max],
//
//		in[X_max], in[Y_min], in[Z_max],in[X_max], in[Y_max], in[Z_max],in[X_min], in[Y_max], in[Z_max],
//
//		in[X_min], in[Y_max], in[Z_max],in[X_max], in[Y_max], in[Z_max],in[X_min], in[Y_max], in[Z_min],
//
//		in[X_max], in[Y_max], in[Z_max],in[X_max], in[Y_max], in[Z_min],in[X_min], in[Y_max], in[Z_min],
//
//		in[X_min], in[Y_max], in[Z_min],in[X_max], in[Y_max], in[Z_min],in[X_min], in[Y_min], in[Z_min],
//
//		in[X_max], in[Y_max], in[Z_min],in[X_max], in[Y_min], in[Z_min],in[X_min], in[Y_min], in[Z_min],
//
//		in[X_min], in[Y_min], in[Z_min],in[X_max], in[Y_min], in[Z_min],in[X_min], in[Y_min], in[Z_max],
//
//		in[X_max], in[Y_min], in[Z_min],in[X_max], in[Y_min], in[Z_max],in[X_min], in[Y_min], in[Z_max],
//
//		in[X_max], in[Y_min], in[Z_max],in[X_max], in[Y_min], in[Z_min],in[X_max], in[Y_max], in[Z_max],
//
//		in[X_max], in[Y_min], in[Z_min],in[X_max], in[Y_max], in[Z_min],in[X_max], in[Y_max], in[Z_max],
//
//		in[X_min], in[Y_min], in[Z_min],in[X_min], in[Y_min], in[Z_max],in[X_min], in[Y_max], in[Z_min],
//
//		in[X_min], in[Y_min], in[Z_max],in[X_min], in[Y_max], in[Z_max],in[X_min], in[Y_max], in[Z_min]
//
//	};
//	memcpy(out,triData,(int)sizeof(float)*_LENGTH_); 
//	*pLength = _LENGTH_;
//	return out;
//}

#undef X_min
#undef X_max
#undef Y_min
#undef Y_max
#undef Z_min
#undef Z_max
#undef _LENGTH_
static void sortList(float* vertexs,int count,
					 float* pMin,float* pMax,int offsetValue)
{
	int i;
	int _curCount = count*sizeof(float);
	float* xList = (float*)tl_malloc(_curCount);
	//float xList[12000];
	for(i  = 0;i< count;i++){
		xList[i] = vertexs[i*8+5+offsetValue];
	}
	tl_quickSort(xList,count);
	*pMin = xList[0];
	*pMax = xList[count-1];
	tl_free(xList);
}

void tl_minMax(float* vertexs,int count,float* out){
	float minX,maxX,minY,maxY,minZ,maxZ;
	sortList(vertexs,count,&minX,&maxX,0);
	sortList(vertexs,count,&minY,&maxY,1);
	sortList(vertexs,count,&minZ,&maxZ,2);
	out[0] = minX;
	out[1] = maxX;
	out[2] = minY;
	out[3] = maxY;
	out[4] = minZ;
	out[5] = maxZ;
}

float tl_max(float v0,float v1){
	return v0 > v1 ? v0 : v1;
}

//取绝对值最大的做碰撞盒
void tl_absMax(float* vertexs,int count,float* out){
	float minX,maxX,minY,maxY,minZ,maxZ;
	float v0,v1,v2;
	sortList(vertexs,count,&minX,&maxX,0);
	sortList(vertexs,count,&minY,&maxY,1);
	sortList(vertexs,count,&minZ,&maxZ,2);
	if(maxX < 0 || maxY < 0 || maxZ <0 || minX > 0 || minY > 0 || minZ > 0){
		assert(0);
	}
	//abs();
	//max(abs(minX),
	v0 = tl_max((float)abs(minX),maxX);

	v1 = tl_max((float)abs(minY),maxY);

	v2 = tl_max((float)abs(minZ),maxZ);

	out[0] = -v0;
	out[1] = v0;
	out[2] = -v1;
	out[3] = v1;
	out[4] = -v2;
	out[5] = v2;
}
//设置固定长宽高的包围盒
void tl_setAbsBoundBox(float x,float y,float z,float* triData){
	float v[6]={-x/2,x/2,-y/2,y/2,-z/2,z/2};
	tl_boxArray(v,triData);
}

int tl_createAABBbyVert(float* trianglesArray,int vertexCount,float* outData){
	float vMinMax[6]={0,0,0,0,0,0};
	int length;
	tl_minMax(trianglesArray,vertexCount,vMinMax);
	
	length = tl_boxArray(vMinMax,outData);
	/*if(scale!=1){
		int i;
		for( i = 0;i< BOX_SIZE;i++){
			outData[i] = outData[i]*scale;
		}
	}*/
	return length;
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

void tl_hitTriangle(float orig[3],float dir[3],float vert0[3],float vert1[3],float vert2[3],float* out,int* pHit){
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
//=======================================================================================
int gFpsout=-1;
void tl_calculate_fps(){
	static float _fps = 0;        
	static float lastTime = 0.0f; 
	float currentTime = gettime_cur();//(float)clock()*0.001f;
	++_fps;
	if( currentTime - lastTime > 1.0f ){
		lastTime = currentTime;
		gFpsout = (int)_fps;
		_fps = 0;
	}
}
int tl_get_fps(){
	return gFpsout;
}
//四元数数学库=============================================================
/**
* Basic quaternion operations.
*/

void Quat_computeW (Quat4_t q)
{
	float t = 1.0f - (q[X] * q[X]) - (q[Y] * q[Y]) - (q[Z] * q[Z]);

	if (t < 0.0f)
		q[W] = 0.0f;
	else
		q[W] = -(float)sqrt (t);
}

void Quat_normalize (Quat4_t q)
{
	/* compute magnitude of the quaternion */
	float mag = (float)sqrt ((q[X] * q[X]) + (q[Y] * q[Y])
		+ (q[Z] * q[Z]) + (q[W] * q[W]));

	/* check for bogus length, to protect against divide by zero */
	if (mag > 0.0f)
	{
		/* normalize it */
		float oneOverMag = 1.0f / mag;

		q[X] *= oneOverMag;
		q[Y] *= oneOverMag;
		q[Z] *= oneOverMag;
		q[W] *= oneOverMag;
	}
}

void Quat_multQuat (const Quat4_t qa, const Quat4_t qb, Quat4_t out)
{
	out[W] = (qa[W] * qb[W]) - (qa[X] * qb[X]) - (qa[Y] * qb[Y]) - (qa[Z] * qb[Z]);
	out[X] = (qa[X] * qb[W]) + (qa[W] * qb[X]) + (qa[Y] * qb[Z]) - (qa[Z] * qb[Y]);
	out[Y] = (qa[Y] * qb[W]) + (qa[W] * qb[Y]) + (qa[Z] * qb[X]) - (qa[X] * qb[Z]);
	out[Z] = (qa[Z] * qb[W]) + (qa[W] * qb[Z]) + (qa[X] * qb[Y]) - (qa[Y] * qb[X]);
}

void Quat_multVec (const Quat4_t q, const Vec3_t v, Quat4_t out){
	out[W] = - (q[X] * v[X]) - (q[Y] * v[Y]) - (q[Z] * v[Z]);
	out[X] =   (q[W] * v[X]) + (q[Y] * v[Z]) - (q[Z] * v[Y]);
	out[Y] =   (q[W] * v[Y]) + (q[Z] * v[X]) - (q[X] * v[Z]);
	out[Z] =   (q[W] * v[Z]) + (q[X] * v[Y]) - (q[Y] * v[X]);
}

void Quat_rotatePoint (const Quat4_t q, const Vec3_t in, Vec3_t out){
	Quat4_t tmp, inv, final;

	inv[X] = -q[X]; inv[Y] = -q[Y];
	inv[Z] = -q[Z]; inv[W] =  q[W];

	Quat_normalize (inv);

	Quat_multVec (q, in, tmp);
	Quat_multQuat (tmp, inv, final);

	out[X] = final[X];
	out[Y] = final[Y];
	out[Z] = final[Z];
}
/**
* More quaternion operations for skeletal animation.
*/
float Quat_dotProduct (const Quat4_t qa, const Quat4_t qb)
{
	return ((qa[X] * qb[X]) + (qa[Y] * qb[Y]) + (qa[Z] * qb[Z]) + (qa[W] * qb[W]));
}

/*
*	将四元数转化为矩阵
*/
void
Quat_to_matrrix(const Quat4_t q,Matrix44f m){
	m[0] = 2*(q[X]*q[X] + q[W]*q[W]) - 1;
	m[1] = 2*(q[X] * q[Y] + q[Z] * q[W]);
	m[2] = 2*(q[X]*q[Z] - q[Y]*q[W]);
	m[3] = 0;

	m[4] = 2 * (q[X]*q[Y] - q[Z]*q[W]);
	m[5] = 2 * (q[Y]*q[Y] + q[W]*q[W]) - 1;
	m[6] = 2 * (q[Y]*q[Z] + q[X]*q[W]);
	m[7] = 0;

	m[8] = 2 * (q[X]*q[Z] + q[Y]*q[W]);
	m[9] = 2 * (q[Y]*q[Z] - q[X]*q[W]);
	m[10]= 2 * (q[Z]*q[Z] + q[W]*q[W]) - 1;
	m[11]= 0;

	m[12] = 0;
	m[13] = 0;
	m[14] = 0;
	m[15] = 1;
}

/*
*	四元数球面插值
*	t : 0 ~ 1.0f之间
*/
void Quat_slerp (const Quat4_t qa, const Quat4_t qb, float t, Quat4_t out)
{
	float q1w,q1x,q1y,q1z;
	float cosOmega;

	/* Compute interpolation fraction, checking for quaternions
	almost exactly the same */
	float k0, k1;

	/* Check for out-of range parameter and return edge points if so */
	if (t <= 0.0)
	{
		memcpy (out, qa, sizeof(Quat4_t));
		return;
	}

	if (t >= 1.0)
	{
		memcpy (out, qb, sizeof (Quat4_t));
		return;
	}

	/* Compute "cosine of angle between quaternions" using dot product */
	cosOmega = Quat_dotProduct (qa, qb);

	/* If negative dot, use -q1.  Two quaternions q and -q
	represent the same rotation, but may produce
	different slerp.  We chose q or -q to rotate using
	the acute angle. */
	q1w = qb[W];
	q1x = qb[X];
	q1y = qb[Y];
	q1z = qb[Z];

	if (cosOmega < 0.0f)
	{
		q1w = -q1w;
		q1x = -q1x;
		q1y = -q1y;
		q1z = -q1z;
		cosOmega = -cosOmega;
	}

	/* We should have two unit quaternions, so dot should be <= 1.0 */
	assert (cosOmega < 1.1f);



	if (cosOmega > 0.9999f)
	{
		/* Very close - just use linear interpolation,
		which will protect againt a divide by zero */

		k0 = 1.0f - t;
		k1 = t;
	}
	else
	{
		/* Compute the sin of the angle using the
		trig identity sin^2(omega) + cos^2(omega) = 1 */
		float sinOmega = (float)sqrt (1.0f - (cosOmega * cosOmega));

		/* Compute the angle from its sin and cosine */
		float omega = (float)atan2 (sinOmega, cosOmega);

		/* Compute inverse of denominator, so we only have
		to divide once */
		float oneOverSinOmega = 1.0f / sinOmega;

		/* Compute interpolation parameters */
		k0 = (float)sin ((1.0f - t) * omega) * oneOverSinOmega;
		k1 = (float)sin (t * omega) * oneOverSinOmega;
	}

	/* Interpolate and return new quaternion */
	out[W] = (k0 * qa[3]) + (k1 * q1w);
	out[X] = (k0 * qa[0]) + (k1 * q1x);
	out[Y] = (k0 * qa[1]) + (k1 * q1y);
	out[Z] = (k0 * qa[2]) + (k1 * q1z);
}

//=============================================================
/*
解析输出
u,v	normalX,normalY,normalZ posX,posY,posZ
*/
void tl_parseVerts(float* verts,int* pVerts,int* pUVs,int* pNormals,int len){
	int i,k = 0,u = 0,n = 0,ver_span = 3,uv_spane=2,normal_span = 3;
	int all_span = ver_span + normal_span + uv_spane;
	float* vs =			(float*)tl_malloc(len*ver_span);
	float* normals =	(float*)tl_malloc(len*normal_span);
	float* uvs =		(float*)tl_malloc(len*uv_spane);

	memset(vs,0,len*ver_span);
	memset(normals,0,len*normal_span);
	memset(uvs,0,len*uv_spane);

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
	*pVerts = (int)vs;
	*pNormals = (int)normals;
	*pUVs = (int)uvs;
}
//uvs normals vs
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

char tl_str2ch(const char* str)
{
	if(strlen(str)>=1)
	{
		return str[0];
	}
	return (char)NULL;
}

int animsSplit(char* point)
{
	int* parms = (int*)point;
	struct SplitObj* p = (struct SplitObj*)parms[0];
	char* str =  (char*)parms[1];

	if(p->index == p->type){
		memcpy(p->cur,str,strlen(str));
	}
	p->index++;
	return 1;
}
void tl_strType(const char* str,int type,char* out,int wordSize)
{
	struct SplitObj pp;
	struct SplitObj* p = &pp;

	int parms[2];

	p->index = 0;
	p->type  = type;
	p->cur = (char*)tl_malloc(wordSize);
	memset(p->cur,0,wordSize);

	parms[0] = (int)p;
	tl_split_s((const char*)str,',',animsSplit,parms,1);

	memset(out,0,wordSize);
	if(strlen(p->cur)>0){
		memcpy(out,p->cur,strlen(p->cur));
	}else{
		printf("didn`t find type:%d\n",type);
	}
	tl_free(p->cur);
	p->cur = NULL;
}
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
int tl_typeStr(const char* str,const char* target)
{
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
	tl_split_s((const char*)str,',',typeSplit,parms,1);
	tl_free(p->cur);
	return p->type;
}
void tl_printfVers(float* vert,int count){
	int i,m = 0;
	printf("0		1		2		3		4		5		6			7\n");
	printf("u		v		nx		ny		nz		x		y			z\n");
	for( i =0;i<count;i++){

		if(m<7){
			m++;
			if(m == 1){
				printf("\n");
			}
		}else{
			m = 0;
		}
		printf("(%d)%f	",i,vert[i]);
	}
}
void tl_scale_vec(Vec3_t vec,Vec3_t res,float scale){
	res[X] = vec[X] * scale;
	res[Y] = vec[Y] * scale;
	res[Z] = vec[Z] * scale;
}
//void tl_set_vec(Vec3_t res,float x,float y,float z){
//	res[X] = x;
//	res[Y] = y;
//	res[Z] = y;
//}

void vec3Set(struct Vec3* v,float x,float y,float z){
	v->x = x;
	v->y = y;
	v->z = z;
}

void vec3Add(struct Vec3* a,struct Vec3* b,struct Vec3* out){
	out->x = a->x+b->x;
	out->y = a->y+b->y;
	out->z = a->z+b->z;
}
//out = a - b    
void vec3Sub(struct Vec3* a,struct Vec3* b,struct Vec3* out){
	out->x = a->x-b->x;
	out->y = a->y-b->y;
	out->z = a->z-b->z;
}
void vec3Cross(struct Vec3* a,struct Vec3* v,struct Vec3* out){
	out->x = a->y*v->z - a->z*v->y;
	out->y = a->z*v->x - a->x*v->z;
	out->z = a->x*v->y - a->y*v->x;
}
void vec3Mult(struct Vec3* a,float value){
	a->x *= value;
	a->y *= value;
	a->z *= value;
}

float vec3Distance(struct Vec3* a,struct Vec3* b){
	return	(float)sqrt(pow(a->x - b->x,2)+pow(a->y - b->y,2)+pow(a->z - b->z,2));
}
void vec3Div(struct Vec3* o,float v){
	o->x /=v;
	o->y /=v;
	o->z /=v;
}
float vec3Length(struct Vec3* v){
	//平方根x*x + y*y + z*z
	return	(float)sqrt(v->x*v->x + v->y*v->y + v->z*v->z );
}
int 
vec3IsZero(struct Vec3* v){
	if(v->x==0 && v->y==0 && v->z==0){
		return 1;
	}
	return 0;
}
void vec3CalculateNormal(struct Vec3* a,struct Vec3* b,struct Vec3* c,struct Vec3* res)
{
	struct Vec3 out1,out2;
	vec3Sub(a,b,&out1);
	vec3Sub(a,c,&out2);
	vec3Cross(&out1,&out2,res);
}

/*
	射线检测三角形角点
	triangle:	三个顶点,表示一个三角形
*/
int f_testHit(struct Vec3* base,struct Vec3* nearPoint,struct Vec3* farPoint,float* t,float* out){
	//struct HeadInfo* base =(struct HeadInfo*)_md2->head;
	float orig[3]= {nearPoint->x,nearPoint->y,nearPoint->z};//射线坐标
	float target[3] = {farPoint->x,farPoint->y,farPoint->z};//目标点

	float dir[3] = {orig[0]-target[0],orig[1]-target[1],orig[2]-target[2]};//射线向量-目标目标向量=方向向量

	float vert0[3] = {t[0] + base->x,t[1]+ base->y,t[2] + base->z};
	float vert1[3] = {t[3] + base->x,t[4]+ base->y,t[5] + base->z};
	float vert2[3] = {t[6] + base->x,t[7]+ base->y,t[8] + base->z};
	
	int hit;


	tl_hitTriangle(orig,dir,vert0,vert1,vert2,out,&hit);

//#define _HIT_LOG_	//是否打印日志

#ifdef _HIT_LOG_
	int i;
	if(GL_TRUE){
		printf("是否有交点 %d,交点坐标 x:%f y:%f z:%f  检测坐标:",hit,out[0],out[1],out[2]);
		
		for(i = 0;i<3;i++){
			printf("%f,",vert0[i]);
		}

		for(i = 0;i<3;i++){
			printf("%f,",vert1[i]);
		}

		for(i = 0;i<3;i++){
			printf("%f,",vert2[i]);
		}

		printf("\n");
	}
#endif
#undef _HIT_LOG_
	return hit;

	
}

void 
tl_pickTriangle(float* tri,int dataCount,
	struct Vec3* nearPoint,struct Vec3* farPoint,struct Vec3* fpos,
	struct HitResultObject* pResult)
{
	int i;
	float out[3];
	struct Vec3 hitVec;
	struct Vec3 _tpos;
	struct Vec3* pos = &_tpos;
	struct Vec3* pHitVec=&hitVec;

	//一个三角形需要的float个数,代表3个顶点
	#define _VERT_FLOAT_COUNT_ 9
	
	int count = _VERT_FLOAT_COUNT_;				
	int len = dataCount / count;	//三角形个数
	float v[_VERT_FLOAT_COUNT_];
	
	#undef _VERT_FLOAT_COUNT_

	//printf("-------------->开始检测一个模型的包围盒!\n");
	float _distance = INF;
	int size = len*count;

	if(tri==NULL){
		//没有顶点数据,不存在包围盒数据
		return;
	}
	
	for(i = 0;i < size;i+=count){
		
		pos->x = fpos->x;
		pos->y = fpos->y;
		pos->z = fpos->z;



		//对v进行赋值(v长度为9 一个三角形需要的float个数为9 描述其坐标)
		memcpy(v,tri+i,sizeof(float)*count);
		
		

		if(f_testHit(pos,nearPoint,farPoint,v,out)){

			//交点与摄影机的距离
			float _temp;

			struct Vec3 hit;
			
			//对hit进行赋值
			memcpy(&hit,out,sizeof(float)*3);

			//找到多个交点的时候,取距离射线起点最近的交点
			_temp=vec3Distance(&hit,nearPoint);

			if(_temp < _distance){
				_distance = _temp;

				//对pHitVec进行赋值
				memcpy(pHitVec,out,sizeof(float)*3);
			}
			//printf("=====================>检测成功! 射线与三角面的相交点:%.3f,%.3f,%.3f 三角形索引号 %d 距离射线起点的距离 %.3f\n",out[0],out[1],out[2],i,_temp);
			
			/*printf("三角形数据:");
			for(k = 0; k < count;k++){
				printf("%.3f  ",v[k]);
			}
			printf("\n");*/
			//可以将所有的交点点放到列表中
		}
		else
		{
			
		}
	}

	//printf("------------->检测结束!\n");
	if(_distance!=INF){
		pResult->isHit = TRUE;
		pResult->x = pHitVec->x;
		pResult->y = pHitVec->y;
		pResult->z = pHitVec->z;
		pResult->distance = _distance;

		//memcpy(pResult->pickTriangle,v,count*sizeof(float));
		//printf("***********************射线检测成功! 交点坐标:%.3f,%.3f,%.3f 距离射线起点: %.3f \n",pHitVec->x,pHitVec->y,pHitVec->z,pResult->distance);

	}else{
		//未拾取到三角形,点击到了空的区域
	}
	//return _distance;
}

/*
	复制一个字符串,返回一个堆上的引用
*/
char* tl_strCopy(const char* str){
	int len = (int)strlen(str);
	char* s = NULL;
	len++;
	s = (char*)tl_malloc(len);
	memset(s,0,len);
	memcpy(s,str,len);
	return s;
}
//==========================================================================
typedef struct BitmapFileHead{
	/*位图类别，根据不同的操作系统而不同，在Windows中，此字段的值总为‘BM’*/
	unsigned short bfType;			
	/*BMP图像文件的大小*/
	unsigned long bfSize;			
	unsigned short bfReserved1;
	unsigned short bfReserved2;
	/*BMP图像数据的地址 此地址开始位图数据的彩色表/调色板*/
	unsigned short bfOffBits;
}BitmapFileHead;
/*	颜色结构体*/
typedef struct Color{
	unsigned char r;
	unsigned char g;
	unsigned char b;
}Color;
typedef struct BitmapInfoHead{
	unsigned long biSize;
	unsigned long biWidth;			//位图宽度
	unsigned long biHeight;			//位图高度
	unsigned short biPlanes;		
	unsigned short biBitCount;		//位图位数 24,32   8位（256色）及以下（4位，1位）有颜色表调色板，以上没有
	unsigned long biCompression;
	unsigned long biSizeImage;
	unsigned long biXPelsPerMeter;
	unsigned long biYPelsPerMeter;
	unsigned long biClrUsed;
	unsigned long biClrImportant;
}BitmapInfoHead;
/*解析文件头*/
void parseBmpInfo(const char* _data,struct BitmapFileHead* p_bitmapheader,
struct BitmapInfoHead* p_bitmapinfo)
{
	int l = sizeof(unsigned long);	//4
	int s = sizeof(unsigned short);	//2
	int pos = 0;
	/*文件头*/
	memcpy(&p_bitmapheader->bfType,_data + pos,s);		pos+=s;
	memcpy(&p_bitmapheader->bfSize,_data+pos,l);		pos+=l;
	memcpy(&p_bitmapheader->bfReserved1,_data+pos,s);	pos+=s;
	memcpy(&p_bitmapheader->bfReserved2,_data+pos,s);	pos+=s;
	memcpy(&p_bitmapheader->bfOffBits,_data+pos,l);		pos+=l;
	/*文件信息*/
	memcpy(&p_bitmapinfo->biSize,_data+pos,l);			pos+=l;
	memcpy(&p_bitmapinfo->biWidth,_data+pos,l);			pos+=l;
	memcpy(&p_bitmapinfo->biHeight,_data+pos,l);		pos+=l;
	memcpy(&p_bitmapinfo->biPlanes,_data+pos,s);		pos+=s;
	memcpy(&p_bitmapinfo->biBitCount,_data+pos,s);		pos+=s;
	memcpy(&p_bitmapinfo->biCompression,_data+pos,l);	pos+=l;
	memcpy(&p_bitmapinfo->biSizeImage,_data+pos,l);		pos+=l;
	memcpy(&p_bitmapinfo->biXPelsPerMeter,_data+pos,l);	pos+=l;
	memcpy(&p_bitmapinfo->biYPelsPerMeter,_data+pos,l);	pos+=l;
	memcpy(&p_bitmapinfo->biClrUsed,_data+pos,l);		pos+=l;
	memcpy(&p_bitmapinfo->biClrImportant,_data+pos,l);	pos+=l;
}

void static
parseBmp24(const char* _data,struct Color* bs,struct BitmapFileHead* p_bitmapheader,struct BitmapInfoHead *p_bitmapinfo)
{
	int	bitOffset=p_bitmapheader->bfOffBits;
	int bfSize = (int)p_bitmapheader->bfSize;
	int _sl = (int)sizeof(struct Color);
	int w = (int)p_bitmapinfo->biWidth;
	int h = (int)p_bitmapinfo->biHeight;
	int offset = h * w+p_bitmapheader->bfOffBits;/*总的像素偏移*/
	while(bitOffset<bfSize){
		int j;
		if(bitOffset >= offset){
			break;
		}
		for(j=0;j<w;j++){
			struct Color _color;
			int k;
			for(k=0;k<h;k++){
				memcpy(&_color,_data + bitOffset,_sl);
				bs[j * w + k].b = _color.r;
				bs[j * w + k].g = _color.g;
				bs[j * w + k].r = _color.b;
				bitOffset=bitOffset+_sl;
			}
		}
	}
}

unsigned char* tl_bmp24(const char* bmppath,int* biWidth,int* biHeight){
	long ct=get_longTime();
	long sub;
	char outName[G_BUFFER_128_SIZE];

	char* fileName=NULL;

	int bytesLength;//字节大小
	struct Color* bs = NULL;
	struct BitmapFileHead bitmapheader ;
	struct BitmapInfoHead bitmapinfo ;
	struct BitmapFileHead * p_bitmapheader = &bitmapheader;
	struct BitmapInfoHead *p_bitmapinfo = &bitmapinfo;
	//char* _path = (char*)tl_malloc(strlen(bmppath)+1);
	char* _data = NULL;

	//memset(_path,0,strlen(bmppath)+1);
	//memcpy(_path,bmppath,strlen(bmppath));
	_data =  tl_loadfile(bmppath,NULL);

	parseBmpInfo(_data,p_bitmapheader,p_bitmapinfo);

	//fileName = tl_getFileName((const char*)bmppath);
	tl_getPathName((const char*)bmppath,outName);

	//tl_free(_path);

	//_path = NULL;
	*biWidth = p_bitmapinfo->biWidth;
	*biHeight= p_bitmapinfo->biHeight;
	bytesLength = sizeof(struct Color)* 3 * p_bitmapinfo->biWidth * p_bitmapinfo->biHeight;
	bs=(struct Color*)tl_malloc(bytesLength);
	/*	bmp24解析	*/
	if(p_bitmapinfo->biBitCount == 24){
		parseBmp24(_data,bs,p_bitmapheader,p_bitmapinfo);
	}
	tl_free(_data);
	_data = NULL;
	sub = get_longTime()-ct;
	
	//printf("bmp24解析贴图 [%s](width %d,height %d) 使用 %ld 毫秒\n",outName,*biWidth,*biHeight,sub);//打印解析bmp耗时 2s
	
	return (unsigned char*)bs;
}

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
void vec3Normalize(struct Vec3* v){
	float length = vec3Length(v);
	v->x = v->x / length;
	v->y = v->y / length;
	v->z = v->z / length;
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

int tl_bintree(int t){
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
struct Array* array_create(int length,int mode){
	struct Array* p = (struct Array*)tl_malloc(sizeof(struct Array));
	memset(p,0,sizeof(struct Array));
	p->mode = mode;
	p->ptr = (int*)tl_malloc(sizeof(int)*length);
	p->length = length;
	memset(p->ptr,0,sizeof(int)*length);
	return p;
}

void array_free(struct Array* p){
	int i = 0;
	for(i = 0;i < p->length;i++){
		int node = p->ptr[i];
		if(node && p->mode == ARRAY_HEAP){
			tl_free((void*)node);
			p->ptr[i] = 0;
		}
	}
	tl_free(p->ptr);
	p->ptr = NULL;
	tl_free(p);
	p = NULL;
}

void array_log(struct Array* p){
	int i = 0;
	printf("数组长度:%d\n",array_len(p));
	for(i = 0;i < p->length;i++){
		int node = p->ptr[i];
		printf("0x%0x array[%d]:%d\n",&(p->ptr[i]),i,node);
	}
}

int array_len(struct Array* ptr){
	return ptr->length;
}

void array_put(struct Array* ptr,int d){
	if(ptr->cur+1 > ptr->length){
		printf("数组越界!\n");
		assert(0);
	}
	ptr->ptr[ptr->cur] = d;
	ptr->cur++;
}

int array_get(struct Array*p,int index,int* outData){
	if(index+1>p->length){
		return 0;
	}
	*outData = p->ptr[index];
	return 1;
}

int array_set(struct Array* p,int index,int d){
	if(index+1>p->length){
		return 0;
	}
	p->ptr[index]=d;
	return 1;
}

static void f_arradd(struct Array* ptr,const char* source,
				int s,int e){
	if(s == e){
		array_put(ptr,0);
		//printf("()\n");
	}else{
		int n = e - s + 1;
		char* _str = (char*)tl_malloc(n);
		memset(_str,0,n);
		memcpy(_str,source+s,e - s);
		array_put(ptr,(int)_str);
		//printf("(%s)\n",_str);
	}
}

struct Array* array_split(const char* str,char key){

	//_splen用来确定静态数组的创建长度
	int _spLen = tl_split_getlen(str,key);
	
	//int length = tl_strlen(str);
	int offset = 1;//key占用的字符数
	struct Array* list = array_create(_spLen,ARRAY_HEAP);
	int _k = 0;
	int start = 0;
	int end;
	int index = tl_charIndex((char*)str,key);
//	int index =	tl_strpos_s(str,key);
	while (index!=-1){

		end = index+start;

		f_arradd(list,str,start,end);

		start=end+offset;
		index = tl_charIndex((char*)(str + start),key);
//		index = tl_strpos_s((char*)(str + start),key);
		_k++;
	}
	f_arradd(list,str,start,tl_strlen(str));
	return list;
}
//=================================================================================
struct Str* str_create( const char* s ){
	struct Str* _str = tl_malloc(sizeof(struct Str));
	int len;
	memset(_str,0,sizeof(struct Str));
	len = tl_strlen(s)+1;
	_str->s = tl_malloc(len);
	memset(_str->s,0,len);
	memcpy(_str->s,s,len-1);
	return _str;
}

void str_dispose( struct Str* _str){
	if(_str->s){
		tl_free(_str->s);
		_str->s = NULL;
	}
	tl_free(_str);
	_str = NULL;
}

void str_appand( struct Str* _str,const char * s ){
	char* _tmps = _str->s;
	int len,ts;
	ts = tl_strlen(_tmps);
	len = tl_strlen(s) + ts;
	_str->s = tl_malloc(len+1);
	memset(_str->s,0,len+1);
	memcpy(_str->s,_tmps,ts);
	memcpy(_str->s+ts,s,tl_strlen(s));
	tl_free(_tmps);
	_tmps = NULL;
}

int str_length( struct Str* str ){
	return tl_strlen(str->s);
}
void str_appand_s( struct Str* _str,const char * s,int length )
{
	char* _tmps = _str->s;
	int len,ts;
	ts = tl_strlen(_tmps);
	len = length + ts;

	_str->s = tl_malloc(len+1);
	memset(_str->s,0,len+1);
	memcpy(_str->s,_tmps,ts);

	memcpy(_str->s+ts,s,length);
	tl_free(_tmps);
	_tmps = NULL;
}
void str_replace(struct Str* pstr, char *oldstr, char *pDst){

	Str* _ss;
	char* ts;
	int i,_l,mode=0;

	_l = tl_strlen(oldstr);//需要替换的字符串结点长度

	_ss = str_create("");
	
	ts = pstr->s;
	i=strcspn(ts,oldstr);

	if(i){
		mode = 1;
	}

	while(i<tl_strlen(ts)){
		char* _new;
		int n;

		if(mode){
			str_appand_s(_ss,ts,i);
			ts += i;
			str_appand(_ss,pDst);
			mode = 0;
		}
		else{
			str_appand(_ss,pDst);
		}

		_new = ts +_l;
		n=strcspn(_new,oldstr);
		str_appand_s(_ss,_new,n);
		ts = _new+n;
		i = n;

		if(!tl_strlen(ts)){
			break;
		}
	}
	tl_free(pstr->s);
	pstr->s = _ss->s;
	tl_free(_ss);
}



void str_copy(struct Str* target,struct Str* dest){
	int len = str_length(dest)+1;
	char* s = tl_malloc(len);
	memset(s,0,len);
	memcpy(s,dest->s,str_length(dest));
	tl_free(target->s);
	target->s = s;
}
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

	LStack_push((void*)p->list,(int)m);
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

int tl_memGetStat()
{
	return GetInstance()->stat;
}

int tl_memByteSize(){
	if(tl_memGetStat()){
		int size = 0;
		LStack_ergodic(GetInstance()->list,memSizeCallBack,(int)&size);
		return size;
	}else{
		printf("未使用内存池模式,取出来的数据有问题 请设置GetInstance()->stat\n");
	}
	return -1;
}

void 
tl_showCurMemery512(const char* str)
{
	if(tl_memGetStat()){
		char buffer[G_BUFFER_512_SIZE];
		memset(buffer,0,G_BUFFER_512_SIZE);
		sprintf_s(buffer,G_BUFFER_512_SIZE,"%s -->%d bytes",str,tl_memByteSize());
		printf("%s\n",buffer);
	}
}

int Split(char* p){
	int* parms = (int*)p;
	int i;
	struct MemHandle * instance=(struct MemHandle *)parms[0];
	char* str = (char*)parms[2];
	int buffer,bufferSize;
	sscanf(str,"%d,%d",&buffer,&bufferSize);

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
	sscanf(str,"%d,%d",&buffer,&bufferSize);

	{
		int _size = bufferSize * buffer;
		_instance->maxSize+=_size;

#ifdef _DEBUG_MODE_
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
	tl_split_s((const char*)pool,G_SIGN_AITA,f_MemSplitAdd,parms,2);
}

void tl_memInit(const char* pool){
	int i = 0;
	int parms[3];

	struct MemHandle * p = (struct MemHandle *)MALLOC(sizeof(struct MemHandle));
	_memInstance = p;
	
	printf("内存池配置:(%s)\n",pool);

	p->maxSize = 0;

	f_GetNeedSize(p,pool);
	p->pos = 0;
	p->list = LStack_create();
	p->buffer = (void*)MALLOC(p->maxSize);

	parms[0] = (int)p;
	parms[1] = 0;
	tl_split_s((const char*)pool,G_SIGN_AITA,Split,parms,2);
	tl_bubbleSortInt(p->bufferList,p->bufferCount);

	printf("内存池总大小%f mb,内存池已分配大小:%f mb\n",(float)(p->maxSize)/1024/1024,(float)p->pos/1024/1024);
}

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

void* tl_malloc(int size){
	return memory_new(size);
/*

	struct MemHandle* instance = GetInstance();
	if(instance->stat){
		int needSize;	
		//此方法可以使用配置生成

		int fSize = 0;
		N* _node = NULL;
		int _stat= getNextSize(size,instance->bufferList,instance->bufferCount,&needSize);

		if(_stat==MEM_NOT_FOUNT)
		{
			printf("请求的内存为:%d字节,重新设置内存池大小\n",size);
			assert(0);
		}
		else if(_stat == MEM_NEED_MORE)
		{
			printf("需要更大的内存块 %d\n",size);
			assert(0);
		}

		fSize = needSize;

		_node=findNode(instance,fSize);
		if(_node != NULL){
			_node->uesd = MEM_USED;
#ifdef _SHOW_MALLOC_LOG_
			printf("申请 0x%x %d/%d\n",n,_node->len,tl_memsize());
#endif
			return _node->p;
		}else{
			//内存池内存不够,为内存池添加节点
			
			//printf("请求内存%d字节未找到,可以重新申请的内存块,检索内存:%d字节\n",size,fSize);
			
			
			//assert(0);
		
		
		//printf("新申请内存:%d字节\n",fSize);
		//	instance->maxSize+=fSize;
		//	realloc(instance->buffer,instance->maxSize);
		//	{
		//		struct MemNode* _n=	ex_addNode(instance ,fSize);
		//		return _n->p;
		//	}
			
			
			return tl_malloc(fSize);
		}
		printf("tl_malloc (%d bytes) fail!\n",size);
		return NULL;
	}
	//printf("MALLOC ");
	return MALLOC(size);
*/
}
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
void tl_free(void* p){
	memory_free(p);
	return;
/*
	struct MemHandle* t = GetInstance();
	if(t->stat){	

		struct MemDelNode delNode;
		memset(&delNode,0,sizeof(struct MemDelNode));
		delNode.stat = FALSE;
		delNode.target = p;
		LStack_ergodic(GetInstance()->list,freeDelNode,(int)&delNode);

		if(delNode.stat== FALSE){
			printf("tl_free 0x%x 失败,找不到释放的节点!\n",(int)p);
			assert(0);
		}else{
			struct MemNode* n = delNode.result;
			#ifdef _SHOW_MALLOC_LOG_
			printf("        释放 0x%x %d/%d\n",n,n->len,tl_memsize());
			#endif
		}
		p = NULL;

		return;
	}
	FREE(p);
	*/

}
//删除内存池
void tl_memdel(){
	struct MemHandle* t = GetInstance();
	FREE(t->buffer);
	t->buffer = NULL;
	FREE(t);
	t = NULL;
}

void tl_memState(int _stat){
	struct MemHandle* t = GetInstance();
	t->stat=_stat;
}
//=================================================================================
//ramp链表
struct LStackNode* rampList = NULL;

void f_rampMove(struct BaseLoopVO* p);

/*
*	获取一个节点
*/
struct BaseLoopVO* f_rampget();
/*
	v1:目标向量
	ms:需要消耗的毫秒数
*/
void f_ramp_vec(struct BaseLoopVO* move,float* px,float* py, float* pz,struct Vec3* v1,float ms){
	int fps = tl_get_fps();

	//out
	struct Vec3 o;

	struct Vec3 v0;

	int count;

	vec3Set(&v0,*px,*py,*pz);

	
	vec3Sub(v1,&v0,&o);

	//	回调的次数,float n = 1000/fps;//1帧 需要n毫秒
	
	count = (int)(ms/(1000.0f/(float)fps));

	move->tx = v1->x;			move->ty = v1->y;			move->tz = v1->z;
	move->px = px;				move->py = py;				move->pz = pz;

	if(count<=0){
		//printf("=====================>直接结束 ms:%.3f,fps:%d,count:%d\n",ms,fps,count);
		//直接结束,直接赋值
		f_rampMove(move);
	}else{
		//有回调次数
		//struct Vec3 ev;
		vec3Div(&o,(float)count);

		move->start=get_longTime();
		move->count = count;

		move->o.x = o.x;		move->o.y = o.y;		move->o.z = o.z;
		move->usems = (long)ms;
		move->callBack = f_rampMove;
	}
}
/*
	实现步骤思路:
	1.加一个回调到渲染主循环中,然后获取每一帧间隔的毫秒数every_ms
	2.用callbackCount =	ms / every_ms获取到需要回调的次数
	3.计算从[向量v0]移动到[向量v1]每次偏移的向量数
	4.如果到达目标向量,则停止回调
*/
void f_rampMove(struct BaseLoopVO* p){
	struct Vec3 v0,v1;
	p->count--;
	
	vec3Set(&v0,*p->px ,*p->py ,*p->pz );
	vec3Add(&v0,&p->o,&v1);

	*p->px = v1.x;	*p->py = v1.y; *p->pz = v1.z;

	if(p->count<=0){
		p->callBack = NULL;
		*p->px = p->tx;		*p->py = p->ty;	*p->pz = p->tz;
		//printf(" count: %d   消耗时间%ld毫秒, 开始移动 %.3f,%.3f,%.3f\n",p->count,			get_longTime() - tmpms,v1.x ,v1.y ,v1.z);		
		//printf("end\n");
		return;
	}else{
		struct Vec3 v1;
		v1.x = p->tx;
		v1.y = p->ty;
		v1.z = p->tz;

		f_ramp_vec(p,
			p->px,p->py,p->pz,
			&v1, (float)(p->usems- (get_longTime()-p->start)));
	}
}

void ramp_callBack(){
	struct LStackNode* s = (struct LStackNode* )rampList;
	void* top,*p;
	int isFind = 0;
	if(!s){
		return;
	}
	top = s;
	p=top;
	while((int)LStack_next(p)){
		int data;
		struct BaseLoopVO* b;

		p=(void*)LStack_next(p);
		data = LStack_data(p);

		b = (struct BaseLoopVO*)data;

		//回调函数引用==NULL的时候 说明该节点未使用
		if(b->callBack!=NULL){
			b->callBack(b);
		}
	}
}
struct BaseLoopVO* f_rampget(){

	struct LStackNode* s;
	void* top,*p;
	int isFind = 0;
	struct BaseLoopVO* node;

	if(!rampList){
		rampList =LStack_create();
	}
	s = (struct LStackNode* )rampList;
	
	top = s;
	p=top;
	while((int)LStack_next(p)){
		int data;
		struct BaseLoopVO* b;

		p=(void*)LStack_next(p);
		data = LStack_data(p);
		
		b = (struct BaseLoopVO*)data;
		
		//回调函数引用==NULL的时候 说明该节点未使用
		if(!b->callBack){
			memset(b,0,sizeof(struct BaseLoopVO));
			return b;
		}
	}
	node = (struct BaseLoopVO*)tl_malloc(sizeof(struct BaseLoopVO));
	memset(node,0,sizeof(struct BaseLoopVO));
	LStack_push((void*)s,(int)node);

	printf("创建节点[struct BaseLoopVO*],ramp_list.length:%d\n",LStack_length(s));

	return node;
}

struct BaseLoopVO* ramp_vec_trace(float* px,float* py,float* pz,float tx,float ty,float tz,float ms){
	struct Vec3 v1;
	struct BaseLoopVO* n = f_rampget();
	vec3Set(&v1,tx,ty,tz);
	f_ramp_vec(n,px,py,pz,&v1,ms);
	return n;
}

void f_ramp_delay_callBack(struct BaseLoopVO* p){
	//long n = ;
	//printf("已经使用了%ld ms\n",get_longTime());
	if(get_longTime() - p->start>=p->usems){
		p->delayCallBack(p->delayParms);
		p->callBack=NULL;
	}
}

struct BaseLoopVO* ramp_delay(void (*callBack)(void*),void* parms,float ms){
	struct BaseLoopVO* n = f_rampget();
	n->start = get_longTime();
	n->usems = (long)ms;
	n->delayCallBack = callBack;
	n->delayParms = parms;
	n->callBack=f_ramp_delay_callBack;
	return n;
}

int ramp_isPlaying(struct BaseLoopVO* node){
	if(!node){
		return 0;
	}
	return node->callBack==NULL ? 0 : 1;
}

void ramp_stopPlaying(struct BaseLoopVO* node){
	if(node && ramp_isPlaying(node))
		node->callBack=NULL;
}


//矩阵处理

void mat4x4_zero(Matrix44f M){
	int i;
	for(i = 0; i < 16; i++)
		M[i] =0;
}

void mat4x4_2t1(Matrix44f M,int m,int n,float value)
{
	M[4 * m + n] = value;
}

/**
 * 矩阵行列转置
 * Transpose the values of a mat4
 *
 * @param {mat4} out the receiving matrix
 * @param {mat4} a the source matrix
 * @returns {mat4} out
 */
void mat4x4_transpose(Matrix44f a) 
{
	Matrix44f out;
	out[0] = a[0];
	out[1] = a[4];
	out[2] = a[8];
	out[3] = a[12];
	out[4] = a[1];
	out[5] = a[5];
	out[6] = a[9];
	out[7] = a[13];
	out[8] = a[2];
	out[9] = a[6];
	out[10] = a[10];
	out[11] = a[14];
	out[12] = a[3];
	out[13] = a[7];
	out[14] = a[11];
	out[15] = a[15];
	memcpy(a,out,sizeof(Matrix44f));
}

void 
mat4x4_lookAt(Matrix44f out,struct Vec3* eye,struct Vec3* center,struct Vec3* up) 
{
	float x0 =0,x1=0 ,
		x2 =0,
		y0 =0,
		y1 =0,
		y2 =0,
		z0 =0,
		z1 =0,
		z2 =0,
		len =0;
	float eyex = eye->x;
	float eyey = eye->y;
	float eyez = eye->z;
	float upx = up->x;
	float upy = up->y;
	float upz = up->z;
	float centerx = center->x; 
	float centery = center->y;
	float centerz = center->z;

	if (abs(eyex - centerx) < EPSILON && abs(eyey - centery) < EPSILON && abs(eyez - centerz) < EPSILON) {
		mat4x4_identity(out);
		return;
	}

	z0 = eyex - centerx;
	z1 = eyey - centery;
	z2 = eyez - centerz;

	len = 1 / sqrt(z0 * z0 + z1 * z1 + z2 * z2);
	z0 *= len;
	z1 *= len;
	z2 *= len;

	x0 = upy * z2 - upz * z1;
	x1 = upz * z0 - upx * z2;
	x2 = upx * z1 - upy * z0;
	len = sqrt(x0 * x0 + x1 * x1 + x2 * x2);
	if (!len) {
		x0 = 0;
		x1 = 0;
		x2 = 0;
	} else {
		len = 1 / len;
		x0 *= len;
		x1 *= len;
		x2 *= len;
	}

	y0 = z1 * x2 - z2 * x1;
	y1 = z2 * x0 - z0 * x2;
	y2 = z0 * x1 - z1 * x0;

	len = sqrt(y0 * y0 + y1 * y1 + y2 * y2);
	if (!len) {
		y0 = 0;
		y1 = 0;
		y2 = 0;
	} else {
		len = 1 / len;
		y0 *= len;
		y1 *= len;
		y2 *= len;
	}

	out[0] = x0;
	out[1] = y0;
	out[2] = z0;
	out[3] = 0;
	out[4] = x1;
	out[5] = y1;
	out[6] = z1;
	out[7] = 0;
	out[8] = x2;
	out[9] = y2;
	out[10] = z2;
	out[11] = 0;
	out[12] = -(x0 * eyex + x1 * eyey + x2 * eyez);
	out[13] = -(y0 * eyex + y1 * eyey + y2 * eyez);
	out[14] = -(z0 * eyex + z1 * eyey + z2 * eyez);
	out[15] = 1;
}


/////////////////////////////////////////////////////////////////////
//
void mat4x4_translate(Matrix44f M, float x, float y, float z)
{
	//clearMat(M);
    M[3]  =x;
    M[7]  =y;
    M[11] =z;
    M[15] =1.0f;
}
/*
 *矩阵的逆
 */
void mat4x4_invert(Matrix44f out,Matrix44f a) {
	int i;
	float a00 = a[0],
		a01 = a[1],
		a02 = a[2],
		a03 = a[3];
	float a10 = a[4],
		a11 = a[5],
		a12 = a[6],
		a13 = a[7];
	float a20 = a[8],
		a21 = a[9],
		a22 = a[10],
		a23 = a[11];
	float a30 = a[12],
		a31 = a[13],
		a32 = a[14],
		a33 = a[15];

	float b00 = a00 * a11 - a01 * a10;
	float b01 = a00 * a12 - a02 * a10;
	float b02 = a00 * a13 - a03 * a10;
	float b03 = a01 * a12 - a02 * a11;
	float b04 = a01 * a13 - a03 * a11;
	float b05 = a02 * a13 - a03 * a12;
	float b06 = a20 * a31 - a21 * a30;
	float b07 = a20 * a32 - a22 * a30;
	float b08 = a20 * a33 - a23 * a30;
	float b09 = a21 * a32 - a22 * a31;
	float b10 = a21 * a33 - a23 * a31;
	float b11 = a22 * a33 - a23 * a32;

	// Calculate the determinant
	float det = b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06;

	for(i = 0;i < 16;i++)
	{
		out[i] = 0;
	}
	if (!det) {
		return;
	}
	det = 1.0 / det;

	out[0] = (a11 * b11 - a12 * b10 + a13 * b09) * det;
	out[1] = (a02 * b10 - a01 * b11 - a03 * b09) * det;
	out[2] = (a31 * b05 - a32 * b04 + a33 * b03) * det;
	out[3] = (a22 * b04 - a21 * b05 - a23 * b03) * det;
	out[4] = (a12 * b08 - a10 * b11 - a13 * b07) * det;
	out[5] = (a00 * b11 - a02 * b08 + a03 * b07) * det;
	out[6] = (a32 * b02 - a30 * b05 - a33 * b01) * det;
	out[7] = (a20 * b05 - a22 * b02 + a23 * b01) * det;
	out[8] = (a10 * b10 - a11 * b08 + a13 * b06) * det;
	out[9] = (a01 * b08 - a00 * b10 - a03 * b06) * det;
	out[10] = (a30 * b04 - a31 * b02 + a33 * b00) * det;
	out[11] = (a21 * b02 - a20 * b04 - a23 * b00) * det;
	out[12] = (a11 * b07 - a10 * b09 - a12 * b06) * det;
	out[13] = (a00 * b09 - a01 * b07 + a02 * b06) * det;
	out[14] = (a31 * b01 - a30 * b03 - a32 * b00) * det;
	out[15] = (a20 * b03 - a21 * b01 + a22 * b00) * det;
}

/**
 * Transforms the vec2 with a mat4
 * 3rd vector component is implicitly '0'
 * 4th vector component is implicitly '1'
 *
 * @param {vec2} out the receiving vector
 * @param {vec2} a the vector to transform
 * @param {mat4} m matrix to transform with
 * @returns {vec2} out
 */
//void mat4x4_transformMat4(float out[4],float a[4],Matrix44f m) {
//  float x = a[0];
//  float y = a[1];
//  out[0] = m[0] * x + m[4] * y + m[12];
//  out[1] = m[1] * x + m[5] * y + m[13];
//}

void mat4x4_transformMat4(float out[4],float a[4],Matrix44f m) {
	float x = a[0],
		y = a[1],
		z = a[2];
	float w = m[3] * x + m[7] * y + m[11] * z + m[15];
	w = w || 1.0;
	out[0] = (m[0] * x + m[4] * y + m[8] * z + m[12]) / w;
	out[1] = (m[1] * x + m[5] * y + m[9] * z + m[13]) / w;
	out[2] = (m[2] * x + m[6] * y + m[10] * z + m[14]) / w;
}

//void mat4x4_getTranslate(Matrix44f M,struct Vec3* pos)
//{
//	pos->x = M[3];
//	pos->y = M[7];
//	pos->z = M[11];
//}
void mat4x4_scale(Matrix44f M, float x, float y, float z)
{
	//clearMat(M);
    M[0]  =x;
    M[5]  =y;
    M[10] =z;
    M[15] =1.0;
}
void mat4x4_identity(Matrix44f M){
	mat4x4_zero(M);
	M[0] =1.0;
    M[5]  =1.0;
    M[10] =1.0;
    M[15] =1.0;
}
//void matx4x4_zore(Matrix44f M){
//	mat4x4_clearMat(M);
//	//mat4x4_identity(M);
//}
void mat4x4_printf(const char* name,Matrix44f M)
{
	int i,n=0;
	
	printf("矩阵%s:=\n",name);
	for(i = 0;i < 16;i++){
		if(n >= 4){
			printf("\n");
			n=0;
		}
		printf("[%d]	%.3f\t\t",i,M[i]);//保留小数点3位
		n++;
	}
	printf("\n****************************************************\n");
}
void mat4x4_double_printf(const char* name,double m[16])
{
	int i,n=0;

	printf("矩阵%s:=\n",name);
	for(i=0;i<16;i++){
		if(n>=4){
			printf("\n");
			n=0;
		}
		printf("[%d]	%f\t\t",i,m[i]);
		n++;
	}
	printf("\n****************************************************\n");
}
///////////////////////////////////////////////////////////////////////////
////
void mat4x4_rotateX(Matrix44f M, float radian)
{
	//clearMat(M);

    M[0]  =1.0;
    M[5]  =(float)cos(radian);
    M[6]  =(float)sin(radian);
    M[9]  =-(float)sin(radian);
    M[10] =(float)cos(radian);
    M[15] =1.0;
}
void mat4x4_rotateY(Matrix44f M, float radian)
{
	//clearMat(M);
    M[0]  =(float)cos(radian);
    M[2]  =-(float)sin(radian);
    M[5]  =1.0;
    M[8]  =(float)sin(radian);
    M[10] =(float)cos(radian);
    M[15] =1.0;
	//printf("%f %f\n",(float)cos(radian),cos(radian));
}

void mat4x4_copy(Matrix44f M,Matrix44f out)
{
	memcpy(out,M,sizeof(Matrix44f));
}
void mat4x4_rotateZ(Matrix44f M, float radian)
{
	//clearMat(M);
    M[0]  =(float)cos(radian);
    M[1]  =-(float)sin(radian);
    M[4]  =(float)sin(radian);
    M[5]  =(float)cos(radian);
    M[10] =1.0;
    M[15] =1.0;
}

void mat4x4_rotateXYZ(Matrix44f M, float radian, float x, float y, float z)
{
    int i;
     
 
    for(i= 0; i <16; i++)
        M[i] =0;
 
}
 
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
void perspectiveFrustum(Matrix44f M, float left, float right, float top, float bottom, float near, float far)
{	
	M[0]  =2*near/(right -left);
    M[2]  =(right +left)/(right -left);
    
	M[5]  = 2*near/(top -bottom);
	M[5]  = -2*near/(top -bottom);//这里翻转一下

    M[6]  =(top +bottom)/(top -bottom);
    M[10] =(near +far)/(near -far);

    M[11] =2*near*far/(near -far);
    M[14] =-1.0;

	/*M[11] =-1.0;
	M[14] =2*near*far/(near -far);*/
}
 
void orthoPerspect(Matrix44f M,  float left, float right, float top, float bottom, float near, float far)
{
	//clearMat(M);
    M[0]  =2/(right -left);
    M[3]  =(left +right)/(left -right);
    M[5]  =2/(top -bottom);
    M[7]  =(bottom +top)/(bottom -top);
    M[10] =2/(near -far);
    M[11] =(near +far)/(far -near);
    M[15] =1.0;
}
//
//void matrix_m(float **a_matrix, const float **b_matrix, const float **c_matrix,
//			  int krow, int kline, int kmiddle, int ktrl)
//			  ////////////////////////////////////////////////////////////////////////////
//			  //	a_matrix=b_matrix*c_matrix
//			  //	krow  :行数
//			  //	kline :列数
//			  //	ktrl  :	大于0:两个正数矩阵相乘 不大于0:正数矩阵乘以负数矩阵
//			  ////////////////////////////////////////////////////////////////////////////
//{
//	int k, k2, k4;
//	float stmp;
//
//	for (k = 0; k < krow; k++)     
//	{
//		for (k2 = 0; k2 < kline; k2++)   
//		{
//			stmp = 0.0;
//			for (k4 = 0; k4 < kmiddle; k4++)  
//			{
//				stmp += b_matrix[k][k4] * c_matrix[k4][k2];
//			}
//			a_matrix[k][k2] = stmp;
//		}
//	}
//	if (ktrl <= 0)   
//	{
//		for (k = 0; k < krow; k++)
//		{
//			for (k2 = 0; k2 < kline; k2++)
//			{
//				a_matrix[k][k2] = -a_matrix[k][k2];
//			}
//		}
//	}
//}

/************************************************************************/
/* 将arr中的矩阵相乘之后存储到o中                                       */
/************************************************************************/
static
void f_get_mat4x4(int lim,int* arr,Matrix44f o){
	int i;
	Matrix44f s,t,r,m;
	mat4x4_identity(r);

	for(i = 0; i < lim; i++){
		mat4x4_copy(r,t);
		memcpy(s,arr[i],16 * sizeof(float));
		mat4x4_zero(m);
		multi2(m,t,s);
		mat4x4_copy(m,r);
	}

	mat4x4_copy(r,o);
}

#define _Matrix44_Max_Size_ 16//最大支持的矩阵连乘个数

void 
mat4x4_mult(int lim,Matrix44f o,...){
	
	mat4x4_zero(o);
	
	if(lim>_Matrix44_Max_Size_){
		printf("mat4x4_mult长度超出%d\n",_Matrix44_Max_Size_);
		assert(0);
	}else{
		
		//矩阵地址列表
		int arr[_Matrix44_Max_Size_];
		int i;
		
		va_list ap;
		
		va_start(ap, o);

		for(i = 0; i < lim; i++){
			int p = va_arg(ap, int);			
			arr[i] = p;
		}
		va_end(ap);

		f_get_mat4x4(lim,arr,o);
	}
}

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

void 
tl_address2str(int v,char* buffer,int bufferSize)
{
	memset(buffer,0,bufferSize);
	sprintf_s(buffer,bufferSize,"0x%0x",v);
}

int tl_getGap(int type)
{
	if(type == OBJ_UV_VERTEX)
	{
		return UV_GAP + VERTEX_GAP;
	}
	return UV_GAP + VERTEX_GAP + NORMAL_GAP;
}
static int _nameId;

void 
tl_newName(char* buffer,int bufferSize)
{
	memset(buffer,0,bufferSize);
	sprintf_s(buffer,bufferSize,"instance%d",_nameId);
	_nameId++;
}
int 
tl_boolString2Int(const char* str)
{
	return !strcmp(str,"true") ? 1 : 0;
}
const char* 
tl_intConvertBool(int i)
{
	return i == 1?"true":"false";
}


