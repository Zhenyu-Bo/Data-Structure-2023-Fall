#ifndef _HUFFMANTREE_CPP_
#define _HUFFMANTREE_CPP_

//哈夫曼树 ：分配连续存储空间存储树 

#include<stdio.h>
#include<stdlib.h>

//定义编码最大长度
#define MCL 255//最坏的情况下，Huffman树的高度为255

//定义叶结点数，8bit的unsigned char
#define n   256

//定义结点总数，Huffman树中应为2n-1
#define m   511

#ifndef todo
#define todo


//用三叉链表定义树节点
typedef struct node{
    unsigned int id;
    long w;//权值
    struct node *parent, *lchild, *rchild;//双亲指针和左右孩子指针
} HTNode;

//定义哈夫曼编码表，前面都是0，后面len位是编码
typedef struct {
    unsigned code;
    int len;//编码长度
} HCode;

void printHTree1(HTNode *T);//声明打印Huffman树的函数

//包含头文件"showGT.h"，用于生成html文件，查看图
//#include "showGT.h"

/*** 生成Huffman树 ***/

//输入长度为n = 256的向量（表示每个字符出现的次数），构造Huffman树并返回树根的下标
int createHTree(HTNode *ht[], long *w)
{
    long all_w = 0, numZero = 0;//所有字符出现的次数总和,未出现的字符数
    
    int i = 0, j = 0;
    //初始化Huffman树，每个结点都是根节点
    for(;i < m;i++)
    {
        ht[i] = (HTNode *)malloc(sizeof(HTNode));
        if(!ht[i]){
            printf("存储空间分配失败!\n");
            exit(0);
        }
        //先清空所有结点
        ht[i]->parent = ht[i]->lchild = ht[i]->rchild = NULL;
        ht[i]->id = i;
        ht[i]->w = 0;
        //若i < n，说明其为叶结点，对它进行赋值
        if(i < n)
        {
            ht[i]->w = w[i];
            if(!w[i])
                numZero++;//统计未出现的字符数
            else
                all_w += w[i];//统计所有字符出现次数的总和
        }
    }
    //构造Huffman树
    i = n;
    long min1,min2;
    while(i < m - numZero)
    {
        min1 = min2 = all_w + 10;//设置为最大的w值
        /*HTNode *pos1,*pos2;//指向权值最小的两个根结点
        pos1 = (HTNode *)malloc(sizeof(HTNode));
        pos2 = (HTNode *)malloc(sizeof(HTNode));*/
        unsigned pos1,pos2;
        for(j = 0;j < i;j++)
        {
            if(ht[j]->w == 0)
                continue;//跳过未曾出现过的结点
            if(!ht[j]->parent)//若all_nodes[j]为根节点，则对其进一步处理
            {
                //寻找权值最小的两个根节点，过程中始终保持min1 < min2
                if(ht[j]->w < min1)
                {
                    pos2 = pos1;
                    min2 = min1;
                    pos1 = j;
                    min1 = ht[j]->w;
                }
                else if(ht[j]->w < min2)
                {
                    pos2 = j;
                    min2 = ht[j]->w;
                }
            }
        }
        //内层循环结束后，找到两个权值最小的根节点，将它们连接到all_nodes[i]的左右孩子上，双亲指针指向all_nodes[i]
        ht[i]->lchild = ht[pos1];
        ht[i]->rchild = ht[pos2];
        ht[i]->w = min1 + min2;
        ht[pos1]->parent = ht[pos2]->parent = ht[i];
        i++;
    }
    //上述循环结束后，根节点的下标为m - numZero - 1
	//all_nodes[m - numZero - 1]->parent->id = -1;
    return m - numZero - 1;
}

//利用先序遍历遍历二叉树
void printHTree0(HTNode *root)
{
    if(!root)
    {
        if(root->w)
        {
            printf("%3d->%5d\n",(root->id < n) ? (root->id) : (root->id + n),root->w);
            printHTree0(root->lchild);
            printHTree0(root->rchild);
        }
    }
}

//直接当数组打印出来，调试用 
void printHtree1(HTNode *ht[]){
	printf("\n哈夫曼树(数组次序)：\n"); 
	for(int i=0;i<m;++i)
		if (i<n && ht[i]->parent)
			printf("%2x %3d -> %5d => p:%3d lc:%3d rc:%3d\n",ht[i]->id,ht[i]->id, ht[i]->w, ht[i]->parent->id, ht[i]->lchild->id,ht[i]->rchild->id); 
		else if(ht[i]->parent)
			printf("%2x %3d -> %5d => p:%3d lc:%3d rc:%3d\n",ht[i]->id,ht[i]->id+256, ht[i]->w, ht[i]->parent->id, ht[i]->lchild->id,ht[i]->rchild->id); 
		else
			printf("根节点id:%3d",ht[i]->id);

}

/*** 以下代码分析文件和从文件中生成哈夫曼编码表 ***/ 
//从filename中二进制读取字符，统计每个字符出现的次数，写入w并返回 
//w数组长度为256，如果字符不足256，那么没出现的字符权值为0  
unsigned char *parseFile(const char filename[], long *w,long *fsize){
	FILE *fp = fopen(filename,"rb");	
	if(fp == NULL){
        printf("无法打开文件!\n");
        exit(0);
    }
    fseek(fp,0,SEEK_END); 
	*fsize = ftell(fp); 
	rewind(fp);
	unsigned char *dataArray;
	dataArray = (unsigned char *)malloc(sizeof(unsigned char)*(*fsize));
	if (!dataArray) {
		printf("文件太大，内存不够，读入错误!\n");
		exit(0);
	}
	fread(dataArray,sizeof(unsigned char),*fsize,fp); //读取待压缩文件
	fclose(fp);
	
	for (int i=0;i<n;++i) //权值清空 
		w[i] = 0;
		
	for(int i=0;i<*fsize;++i) //权值+1，若对应字符出现 
		w[dataArray[i]]++;
		
	return dataArray;
}

//输入任何字符c，得到它的哈夫曼编码, 被genCodes()调用 
void getCode(HTNode *ht[], int c,unsigned *code1, int *clen1){	
	int clen = 0;        //编码长度 
	unsigned code = 0;   //编码 

	int size = sizeof(code);  //编码的最大bits数 
	//unsigned mask = 1<<(size*8-1); 
	unsigned mask = 1;//最高位为1其它为0，用来设置最高位为1 

	//int parent_id = ht[c]->parent->id;
    HTNode *par = ht[c]->parent;
    //编码规则：从根节点向下遍历，左孩子为0，右孩子为1
    //这里给定特定字符即特定节点，向根节点回溯即可
	while(par){
		code = code >> 1;//code右移一位，将原来的最高位变成次高位
		(par->lchild->id == c) ? code : (code |= mask); //如果是右孩子，那么最高位设置为1，否则就是0 
		clen++;   
		c = par->id;
		//parent_id = ht[c]->parent->id;
        par = par->parent;
        mask <<= 1;
	}

	//code的前面clen位是哈夫曼编码，将其移动到最后
	code=code >> (size*8-clen); 
	//printf("0x%x-%d\n",code,clen);
	*code1 = code;  //返回值 
	*clen1 = clen;  //返回值 
}

//从哈夫曼树ht生成完整的编码表hc，输出hc为encode()的输入参数 
void genHCodes(HCode hc[], HTNode *ht[]){
    if(!ht)
    {
        printf("Huffman树为空树!\n");
        exit(0);
    }
	for(int i=0;i<n;++i)
    {
        if (ht[i]->w > 0)
			getCode(ht,i,&(hc[i].code),&(hc[i].len));
    }	
} 

/*** 以下为编解码实现 ***/ 
//编码,原长olen的内容orgi，压缩编码后为长nlen的内容newc，需要利用编码表hc 
void encode(unsigned char *orgi, long olen, unsigned char *newc, long *nlen, HCode hc[]){
	int sizec = sizeof(unsigned int);  //要确保字符编码的最大长度要小于sizec*8-8 
	long i=0; //orgi的下标 
	long j=0; //newc的下标 
	newc[j] = 0;
	unsigned code;
	int len;
	int usedbits=0; //newc[j]中被占用的bits数 
	while (i < olen){//对原始数据逐个字符处理 
		int idx = orgi[i]; //获得hc下标
		code = hc[idx].code;
		len = hc[idx].len;//获得字符的哈夫曼编码及长度
        //Huffman编码中的每一个字符的编码并不一定是8位的，当出现无法将一个字符完整填入一个字节中时，需要将其拆分填入
		while (len+usedbits >= 8){//code需要左移usedbits长的一段，然后和newc[j]位或操作 
			unsigned code1 = code >> len+usedbits-8;
			newc[j] = newc[j] | code1; //填满前一个编码字符 
			len = len+usedbits-8;      //为填入newc的code的现在有效长度
			usedbits = 0;
			newc[++j]=0;//准备填充newc的下一个字节
			//printf("\n编码的第 %d 个字符为：#%2x\n",j-1,newc[j-1]);
		}//循环，使得len+usedbits <8
		//将code的前面各位清0,因为有效位是len
		code = code & ((1 << len) - 1); //code和后面k个1，其它全为0的数据进行与，保留后面len位 
		code = code << 8-len-usedbits;//向前移动若干位
		newc[j] = newc[j] | code;
		usedbits = len+usedbits;//更新usedbits
		i++;
	} 
	*nlen = j+1;
} 

//生成和保存压缩文件,指定文件名fout，将所用的哈夫曼树存入文件 
void zip(char fout[],HTNode *ht[],int root,HCode hc[],unsigned char *content,long fsize){
	unsigned char *zipContent; //编码后的内容	
	long zipsize;//压缩后文件大小 
	zipContent = (unsigned char *)malloc(sizeof(unsigned char)*(fsize+10000));//压缩后的文件可能更大，考虑将fsize扩大一点 
	if (!zipContent) {
		printf("文件太大，内存不够，读入错误!\n");
		exit(0);
	}
	encode(content,fsize,zipContent,&zipsize,hc); //编码后返回长度zipsize的内容zipContent 
	
	FILE *fp = fopen(fout,"wb");
	if(fp == NULL){
        printf("无法打开写入文件!\n");
        exit(0);
    }
    long ht_size = sizeof(HTNode *)*m;      //哈夫曼编码表的大小 
    fwrite(&ht_size,sizeof(ht_size),1,fp);  //保存哈夫曼树的大小
	fwrite(&zipsize,sizeof(zipsize),1,fp);  //保存编码内容的大小 
	fwrite(&fsize,sizeof(fsize),1,fp);      //保存原始内容的大小 
	fwrite(&root,sizeof(root),1,fp);        //保存哈夫曼树根节点的下标 
    fwrite(ht,sizeof(HTNode *),m,fp);       //保存哈夫曼树 ，解码要用到 
    fwrite(zipContent,sizeof(unsigned char),zipsize,fp);//保存编码后的内容 
	fclose(fp); 
	
	free(zipContent);	//释放文件内容
	printf("压缩文件 %s 已经生成！\n",fout);
}

//读取压缩文件，解压 
void unzip(char zfile[],char ofile[]){
	FILE *fp = fopen(zfile,"rb");
	if(fp == NULL){
        printf("无法打开压缩文件进行读取!\n");
        exit(0);
    }
    long ht_size1,zipsize1,fsize1;
    int root; //树的根结点下标 
    fread(&ht_size1,sizeof(ht_size1),1,fp); //哈夫曼树的大小(字节数） 
    fread(&zipsize1,sizeof(zipsize1),1,fp); //压缩后内容的大小 
    fread(&fsize1,sizeof(fsize1),1,fp);     //被压缩内容的大小 
    fread(&root,sizeof(root),1,fp);         //哈夫曼树根的下标 

	unsigned char *zcontent = (unsigned char *)malloc(zipsize1);//存放读取的编码数据 
	unsigned char *ocontent = (unsigned char *)malloc(fsize1);//解码后的数据 
	HTNode *ht1[m];
	fread(ht1,ht_size1,1,fp);//读取哈夫曼树 
	fread(zcontent,zipsize1,1,fp); //读取编码数据
	fclose(fp);

	fp = fopen(ofile,"wb");
	if(fp == NULL){
        printf("无法打开解压后文件进行解压!\n");
        exit(0);
    }
	
	int idx = root; //从根开始 
	int j,k=0,i=0;//zcontent的下标 k,ocontent的下标i 
	while (i < fsize1){//依次写入解压后数据的每个字节 
		j =128; //10000000 		
		while (j > 0){
			if ((zcontent[k] & j) > 0)
            {   //向右走 1
				if (ht1[idx]->rchild == NULL){
                    //此时解码到了下一个字符的第一个bit，它是根的右孩子
					ocontent[i++] = ht1[idx]->id;					
					idx = root;//解码了下一个字符的第一个bit 
					fprintf(fp,"%c",ocontent[i-1]);
					if (i >= fsize1) //解码结束 
						break;
					j = j << 1;//因为解码了下一个字符的第一位，所以需要从头解码下一个字符，j = j << 1,idx = root
				}
                else
					idx = ht1[idx]->rchild->id;		
			}
            else
            {   //向左走 0
				if (ht1[idx]->lchild == NULL){
                    //同上面ht1[idx]->rchild == NULL的情况
					ocontent[i++] = ht1[idx]->id;
					idx = root;//解码了下一个字符的第一个bit 
					fprintf(fp,"%c",ocontent[i-1]);
					if (i >= fsize1) //解码结束 
						break;
					j = j << 1;
				}
                else
					idx = ht1[idx]->lchild->id;	
			}
			j = j >> 1; //j控制while循环8次，因为zcontent[k]是8位的，求出zcontent[k]的每一位 
		}
		if (i >= fsize1) //解码结束 
						break; 
		k++;//准备读取下一个字符 
	}
	fclose(fp);
	free(ocontent);
	free(zcontent);
	printf("文件 %s 已经成功解压为 %s ！\n",zfile,ofile);
}

int check(char file1[], char file2[]) {
	FILE* fp1 = fopen(file1, "r"), * fp2 = fopen(file2, "r");
	unsigned fsize1, fsize2;
	fseek(fp1, 0, SEEK_END);	fseek(fp2, 0, SEEK_END);
	fsize1 = ftell(fp1);		fsize2 = ftell(fp2);
	rewind(fp1);				rewind(fp2);
	if (fsize1 != fsize2) return 0;
	char c1, c2;
	for (unsigned i = 0; i < fsize1; ++i) {
		fread(&c1, 1, 1, fp1);		fread(&c2, 1, 1, fp2);
		if (c1 != c2) return printf("at fsize = %d, c1 is %d, c2 is %d\n", i, c1, c2);
	}
	fclose(fp1);				
    fclose(fp2);
	return 1;
}

void Test(char infile[], char zfile[], char ofile[])
{
    HTNode *ht[m];   //指针数组，表示哈夫曼树 : id,w,parent,lchild,rchild，用于生成编码表和解压 
	HCode hc[n];    //结构体数组，表示哈夫曼编码表 :code,len，用于编码文件 
	
	/*** 读取文件，分析文件字符出现规律，构建哈夫曼树，生成哈夫曼编码表 ***/ 
	//char infile[]="lab6.pdf"; 
	long wDist[256]; //保存字符的分布（字符在文件中出现的次数） 
	long fsize;      //文件长度 
    //获取文件内容，分析待压缩文件，返回权值向量wDist,文件内容content和长度fsize 
	unsigned char *content = parseFile(infile,wDist,&fsize);			 
	int root = createHTree(ht,wDist); //root是哈夫曼树ht的根结点的下标 
	genHCodes(hc,ht);//生成完整的编码表,用于编码文件 
	
	//下面的代码用于测试哈夫曼树 
	//printHtree1(ht); //将哈夫曼树按数组打印出来 	
	//printHtree0(ht,root); //先序遍历哈夫曼树 
	//saveTree(*ht,root,"aa1.html"); //图形展示哈夫曼树 

	//编码，压缩文件,写入文件 fout
	//char zfile[256]="my.lzip"; 
	zip(zfile,ht,root,hc,content,fsize);

	
/*************************** 以下为解码内容 ****************************/
	//char ofile[256]="ndata.txt";
	unzip(zfile,ofile); 
	
	free(content);

    if(check(infile,ofile))
        printf("解压前后的文件相同!\n");
    else 
        printf("解压前后的文件不同!\n");

}

int main(){
	Test("lab6.pdf","zip1","ndata1.txt");
    Test("lab6_2.c","zip2","ndata2.txt");
    Test("test","zip3","ndata3.txt");
	Test("pic.png","zip4","ndata4.txt");
	//system("pause");
	return 1; 
} 

#endif

#endif