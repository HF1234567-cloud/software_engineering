# -*- coding: utf-8 -*-
"""
全局配置文件
"""
import os

# 获取当前文件所在目录(py改进1目录)
CURRENT_DIR = os.path.dirname(os.path.abspath(__file__))

# 数据目录(在py改进1目录下)
DATA_DIR = os.path.join(CURRENT_DIR, "data")

# 数据文件路径
CARD_FILE = os.path.join(DATA_DIR, "cards.json")

# 卡状态
STATUS_NORMAL = 0    # 正常
STATUS_FROZEN = 1    # 已冻结
STATUS_CLOSED = 2    # 已注销(永久)
STATUS_INVALID = 3   # 失效(欠费)

# 删除标识
NOT_DELETED = 0
DELETED = 1

# 创建数据目录
if not os.path.exists(DATA_DIR):
    os.makedirs(DATA_DIR)
