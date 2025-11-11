#include<stdio.h>
#include"menu.h"
#include<stdlib.h>
#include <windows.h>


int main()
{
    printf("             ---------------欢迎使用计费管理系统-----------\n");
    int nSelection;
    do {
        //输出主界面
        outputmenu();
        scanf("%d", &nSelection);
        system("cls");
        //指令判断
        switch (nSelection)
        {
        case 1:
            printf("【1】添加卡\n");
            add();
            printf("\n");
            system("pause");//暂停
            system("cls");
            break;

        case 2:
            printf("【2】查询卡\n");
            query();
            printf("\n");
            system("pause");//暂停
            system("cls");
            break;
        case 3:
            printf("【3】上机功能\n");
            logon();
            printf("\n");;
            system("pause");//暂停
            system("cls");
            break;
        case 4:
            printf("【4】下机\n");
            settle();
            printf("\n");
            system("pause");//暂停
            system("cls");
            break;
        case 5:
            printf("【5】充值\n");
            addMoney();
            printf("\n");
            system("pause");//暂停
            system("cls");
            break;
        case 6:
            printf("【6】退费\n");
            refundMoney();
            printf("\n");
            system("pause");//暂停
            system("cls");
            break;
        case 7:
            printf("【7】注销卡\n");
            annul();
            printf("\n");
            system("pause");//暂停
            system("cls");
            break;
        case 8:
            printf("【8】激活卡");
            renew();
            printf("\n");
            system("pause");//暂停
            system("cls");
            break;
        case 9:
            printf("【9】统计");
            printf("\n");
            statistic();
            printf("\n");
            system("pause");//暂停
            system("cls");
            break;
        case 10:
            printf("【10】显示卡信息\n");
            printAllCard();
            printf("\n");
            system("pause");// 暂停
            system("cls");
            break;
        case 0:
            exitapp();
            printf("谢谢使用 !__!  ");
            system("pause");//暂停
            system("cls");
            return 0;
        
        default:
            printf("无法识别的功能\n");
            printf("\n");
            system("pause");//暂停
            system("cls");

            break;

        }
    } while (nSelection != 0);
    //无条件循环

    return 0;
}