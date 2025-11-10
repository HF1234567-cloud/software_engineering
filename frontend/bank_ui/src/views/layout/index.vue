<script setup>
import { ref } from 'vue'
import { Menu as IconMenu, Message, Setting, Promotion, UserFilled, HomeFilled, Tools, HelpFilled, Histogram, InfoFilled, Share, List } from '@element-plus/icons-vue'
import router from '../../router'

const item = {
  date: '2016-05-02',
  name: 'Tom',
  address: 'No. 189, Grove St, Los Angeles',
}
const tableData = ref(Array.from({ length: 20 }).fill(item))

</script>

<template>
  <div class="common-layout">
    <el-header>
      <div class="toolbar-left">
        <span style="font-size: 30px;font-family:楷体;">Tilas智能学习辅助系统</span>
      </div>
      <div class="toolbar-right">
        <span><el-icon><Edit /></el-icon>修改密码</span>
        <span><el-icon><SwitchButton /></el-icon>退出登录</span>
      </div>
    </el-header>

      <el-container>
         <el-aside width="200px">
      <el-scrollbar>
        <!-- 默认打开'1'和'3'菜单:default-openeds="['1', '3']"-->
        <el-menu :router="true">
            <el-menu-item index="/home">
              <el-icon><Promotion /></el-icon>首页
            </el-menu-item>

          <el-sub-menu index="/transaction" >
            <template #title>
              <el-icon><Menu /></el-icon>班级学员管理
            </template>

           
          </el-sub-menu>

          <el-sub-menu index="/account" >
            <template #title>
              <el-icon><Tools /></el-icon>账户信息管理
            </template>

            <!-- <el-menu-item index="/dept"><el-icon><HelpFilled /></el-icon>部门管理</el-menu-item>
            <el-menu-item index="/emp"><el-icon><UserFilled /></el-icon>员工管理</el-menu-item> -->
          </el-sub-menu>

          

        </el-menu>
      </el-scrollbar>
    </el-aside>

        <el-main>
          <router-view />
      </el-main>
      </el-container>
  </div>
</template>

<style scoped>
.common-layout .el-header {
  display: flex;
  justify-content: space-between;
  background-color: #e4e2fc;
  color: var(--el-text-color-primary);
  height: 80px;
}
.toolbar-left, .toolbar-right {
  display: flex;
  align-items:center;
  gap: 20px; /* 子元素间距 */
}

.common-layout .el-aside {
  color: var(--el-text-color-primary);
  height: 100vh; /* 让侧边栏高度撑满视口高度 */
  border-right: 1px solid #0000001b; /* 在侧边栏容器上加边框 */
  box-sizing: border-box; /* 包含边框在内 */
}

.common-layout .el-menu {
  border-right: none;
}

/* 鼠标悬停菜单项背景色 */
.common-layout .el-menu-item:hover {
  background-color: #edecfd;
  color: white !important;
}

.common-layout ::v-deep .el-sub-menu__title:hover {
  background-color: #edecfd !important;
  color: white !important;
}

/* 新增：激活菜单项背景色 */
.common-layout ::v-deep .el-menu-item.is-active {
  background-color: white !important; /* 这里改成你想要的颜色 */
  color: #d3cff8 !important;
}

.common-layout .el-main {
  padding: 0;
}
.common-layout .toolbar {
  display: inline-flex;
  align-items: center;
  justify-content: center;
  height: 100%;
  right: 20px;

}


.common-layout .el-sub-menu__title:hover .el-icon {
  color: white !important;
}



</style>