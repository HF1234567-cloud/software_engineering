#include<stdio.h>
#include<time.h>
#include<windows.h>
#include"tool.h"
#include"global.h"
#include"model.h"
#include"billing_service.h"



//统计每月上机获取金额
void statisticMonth() 
{
	int nMonth, nYear;
	float fTotalUse = 0.0;
	time_t Now = time(NULL);	//当前时间

	printf("请输入需要统计的年份月份(以结束时间为准，用空格隔开)：");
	scanf("%d %d", &nYear, &nMonth);
	system("cls");

	//判断输入时间是否符合要求
	if (nMonth > 12 || nMonth < 1 || nYear > timeToYear(Now)) 
	{
		printf("输入的需统计时间错误！\n");
		Sleep(1000);
		system("cls");
		return;
	}

	if (doStatisticMonth(nYear, nMonth, &fTotalUse)==FALSE) //函数只改变并返回该月份中的总营业额
	{
		printf("统计失败！\n");
		system("pause");
		system("cls");
		return;
	}

	//输出
	printf("查询到的信息如下：\n%d年%d月的总使用金额为：%5.2f\n", nYear, nMonth, fTotalUse);
	system("pause");
	system("cls");
	return;
}


//统计一个用户上机数据
void statisticUser() 
{
	char aName[18];		//用于存放用户名称；
	char aStartTime[30] = { 0 };		//存放开始时间
	char aLastTime[30] = { 0 };		//存放结束时间
	float sum = 0;		//统计数目
	lpBillingNode node = NULL;
	printf("请输入需要查询的用户卡号：");
	scanf("%s", aName);
	system("cls");
	if (FALSE == nameRecheck(aName))
	{
		printf("卡号输入错误\n");
		printf("请重新输入！\n");
		system("pause");
		system("cls");
		return;
	}
	if (FALSE == doStatisticUser(aName, &node))
	{
		printf("查询错误！\n");
		system("pause");
		system("cls");
		return;
	}


	else 
	{
		printf("卡号%s的使用信息如下：\n", aName);		//输出表头
		printf("--------------------------------------------------------------------------------------------\n");
		printf("上机时间                      下机时间                      使用金额       是否结算  是否删除\n");
		while (node->next != NULL) 
		{
			node = node->next;
			timetostring(node->data.tStart, aStartTime);
			printf("%s", aStartTime);

			if (node->data.tEnd == 0) //如果tEnd为0，证明未下机，执行输出未下机操作
			{		
				printf("未下机                        ");
			}
			else 
			{
				timetostring(node->data.tEnd, aLastTime);
				printf("%30s", aLastTime);
			}
			printf("%20.2f", node->data.fAmount);
			printf("%10d%10d\n", node->data.nStatus, node->data.nDel);

			sum=sum+node->data.fAmount;
		}
		printf("\n该卡消费金额为%.2f\n",sum );
		system("pause");
		system("cls");
		return;
	}
}

// 按年（月）统计开卡情况
void statisticCardOpening()
{
	int nMonth, nYear;
	time_t Now = time(NULL);  // 当前时间

	// 获取用户输入的年份和月份
	printf("请输入需要统计的年份月份(以结束时间为准，用空格隔开)：");
	if (scanf("%d %d", &nYear, &nMonth) != 2) {
		printf("输入格式错误，请输入有效的年份和月份！\n");
		while (getchar() != '\n'); // 清除输入缓冲区
		Sleep(1000);
		system("cls");
		return;
	}
	system("cls");

	// 判断输入时间是否符合要求
	if (nMonth > 12 || nMonth < 1 || nYear > timeToYear(Now)) {
		printf("输入的需统计时间错误！\n");
		Sleep(1000);
		system("cls");
		return;
	}

	// 获取卡信息数量
	int nCount = getcardcount(CARDPATH);
	if (nCount == FALSE) {
		printf("获取卡信息数量失败！\n");
		return;
	}

	// 动态分配内存
	Card* pcard = (Card*)malloc(sizeof(Card) * nCount);
	if (pcard == NULL) {
		printf("内存分配失败，无法统计开卡情况。\n");
		return;
	}

	// 从文件中读取卡信息
	if (readcard(pcard, CARDPATH) == FALSE) {
		printf("读取卡信息文件失败！\n");
		free(pcard);
		return;
	}

	int openingCount = 0;  // 开卡数
	float totalAmount = 0; // 总充值金额

	// 输出表头
	printf("--------------------------------------------------------------------------------------------\n");
	printf("--------------------------------%d年%d月开卡情况统计表-------------------------------------\n", nYear, nMonth);
	printf("账号\t\t\t开卡时间\t\t首次充值金额 \n");

	// 遍历卡信息，统计符合条件的开卡记录
	for (int i = 0; i < nCount; i++) {
		struct tm* tm_info = localtime(&pcard[i].tStart);
		if (tm_info->tm_year + 1900 == nYear && tm_info->tm_mon + 1 == nMonth) {
			char startTime[30];
			timetostring(pcard[i].tStart, startTime);
			printf("%-16s\t%s\t%.2f\n", pcard[i].aName, startTime, pcard[i].totaluse);
			openingCount++;
			totalAmount += pcard[i].totaluse;
		}
	}

	// 输出汇总信息

	printf("合计\t\t开卡数：%d\t\t金额：%.2f\n", openingCount, totalAmount);
	

	// 释放动态分配的内存
	free(pcard);
}
