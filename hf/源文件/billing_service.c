#include"model.h"
#include<stdio.h>
#include<time.h>
#include<string.h>
#include<stdlib.h>
#include"global.h"
#include"model.h"
#include"tool.h"
#include"Billing_file.h"


lpBillingNode billingList = NULL;

int addBilling(const char* pName, Billing* pBilling)

{
	//将消费信息写入结构体
	strcpy(pBilling->aCardName, pName);		//上机卡号
	pBilling->tStart = time(NULL);		//上机时间
	pBilling->tEnd = 0;				//下机时间，若未下机，则标注为0；
	pBilling->fAmount = 0;		//消费金额
	pBilling->nStatus = 0;		//消费状态	0未结算1结算
	pBilling->nDel = 0;		//删除状态 0未删1删

	strcpy(pBilling->aCardName, pName);//上机卡号


	if (FALSE == saveBilling(pBilling, BILLINGPATH))
	{
		return FALSE;

	}
	return TRUE;
}

//初始化信息记录链表
void initBillingList() 
{
	lpBillingNode head = NULL;
	if (billingList == NULL) 
	{
		head = (lpBillingNode)malloc(sizeof(BillingNode));
		head->next = NULL;
		billingList = head;
	}
}


//释放链表空间
void releaseBillingList() 
{
	lpBillingNode cur = billingList;
	lpBillingNode next = NULL;
	//销毁链表
	while (cur != NULL)
	{
		next = cur->next;		//结点内存释放前，next保存其后继
		free(cur);
		cur = next;
	}
	billingList = NULL;
}



//获取文件中的记录信息
int getBilling() 
{
	int i = 0;
	int nCount = 0;		//实际的计费信息数
	Billing* pBilling = NULL;		// 读取到的一条计费信息
	lpBillingNode node = NULL;		//当前的尾节点
	lpBillingNode cur = NULL;		//添加到链表中的节点

	//清空链表中已经存在的数据
	if (billingList != NULL) {
		releaseBillingList();
	}
	//初始化链表
	initBillingList();

	//获取文件中计费信息个数
	nCount = getBillingCount(BILLINGPATH);

	//动态分配内存用来保存所有的计费信息，相当于结构体数组，pCard相当于数组名

	pBilling = (Billing*)malloc(sizeof(Billing) * nCount);
	if (pBilling != NULL) 
	{
		//如果返回FALSE则读取失败
		if (readBilling(pBilling, BILLINGPATH) == FALSE)
		{
			free(pBilling);
			return FALSE;
		}

		//将读取的信息，保存到链表中
		for (i = 0, node = billingList; i < nCount; i++) {
			//为节点分配内存
			cur = (lpBillingNode)malloc(sizeof(BillingNode));
			//如果分配内存失败，则返回
			if (cur == NULL) {
				free(pBilling);
				return FALSE;
			}
			//初始化新的空间，全部赋值为0；
			memset(cur, 0, sizeof(BillingNode));

			//将数据保存到链表中
			cur->data = pBilling[i];
			cur->next = NULL;

			//将结点添加到链表的尾部
			node->next = cur;
			node = cur;
		}
		//释放内存
		free(pBilling);
		return TRUE;
	}
	return FALSE;
}

//找到指定卡号且未结算的信息
Billing* queryBilling(const char* pName, int* pIndex)
{
	lpBillingNode node = NULL;
	int nIndex = 0;

	//从信息文件读取信息列表
	if (getBilling() == FALSE)
	{
		return NULL;

	}

	//node指向第一个数据，开始遍历
	node = billingList;//头数据

	//遍历链表。到NULL时停止
	while (node != NULL)
	{
		//确定信息与查找情况符合
		if (strcmp(node->data.aCardName, pName) == 0 && node->data.nStatus == 0)
		{
			//卡号正确并且没有结算信息
			return &node->data;
		}
		//移动下一个节点
		node = node->next;
		nIndex++;
		*pIndex = nIndex;
	}
	return NULL;
}



//对确定月份内数据进行统计
int doStatisticMonth(const int nYear, const int nMonth, float* fTotalUse) 
{
	lpBillingNode node = NULL;
	*fTotalUse = 0.0;	//初始化金额
	char aName[18];		//用于存放用户名称；
	char aStartTime[30] = { 0 };		//存放开始时间
	char aLastTime[30] = { 0 };		//存放结束时间

	//从计费信息文件中读取计费信息列表，失败返回NULL
	if (getBilling() == FALSE) 
	{
		return FALSE;
	}

	//node指向第一个数据，准被开始遍历
	node = billingList;		//第一个数据
	printf("--------------------------------------------------------------------------------------------\n");
	printf("-----------------------------%d年%d月上机下机消费情况统计表--------------------------------\n",nYear,nMonth);
	printf("账号       上机时间                      下机时间                 使用金额 \n");
	while (node != NULL) 
	{

		if ((node->data.nStatus == 1) && (timeToYear(node->data.tEnd) == nYear) && (timeToMonth(node->data.tEnd) == nMonth)) 
		{
			printf("%s", node->data.aCardName);
			//已缴费并且以最后下机时间来计算消费金额
			(*fTotalUse) = (*fTotalUse) + node->data.fAmount;
		
			//进行判断后输出每一条满足的信息
			timetostring(node->data.tStart, aStartTime);
			printf("%20s", aStartTime);

			if (node->data.tEnd == 0) //如果tEnd为0，证明未下机，执行输出未下机操作
			{
				printf("未下机                        ");
			}
			else
			{
				timetostring(node->data.tEnd, aLastTime);
				printf("%30s", aLastTime);
			}
			printf("%20.2f\n", node->data.fAmount);
	

		}
		node = node->next;
	}
	return TRUE;
}


//统计一位用户的使用数据

int doStatisticUser(const char* aName, lpBillingNode* curHead) 
{

	int i = 0;
	int nCount = 0;		//实际的计费信息数
	Billing* pBilling = NULL;		// 读取到的一条计费信息
	lpBillingNode node = NULL;		//当前的尾节点
	lpBillingNode cur = NULL;		//添加到链表中的节点

	//清空链表中已经存在的数据
	if (billingList != NULL)
	{
		releaseBillingList();
	}
	//初始化链表
	initBillingList();

	//获取文件中计费信息个数
	nCount = getBillingCount(BILLINGPATH);

	//动态分配内存用来保存所有的计费信息，相当于结构体数组，pCard相当于数组名
	pBilling = (Billing*)malloc(sizeof(Billing) * nCount);

	if (pBilling != NULL) 
	{
		//如果返回FALSE则读取失败
		if (readBilling(pBilling, BILLINGPATH) == FALSE) 
		{
			free(pBilling);
			return FALSE;
		}

		//将读取的信息，保存到链表中
		for (i = 0, node = billingList; i < nCount; i++) 
		{
			if (strcmp(pBilling[i].aCardName, aName) == 0) 
			{		
				//为节点分配内存
				cur = (lpBillingNode)malloc(sizeof(BillingNode));
				//如果分配内存失败，则返回
				if (cur == NULL) {
					free(pBilling);
					return FALSE;
				}
				//初始化新的空间，全部赋值为0；
				memset(cur, 0, sizeof(BillingNode));

				//将数据保存到链表中
				cur->data = pBilling[i];
				cur->next = NULL;

				//将结点添加到链表的尾部
				node->next = cur;
				node = cur;
			}
		}
		*curHead = billingList;
		//释放内存
		free(pBilling);
		return TRUE;
	}
	return FALSE;
}