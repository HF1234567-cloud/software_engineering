# -*- coding: utf-8 -*-
"""
管理员端菜单模块 - 管理员功能界面
"""
import os
import time
from datetime import datetime
from card_service import CardService
from statistics import Statistics


class AdminMenu:
    """管理员端菜单类"""
    
    def __init__(self):
        self.card_service = CardService()
        self.statistics = Statistics(self.card_service)
    
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
        """管理员登录(暂时无需密码)"""
        self.clear_screen()
        self.print_header("【管理员登录】")
        
        print("管理员身份验证...")
        time.sleep(0.5)
        print("✓ 验证通过,欢迎管理员!")
        time.sleep(1)
        return True
    
    def show_menu(self):
        """显示管理员菜单"""
        self.clear_screen()
        print("\n")
        print("    ╔════════════════════════════════════════════════════╗")
        print("    ║              管理员端功能菜单                      ║")
        print("    ╠════════════════════════════════════════════════════╣")
        print("    ║  1. 添加卡                                         ║")
        print("    ║  2. 查询卡                                         ║")
        print("    ║  3. 冻结卡                                         ║")
        print("    ║  4. 解冻卡                                         ║")
        print("    ║  5. 注销卡                                         ║")
        print("    ║  6. 统计                                           ║")
        print("    ║  7. 显示所有卡信息                                 ║")
        print("    ║  0. 退出系统                                       ║")
        print("    ╚════════════════════════════════════════════════════╝")
        print("\n")
    
    def add_card(self):
        """添加卡"""
        self.print_header("【1】添加卡")
        
        card_name = input("请输入卡号(长度1-18位): ").strip()
        password = input("请输入密码(长度1-8位): ").strip()
        
        try:
            balance = float(input("请输入开卡金额: "))
        except ValueError:
            print("金额格式错误!")
            return
        
        success, msg = self.card_service.add_card(card_name, password, balance)
        
        if success:
            print(f"\n✓ {msg}")
            print(f"\n卡号: {card_name}")
            print(f"余额: {balance:.2f}元")
            print(f"开卡时间: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
        else:
            print(f"\n✗ {msg}")
    
    def query_card(self):
        """查询卡"""
        self.print_header("【2】查询卡")
        
        print("1. 精确查询")
        print("2. 模糊查询")
        choice = input("\n请选择查询方式: ").strip()
        
        if choice == '1':
            card_name = input("请输入卡号: ").strip()
            card = self.card_service.find_card_by_name(card_name)
            if card:
                self._print_card_info([card])
            else:
                print("\n未找到该卡")
        
        elif choice == '2':
            keyword = input("请输入关键字: ").strip()
            cards = self.card_service.search_cards(keyword)
            if cards:
                self._print_card_info(cards)
            else:
                print("\n未找到匹配的卡")
        else:
            print("无效的选择")
    
    def _print_card_info(self, cards):
        """打印卡信息"""
        print("\n" + "-"*120)
        print(f"{'卡号':<18} {'状态':<10} {'余额':<10} {'累计存款':<10} {'使用次数':<10} {'开卡时间':<20} {'截止时间':<20}")
        print("-"*120)
        
        for card in cards:
            status_name = self.card_service.get_status_name(card.status)
            start_time = datetime.fromisoformat(card.start_time).strftime('%Y-%m-%d %H:%M:%S')
            end_time = datetime.fromisoformat(card.end_time).strftime('%Y-%m-%d %H:%M:%S')
            
            print(f"{card.card_name:<18} {status_name:<10} {card.balance:<10.2f} "
                  f"{card.total_use:<10.2f} {card.use_count:<10} "
                  f"{start_time:<20} {end_time:<20}")
        
        print("-"*120)
        print(f"共找到 {len(cards)} 张卡")
    
    def freeze_card(self):
        """冻结卡"""
        self.print_header("【3】冻结卡")
        
        card_name = input("请输入卡号: ").strip()
        password = input("请输入密码: ").strip()
        
        confirm = input("确认冻结该卡? (y/n): ").strip().lower()
        if confirm != 'y':
            print("已取消操作")
            return
        
        success, msg = self.card_service.freeze_card(card_name, password)
        
        if success:
            print(f"\n✓ {msg}")
        else:
            print(f"\n✗ {msg}")
    
    def unfreeze_card(self):
        """解冻卡"""
        self.print_header("【4】解冻卡")
        
        card_name = input("请输入卡号: ").strip()
        password = input("请输入密码: ").strip()
        
        success, msg = self.card_service.unfreeze_card(card_name, password)
        
        if success:
            print(f"\n✓ {msg}")
        else:
            print(f"\n✗ {msg}")
    
    def close_card(self):
        """注销卡(永久)"""
        self.print_header("【5】注销卡")
        
        print("警告: 注销操作不可恢复!")
        card_name = input("请输入卡号: ").strip()
        password = input("请输入密码: ").strip()
        
        confirm = input("确认永久注销该卡? (y/n): ").strip().lower()
        if confirm != 'y':
            print("已取消操作")
            return
        
        # 二次确认
        confirm2 = input("再次确认,此操作不可恢复! (yes/no): ").strip().lower()
        if confirm2 != 'yes':
            print("已取消操作")
            return
        
        success, msg = self.card_service.close_card(card_name, password)
        
        if success:
            print(f"\n✓ {msg}")
        else:
            print(f"\n✗ {msg}")
    
    def statistic_menu(self):
        """统计菜单"""
        self.print_header("【6】统计")
        
        print("1. 统计已冻结的卡")
        print("2. 统计已注销的卡")
        print("3. 统计欠费卡")
        print("4. 卡状态汇总")
        print("0. 返回")
        
        choice = input("\n请选择: ").strip()
        
        if choice == '1':
            self._statistic_frozen()
        elif choice == '2':
            self._statistic_closed()
        elif choice == '3':
            self._statistic_arrears()
        elif choice == '4':
            self._statistic_summary()
    
    def _statistic_frozen(self):
        """统计已冻结的卡"""
        print("\n--- 统计已冻结的卡 ---\n")
        
        cards = self.statistics.statistic_frozen_cards()
        
        if not cards:
            print("暂无已冻结的卡")
            return
        
        self._print_card_info(cards)
    
    def _statistic_closed(self):
        """统计已注销的卡"""
        print("\n--- 统计已注销的卡 ---\n")
        
        cards = self.statistics.statistic_closed_cards()
        
        if not cards:
            print("暂无已注销的卡")
            return
        
        self._print_card_info(cards)
    
    def _statistic_arrears(self):
        """统计欠费卡"""
        print("\n--- 统计欠费卡 ---\n")
        
        cards = self.statistics.statistic_arrears_cards()
        
        if not cards:
            print("暂无欠费卡")
            return
        
        self._print_card_info(cards)
    
    def _statistic_summary(self):
        """卡状态汇总"""
        print("\n--- 卡状态汇总 ---\n")
        
        summary = self.statistics.statistic_cards_summary()
        
        print(f"总卡数: {summary['total']}")
        print(f"正常状态: {summary['normal']}")
        print(f"已冻结: {summary['frozen']}")
        print(f"已注销: {summary['closed']}")
        print(f"欠费失效: {summary['invalid']}")
        print(f"总余额: {summary['total_balance']:.2f}元")
    
    def print_all_cards(self):
        """显示所有卡信息"""
        self.print_header("【7】显示所有卡信息")
        
        cards = self.card_service.get_all_cards()
        
        if not cards:
            print("暂无卡信息")
            return
        
        self._print_card_info(cards)
    
    def run(self):
        """运行管理员端程序"""
        # 先登录
        if not self.login():
            self.pause()
            return
        
        # 登录成功后显示菜单
        while True:
            self.show_menu()
            choice = input("请输入0-7进行选择: ").strip()
            
            self.clear_screen()
            
            if choice == '1':
                self.add_card()
            elif choice == '2':
                self.query_card()
            elif choice == '3':
                self.freeze_card()
            elif choice == '4':
                self.unfreeze_card()
            elif choice == '5':
                self.close_card()
            elif choice == '6':
                self.statistic_menu()
            elif choice == '7':
                self.print_all_cards()
            elif choice == '0':
                self.clear_screen()
                print("\n谢谢使用! 再见!\n")
                break
            else:
                print("\n无法识别的功能,请重新选择")
            
            self.pause()
