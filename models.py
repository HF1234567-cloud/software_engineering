# -*- coding: utf-8 -*-
"""
数据模型定义
"""
from datetime import datetime
from config import STATUS_NORMAL, NOT_DELETED


class Card:
    """银行卡信息模型"""
    
    def __init__(self, card_name, password, balance=0.0):
        self.card_name = card_name      # 卡号
        self.password = password        # 密码
        self.status = STATUS_NORMAL     # 卡状态
        self.total_use = balance        # 累计金额
        self.use_count = 0              # 使用次数
        self.balance = balance          # 余额
        self.deleted = NOT_DELETED      # 删除标识
        self.start_time = datetime.now().isoformat()  # 开卡时间
        self.end_time = self._get_end_time()          # 截止时间(一年后)
        self.last_time = datetime.now().isoformat()   # 最后使用时间
    
    def _get_end_time(self):
        """获取截止时间(一年后)"""
        now = datetime.now()
        end_year = now.year + 1
        end_time = now.replace(year=end_year)
        return end_time.isoformat()
    
    def to_dict(self):
        """转换为字典"""
        return {
            'card_name': self.card_name,
            'password': self.password,
            'status': self.status,
            'total_use': self.total_use,
            'use_count': self.use_count,
            'balance': self.balance,
            'deleted': self.deleted,
            'start_time': self.start_time,
            'end_time': self.end_time,
            'last_time': self.last_time
        }
    
    @staticmethod
    def from_dict(data):
        """从字典创建对象"""
        card = Card(data['card_name'], data['password'], 0)
        card.status = data.get('status', STATUS_NORMAL)
        card.total_use = data.get('total_use', 0.0)
        card.use_count = data.get('use_count', 0)
        card.balance = data.get('balance', 0.0)
        card.deleted = data.get('deleted', NOT_DELETED)
        card.start_time = data.get('start_time', datetime.now().isoformat())
        card.end_time = data.get('end_time', card._get_end_time())
        card.last_time = data.get('last_time', datetime.now().isoformat())
        return card
