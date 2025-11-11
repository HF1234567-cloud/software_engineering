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
          @click="showLossConfirm"
        >
          🚨 申请挂失
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
          @click="showCloseConfirm"
        >
          🗑️ 申请销户
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
          @click="showFreezeConfirm"
        >
          ❄️ 申请冻结
        </a-button>
      </a-card>
    </div>

    <!-- 挂失二次确认弹窗：修复 v-model:visible -->
    <a-modal
      v-model:visible="lossModalVisible"
      title="账户挂失确认"
      ok-text="确认挂失"
      cancel-text="取消"
      @ok="handleLossConfirm"
      @cancel="lossModalVisible = false"
    >
      <p>你确定要为账户 <span class="confirm-username">{{ username }}</span> 申请挂失吗？</p>
      <p class="tip-text">挂失后账户将无法正常使用，请谨慎操作！</p>
    </a-modal>

    <!-- 冻结二次确认弹窗：修复 v-model:visible -->
    <a-modal
      v-model:visible="freezeModalVisible"
      title="账户冻结确认"
      ok-text="确认冻结"
      cancel-text="取消"
      @ok="handleFreezeConfirm"
      @cancel="freezeModalVisible = false"
    >
      <p>你确定要为账户 <span class="confirm-username">{{ username }}</span> 申请冻结吗？</p>
      <p class="tip-text">冻结后账户交易将被限制，需手动解冻！</p>
    </a-modal>

    <!-- 销户二次确认弹窗：修复 v-model:visible -->
    <a-modal
      v-model:visible="closeModalVisible"
      title="账户销户确认"
      ok-text="确认销户"
      cancel-text="取消"
      @ok="handleCloseConfirm"
      @cancel="closeModalVisible = false"
      danger
      :maskClosable="false"
    >
      <p class="danger-tip">⚠️ 警告：销户为不可恢复操作！</p>
      <p>你确定要注销账户 <span class="confirm-username">{{ username }}</span> 吗？</p>
      <p>注销后账户所有数据将永久删除，余额需提前取出！</p>
    </a-modal>
  </div>
</template>

<script setup>
import { ref, computed, onMounted } from 'vue'
// 确保完整引入 Modal 和 message（Ant Design Vue 3 需显式引入）
import { message, Modal } from 'ant-design-vue'
import axios from 'axios'

// 状态初始化（从本地存储获取用户名，确保有默认值）
const username = ref(localStorage.getItem('username') || 'testuser') // 改为常见测试用户名，避免空值
const accountId = ref('')
// 初始状态改为后端兼容的“使用中”（原“正常”与后端状态不一致）
const status = ref('使用中')

// 弹窗显示状态（初始为 false，确保可触发）
const lossModalVisible = ref(false)
const freezeModalVisible = ref(false)
const closeModalVisible = ref(false)

// 计算属性：状态文字颜色（匹配后端状态值）
// 状态配置（新增处理中状态）
const statusOptions = {
  '使用中': { class: 'status-normal', text: '使用中' },
  '处理中': { class: 'status-pending', text: '处理中' },
  '挂失': { class: 'status-loss', text: '挂失' },
  '冻结': { class: 'status-freeze', text: '冻结' },
  '已销户': { class: 'status-closed', text: '已销户' }
}

// 计算属性（适配新状态）
const statusClass = computed(() => statusOptions[status.value].class)
const statusText = computed(() => statusOptions[status.value].text)


// 页面加载时同步后端账户信息（添加错误捕获，避免阻塞）
onMounted(async () => {
  try {
    await syncAccountInfo()
  } catch (err) {
    message.error('同步账户信息失败，可正常操作功能')
  }
})

// 同步后端账户信息（优化请求逻辑，避免跨域/404导致功能阻塞）
const syncAccountInfo = async () => {
  try {
    const res = await axios.get('http://127.0.0.1:5003/get_account_status', {
      params: { username: username.value },
      timeout: 5000 // 超时时间5秒，避免无限等待
    })
    status.value = res.data.status || '使用中' // 兜底默认值
    accountId.value = formatCardNumber(res.data.card) || '6222 8888 1234 5678' // 兜底卡号
  } catch (err) {
    // 请求失败时不阻塞功能，使用默认值
    accountId.value = '6222 8888 1234 5678'
    throw new Error(err.response?.data?.message || '同步失败')
  }
}

// 卡号格式化（添加空值判断，避免报错）
const formatCardNumber = (card) => {
  if (!card) return ''
  return card.replace(/(\d{4})(?=\d)/g, '$1 ')
}

// ---------------------- 挂失功能（添加日志打印，便于调试） ----------------------
// ---------------------- 挂失功能 ----------------------
const showLossConfirm = () => {
  console.log('点击挂失：当前状态', status.value)
  if (status.value === '挂失') {
    message.warning('账户已处于挂失状态')
    return
  }
  if (status.value === '已销户' || status.value === '处理中') {
    message.error(`${status.value}账户无法挂失`)
    return
  }
  lossModalVisible.value = true
}

const handleLossConfirm = async () => {
  try {
    const res = await axios.post('http://127.0.0.1:5003/apply_loss', {
      username: username.value
    })
    status.value = '处理中'  // 申请后立即变为处理中
    message.success(res.data.message)
    lossModalVisible.value = false
  } catch (err) {
    message.error(err.response?.data?.message || '挂失申请失败，请重试')
    lossModalVisible.value = false
  }
}

// ---------------------- 冻结功能 ----------------------
const showFreezeConfirm = () => {
  console.log('点击冻结：当前状态', status.value)
  if (status.value === '冻结') {
    message.warning('账户已被冻结')
    return
  }
  if (status.value === '已销户' || status.value === '处理中') {
    message.error(`${status.value}账户无法冻结`)
    return
  }
  freezeModalVisible.value = true
}

const handleFreezeConfirm = async () => {
  try {
    const res = await axios.post('http://127.0.0.1:5003/apply_freeze', {
      username: username.value
    })
    status.value = '处理中'
    message.success(res.data.message)
    freezeModalVisible.value = false
  } catch (err) {
    message.error(err.response?.data?.message || '冻结申请失败，请重试')
    freezeModalVisible.value = false
  }
}

// ---------------------- 销户功能 ----------------------
const showCloseConfirm = () => {
  console.log('点击销户：当前状态', status.value)
  if (status.value === '已销户') {
    message.warning('该账户已销户')
    return
  }
  if (status.value === '处理中') {
    message.error('当前有申请待审核，无法提交销户')
    return
  }
  // 销户前校验余额
  axios.get('http://127.0.0.1:5003/get_balance', {
    params: { username: username.value }
  }).then(res => {
    if (res.data.balance > 0) {
      message.error('账户余额不为0，请先提现再申请销户')
      return
    }
    closeModalVisible.value = true
  }).catch(err => {
    message.error('校验余额失败，请重试')
  })
}

const handleCloseConfirm = async () => {
  try {
    const res = await axios.post('http://127.0.0.1:5003/apply_close', {
      username: username.value
    })
    status.value = '处理中'
    message.success(res.data.message)
    closeModalVisible.value = false
  } catch (err) {
    message.error(err.response?.data?.message || '销户申请失败，请重试')
    closeModalVisible.value = false
  }
}
</script>

<style scoped>
/* 样式保持不变，新增按钮hover效果，确认可点击 */
.action-card .ant-btn {
  cursor: pointer !important;
  opacity: 1 !important;
}
.action-card .ant-btn:hover {
  transform: scale(1.02);
}

/* 原有样式... */
.account-container {
  padding: 30px;
  color: #fff;
  background: linear-gradient(145deg, #0f172a, #1e293b);
  min-height: 100%;
  border-radius: 16px;
  box-shadow: 0 0 20px rgba(0, 255, 255, 0.15);
}

:deep(.freeze .ant-card-head-title),
:deep(.loss .ant-card-head-title),
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

:deep(.ant-modal-content) {
  background: rgba(15, 23, 42, 0.95);
  border: 1px solid rgba(0, 224, 255, 0.3);
  border-radius: 12px;
  color: #fff;
}
:deep(.ant-modal-header) {
  border-bottom: 1px solid rgba(0, 224, 255, 0.2);
}
:deep(.ant-modal-title) {
  color: #00e0ff;
  font-weight: 600;
}
:deep(.ant-modal-footer) {
  border-top: 1px solid rgba(0, 224, 255, 0.2);
}
.confirm-username {
  color: #00c8ff;
  font-weight: bold;
  margin: 0 4px;
}
.tip-text {
  color: #ccc;
  font-size: 14px;
  margin-top: 8px;
}
.danger-tip {
  color: #ff4d4f;
  font-weight: bold;
  margin-bottom: 8px;
}

/* 处理中状态样式（灰色） */
.status-pending {
  color: #999;
}

/* 处理中状态下按钮禁用样式 */
:deep(.action-card .ant-btn:disabled) {
  background: #666 !important;
  border-color: #666 !important;
  cursor: not-allowed !important;
  transform: none !important;
  opacity: 0.7 !important;
}

/* 优化状态显示样式 */
.status {
  font-weight: 500;
}
</style>