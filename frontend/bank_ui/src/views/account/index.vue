<template>
  <div class="account-container">
    <h1 class="page-title"><i class="bx bxs-user-account"></i> 账户管理</h1>

    <!-- 用户账户信息 -->
    <a-card class="info-card" bordered={false}>
      <template #title>
        <span class="card-title">账户信息</span>
      </template>
      <p class="user-name">👤 用户名：{{ username }}</p>
      <p class="account-id">🏦 账户号：{{ accountId }}</p>
      <p class="status">🧩 当前状态：<span :class="statusClass">{{ statusText }}</span></p>
    </a-card>

    <!-- 三大功能卡片 -->
    <div class="actions-grid">
      <!-- 挂失 -->
      <a-card hoverable class="action-card loss">
        <template #title>挂失</template>
        <p>若账户遗失，可申请挂失防止被他人盗用。</p>
        <a-button
          type="primary"
          block
          shape="round"
          class="mt-3"
          @click="reportLoss"
        >
          🚨 确认挂失
        </a-button>
      </a-card>

      <!-- 销户 -->
      <a-card hoverable class="action-card close">
        <template #title>销户</template>
        <p>如需终止账户使用，可申请销户。</p>
        <a-button
          danger
          block
          shape="round"
          class="mt-3"
          @click="closeAccount"
        >
          🗑️ 确认销户
        </a-button>
      </a-card>

      <!-- 冻结 -->
      <a-card hoverable class="action-card freeze">
        <template #title>账户冻结</template>
        <p>若发现异常交易，可临时冻结账户。</p>
        <a-button
          type="default"
          block
          shape="round"
          class="mt-3"
          @click="freezeAccount"
        >
          ❄️ 冻结账户
        </a-button>
      </a-card>
    </div>
  </div>
</template>

<script setup>
import { ref, computed } from 'vue'   // ✅ 增加 computed
import { message } from 'ant-design-vue'

const username = ref('张三')
const accountId = ref('6222 8888 1234 5678')
const status = ref('正常') // 正常 / 挂失 / 冻结 / 已销户

// ✅ 计算属性
const statusClass = computed(() => {
  switch (status.value) {
    case '正常': return 'status-normal'
    case '挂失': return 'status-loss'
    case '冻结': return 'status-freeze'
    case '已销户': return 'status-closed'
  }
})
const statusText = computed(() => status.value)

// ✅ 功能逻辑
const reportLoss = () => {
  if (status.value === '挂失') {
    message.warning('账户已处于挂失状态')
    return
  }
  if (status.value === '已销户') {
    message.error('已销户账户无法挂失')
    return
  }
  status.value = '挂失'
  message.success('账户挂失成功！')
}

const closeAccount = () => {
  if (status.value === '已销户') {
    message.warning('该账户已销户')
    return
  }
  status.value = '已销户'
  message.success('账户销户成功！')
}

const freezeAccount = () => {
  if (status.value === '冻结') {
    message.warning('账户已被冻结')
    return
  }
  if (status.value === '已销户') {
    message.error('已销户账户无法冻结')
    return
  }
  status.value = '冻结'
  message.success('账户冻结成功！')
}
</script>


<style scoped>
.account-container {
  padding: 30px;
  color: #fff;
  background: linear-gradient(145deg, #0f172a, #1e293b);
  min-height: 100%;
  border-radius: 16px;
  box-shadow: 0 0 20px rgba(0, 255, 255, 0.15);
}

:deep(.freeze .ant-card-head-title) {
  color: #fff;
}

:deep(.loss .ant-card-head-title) {
  color: #fff;
}
:deep(.close .ant-card-head-title) {
  color: #fff;
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

.info-card {
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

.user-name,
.account-id,
.status {
  font-size: 16px;
  margin: 6px 0;
}

/* 状态文字颜色 */
.status-normal {
  color: #00ffb7;
}
.status-loss {
  color: #ffcc00;
}
.status-freeze {
  color: #00c2ff;
}
.status-closed {
  color: #ff4d4f;
}

/* 功能卡片布局 */
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
  min-height: 160px;
}

.action-card:hover {
  transform: translateY(-5px);
  box-shadow: 0 0 15px rgba(0, 255, 255, 0.3);
}

.mt-3 {
  margin-top: 15px;
}

.bx {
  font-size: 26px;
}
</style>
