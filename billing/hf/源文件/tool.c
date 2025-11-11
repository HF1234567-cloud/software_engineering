#define _CRT_SECURE_NO_DEPRECATE
#include "tool.h"
#include<stdio.h>
#include<time.h>

//将时间格式转化为年月日
void timetostring(time_t t, char* ptime)
{
	struct tm *pt;
	
	pt =  localtime(&t);
	 //声明time_t类型变量
    //获取系统日期和时间
   
	strftime(ptime, 20, "%Y-%m-%d %H:%M",pt);
}

time_t  stringtotime(char* pTime)
{
	struct tm tml;
	time_t   time1;

	sscanf(pTime, "%d-%d-%d %d:%d", &tml.tm_year, &tml.tm_mon,&tml.tm_mday, &tml.tm_hour, &tml.tm_min);

	tml.tm_year -= 1900;  //年份为从1900年开始
	tml.tm_mon -= 1;      //月份为1^11
	tml.tm_sec = 0;
	tml.tm_isdst -= -1;

	time1 = mktime(&tml);

	return time1;
}

//获得time中的年份
int timeToYear(time_t t)
{
	struct tm* timeinfo;
	timeinfo = (struct tm*)malloc(sizeof(struct tm));
	timeinfo = localtime(&t);
	return timeinfo->tm_year + 1900;
}


//[得到time_t所表示的月份
int timeToMonth(time_t t) 
{
	struct tm* timeinfo;
	timeinfo = (struct tm*)malloc(sizeof(struct tm));
	timeinfo = localtime(&t);
	return timeinfo->tm_mon + 1;
}

