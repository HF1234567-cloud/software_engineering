# -*- coding: utf-8 -*-
"""
文件管理模块 - 负责数据的读写
"""
import json
import os
from models import Card
from config import CARD_FILE


class FileManager:
    """文件管理器"""
    
    @staticmethod
    def save_cards(cards):
        """保存卡信息到文件"""
        try:
            data = [card.to_dict() for card in cards]
            with open(CARD_FILE, 'w', encoding='utf-8') as f:
                json.dump(data, f, ensure_ascii=False, indent=2)
            return True
        except Exception as e:
            print(f"保存卡信息失败: {e}")
            return False
    
    @staticmethod
    def load_cards():
        """从文件加载卡信息"""
        if not os.path.exists(CARD_FILE):
            return []
        
        try:
            with open(CARD_FILE, 'r', encoding='utf-8') as f:
                data = json.load(f)
                return [Card.from_dict(item) for item in data]
        except Exception as e:
            print(f"加载卡信息失败: {e}")
            return []
