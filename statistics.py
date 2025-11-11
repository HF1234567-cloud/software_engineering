# -*- coding: utf-8 -*-
"""
统计模块 - 提供各种统计功能
"""
from datetime import datetime
from config import STATUS_FROZEN, STATUS_CLOSED, STATUS_INVALID, NOT_DELETED


class Statistics:
    """统计类"""
    
    def __init__(self, card_service):
        self.card_service = card_service
    
    def statistic_frozen_cards(self):
        """统计已冻结的卡"""
        frozen_cards = []
        for card in self.card_service.get_all_cards():
            if card.status == STATUS_FROZEN:
                frozen_cards.append(card)
        
        return frozen_cards
    
    def statistic_closed_cards(self):
        """统计已注销的卡"""
        closed_cards = []
        for card in self.card_service.get_all_cards():
            if card.status == STATUS_CLOSED:
                closed_cards.append(card)
        
        return closed_cards
    
    def statistic_arrears_cards(self):
        """统计欠费卡"""
        arrears_cards = []
        for card in self.card_service.get_all_cards():
            if card.status == STATUS_INVALID or card.balance < 0:
                arrears_cards.append(card)
        
        return arrears_cards
    
    def statistic_cards_summary(self):
        """统计卡状态汇总"""
        all_cards = self.card_service.get_all_cards()
        
        summary = {
            'total': len(all_cards),
            'normal': 0,
            'frozen': 0,
            'closed': 0,
            'invalid': 0,
            'total_balance': 0.0
        }
        
        for card in all_cards:
            if card.status == STATUS_FROZEN:
                summary['frozen'] += 1
            elif card.status == STATUS_CLOSED:
                summary['closed'] += 1
            elif card.status == STATUS_INVALID:
                summary['invalid'] += 1
            else:
                summary['normal'] += 1
            
            summary['total_balance'] += card.balance
        
        return summary
