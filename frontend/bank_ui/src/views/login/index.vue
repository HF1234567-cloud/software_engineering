<template>
  <div class="app-root">
    <div class="background"></div>

    <div class="container">
      <div class="item">
        <h2 class="logo"><i class="bx bxl-xing"></i>BankEase</h2>
        <div class="text-item">
          <h2>Welcome! <br /><span>To Our BankEase</span></h2>
          <p>Connect to your digital banking platform securely.</p>
        </div>
      </div>

      <!-- 登录注册区 -->
      <div class="login-section" :class="{ active: isRegisterActive }">
        <!-- 登录 -->
        <div class="form-box login">
          <form @submit.prevent="handleLogin">
            <h2>Sign In</h2>
            <div class="input-box">
              <span class="icon"><i class="bx bxs-user"></i></span>
              <input v-model="loginForm.username" type="text" required />
              <label>Username</label>
            </div>
            <div class="input-box">
              <span class="icon"><i class="bx bxs-lock-alt"></i></span>
              <input v-model="loginForm.password" type="password" required />
              <label>Password</label>
            </div>

            <div class="remember-password">
              <label :class="{ 'checkbox-error': !loginForm.rememberChecked && loginError }">
                <input type="checkbox" v-model="loginForm.rememberChecked" />
                Remember Me
              </label>
              <a href="#">Forget Password</a>
            </div>

            <button class="btn" type="submit">Login In</button>
            <div class="create-account">
              <p>
                Create A New Account?
                <a href="#" @click.prevent="toggleToRegister">Sign Up</a>
              </p>
            </div>
          </form>
        </div>

        <!-- 注册 -->
        <div class="form-box register">
          <form @submit.prevent="handleRegister">
            <h2>Sign Up</h2>
            <div class="input-box">
              <span class="icon"><i class="bx bxs-user"></i></span>
              <input v-model="registerForm.username" type="text" required />
              <label>Username</label>
            </div>
            <div class="input-box">
              <span class="icon"><i class="bx bx-credit-card-alt bx-rotate-90 bx-flip-vertical"></i></span>
              <input v-model="registerForm.card" type="text" required />
              <label>Card</label>
            </div>
            <div class="input-box">
              <span class="icon"><i class="bx bxs-lock-alt"></i></span>
              <input v-model="registerForm.password" type="password" required />
              <label>Password</label>
            </div>

            <div class="remember-password">
              <label :class="{ 'checkbox-error': !registerForm.agreed && registerError }">
                <input type="checkbox" v-model="registerForm.agreed" />
                I agree with this statement
              </label>
            </div>

            <button class="btn" type="submit">Sign Up</button>
            <div class="create-account">
              <p>
                Already Have An Account?
                <a href="#" @click.prevent="toggleToLogin">Sign In</a>
              </p>
            </div>
          </form>
        </div>
      </div>
    </div>
  </div>
</template>

<script setup>
import { ref } from "vue";
import axios from "axios";
import { message } from "ant-design-vue";
import "ant-design-vue/dist/reset.css"; // 确保样式加载

const isRegisterActive = ref(false);

// --- 登录表单数据 ---
const loginForm = ref({
  username: "",
  password: "",
  rememberChecked: false,
});
const loginError = ref(false);

// --- 注册表单数据 ---
const registerForm = ref({
  username: "",
  password: "",
  card: "",
  agreed: false,
});
const registerError = ref(false);

// --- 切换 ---
const toggleToRegister = () => {
  isRegisterActive.value = true;
};
const toggleToLogin = () => {
  isRegisterActive.value = false;
};

// --- 登录逻辑 ---
const handleLogin = async () => {
  if (!loginForm.value.rememberChecked) {
    loginError.value = true;
    message.warning("请勾选 'Remember Me' 后再登录！");
    return;
  }

  try {
    message.loading({ content: "正在登录中...", key: "login" });
    const res = await axios.post("http://127.0.0.1:5003/login", {
      username: loginForm.value.username,
      password: loginForm.value.password,
      lasttime: new Date().toISOString(),
    });

    if (res.status === 200) {
      localStorage.setItem('username', res.data.username)
      window.location.href = "/home";
     
    }
  } catch (err) {
    message.error({
      content: err.response?.data?.message || "网络错误，请检查服务器连接",
      key: "login",
    });
  }
};

// --- 注册逻辑 ---
const handleRegister = async () => {
  if (!registerForm.value.agreed) {
    registerError.value = true;
    message.warning("请先勾选 'I agree with this statement' 后再注册！");
    return;
  }

  try {
    message.loading({ content: "正在注册中...", key: "register" });
    const res = await axios.post("http://127.0.0.1:5003/sign", {
      username: registerForm.value.username,
      password: registerForm.value.password,
      card: registerForm.value.card,
    });

    if (res.status === 201) {
      message.success({ content: "注册成功，请登录！", key: "register" });
      registerForm.value = {
        username: "",
        password: "",
        card: "",
        agreed: false,
      };
      registerError.value = false;
      isRegisterActive.value = false;
     
    }
  } catch (err) {
    message.error({
      content: err.response?.data?.message || "网络错误，请检查服务器连接",
      key: "register",
    });
  }
};
</script>


<style scoped>
@import url("https://fonts.googleapis.com/css2?family=Poppins:ital,wght@0,300;0,400;0,500;1,500&display=swap");
@import url("https://unpkg.com/boxicons@2.1.4/css/boxicons.min.css");
.checkbox-error {
  color: #ff4444;
  animation: shake 0.3s ease-in-out;
}

@keyframes shake {
  0% { transform: translateX(0); }
  25% { transform: translateX(-4px); }
  50% { transform: translateX(4px); }
  75% { transform: translateX(-4px); }
  100% { transform: translateX(0); }
}

* {
  margin: 0;
  padding: 0;
  box-sizing: border-box;
  font-family: "Poppins", sans-serif;
}

/* 最外层容器：控制整体层级，避免背景穿透 */
.app-root {
  position: relative;
  min-height: 100vh;
  overflow: hidden;
}

body {
  height: 100vh;
  width: 100%;
  background: #000;
}

/* 1. 还原背景模糊效果 + 层级（放在最底层，仅做模糊背景） */
.background {
  position: absolute; /* 绝对定位，脱离文档流 */
  top: 0;
  left: 0;
  width: 100%;
  height: 100%;
  background: url("@/assets/01.jpg") no-repeat; /* 统一图片路径（与container一致） */
  background-position: center;
  background-size: cover;
  filter: blur(10px);/* 恢复原模糊效果 */
  z-index: 1; /* 背景层层级最低 */
  transform: scale(1.05); /*避免模糊边缘出现空白 */ 
}

/* 2. 导航栏：保持原样式，层级高于背景 */
.header {
  position: fixed;
  top: 0;
  left: 0;
  width: 100%;
  padding: 25px 13%;
  background: transparent;
  display: flex;
  justify-content: space-between;
  align-items: center;
  z-index: 100; /* 导航栏层级最高，不被遮挡 */
}

.navbar a {
  position: relative;
  font-size: 16px;
  color: #fff;
  margin-right: 30px;
  text-decoration: none;
}
.navbar a::after {
  content: "";
  position: absolute;
  left: 0;
  width: 100%;
  height: 2px;
  background: #fff;
  bottom: -5px;
  border-radius: 5px;
  transform: translateY(10px);
  opacity: 0;
  transition: 0.5s ease;
}
.navbar a:hover::after {
  transform: translateY(0);
  opacity: 1;
}

.search-bar {
  width: 250px;
  height: 45px;
  background-color: transparent;
  border: 2px solid #fff;
  border-radius: 6px;
  display: flex;
  align-items: center;
}
.search-bar input {
  width: 100%;
  background-color: transparent;
  border: none;
  outline: none;
  color: #fff;
  font-size: 16px;
  padding-left: 10px;
}
.search-bar button {
  width: 40px;
  height: 100%;
  background: transparent;
  outline: none;
  border: none;
  color: #fff;
  cursor: pointer;
}
.search-bar input::placeholder {
  color: #fff;
}
.search-bar button i {
  font-size: 22px;
}

/* 3. 主容器：还原原视觉效果，层级高于背景 */
.container {
  position: absolute;
  left: 50%;
  top: 50%;
  transform: translate(-50%, -50%);
  width: 80%;
  height: 500px;
  margin-top: 20px;
  background: url("@/assets/01.jpg") no-repeat; /* 统一图片路径，与背景图一致 */
  background-position: center;
  background-size: cover;
  border-radius: 20px;
  overflow: hidden;
  z-index: 10; /* 主容器层级高于背景，低于导航栏 */
  box-shadow: 0 0 50px rgba(0, 0, 0, 0.5); /* 还原原阴影质感 */
}

/* 4. 左侧信息区：保持原样式 */
.item {
  position: absolute;
  top: 0;
  left: 0;
  width: 58%;
  height: 100%;
  color: #fff;
  background: transparent;
  padding: 80px;
  display: flex;
  justify-content: space-between;
  flex-direction: column;
  text-shadow: 0 2px 4px rgba(0, 0, 0, 0.5); /* 增强文字可读性，还原原效果 */
}
.item .logo {
  color: #fff;
  font-size: 30px;
}

.text-item h2 {
  font-size: 40px;
  line-height: 1;
}
.text-item p {
  font-size: 16px;
  margin: 20px 0;
}
.social-icon a i {
  color: #fff;
  font-size: 24px;
  margin-left: 10px;
  cursor: pointer;
  transition: 0.5s ease;
}
.social-icon a:hover i {
  transform: scale(1.2);
}

/* 5. 登录注册区：还原原透明模糊质感 + 动画 */
.container .login-section {
  position: absolute;
  top: 0;
  right: 0;
  width: calc(100% - 58%);
  height: 100%;
  color: #fff;
  backdrop-filter: blur(10px); /* 还原原透明模糊效果 */
  background: rgba(0, 0, 0, 0.1); /* 增强背景遮罩，还原原视觉层次 */
}
.login-section .form-box {
  position: absolute;
  display: flex;
  justify-content: center;
  align-items: center;
  width: 100%;
  height: 100%;
}

/* 还原表单切换动画（原位移逻辑不变） */
.login-section .form-box.register {
  transform: translateX(430px);
  transition: transform 0.6s ease;
  transition-delay: 0s;
}
.login-section.active .form-box.register {
  transform: translateX(0px);
  transition-delay: 0.7s;
}

.login-section .form-box.login {
  transform: translateX(0px);
  transition: transform 0.6s ease;
  transition-delay: 0.7s;
}
.login-section.active .form-box.login {
  transform: translateX(430px);
  transition-delay: 0s;
}

.login-section .form-box h2 {
  text-align: center;
  font-size: 25px;
  margin-bottom: 30px; /* 还原原间距 */
}

/* 6. 输入框：还原原交互效果 */
.form-box .input-box {
  width: 340px;
  height: 50px;
  border-bottom: 2px solid #fff;
  margin: 30px 0;
  position: relative;
}
.input-box input {
  width: 100%;
  height: 100%;
  background: transparent;
  border: none;
  outline: none;
  font-size: 16px;
  padding-right: 28px;
  color: #fff; /* 确保输入文字为白色，还原原效果 */
}
.input-box label {
  position: absolute;
  top: 50%;
  left: 0;
  transform: translateY(-50%);
  font-size: 16px;
  font-weight: 600; /* 还原原字体粗细 */
  pointer-events: none;
  transition: 0.5s ease;
  color: #fff; /* 确保标签文字为白色 */
}
.input-box .icon {
  position: absolute;
  top: 13px;
  right: 0;
  font-size: 19px;
  color: #fff; /* 确保图标为白色 */
}
/* 输入框聚焦/有值时的标签动画（还原原效果） */
.input-box input:focus ~ label,
.input-box input:valid ~ label {
  top: -5px;
  font-size: 14px; /* 还原原标签缩小效果 */
}

/* 7. 勾选区域：保持原样式 */
.remember-password {
  font-size: 14px;
  font-weight: 500;
  margin: -15px 0 15px;
  display: flex;
  justify-content: space-between;
}
.remember-password label input {
  accent-color: #fff;
  margin-right: 3px;
}
.remember-password a {
  color: #fff;
  text-decoration: none;
}
.remember-password a:hover {
  text-decoration: underline;
}

/* 8. 按钮：还原原颜色和质感 */
.btn {
  background: #3a2df7; /* 还原原按钮红色 */
  width: 100%;
  height: 45px;
  outline: none;
  border: none;
  border-radius: 4px;
  cursor: pointer;
  font-size: 16px;
  color: #fff;
  box-shadow: rgba(0, 0, 0, 0.4); /* 还原原阴影 */
  transition: background 0.3s ease; /* 新增按钮 hover 过渡，还原原交互感 */
}
.btn:hover {
  background: #421ed3; /* 按钮 hover 深色变化，还原原质感 */
}

/* 9. 切换账号文本：保持原样式 */
.create-account {
  font-size: 14.5px;
  text-align: center;
  margin: 25px;
}
.create-account p a {
  color: #fff;
  font-weight: 600;
  text-decoration: none;
}
.create-account p a:hover {
  text-decoration: underline;
}
</style>