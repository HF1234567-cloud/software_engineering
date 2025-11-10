#include"model.h"
#include<string.h>
#include"global.h"
#include<stdlib.h>
#include"card_file.h"
#include<time.h>
#include <windows.h>
#include"billing_file.h"
#include"billing_service.h"

struct Card acard[50];//定义结构体数组全局变量

int ncount = 0;//定义全局变量添加的个数

pcardnode cardlist = NULL;//定义一个全局临时链表单元

int getcard();

int std[50] = { 0 };
//初始化链表
int incardlist()
{
	pcardnode head = NULL;
	head = (pcardnode)malloc(sizeof(cardnode));
	if (head != NULL)
	{
		head->next = NULL;
		cardlist = head;

		return TRUE;
	}
	free(head);
	return FALSE;
}

//释放节点空间
void freecardlist()
{
	pcardnode cur;
	if (cardlist != NULL)
	{
		cur = cardlist->next;//!!!
		while (cur != NULL)
		{
			cur = cardlist->next;
			free(cur);
			cur = NULL;
		}

	}
	else
		cur = NULL;

	free(cur);
}

  //将添加的卡信息添加到文件中
int addcard(struct Card card)
{
	
	return savecard(&card, CARDPATH);

}

//模糊查询的实现
 Card* querycard(const char* qname,int *p)
 {
	 pcardnode cur = NULL;

	 Card* pcard = (Card*)malloc(sizeof(Card));

	 if (FALSE == getcard())
	 {
		 return FALSE;
	 }
	 	 cur = cardlist->next;

	//遍历链表，节点为空则到达尾部 
	 while (cur != NULL)
	 {
		 //遍历链表查找是否包含pname字符串
		 if (strstr(cur->data.aName, qname)!=NULL)
		 {
			 pcard[*p] = cur->data;
			 (*p)++;
			 pcard = (Card*)realloc(pcard, ((*p) + 1) * sizeof(Card));
		 }
		 cur = cur->next;
	 }
	 return pcard;	
 }
 //精确查询的实现
 Card* olquerycard(const char* qname, int* p)
 {
	 pcardnode cur = NULL;

	 Card* pcard = (Card*)malloc(sizeof(Card));

	 if (FALSE == getcard())
	 {
		 return FALSE;
	 }

	 cur = cardlist->next;

	 //遍历链表，节点为空则到达尾部 
	 while (cur != NULL)
	 {
		 //遍历链表查找是否包含pname字符串
		 if (strcmp(cur->data.aName, qname) == 0)
		 {
			 pcard[*p] = cur->data;

			 (*p)++;

			 pcard = (Card*)realloc(pcard, ((*p) + 1) * sizeof(Card));
		 }
		 cur = cur->next;

	 }
	 return pcard;

 }

 //防止同用户名注册
 int nameRecheck(const char* pName)
 {
	 pcardnode cur = NULL;

	 if (FALSE == getcard())
	 {
		 return FALSE;
	 }

	 cur = cardlist->next;

	 while (cur != NULL)
	 {
		 if (strcmp(cur->data.aName, pName) == 0)
		 {
			 return TRUE;
		 }
		 cur = cur->next;
	 }

	 return FALSE;
 }


 //将文件中的卡信息保存到链表
 int getcard()
 {
	 int ncount = 0;

	 int i = 0;

	 Card* pcard = NULL;

	 pcardnode node=NULL;

	 pcardnode cur = NULL;

	 if (cardlist != NULL)
	 {
		freecardlist();
	 }
	 incardlist();

	 //获取卡信息数量
	 ncount = getcardcount(CARDPATH);

	 //动态分配内存
	 pcard = (Card*)malloc(sizeof(Card) * ncount);
	 if (pcard == NULL)
	 {
		 return FALSE;
	 }
	 //获取卡信息
	 if (FALSE == readcard(pcard, CARDPATH))
	 {
		 free(pcard);
		 pcard = NULL;
		 return FALSE;
	 }
		 for (i = 0,node=cardlist; i < ncount; i++)
	 {
		 //为节点分配内存
		 cur = (pcardnode)malloc(sizeof(cardnode));
		 //分配失败，则返回
		 if (cur == NULL)
		 {
			 free(pcard);
			 return FALSE;
		 }
		 //初始新的空间，赋值为零
		 memset(cur, 0, sizeof(cardnode));

		 //将信息保存到节点中

		 cur->data = pcard[i];
		 cur->next = NULL;

		 //将节点添加到链表尾部

		 node->next = cur;
		 node = cur;
	 }
		 free(pcard);
		 pcard = NULL;
		 return TRUE;
	 }

 

Card* checkCard(const char* pname, const char* ppwd,int* pIndex)
 {

	pcardnode cardNode= NULL;		//当前结点
	int nIndex = 0;					//匹配的卡的信息结点再链表中的序号，用于更新卡信息
									//从卡信息中读取卡信息到链表。失败返回NULL
	if (getcard() == FALSE)
	{
		return NULL;
	}
	//当前结点指向头结点
	if (cardlist != NULL) 
		cardNode = cardlist;

	while (cardNode != NULL) 
	{
		//在链表中查找是否有对应的卡信息
	if ((strcmp(cardNode->data.aName, pname) == 0) && (strcmp(cardNode->data.apwd, ppwd) == 0))
		{
			
			*pIndex = nIndex;
			return &cardNode->data;//返回卡信息节点数据地址
		}
		cardNode = cardNode->next;
		nIndex++;
	}
	return NULL;
}


int doLogon(const char* pName, const char* pPwd, LogonInfo* pInfo)
{

	pcardnode node = NULL;
	pcardnode head = NULL;
	time_t logintime;
	Billing billing;

	int nPosition = 0;		//计费信息在链表中的索引号
	//获取卡信息
	if (getcard() == FALSE)
	{
		return NULL;
	}

	node = cardlist->next;
	head = node;

	//判断能否下机
	while (node != NULL)
	{
		if (strcmp(node->data.aName, pName) == 0 && strcmp(node->data.apwd, pPwd) == 0)
		{   //状态确认
			if (node->data.status == 1)
			{
				printf("\n该卡未下机\n");
				return NULL;
			}
			if (node->data.balance < 0) {
				printf("\n该卡余额不足\n若想使用请充值\n");
				return NULL;
			}
			else if (node->data.status == 2)
			{
				printf("\n该卡已注销\n若想继续使用请重新激活\n");
				return NULL;
			}
			if (node->data.tLast - node->data.tEnd >= 0)
			{
				printf("\n该卡已过期\n若需要退款请直接注销卡\n若想继续使用请重新激活\n");
				return NULL;
			}
			//更新链表中的卡信息；
			node->data.status = 1;
			node->data.usecount++;
			node->data.tLast = time(NULL);
			//更新卡文件信息
			if (updata(head, CARDPATH) == TRUE)
			{
				//添加消费记录

				strcpy(billing.aCardName, pName);		//上机卡号
				billing.tStart = time(NULL);			//上机时间
				billing.tEnd = 0;						//下机时间
				billing.nStatus = 0;					//消费状态				
				billing.fAmount = 0;					//消费金额
				billing.nDel = 0;						//删除标识

				//先将计费信息保存到文件中
				if (TRUE == saveBilling(&billing, BILLINGPATH))
				{
					//组装上机信息
					strcpy(pInfo->aCardName, pName);
					pInfo->fBalance = node->data.balance;
					pInfo->tLogon = billing.tStart;
					return TRUE;
				}
			}
		}
		node = node->next;
	}
	printf("\n卡号或密码输入错误\n");
	return FALSE;
}

//计算消费金额
double consume(time_t tStart)
{
	double dbAmount = 0;	  // 消费金额
	int nCount = 0;			  // 上机的时间单元数，每个单元15分钟
	int nSec = 0;			  // 消费时间（单位：秒）
	int nMinutes = 0;		  // 消费时间（单位：分钟）
	time_t tEnd = time(NULL); // 结算时间为当前时间

	// 计算消费时长
	nSec = (int)(tEnd - tStart);
	nMinutes = nSec / 60;
	// 计算消费的时间单元数
	if (nMinutes % UNIT == 0)
	{

		nCount = nSec / UNIT; // 整除
		if (nCount == 0) {
			nCount = 1;
		}
	}
	else
	{
		nCount = nSec / UNIT + 1; // 不整除
	}
	system("cls");
	Sleep(200);
	// 计算消费金额
	dbAmount = nCount * CHARGE;
	return dbAmount;
}



 //下机状态更改
Card* doSettle(const char* pName, const char* pPwd, SettleInfo* pInfo)
{
	char tlogout[20] = { '\0' };
	pcardnode node = NULL;
	pcardnode head = NULL;
	time_t logintime;
	double money = 0;

	Billing* pBilling = NULL;

	int nPosition = 0;		//计费信息在链表中的索引号
	//获取卡信息
	if (getcard() == FALSE)
	{
		return NULL;
	}

	node = cardlist->next;
	head = node;

	//判断能否下机
	while (node != NULL)
	{
		if (strcmp(node->data.aName, pName) == 0 && strcmp(node->data.apwd, pPwd) == 0)
		{   //状态确认
			if (node->data.status == 0)
			{
				printf("\n该卡未上机\n");
				return NULL;
			}
			else if (node->data.status == 2)
			{
				printf("\n该卡已注销\n若想继续使用请重新激活\n");
				return NULL;
			}
			

			if (node->data.tLast - node->data.tEnd >= 0)
			{
				printf("\n该卡已过期\n若需要退款请直接注销卡\n若想继续使用请重新激活\n");
				return NULL;
			}

			//根据卡号，查询计费信息
			pBilling = queryBilling(pName, &nPosition);

			//更新链表卡信息
			node->data.status = 0;
			logintime = node->data.tLast;

			//获取此次消费金额
			money = consume(logintime);
			node->data.balance = node->data.balance - money;
			//对余额进行判断，不足则需进行充值
			if (node->data.balance < 0)
			{
				printf("\n此次消费金额：%.2lf\n\n", money);
				printf("余额不足，请先进行充值");
			}
			else
				printf("\n此次消费金额：%.2lf\n\n", money);

			//更新计费信息
			pBilling->fAmount = (float)money;		//消费信息
			pBilling->nStatus = 1;					//状态，已结算
			pBilling->tEnd = time(NULL);			//下机时间

			//更新文件中的计费信息
			if (FALSE == updataBilling(pBilling, BILLINGPATH, nPosition))
			{
				return FALSE;
			}

			//组装下机信息
			strcpy(pInfo->aCardName, pName);	//卡号
			pInfo->fAmount = (float)money;		//消费金额
			pInfo->fBalance = pInfo->fBalance-money;		//余额
			pInfo->tStart = pBilling->tStart;		//上机时间
			pInfo->tEnd = time(NULL);		//下机时间


		
			//更新卡文件信息

			if (updata(head, CARDPATH) == TRUE)
				return &node->data;
		}
		node = node->next;
	}
	printf("\n卡号或密码输入错误\n");
	return NULL;
}


	 

 //充值
 Card* doAddMoney(const char* fcardname, const char* fpassword, double money)
 {
	 pcardnode node = NULL;
	 pcardnode head = NULL;

	 //获取卡信息
	 if (getcard() == FALSE)
	 {
		 return NULL;
	 }

	 node = cardlist->next;
	 head = node;
	 while (node != NULL)                             //判断账号密码是否输入正确
	 {
		 if (strcmp(node->data.aName, fcardname) == 0&& strcmp(node->data.apwd, fpassword) == 0)
		 {   //状态确认
			 if (node->data.status == 2)
			 {
				 printf("\n该卡已注销\n若想继续使用请重新激活\n");
				 return NULL;
			 }

			 if (node->data.tLast - node->data.tEnd >= 0)
			 {
				 printf("\n该卡已过期\n若需要退款请直接注销卡\n若想继续使用请重新激活\n");
				 return NULL;
			 }
			 if (money < 0) {
				 printf("\n充值金额不能为负数\n若想继续使用请重新充值\n");
				 return NULL;
			 }
			 //更新时间
			 node->data.tLast = time(NULL);
			 //更新链表卡信息
			 node->data.balance = node->data.balance + money;
			 //更新卡文件信息
			 if (updata(head, CARDPATH) == TRUE)
				 return &node->data;
		 }
		 node = node->next;
	 }

	 printf("卡号或密码输入错误\n");
	 return NULL;
 }


 // 退费
 Card * doRefunMoney(const char* fcardname, const char* fpassword, double money)
 {
	 pcardnode node = NULL;
	 pcardnode head = NULL;

	 //获取卡信息
	 if (getcard() == 0)
		 return NULL;
	 node = cardlist->next;
	 head = node;
	 while (node != NULL)                            //判断账号密码是否输入正确
	 {
		 if (strcmp(node->data.aName, fcardname) == 0 && strcmp(node->data.apwd, fpassword) == 0)
		 {   //状态确认
			 if (node->data.status == 2)
			 {
				 printf("\n该卡已注销\n");
				 return NULL;
			 }
			 node->data.tLast = time(NULL);
			 if (node->data.tLast - node->data.tEnd >= 0)
			 {
				 printf("\n该卡已过期\n若需要退款请直接注销卡\n若想继续使用请重新激活\n");
				 return NULL;
			 }
			 if (node->data.balance < money)          //余额确认
			 {
				 printf("\n该卡余额不足\n");
				 return NULL;
			 }
			 //更新链表卡信息
			 node->data.balance = node->data.balance - money;
			 //更新卡文件信息
			 if (updata(head, CARDPATH) == TRUE)
				 return &node->data;
		 }
		 node = node->next;
	 }
	 printf("\n卡号或密码输入错误\n");
	 return NULL;
 }



  //注销卡
	 Card* doAnnul(const char* fcardname, const char* fpassword)
	 {
		 pcardnode node = NULL;
		 pcardnode head = NULL;

		 //获取卡信息
		 if (getcard() == FALSE)
		 {
			 return NULL;
		 }			
		 node = cardlist->next;
		 head = node;
		 while (node != NULL)                           //判断账号密码是否输入正确
		 {
			 if (strcmp(node->data.aName, fcardname) == 0 && strcmp(node->data.apwd, fpassword) == 0)
			 {   //状态确认
				 if (node->data.status == 2)
				 {
					 printf("\n该卡已注销\n");
					 return NULL;
				 }
				 node->data.tLast = time(NULL);
				 if (node->data.tLast - node->data.tEnd >= 0)
				 {
					 printf("\n该卡已过期\n若需要退款请直接注销卡\n");
					 return NULL;
				 }
				 printf("\n该退余额：%.2lf\n", node->data.balance);
				 //更新链表卡信息
				 node->data.balance = 0.00;
				 node->data.status = 2;
				 //更新卡文件信息
				 if (updata(head, CARDPATH) == TRUE)
					 return &node->data;
			 }
			 node = node->next;
		 }
		 printf("\n卡号或密码输入错误\n");
		 return NULL;
	 }


	 //重新激活卡
Card* doRenew(const char* fcardname, const char* fpassword)
{
		 struct tm* stime;
		 struct tm* etime;
		 pcardnode node = NULL;;
		 pcardnode head = NULL;
		 //获取卡信息
		 if (FALSE == getcard())
		 {
			 return NULL;
		 }
		 node = cardlist->next;
		 head = node;
		 while (node != NULL)
		 {   //判断账号密码是否输入正确
			 if (strcmp(node->data.aName, fcardname) == 0&&strcmp(node->data.apwd, fpassword) == 0)
			 {
				 node->data.tLast = node->data.tStart = time(NULL);
				 if ((node->data.tLast - node->data.tEnd < 0))
				 {
					 if (node->data.status == 0 || node->data.status == 1)
					 {
						 printf("\n该卡仍能使用，不需要重新激活\n");
						 return NULL;
					 }
				 }
				 //更新链表卡信息
				 node->data.tEnd = time(NULL);
				 node->data.status = 0;
				 stime = localtime(&node->data.tStart);
				 etime = localtime(&node->data.tEnd);
				 etime->tm_year = stime->tm_year + 1;
				 node->data.tEnd = mktime(etime);

				 //更新卡文件信息
				 if (updata(head, CARDPATH) == TRUE)
					 return &node->data;
			 }
			 node = node->next;
		 }
		 printf("\n卡号或密码输入错误\n");
		 return NULL;
}

void printAllCards() {
	int ncount = 0;
	Card* pcard = NULL;
	int i = 0;

	// 获取卡信息数量
	ncount = getcardcount(CARDPATH);

	// 动态分配内存
	pcard = (Card*)malloc(sizeof(Card) * ncount);
	if (pcard == NULL) {
		printf("内存分配失败！\n");
		return;
	}

	// 获取卡信息
	if (FALSE == readcard(pcard, CARDPATH)) {
		free(pcard);
		pcard = NULL;
		printf("读取卡信息失败！\n");
		return;
	}

	// 输出卡信息
	printf("卡号\t状态\t总使用金额\t使用次数\t余额\t开始时间\t\t结束时间\n");
	for (i = 0; i < ncount; i++) {
		char starttime[TIME] = { 0 };
		char endtime[TIME] = { 0 };
		char lasttime[TIME] = { 0 };

		// 时间转换为字符串
		timetostring(pcard[i].tStart, starttime);
		timetostring(pcard[i].tEnd, endtime);
		timetostring(pcard[i].tLast, lasttime);

		printf("%s\t%d\t%.1f\t\t%d\t\t%.1f\t%s\t%s\n",
			pcard[i].aName, pcard[i].status, pcard[i].totaluse,
			pcard[i].usecount, pcard[i].balance, starttime, endtime);
	}

	// 释放内存
	free(pcard);
	pcard = NULL;
}

// 统计欠费卡信息
void statisticArrears() {
	pcardnode cur = NULL;
	int hasArrearsCard = 0; // 标志变量，用于判断是否有欠费卡

	if (getcard() == FALSE) {
		return;
	}
	cur = cardlist->next;
	printf("----- 欠费卡信息统计 -----\n");
	printf("卡号\t状态\t余额\t累计使用\t使用次数\t上次使用时间\n");

	while (cur != NULL) {
		if (cur->data.balance < 0) {
			char aTime[20] = { 0 };
			timetostring(cur->data.tLast, aTime);
			printf("%s\t%d\t%0.1f\t%0.1f\t\t%d\t\t%s\n", cur->data.aName, cur->data.status, cur->data.balance, cur->data.totaluse, cur->data.usecount, aTime);
			hasArrearsCard = 1; // 找到欠费卡，设置标志为 1
		}
		cur = cur->next;
	}

	// 如果没有找到欠费卡，输出提示信息
	if (!hasArrearsCard) {
		printf("无欠费卡信息\n");
	}

	system("pause");
	system("cls");
}

// 统计注销卡信息
void statisticCancelled() {
	pcardnode cur = NULL;
	int hasCancelledCard = 0; // 标志变量，用于判断是否有注销卡

	if (getcard() == FALSE) {
		return;
	}
	cur = cardlist->next;
	printf("----- 注销卡信息统计 -----\n");
	printf("卡号\t状态\t余额\t累计使用\t使用次数\t上次使用时间\n");

	while (cur != NULL) {
		if (cur->data.status == 2) {
			char aTime[20] = { 0 };
			timetostring(cur->data.tLast, aTime);
			printf("%s\t%d\t%0.1f\t%0.1f\t\t%d\t\t%s\n", cur->data.aName, cur->data.status, cur->data.balance, cur->data.totaluse, cur->data.usecount, aTime);
			hasCancelledCard = 1; // 找到注销卡，设置标志为 1
		}
		cur = cur->next;
	}

	// 如果没有找到注销卡，输出提示信息
	if (!hasCancelledCard) {
		printf("无注销卡信息\n");
	}

	system("pause");
	system("cls");
}