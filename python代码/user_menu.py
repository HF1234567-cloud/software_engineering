# -*- coding: utf-8 -*-
"""
用户端菜单模块 - 用户功能界面
"""
import os
import time
from datetime import datetime
from card_service import CardService


class UserMenu:
    """用户端菜单类"""
    
    def __init__(self):
        self.card_service = CardService()
        self.current_card = None  # 当前登录的卡
    
    def clear_screen(self):
        """清屏"""
        os.system('cls' if os.name == 'nt' else 'clear')
    
    def pause(self):
        """暂停"""
        input("\n按回车键继续...")
    
    def print_header(self, title):
        """打印标题"""
        print("\n" + "="*60)
        print(f"  {title}")
        print("="*60 + "\n")
    
    def login(self):
        """用户登录"""
        self.clear_screen()
        self.print_header("【用户登录】")
        
        card_name = input("请输入卡号: ").strip()
        password = input("请输入密码: ").strip()
        
        card, msg = self.card_service.verify_card(card_name, password)
        
        if card:
            self.current_card = card
            print(f"\n✓ 登录成功! 欢迎, {card_name}")
            print(f"当前余额: {card.balance:.2f}元")
            time.sleep(1)
            return True
        else:
            print(f"\n✗ 登录失败: {msg}")
            return False
    
    def show_menu(self):
        """显示用户菜单"""
        self.clear_screen()
        print("\n")
        print("    ╔════════════════════════════════════════════════════╗")
        print("    ║                用户端功能菜单                      ║")
        print("    ╠════════════════════════════════════════════════════╣")
        print(f"    ║  当前用户: {self.current_card.card_name:<38} ║")
        print(f"    ║  当前余额: {self.current_card.balance:<38.2f} ║")
        print("    ╠════════════════════════════════════════════════════╣")
        print("    ║  1. 存款                                           ║")
        print("    ║  2. 取款                                           ║")
        print("    ║  3. 查询余额                                       ║")
        print("    ║  0. 退出登录                                       ║")
        print("    ╚════════════════════════════════════════════════════╝")
        print("\n")
    
    def add_money(self):
        """存款"""
        self.print_header("【存款】")
        
        try:
            amount = float(input("请输入存款金额: "))
        except ValueError:
            print("金额格式错误!")
            return
        
        success, msg = self.card_service.add_money(
            self.current_card.card_name, 
            self.current_card.password, 
            amount
        )
        
        if success:
            print(f"\n✓ {msg}")
            # 更新当前卡的余额显示
            self.current_card = self.card_service.find_card_by_name(self.current_card.card_name)
        else:
            print(f"\n✗ {msg}")
    
    def refund_money(self):
        """取款"""
        self.print_header("【取款】")
        
        print(f"当前余额: {self.current_card.balance:.2f}元")
        
        try:
            amount = float(input("请输入取款金额: "))
        except ValueError:
            print("金额格式错误!")
            return
        
        success, msg = self.card_service.refund_money(
            self.current_card.card_name, 
            self.current_card.password, 
            amount
        )
        
        if success:
            print(f"\n✓ {msg}")
            # 更新当前卡的余额显示
            self.current_card = self.card_service.find_card_by_name(self.current_card.card_name)
        else:
            print(f"\n✗ {msg}")
    
    def query_balance(self):
        """查询余额"""
        self.print_header("【查询余额】")
        
        # 刷新卡信息
        self.current_card = self.card_service.find_card_by_name(self.current_card.card_name)
        
        print(f"卡号: {self.current_card.card_name}")
        print(f"当前余额: {self.current_card.balance:.2f}元")
        print(f"累计金额: {self.current_card.total_use:.2f}元")
        print(f"使用次数: {self.current_card.use_count}")
        print(f"卡状态: {self.card_service.get_status_name(self.current_card.status)}")
        
        start_time = datetime.fromisoformat(self.current_card.start_time).strftime('%Y-%m-%d %H:%M:%S')
        end_time = datetime.fromisoformat(self.current_card.end_time).strftime('%Y-%m-%d %H:%M:%S')
        print(f"开卡时间: {start_time}")
        print(f"截止时间: {end_time}")
    
    def run(self):
        """运行用户端程序"""
        # 先登录
        if not self.login():
            self.pause()
            return
        
        # 登录成功后显示菜单
        while True:
            self.show_menu()
            choice = input("请输入0-3进行选择: ").strip()
            
            self.clear_screen()
            
            if choice == '1':
                self.add_money()
            elif choice == '2':
                self.refund_money()
            elif choice == '3':
                self.query_balance()
            elif choice == '0':
                self.clear_screen()
                print("\n退出登录成功! 再见!\n")
                break
            else:
                print("\n无法识别的功能,请重新选择")
            
            self.pause()
