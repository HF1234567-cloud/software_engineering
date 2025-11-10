#include"global.h"
#include"model.h"
#ifndef CARD_SEVICE_H
#define CARD_SEVICE_H
int addcard(struct Card card);													//保存添加卡信息到结构体数组
int getcard();																	//获得需要卡信息
void  freecardlist();
int nameRecheck(const char* pName);
Card* olquerycard(const char* qname, int* p);									//精确查询
Card* querycard(const char* qname, int* p);										//使用链表查询卡
Card* checkCard(const char* pname, const char* ppwd, int* pIndex);
Card* doSettle(const char* acardname, const char* apassword,SettleInfo* pInfo);	//下机实现
Card* doAddMoney(const char* fcardname, const char* fpassword, double money);   //充值实现
Card* doRefunMoney(const char* fcardname, const char* fpassword, double money); //退费实现
Card* doAnnul(const char* fcardname, const char* fpassword);					//注销卡
Card* doRenew(const char* fcardname, const char* fpassword);
double consume(time_t tstart);													//重新激活
void printAllCards();                                                          // 遍历所有卡信息并输出
void statisticCancelled();                                                     //统计注销卡信息
void statisticArrears();                                                       // 统计欠费卡信息
extern std[50];																	//全局标准数组

#endif