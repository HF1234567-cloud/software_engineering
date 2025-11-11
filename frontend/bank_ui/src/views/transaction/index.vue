<template>
  <div class="transaction-container">
    <h1 class="page-title"><i class="bx bxs-bank"></i> 存取款管理</h1>

    <!-- 用户信息卡 -->
    <a-card class="balance-card" bordered={false}>
      <template #title>
        <span class="card-title">账户信息</span>
      </template>
      <p class="user-name">👤 用户名：{{ username }}</p>
      <p class="balance-text">💰 当前余额：<span class="balance-amount">{{ balance.toFixed(2) }}</span> 元</p>
    </a-card>

    <!-- 三功能区 -->
    <div class="actions-grid">
      <!-- 存款 -->
      <a-card hoverable class="action-card">
        <template #title>存款</template>
        <a-input
          v-model="depositAmount"
          placeholder="请输入存款金额"
          prefix="💵"
          type="number"
        />
        <a-button
          type="primary"
          block
          shape="round"
          class="mt-3"
          @click="deposit"
        >
          确认存款
        </a-button>
      </a-card>

      <!-- 取款 -->
      <a-card hoverable class="action-card">
        <template #title>取款</template>
        <a-input
          v-model="withdrawAmount"
          placeholder="请输入取款金额"
          prefix="💸"
          type="number"
        />
        <a-button
          danger
          block
          shape="round"
          class="mt-3"
          @click="withdraw"
        >
          确认取款
        </a-button>
      </a-card>

      <!-- 查询余额 -->
      <a-card hoverable class="action-card">
        <template #title>余额查询</template>
        <p class="balance-result">当前余额为：</p>
        <p class="balance-amount">{{ balance.toFixed(2) }} 元</p>
        <a-button
          type="default"
          shape="round"
          @click="refreshBalance"
        >
          🔄 刷新余额
        </a-button>
      </a-card>
    </div>
  </div>
</template>

<script setup>
import { ref } from 'vue'
import { message } from 'ant-design-vue'

const username = ref('张三')
const balance = ref(10000)
const depositAmount = ref('')
const withdrawAmount = ref('')

// 存款逻辑
const deposit = () => {
  const amount = parseFloat(depositAmount.value)
  if (isNaN(amount) || amount <= 0) {
    message.error('请输入有效的存款金额')
    return
  }
  balance.value += amount
  message.success(`存款成功：+${amount.toFixed(2)} 元`)
  depositAmount.value = ''
}

// 取款逻辑
const withdraw = () => {
  const amount = parseFloat(withdrawAmount.value)
  if (isNaN(amount) || amount <= 0) {
    message.error('请输入有效的取款金额')
    return
  }
  if (amount > balance.value) {
    message.error('余额不足')
    return
  }
  balance.value -= amount
  message.success(`取款成功：-${amount.toFixed(2)} 元`)
  withdrawAmount.value = ''
}

// 查询余额
const refreshBalance = () => {
  message.info(`当前余额为：${balance.value.toFixed(2)} 元`)
}
</script>

<style scoped>
.transaction-container {
  padding: 30px;
  color: #fff;
  background: linear-gradient(145deg, #0f172a, #1e293b);
  min-height: 100%;
  border-radius: 16px;
  box-shadow: 0 0 20px rgba(0, 255, 255, 0.15);
}

.page-title {
  color: #00e0ff;
  font-size: 26px;
  font-weight: 600;
  margin-bottom: 20px;
  display: flex;
  align-items: center;
  gap: 10px;
}

.balance-card {
  background: rgba(255, 255, 255, 0.05);
  color: #fff;
  margin-bottom: 24px;
  border-radius: 12px;
  backdrop-filter: blur(10px);
}

.card-title {
  color: #00d8ff;
  font-weight: 600;
}

.user-name {
  font-size: 16px;
  margin-bottom: 6px;
}

.balance-text {
  font-size: 18px;
}

.balance-amount {
  font-size: 24px;
  color: #00ffb7;
  font-weight: bold;
}

.actions-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(260px, 1fr));
  gap: 20px;
}

.action-card {
  background: rgba(255, 255, 255, 0.08);
  color: #fff;
  border-radius: 12px;
  transition: transform 0.3s, box-shadow 0.3s;
}

.action-card:hover {
  transform: translateY(-5px);
  box-shadow: 0 0 15px rgba(0, 255, 255, 0.3);
}

.mt-3 {
  margin-top: 15px;
}

.balance-result {
  color: #ccc;
}

.bx {
  font-size: 26px;
}
</style>
