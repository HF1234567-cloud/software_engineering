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

    <!-- 主内容布局 -->
    <a-layout>
      <!-- ✅ 顶部Header：欢迎文字 -->
      <a-layout-header :style="{ padding: '5px 20px', display: 'flex', justifyContent: 'flex-end', alignItems: 'center', background: '#14203b' }">
        <span class="welcome-text">欢迎您，{{ username }}</span>
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
</template>

<script setup>
import { ref, h } from 'vue'
import { useRouter, useRoute } from 'vue-router'
import { PieChartOutlined, HomeOutlined, DesktopOutlined, UserOutlined, AccountBookOutlined } from '@ant-design/icons-vue'
import { theme } from 'ant-design-vue'

const {
  token: { colorBgContainer, borderRadiusLG },
} = theme.useToken()

const collapsed = ref(false)
const selectedKeys = ref(['1'])
const breadcrumb = ref(['User', 'Bill'])
const username = ref('张三') // ✅ 这里可以替换成实际登录用户

const router = useRouter()
const route = useRoute()

// 菜单项定义
const menuItems = [
  { key: '1', icon: () => h(HomeOutlined), label: '首页', path: '/home' },
  { key: '2', icon: () => h(DesktopOutlined), label: '存取款管理', path: '/transaction' },
  { key: '3', icon: () => h(AccountBookOutlined), label: '账户管理', path: '/account' },
  { key: '4', icon: () => h(UserOutlined), label: '用户管理', path: '/user' },
]

// 菜单点击事件
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

.logo-text {
  transition: opacity 0.3s;
}

.demo-logo-vertical {
  height: 32px;
  margin: 16px;
  background: rgba(255, 255, 255, 0.3);
  border-radius: 6px;
}

/* ===== Header文字样式 ===== */
.welcome-text {
  color: #fff;
  font-size: 16px;
  font-weight: 500;
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
