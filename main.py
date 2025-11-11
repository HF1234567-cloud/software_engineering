# -*- coding: utf-8 -*-
"""
银行卡管理系统主程序 - 改进版v1.0
支持用户端和管理员端分离
"""
import os
import time
from user_menu import UserMenu
from admin_menu import AdminMenu


def clear_screen():
    """清屏"""
    os.system('cls' if os.name == 'nt' else 'clear')


def show_welcome():
    """显示欢迎界面"""
    clear_screen()
    print("\n")
    print("    ╔════════════════════════════════════════════════════╗")
    print("    ║                                                    ║")
    print("    ║          欢迎使用银行卡管理系统                    ║")
    print("    ║                                                    ║")
    print("    ║          Bank Card Management System               ║")
    print("    ║                                                    ║")
    print("    ║                                                    ║")
    print("    ╚════════════════════════════════════════════════════╝")
    print("\n")
    time.sleep(1)


def show_login_menu():
    """显示登录方式选择菜单"""
    clear_screen()
    print("\n")
    print("    ╔════════════════════════════════════════════════════╗")
    print("    ║              请选择登录方式                        ║")
    print("    ╠════════════════════════════════════════════════════╣")
    print("    ║  1. 用户登录                                       ║")
    print("    ║  2. 管理员登录                                     ║")
    print("    ║  0. 退出系统                                       ║")
    print("    ╚════════════════════════════════════════════════════╝")
    print("\n")


def main():
    """主函数"""
    try:
        # 显示欢迎界面
        show_welcome()
        
        while True:
            # 显示登录方式选择
            show_login_menu()
            choice = input("请选择登录方式(0-2): ").strip()
            
            if choice == '1':
                # 用户登录
                user_menu = UserMenu()
                user_menu.run()
            
            elif choice == '2':
                # 管理员登录
                admin_menu = AdminMenu()
                admin_menu.run()
            
            elif choice == '0':
                # 退出系统
                clear_screen()
                print("\n")
                print("    ╔════════════════════════════════════════════════════╗")
                print("    ║                                                    ║")
                print("    ║          谢谢使用! 再见!                           ║")
                print("    ║                                                    ║")
                print("    ║          Thank you! Goodbye!                       ║")
                print("    ║                                                    ║")
                print("    ╚════════════════════════════════════════════════════╝")
                print("\n")
                break
            
            else:
                print("\n无效的选择,请重新输入!")
                time.sleep(1)
    
    except KeyboardInterrupt:
        print("\n\n程序已中断")
    except Exception as e:
        print(f"\n发生错误: {e}")
        import traceback
        traceback.print_exc()


if __name__ == "__main__":
    main()
