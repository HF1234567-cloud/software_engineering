<template>
  <div style="text-align: center; padding: 20px">
    <a-result
      status="403"
      title="403"
      sub-title="对不起，您没有访问此页面的权限。"
    >
      <template #extra>
        <div style="display: flex; justify-content: center; gap: 10px">
          <!-- 返回首页按钮 -->
          <a-button type="default" @click="$router.push('/home')">
            返回首页
          </a-button>

          <!-- 管理员登录按钮 -->
          <a-button type="primary" @click="showLogin = true">
            管理员登录
          </a-button>
        </div>
      </template>
    </a-result>

    <!-- 管理员登录弹窗 -->
    <a-modal
      v-model:open="showLogin"
      title="管理员登录"
      ok-text="登录"
      cancel-text="取消"
      @ok="handleLogin"
    >
      <a-form :model="form" layout="vertical">
        <a-form-item label="用户名">
          <a-input v-model:value="form.username" placeholder="输入管理员账号" />
        </a-form-item>
        <a-form-item label="密码">
          <a-input-password
            v-model:value="form.password"
            placeholder="输入密码"
          />
        </a-form-item>
      </a-form>
    </a-modal>
  </div>
</template>

<script setup>
import { ref } from "vue";
import { message } from "ant-design-vue";

// 控制弹窗显示
const showLogin = ref(false);

// 表单数据
const form = ref({
  username: "",
  password: "",
});

// 模拟管理员账户
const adminAccount = {
  username: "admin",
  password: "123456",
};

// 登录逻辑
const handleLogin = () => {
  const { username, password } = form.value;

  if (
    username === adminAccount.username &&
    password === adminAccount.password
  ) {
    message.success("登录成功，正在进入管理员界面...");
    showLogin.value = false;

    // 保存管理员状态（可换成 Vuex / Pinia）
    localStorage.setItem("isAdmin", "true");

    // 跳转到管理员页面
    window.location.href = "/admin"; // 或者使用 $router.push('/admin')
  } else {
    message.error("用户名或密码错误！");
  }
};
</script>
<style scoped>
:deep(.ant-result-title),
:deep(.ant-result-subtitle) {
  color: #ffffff !important; /* 白色字体 */
}

:deep(.ant-result) {
  color: #ffffff !important;
}

/* 调整按钮区域的布局和对比度 */
:deep(.ant-btn-default) {
  background-color: transparent;
  border-color: #ffffff;
  color: #ffffff;
}
:deep(.ant-btn-default:hover) {
  background-color: rgba(255, 255, 255, 0.2);
}

:deep(.ant-btn-primary) {
  background-color: #00bcd4;
  border-color: #00bcd4;
}
:deep(.ant-btn-primary:hover) {
  background-color: #00d4f0;
}
</style>
