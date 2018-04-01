
#ifndef __BIN_TREE__
#define __BIN_TREE__

typedef long		int8b;
typedef int		int4b;
typedef unsigned long	uint8b;
typedef unsigned int	uint4b;
typedef unsigned char	uint1b;

typedef long	LONGINT;
typedef long	POINTER;

#ifndef FOUND
#define FOUND		1
#endif
#ifndef NOTFOUND
#define NOTFOUND	0
#endif

#ifndef TRUE
#define TRUE		1
#endif

#ifndef FALSE
#define FALSE		0
#endif


typedef struct __List__
{
	struct __List__ *pNext;
} LIST;


/*��������ĸ���*/
int CountList(LIST* ptHead);

/*����operate_list�ǶԵ����ڵ�Ĳ�������*/
/*�ú��������ÿ���ʵ�ִ�ӡ�ͷ��ڴ�Ȳ���*/
int TravelList(LIST *ptHead,int (*operate_list)(LIST *));


/*ֱ�ӽ����ݲ���ͷ�ڵ�*/
void InsertList(LIST **pptHead,LIST *p);
void InsertListTail(LIST **pptHead,LIST *p);
void AppendListTail(LIST **pptHead,LIST *p);


/*����comp_list�ǶԲ���ľ������,����������,ֻ����*/
/*comp_list����1������������2�����ڵ����� 	   */
/*�Բ���Ľڵ��pNextû��Ҫ��(�����ڲ��Զ���λ)    */
/*�ú�����LIST *p �������ⲿ������ڴ� 			*/
void InsertListSort(LIST **pptHead,LIST *p,int (*comp_list)(LIST *,LIST *));


/*����comp_list�ǶԲ���ľ���Ƚϲ���,����������,ֻ����*/
/*comp_list����1������������2�����ڵ����� 	    */
/*sum_list ����1���ۼӵ��ⲿ���ݣ�����2�����ڵ����� */
/*�Բ���Ľڵ��pNextû��Ҫ��(�����ڲ��Զ���λ)     */
/*�ú�����LIST *p �������ⲿ������ڴ� */
/*����ֵ
	FALSE:û��SUM�����е������Ա���ⲿ�ڴ汻������ȥ
	TRUE :�ò�����ֵ��SUM�����е��ڴ�LIST�У��ڴ���ظ�����
*/
int InsertListSumm(LIST **pptHead,LIST *p,
      int (*comp_list)(LIST *,LIST *),
      void (*sum_list)(LIST *,LIST *));
int InsertListSortSum(LIST **pptHead,LIST *p,
      int (*comp_list)(LIST *,LIST *),
      void (*sum_list)(LIST *,LIST *));

/*���� *pi��Ҫ���ҵ�����*search_list�ǲ�ѯƥ������*/
/*search_list����1�������ڵĵ���ITEM������2���ⲿ��������� */
int SearchList(LIST *ptHead,void *pi,LIST **p,int (*comp_list)(void *,LIST *));


/*���� *pҪɾ���ڵ��ָ��*/
/*������ֵ��
	TRUEɾ���� FALSEδɾ��*/
int DeleteList(LIST **pptHead,LIST *p);

void DestroyList(LIST *p);
void FreeList(LIST *p);
void ReverseList(LIST **pptHead);
void assign_insert_list(void **ppData,void *pData);

#ifndef HIGHER
#define HIGHER 1 
#endif
#ifndef LOWER
#define LOWER  -1
#endif
#ifndef EQUAL
#define EQUAL  0 
#endif

typedef struct __Bin_Tree__
{
	struct __Bin_Tree__ *pLeft;
	struct __Bin_Tree__ *pRight;
	void   *pData;
	int    diff;
}   BINTREE;
void LinkBin(BINTREE *ptHead,void (*link_node_v)(void **,void **));
void HandShakeBin(BINTREE *ptHead);
void AdjustBin(BINTREE **pptHead);
int4b InsertBin(BINTREE **pptHead, void *pValue, 
        int4b ( *pFunctionExt)(void *,void*), 
        void ( *pAssign)(void **,void *));
BINTREE * MallocBin();
int4b SearchBin(BINTREE *ptHead, void *pValue,
	int ( *pFunction)(void *,void *), void **pptCur);
int4b DestroyBin(BINTREE *ptHead);
int TravelBin(BINTREE *ptHead, int ( *pOperation)(void **));
int4b DeleteBinMin(BINTREE **pptHead, BINTREE **pptCur);
int4b DeleteBin(BINTREE **pptHead, void *p,
	int4b ( *pFunction)(void *,void *),BINTREE **pptCur);
void assign_insert_bintree(void **ppData,void *pData);


#endif

