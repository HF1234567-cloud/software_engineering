# -*- coding: utf-8 -*-
"""
卡服务模块 - 处理卡相关的业务逻辑
"""
from datetime import datetime
from models import Card
from file_manager import FileManager
from config import (STATUS_NORMAL, STATUS_FROZEN, STATUS_CLOSED, 
                    STATUS_INVALID, NOT_DELETED, DELETED)


class CardService:
    """卡服务类"""
    
    def __init__(self):
        self.cards = FileManager.load_cards()
    
    def save(self):
        """保存卡信息"""
        return FileManager.save_cards(self.cards)
    
    def add_card(self, card_name, password, balance):
        """添加卡"""
        # 检查卡号是否已存在
        if self.find_card_by_name(card_name):
            return False, "卡号已存在"
        
        # 验证输入
        if len(card_name) > 18 or len(card_name) == 0:
            return False, "卡号长度应为1-18位"
        
        if len(password) > 8 or len(password) == 0:
            return False, "密码长度应为1-8位"
        
        if balance < 0:
            return False, "开卡金额不能为负数"
        
        # 创建并添加卡
        card = Card(card_name, password, balance)
        self.cards.append(card)
        
        if self.save():
            return True, "添加成功"
        else:
            self.cards.pop()  # 保存失败则撤销添加
            return False, "保存失败"
    
    def find_card_by_name(self, card_name, include_deleted=False):
        """根据卡号查找卡(精确查询)"""
        for card in self.cards:
            if card.card_name == card_name:
                if include_deleted or card.deleted == NOT_DELETED:
                    return card
        return None
    
    def search_cards(self, keyword):
        """模糊查询卡"""
        results = []
        for card in self.cards:
            if card.deleted == NOT_DELETED and keyword in card.card_name:
                results.append(card)
        return results
    
    def verify_card(self, card_name, password):
        """验证卡号和密码"""
        card = self.find_card_by_name(card_name)
        if not card:
            return None, "卡号不存在"
        
        if card.deleted == DELETED:
            return None, "该卡已删除"
        
        if card.password != password:
            return None, "密码错误"
        
        if card.status == STATUS_FROZEN:
            return None, "该卡已冻结"
        
        if card.status == STATUS_CLOSED:
            return None, "该卡已注销"
        
        return card, "验证成功"
    
    def get_all_cards(self, include_deleted=False):
        """获取所有卡信息"""
        if include_deleted:
            return self.cards
        else:
            return [card for card in self.cards if card.deleted == NOT_DELETED]
    
    def freeze_card(self, card_name, password):
        """冻结卡"""
        card, msg = self.verify_card(card_name, password)
        if not card:
            return False, msg
        
        if card.status == STATUS_FROZEN:
            return False, "该卡已经是冻结状态"
        
        if card.status == STATUS_CLOSED:
            return False, "该卡已注销,无法冻结"
        
        card.status = STATUS_FROZEN
        
        if self.save():
            return True, "冻结成功"
        else:
            card.status = STATUS_NORMAL  # 回滚
            return False, "保存失败"
    
    def unfreeze_card(self, card_name, password):
        """解冻卡"""
        card = self.find_card_by_name(card_name)
        if not card:
            return False, "卡号不存在"
        
        if card.deleted == DELETED:
            return False, "该卡已删除"
        
        if card.password != password:
            return False, "密码错误"
        
        if card.status == STATUS_CLOSED:
            return False, "该卡已注销,无法解冻"
        
        if card.status != STATUS_FROZEN:
            return False, "该卡未处于冻结状态"
        
        card.status = STATUS_NORMAL
        
        if self.save():
            return True, "解冻成功"
        else:
            card.status = STATUS_FROZEN  # 回滚
            return False, "保存失败"
    
    def close_card(self, card_name, password):
        """注销卡(永久)"""
        card, msg = self.verify_card(card_name, password)
        if not card:
            # 如果是冻结状态,也允许注销
            card = self.find_card_by_name(card_name)
            if not card:
                return False, "卡号不存在"
            
            if card.deleted == DELETED:
                return False, "该卡已删除"
            
            if card.password != password:
                return False, "密码错误"
            
            if card.status == STATUS_CLOSED:
                return False, "该卡已经是注销状态"
        
        if card.status == STATUS_CLOSED:
            return False, "该卡已经是注销状态"
        
        card.status = STATUS_CLOSED
        
        if self.save():
            return True, "注销成功,该操作不可恢复"
        else:
            card.status = STATUS_NORMAL  # 回滚
            return False, "保存失败"
    
    def add_money(self, card_name, password, amount):
        """存款"""
        card, msg = self.verify_card(card_name, password)
        if not card:
            return False, msg
        
        if amount <= 0:
            return False, "存款金额必须大于0"
        
        old_balance = card.balance
        card.balance += amount
        card.total_use += amount
        
        # 如果之前是欠费状态,存款后恢复正常
        if card.status == STATUS_INVALID:
            card.status = STATUS_NORMAL
        
        if self.save():
            return True, f"存款成功,当前余额: {card.balance:.2f}元"
        else:
            card.balance = old_balance  # 回滚
            card.total_use -= amount
            return False, "保存失败"
    
    def refund_money(self, card_name, password, amount):
        """取款"""
        card, msg = self.verify_card(card_name, password)
        if not card:
            return False, msg
        
        if amount <= 0:
            return False, "取款金额必须大于0"
        
        if card.balance < amount:
            return False, f"余额不足,当前余额: {card.balance:.2f}元"
        
        old_balance = card.balance
        card.balance -= amount
        card.total_use -= amount
        
        if self.save():
            return True, f"取款成功,当前余额: {card.balance:.2f}元"
        else:
            card.balance = old_balance  # 回滚
            card.total_use += amount
            return False, "保存失败"
    
    def get_status_name(self, status):
        """获取状态名称"""
        status_map = {
            STATUS_NORMAL: "正常",
            STATUS_FROZEN: "已冻结",
            STATUS_CLOSED: "已注销",
            STATUS_INVALID: "欠费失效"
        }
        return status_map.get(status, "未知状态")
