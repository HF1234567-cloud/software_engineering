import { createApp } from 'vue'
import App from './App.vue'
import * as ElementPlusIconsVue from '@element-plus/icons-vue'
import ElementPlus from 'element-plus'
import zhCn from 'element-plus/es/locale/lang/zh-cn'
import 'element-plus/dist/index.css'
import router from './router'
import "element-plus/dist/index.css";
import { MotionPlugin } from "@vueuse/motion";
import axios from 'axios';
import 'element-plus/dist/index.css';
axios.defaults.withCredentials = true;  // 允许跨域携带凭证

// import './assets/main.css'

const app = createApp(App)
app.use(router)
app.use(ElementPlus, {
  locale: zhCn,
})
for (const [key, component] of Object.entries(ElementPlusIconsVue)) {
  app.component(key, component)
}
app.mount('#app')