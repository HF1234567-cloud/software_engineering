#include<stdio.h>
#include"global.h"
#include"model.h"
#include"card_sevice.h"
#include"tool.h"
#include<string.h>
#include<stdlib.h>
#include<windows.h>
#include"statistic.h"


//主界面渲染
void print()
{

	printf("	      ------ 计费管理系统启动成功 -------  \n");
	Sleep(70);
	printf("	       .-------.       ____     __        \n");
	Sleep(70);
	printf(" 	      |  _ _   \\      \\   \\   /  /    \n");
	Sleep(70);
	printf("	      | ( ' )  |       \\  _. /  '       \n");
	Sleep(70);
	printf(" 	      |(_ o _) /        _( )_ .'         \n");
	Sleep(70);
	printf("	      | (_,_).' __  ___(_ o _)'          \n");
	Sleep(70);
	printf(" 	      |  |\\ \\  |  ||   |(_,_)'         \n");
	Sleep(70);
	printf(" 	      |  | \\ `'   /|   `-'  /           \n");
	Sleep(70);
	printf("	      |  |  \\    /  \\      /           \n");
	Sleep(70);
	printf("	       ''-'   `'-'    `-..-'              \n");
	Sleep(70);
	return 0;
}


void outputmenu()
{
	print();
	Sleep(100);
	system("cls");


	//输出系统菜单
	printf("			 ------计费系统菜单-----\n");
	Sleep(30);
	printf("			*      1. 添加卡\t*\n");
	Sleep(30);
	printf("			*      2. 查询卡\t*\n");
	Sleep(30);
	printf("			*      3. 上机\t\t*\n");
	Sleep(30);
	printf("			*      4. 下机\t\t*\n");
	Sleep(30);
	printf("			*      5. 充值\t\t*\n");
	Sleep(30);
	printf("			*      6. 退费\t\t*\n");
	Sleep(30);
	printf("			*      7. 注销	\t*\n");
	Sleep(30);
	printf("			*      8. 激活	\t*\n");
	Sleep(30);
	printf("			*      9. 统计	\t*\n");
	Sleep(30);
	printf("			*      10. 显示卡信息\t*\n");
	Sleep(30);
	printf("			*      0. 退出\t\t*\n");
	Sleep(30);
	printf("			 -----------------------\n\n");
	Sleep(30);
	printf("请输入 0-10 数字来进行选择:");
}


//添加卡
void add()//添加获取卡信息
{
	struct Card card;
	
	char aName[50]={'\0'};
	char apwd[20]={'\0'};

	int namesize;
	int pwdsize;
	

	//初始化时间
	struct  tm* startTime;//开卡时间
	struct tm* endTime;//截止时间
	//开卡时间，截止时间和最后使用时间都默认为当前的系统时间
	card.tStart = card.tEnd = card.tLast = time(NULL);
	startTime = localtime(&card.tStart);
	endTime = localtime(&card.tEnd);
	endTime->tm_year = startTime->tm_year + 1;
	card.tEnd = mktime(endTime);//将时间转化为time_t类型


	printf("请输入卡号<长度为0~18>:");

	scanf_s("%s", aName,50);
	if (nameRecheck(aName) == TRUE)
	{
		printf("账号已注册，请重新注册!!!");
		return TRUE;
	}
	//密码账号放入结构体
	strcpy(card.aName, aName);
	
	

	printf("请输入密码<长度为0~8>：");

	scanf_s("%s", apwd,20);
	strcpy(card.apwd, apwd);


	//检查是否超出
	pwdsize = getsize(apwd);

	namesize = getsize(aName);

	if (namesize > 18 || pwdsize > 8)
	{
		printf("账号或密码输入有误,请重新输入!!!\n");

		return;
	}

	printf("请输入开卡金额:");

	scanf_s("%f", &card.balance);

	card.totaluse = card.balance;

	card.usecount = 0;

	card.status = 0;

	card.del = 0;

	
	//输出卡状态

	printf("\n");

	printf("----------添加卡信息如下-------\n");

	printf("卡号\t密码\t卡状态\t余额\t\n");

	printf("%s\t%s\t%d\t%0.1f\t\n", card.aName, card.apwd, card.status, card.balance);
	
	addcard(card);

}


int getsize(const char* p)//计算输入账号密码长度的函数
{
	int n=0;
	while (*(p + n) != '\0')
	{
		n++;
	}

	return n;
}


void query()//查询卡的初始函数
{
	struct Card* qcard=NULL;

	char aName[18] = { 0 };
	char aTime[20] = { 0 };
	int n = 0;
	int i = 0;
	int nslection = 0;
	printf("[1]   模糊查询\n");
	printf("[2]   精确查询\n");
	printf("请输入查询方式: ");
	scanf("%d", &nslection);
	switch (nslection)
	{
	case 1:
		printf("请输入查询卡号：");
		scanf("%s", aName);
		qcard = querycard(aName,&n);
		break;
	case 2:
		printf("请输入查询卡号：");
		scanf("%s", aName);
		qcard = olquerycard(aName, &n);
		break;
	}
	
	if (qcard == NULL || n == 0)
	{
		printf("不存在该卡的信息!!!");
	}
	//将时间转换为字符串
	

	for (i = 0; i < n; i++)
	{
	//输出查询到的卡
	printf("卡号\t卡状态\t余额\t累计使用\t使用次数\t上次使用时间\t\n");

	timetostring(qcard[i].tLast, aTime);

	printf("%s\t%d\t%0.1f\t%0.1f\t\t%d\t\t%s\n", qcard[i].aName, qcard[i].status, qcard[i].balance, qcard[i].totaluse, qcard[i].usecount, aTime);

	}
}

//上机
void logon()
{
	char aName[18] = { 0 };
	char apwd[8] = { 0 };
	Card* pcard = NULL;
	char lasttime[TIME] = { 0 };
	LogonInfo* pInfo = NULL;
	int nResult = 0;

	memset(aName, 0, sizeof(aName));
	memset(apwd, 0, sizeof(apwd));
	memset(lasttime, 0, sizeof(lasttime));

	//提示并且接收用户输入卡号和密码
	printf("请输入上机卡号（长度为1-17）：");
	scanf("%s", aName);

	//判断输入的卡号是否符合要求
	if (strlen(aName) >= 18)
	{
		printf("输入卡号长度超过最大值！\n");

	}

	//提示并接受上机密码；
	printf("请输入上机密码（长度为1-7）:");
	scanf("%s", apwd);


	//判断输入的卡号密码是否符合要求
	if (strlen(apwd) >= 8)
	{
		printf("输入卡号长度超过最大值！\n");
	}

	//开始上机，获取上机信息
	pInfo = (LogonInfo*)malloc(sizeof(LogonInfo));

	//根据上机的结果，提示输出不同信息
	nResult = doLogon(aName, apwd, pInfo);
	switch (nResult)
		{
	case 0:
		printf("------------上机失败！------------\n"); break;
	case 1:
		printf("-----******-----上机信息如下-----******-----\n");
		printf("卡号              余额              上机时间\n");		//18 18

		timetostring(pInfo->tLogon, lasttime);

		//输出信息
		printf("%s", pInfo->aCardName);
		printf("%20.2f", pInfo->fBalance);
		printf("%30s\n", lasttime);
		printf("上机成功！\n");
		break;
	case 2:
		printf("------------该卡不能使用！------------\n"); break;
	case 3:
		printf("------------余额不足！------------\n"); break;
	}
	free(pInfo);
}


	


//下机
void settle()
{
	Card* acard = NULL;
	SettleInfo* pInfo = NULL;		//下机信息
	char tlogout[20] = { '\0' };
	char acardname[18] = { '\0' };
	char apwd[8] = { '\0' };

	//接收卡号和密码
	printf("请输入下机卡号：");
	scanf("%s", &acardname);
	printf("请输入密码：");
	scanf("%s", &apwd);
	//为下机信息动态分配内存
	pInfo = (SettleInfo*)malloc(sizeof(SettleInfo));

	acard = doSettle(acardname, apwd, pInfo);  //下机状态更改

	if (acard == NULL)
		printf("\n下机失败!\n");
	else
	{
		printf("卡号：%s\n", acard->aName);
		printf("余额：%.2f\n", acard->balance);
		timetostring(acard->tLast, tlogout);
		printf("下机时间：%s\n", tlogout);
		printf("\n下机成功!\n");
	}
}

//充值
void addMoney()
{
	Card* acard = NULL;
	double money;
	char fcardname[18] = { '\0' };
	char fpassword[8] = { '\0' };
	printf("请输入卡号<长度为1-18>：");
	scanf("%s", &fcardname);           //接收卡号和密码
	printf("请输入密码<长度为1-8>：");
	scanf("%s", &fpassword);
	printf("请输入充值金额<RMB>：");	//该循环用来解决输入字符(串)后会进入死循环的问题，与main函数中用法相同
	while (scanf("%lf", &money) != 1) 
	{
		printf("您输入的充值金额有误。(请输入整数或小数)!!!\n");
		printf("请重新输入:\n");
		while (getchar() != '\n');
	}

	acard = doAddMoney(fcardname, fpassword, money);

	if (acard == NULL)
		printf("\n充值失败\n");
	else
	{
		printf("\n充值成功\n");
		printf("卡号：%s\n", acard->aName);
		printf("余额：%.2f\n", acard->balance);
	}
}


//退费
void refundMoney() {
	Card* acard = NULL;
	double money;
	char fcardname[18] = { '\0' };
	char fpassword[8] = { '\0' };
	printf("请输入卡号<长度为1-18>：");
	scanf("%s", &fcardname);
	printf("请输入密码<长度为1-8>：");
	scanf("%s", &fpassword);
	printf("请输入退费金额<RMB>：");
	//该循环用来解决输入字符(串)后会进入死循环的问题，与main函数中用法相同
	while (scanf("%lf", &money) != 1) {
		printf("您输入的退费金额有误。(请输入整数或小数)\n");
		printf("请重新输入:\n");
		while (getchar() != '\n');
	}

	acard = doRefunMoney(fcardname, fpassword, money);
	if (acard == NULL)
		printf("\n退费失败\n");
	else
	{
		printf("\n退费成功\n");
		printf("卡号：%s\n", acard->aName);
		printf("余额：%.2f\n", acard->balance);
	}

}

//注销卡
void annul() 
{
	Card* acard = NULL;
	int n = 0;
	char fcardname[18] = { '\0' };
	char fpassword[8] = { '\0' };
	printf("请输入卡号<长度为1-18>：");
	scanf("%s", &fcardname);
	printf("请输入密码<长度为1-8>：");
	scanf("%s", &fpassword);

	acard = doAnnul(fcardname, fpassword);
	if (acard == NULL)
		printf("\n注销失败\n");
	else
		printf("\n注销成功\n");


}//重新激活卡
void renew()
{
	Card* acard = NULL;
	char tend[20] = { '\0' };
	int n = 0;
	char fcardname[18] = { '\0' };
	char fpassword[8] = { '\0' };
	printf("请输入卡号<长度为1-18>：");
	scanf("%s", &fcardname);
	printf("请输入密码<长度为1-8>：");
	scanf("%s", &fpassword);
	acard = doRenew(fcardname, fpassword);
	if (acard == NULL)
		printf("\n重新激活失败\n");
	else
	{
		printf("\n重新激活成功\n");
		printf("卡号：%s\n", acard->aName);
		timetostring(acard->tEnd, tend);
		printf("卡截止时间已更改为：%s\n", tend);
	}
}




//统计
void statistic()
{
	int nSelection = -1;
	printf("（1）.按月份统计用户使用总营业额\n");
	printf("（2）.按用户卡号统计该用户使用金额\n");
	printf("（3）.按月份统计该月的开卡情况\n");
	printf("（4）.返回\n");
	printf("请选择需要进行的统计操作：");

	//读取用户操作
	scanf("%d", &nSelection);
	system("cls");

	switch (nSelection) 
	{
	case 4: 
	{
		return;
	}
	case 1: 
	{
		statisticMonth();
		break;
	}
	case 2:
	{
		statisticUser();
		break;
	}
	case 3:
	{
		statisticCardOpening();
		break;
	}
	
	default:
	{
		printf("输入操作错误！\n");
		Sleep(1000);
		return;
	}
	}
}

void exitapp()//退出函数的定义
{
	freecardlist();
}

void printAllCard() {
	int nSelection;
	printf("（1）.显示所有卡信息\n");
	printf("（2）.显示欠费卡信息\n");
	printf("（3）.显示注销卡信息\n");
	printf("（4）.返回\n");
	printf("请选择需要进行的统计操作：");

	//读取用户操作
	scanf("%d", &nSelection);
	system("cls");

	switch (nSelection)
	{
	case 4:
	{
		return;
	}
	case 1:
	{
		printAllCards();
		break;
	}
	case 2:
	{
		statisticArrears();
		break;
	}
	case 3:
	{
		statisticCancelled();
		break;
	}
	default:
	{
		printf("输入操作错误！\n");
		Sleep(1000);
		return;
	}
	}

}