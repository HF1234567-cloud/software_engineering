<template>
  <a-layout style="min-height: 100vh">
    <!-- 侧边栏 -->
    <a-layout-sider collapsible v-model="collapsed">
      <!-- ✅ Logo区 -->
      <div class="logo">
        <i class="bx bxl-xing"></i>
        <span v-if="!collapsed" class="logo-text">易银行</span>
      </div>

      <a-menu
        theme="dark"
        mode="inline"
        :selectedKeys="selectedKeys"
        :items="menuItems"
        @click="onMenuClick"
      />
    </a-layout-sider>

    <!-- 主内容 -->
    <a-layout>
      <!-- ✅ 顶部Header -->
      <a-layout-header
        :style="{
          padding: '0 20px',
          display: 'flex',
          alignItems: 'center',
          justifyContent: 'space-between',
          background: '#14203b'
        }"
      >
      

        <!-- 中：动态欢迎 -->
        <div class="header-center">
          <span class="welcome-user">欢迎您使用易银行系统，{{ username }}</span>
        </div>

        <!-- 右：管理员登录 + 用户头像下拉 -->
        <div class="header-right">
          <a-button type="primary" ghost size="small" @click="showAdminModal = true">
            管理员登录
          </a-button>

          <a-dropdown trigger="click">
            <a-space class="user-menu">
              <a-avatar size="small" style="background-color: #00bcd4">
                <template #icon><UserOutlined /></template>
              </a-avatar>
              <i class="bx bx-chevron-down"></i>
            </a-space>
            <template #overlay>
              <a-menu @click="onMenuAction">
                <a-menu-item key="changePassword">
                  <i class="bx bx-key"></i> 修改密码
                </a-menu-item>
                <a-menu-item key="logout">
                  <i class="bx bx-log-out"></i> 退出登录
                </a-menu-item>
              </a-menu>
            </template>
          </a-dropdown>
        </div>
      </a-layout-header>

      <!-- 主体内容 -->
      <a-layout-content>
        <div
          :style="{
            minHeight: '360px',
            background: colorBgContainer,
            borderRadius: borderRadiusLG,
          }"
        >
          <router-view />
        </div>
      </a-layout-content>

      <!-- 页脚 -->
      <a-layout-footer style="text-align: center">
        Bank System ©{{ new Date().getFullYear() }} Created by HF
      </a-layout-footer>
    </a-layout>
  </a-layout>

  <!-- ✅ 修改密码弹窗 -->
  <a-modal
    v-model:open="showPasswordModal"
    title="修改密码"
    ok-text="确认修改"
    cancel-text="取消"
    @ok="handleChangePassword"
  >
    <a-form :model="passwordForm" layout="vertical">
      <a-form-item label="旧密码">
        <a-input-password v-model:value="passwordForm.oldPassword" placeholder="请输入旧密码" />
      </a-form-item>
      <a-form-item label="新密码">
        <a-input-password v-model:value="passwordForm.newPassword" placeholder="请输入新密码" />
      </a-form-item>
      <a-form-item label="确认新密码">
        <a-input-password v-model:value="passwordForm.confirmPassword" placeholder="请再次输入新密码" />
      </a-form-item>
    </a-form>
  </a-modal>

  <!-- ✅ 管理员登录弹窗 -->
  <a-modal
    v-model:open="showAdminModal"
    title="管理员登录"
    ok-text="登录"
    cancel-text="取消"
    @ok="handleAdminLogin"
  >
    <a-form :model="adminForm" layout="vertical">
      <a-form-item label="用户名">
        <a-input v-model:value="adminForm.username" placeholder="请输入管理员用户名" />
      </a-form-item>
      <a-form-item label="密码">
        <a-input-password v-model:value="adminForm.password" placeholder="请输入密码" />
      </a-form-item>
    </a-form>
  </a-modal>
</template>

<script setup>
import { ref, h, onMounted } from 'vue'
import { useRouter } from 'vue-router'
import {
  HomeOutlined,
  DesktopOutlined,
  UserOutlined,
  AccountBookOutlined
} from '@ant-design/icons-vue'
import { theme, message, Modal } from 'ant-design-vue'
import axios from 'axios'
const {
  token: { colorBgContainer, borderRadiusLG },
} = theme.useToken()

const collapsed = ref(false)
const selectedKeys = ref(['1'])
const username = ref('') // 动态用户名
const router = useRouter()

// 菜单项（保持不变）
const menuItems = [
  { key: '1', icon: () => h(HomeOutlined), label: '首页', path: '/home' },
  { key: '2', icon: () => h(DesktopOutlined), label: '存取款管理', path: '/transaction' },
  { key: '3', icon: () => h(AccountBookOutlined), label: '账户管理', path: '/account' },
  { key: '4', icon: () => h(UserOutlined), label: '用户管理', path: '/user' },
]

// ✅ 修复：页面加载时从本地存储读取用户名（登录时已存入）
onMounted(() => {
  // 从 localstorage 获取登录成功后的用户名
  const loginUsername = localStorage.getItem('username')
  if (loginUsername) {
    username.value = loginUsername
  } else {
    // 未登录时跳转到登录页
    username.value = '访客'
    router.push('/login')
  }
})

// 菜单点击跳转（保持不变）
function onMenuClick({ key }) {
  selectedKeys.value = [key]
  const findPath = (items, key) => {
    for (const item of items) {
      if (item.key === key && item.path) return item.path
      if (item.children) {
        const res = findPath(item.children, key)
        if (res) return res
      }
    }
  }
  const path = findPath(menuItems, key)
  if (path) router.push(path)
}

/* ========== 修改密码弹窗逻辑（保持不变） ========== */
const showPasswordModal = ref(false)
const passwordForm = ref({
  oldPassword: '',
  newPassword: '',
  confirmPassword: '',
})

function onMenuAction({ key }) {
  if (key === 'changePassword') {
    showPasswordModal.value = true
  } else if (key === 'logout') {
    Modal.confirm({
      title: '确定退出登录？',
      okText: '确认',
      cancelText: '取消',
      onOk: () => {
        // ✅ 退出登录时清除本地存储的用户名
        localStorage.removeItem('username')
        message.success('已退出登录')
        router.push('/login')
      },
    })
  }
}

// 修改密码连接后端接口（保持不变）
async function handleChangePassword() {
  const { oldPassword, newPassword, confirmPassword } = passwordForm.value

  if (!oldPassword || !newPassword || !confirmPassword) {
    message.warning('请填写完整信息')
    return
  }
  if (newPassword !== confirmPassword) {
    message.error('两次密码输入不一致')
    return
  }

  try {
    const res = await axios.post('http://127.0.0.1:5003/change_password', {
      username: username.value, // 当前登录用户名（从本地存储获取，正确）
      old_password: oldPassword,
      new_password: newPassword,
    })

    if (res.status === 200) {
      showPasswordModal.value = false
      passwordForm.value = { oldPassword: '', newPassword: '', confirmPassword: '' }

      Modal.success({
        title: '密码修改成功',
        content: '您的密码已修改成功，请重新登录。',
        okText: '前往登录',
        onOk: () => {
          localStorage.removeItem('username') // 清除本地存储
          router.push('/login')
        },
      })
    }

  } catch (err) {
    message.error(err.response?.data?.message || '修改密码失败，请检查旧密码')
  }
}

/* ========== 管理员登录弹窗逻辑（保持不变） ========== */
const showAdminModal = ref(false)
const adminForm = ref({
  username: '',
  password: '',
})

function handleAdminLogin() {
  const { username, password } = adminForm.value
  if (username === 'admin' && password === '123456') {
    message.success('管理员登录成功！')
    showAdminModal.value = false
    router.push('/admin')
  } else {
    message.error('用户名或密码错误')
  }
}
</script>


<style scoped>
/* ===== Logo样式 ===== */
.logo {
  display: flex;
  align-items: center;
  height: 64px;
  padding: 0 16px;
  font-size: 16px;
  color: #fff;
  background: linear-gradient(90deg, #1a2a4e, #203b6e);
}

.logo i {
  font-size: 28px;
  color: #00bcd4;
  margin-right: 8px;
}

/* ===== Header样式 ===== */
.header-left,
.header-center,
.header-right {
  display: flex;
  align-items: center;
  color: #fff;
}

.header-center {
  flex: 1;
  justify-content: center;
  font-size: 16px;
  font-weight: 500;
  color: #ffffffd9;
}

.header-right {
  gap: 16px;
}

.user-menu {
  cursor: pointer;
  color: #fff;
  display: flex;
  align-items: center;
}

.user-menu i {
  font-size: 18px;
  margin-left: 4px;
  color: #00bcd4;
}

/* ===== 深色主题优化 ===== */
:deep(.ant-layout) {
  background-color: #000 !important;
}

:deep(.ant-layout-header) {
  background-color: #14203b !important;
}

:deep(.ant-layout-content) {
  background-color: #14203b !important;
}

:deep(.ant-menu-dark .ant-menu-item) {
  color: #e0e0e0 !important;
}

:deep(.ant-menu-dark .ant-menu-item-selected) {
  color: #fff !important;
  background-color: #1a2a4e !important;
}

:deep(.ant-layout-footer) {
  background-color: #1a2a4e !important;
  color: #ffffff !important;
}
</style>
