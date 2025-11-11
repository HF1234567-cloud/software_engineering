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
      <!-- 存款 -->
      <a-card hoverable class="action-card">
        <template #title>存款</template>
        <a-input
          v-model="depositAmount"
          placeholder="请输入存款金额（1-100万）"
          prefix="💵"
          type="number"
          @input="onDepositInput"
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
          placeholder="请输入取款金额（1-100万）"
          prefix="💸"
          type="number"
          @input="onWithdrawInput"
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

      
    </div>
  </div>
</template>

<script setup>
import { ref, onMounted } from 'vue'
import { message } from 'ant-design-vue'
import axios from 'axios'

const username = ref(localStorage.getItem('username') || '张三')
const balance = ref(0)
const withdrawAmount = ref('')
const depositAmount = ref(''); // 存款金额绑定

// ✅ 页面加载时获取余额
onMounted(async () => {
  try {
    const res = await axios.get(`http://127.0.0.1:5003/get_balance`, {
      params: { username: username.value },
    })
    balance.value = res.data.balance
  } catch (err) {
    message.error('获取余额失败')
  }
})

// ✅ 存款输入过滤：只保留数字和单个小数点
const onDepositInput = (e) => {
  const value = e.target.value;
  // 过滤非数字、非小数点，且只允许一个小数点
  depositAmount.value = value.replace(/[^0-9.]/g, '').replace(/\.{2,}/g, '.');
}

// ✅ 存款功能（保持原有逻辑，新增type="number"输入限制）
const deposit = async () => {
  const raw = String(depositAmount.value).trim();
  const amount = Number(raw);

  if (!raw || isNaN(amount) || amount <= 0) {
    message.error('请输入有效的存款金额（必须是数字且大于0）');
    return;
  }
  if (amount > 1000000) {
    message.error('单次存款上限为100万元！');
    return;
  }

  try {
    const res = await axios.post('http://127.0.0.1:5003/update_balance', {
      username: username.value,
      change: amount
    });

    balance.value = res.data.balance;
    message.success(`存款成功！+${amount.toFixed(2)} 元`);
    depositAmount.value = '';
  } catch (err) {
    message.error(err.response?.data?.message || '存款失败，请重试！');
  }
}

// ✅ 取款输入过滤：只保留数字和单个小数点
const onWithdrawInput = (e) => {
  const value = e.target.value;
  // 过滤非数字、非小数点，且只允许一个小数点
  withdrawAmount.value = value.replace(/[^0-9.]/g, '').replace(/\.{2,}/g, '.');
}

// ✅ 取款功能（修复核心问题：去空格+严格数字校验）
const withdraw = async () => {
  const raw = String(withdrawAmount.value).trim(); // 去除前后空格
  const amount = Number(raw); // 严格转数字（比parseFloat更稳定）

  // 完整校验逻辑
  if (!raw) {
    message.error('请输入取款金额');
    return;
  }
  if (isNaN(amount) || amount <= 0) {
    message.error('请输入有效的取款金额（必须是数字且大于0）');
    return;
  }
  if (amount > 1000000) {
    message.error('单次取款上限为100万元！');
    return;
  }
  if (amount > balance.value) {
    message.error(`余额不足！当前余额仅${balance.value.toFixed(2)}元`);
    return;
  }

  try {
    const res = await axios.post('http://127.0.0.1:5003/update_balance', {
      username: username.value,
      change: -amount,
    })
    balance.value = res.data.balance;
    message.success(`取款成功：-${amount.toFixed(2)} 元`);
    withdrawAmount.value = '';
  } catch (err) {
    message.error(err.response?.data?.message || '取款失败');
  }
}

// ✅ 刷新余额
const refreshBalance = async () => {
  try {
    const res = await axios.get(`http://127.0.0.1:5003/get_balance`, {
      params: { username: username.value },
    })
    balance.value = res.data.balance
    message.info(`当前余额为：${balance.value.toFixed(2)} 元`)
  } catch (err) {
    message.error('刷新余额失败')
  }
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
:deep(.ant-card-head-title) {
  color: #fff;
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