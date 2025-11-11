<template>
  <div class="home has-animations">
    <!-- === Hero Section === -->
    <section
      class="hero animate__animated"
      :class="{ animate__fadeInDown: heroVisible }"
    >
      <div class="hero-inner">
        <!-- 左侧内容 -->
        <div class="hero-copy">
          <h1 class="hero-title">
            BankEase储蓄系统
          </h1>
          <p class="hero-paragraph">
            一站式银行服务平台，集成智能问答与数据可视化，提升您的金融体验。
          </p>
          <div class="hero-cta">
            <el-button type="primary" @click="scrollToFeatures">立即体验</el-button>
          </div>
        </div>
        

        <!-- 右侧动画方块 -->
        <div ref="figureRef" class="home-figure" :class="{ 'anime-ready': ready }">
          <div class="home-figure-box home-figure-box-01"></div>
          <div class="home-figure-box home-figure-box-02"></div>
          <div class="home-figure-box home-figure-box-03"></div>
          <div class="home-figure-box home-figure-box-04"></div>
          <div class="home-figure-box home-figure-box-05"></div>
          <div class="home-figure-box home-figure-box-06"></div>
          <div class="home-figure-box home-figure-box-07"></div>
          <!-- 方块内交互按钮 -->
          <div class="home-figure-box home-figure-box-routes">
            <el-button class="btn--blue hero-btn" type="info" size="large" @click="goTo('/transaction')"> 存取款管理 </el-button>
            <el-button class="btn--blue hero-btn" type="info" size="large" @click="goTo('/account')"> 账户管理 </el-button>
            <el-button class="btn--blue hero-btn" type="info" size="large" @click="goTo('/user')"> 用户管理 </el-button>
          </div>
     
        </div>
        <!-- 右侧功能按钮 -->
       
      </div>
    </section>
    

    <!-- === Features Section === -->
    <section class="features" ref="featureRef">
      <div
        v-for="(feature, index) in features"
        :key="index"
        class="feature-card"
        v-motion="{
          initial: { opacity: 0, y: 40 },
          visibleOnce: { opacity: 1, y: 0, transition: { delay: index * 0.2 } },
        }"
      >
        <div class="feature-icon">{{ feature.icon }}</div>
        <h3 class="feature-title">{{ feature.title }}</h3>
        <p class="feature-desc">{{ feature.desc }}</p>
      </div>
    </section>
  </div>
</template>

<script setup>
import { ref, onMounted } from "vue";
import { useRouter } from "vue-router";
import "animate.css";
import { useMotion } from "@vueuse/motion";

const router = useRouter();
const heroVisible = ref(false);
const ready = ref(false);
const figureRef = ref(null);
const featureRef = ref(null);
const goTo = (path) => {
  router.push(path); // 调用路由的push方法跳转页面
};
const features = [
  { icon: "🧠", title: "智能问答引擎", desc: "基于知识图谱的自然语言查询与推理系统，快速定位医疗关联信息。" },
  { icon: "📊", title: "数据可视化", desc: "提供3D词云展示，直观呈现医疗数据趋势。" },
  { icon: "🕸️", title: "支持多轮问答", desc: "可以连续进行多轮问答，获取更详细的医疗信息。" },
];

const scrollToFeatures = () => {
  featureRef.value?.scrollIntoView({ behavior: "smooth" });
};

onMounted(() => {
  heroVisible.value = true;
  setTimeout(() => (ready.value = true), 200);

});
</script>

<style lang="less" scoped>
.home {
  font-family: "Microsoft YaHei", sans-serif;
  background:radial-gradient(circle at 20% 30%, #0F172A, #060C1A 80%);
  color: #F8FAFC;
  min-height: 100vh;
  overflow-x: hidden;
}
.btn--blue { background: #6e99d6; color: white; }
/* === Hero Section === */
.hero {
  height: 90vh;
  display: flex;
  align-items: flex-start; /* 从居中改为顶部对齐 */
  justify-content: center;
  background-image: url('@/assets/02.jpg');
  background-size: cover;
  padding-top: 10px; /* 加上内边距控制距离顶部的高度 */
  box-sizing: border-box;
}

.hero-inner {
  display: flex;
  align-items: center;
  justify-content: space-between;
  gap: 40px;
  max-width: 1200px;
  width: 90%;
}

.hero-copy {
  flex: 1.2;
}

.hero-title {
  font-size: 48px;
  font-weight: 700;
  color: #00E0FF;
  text-shadow: 0 0 20px rgba(0, 224, 255, 0.5);
  margin-bottom: 16px;

  &-highlight {
    color: #91aeec;
  }
}

.hero-paragraph {
  font-size: 18px;
  color:  #CBD5E1;
  margin-bottom: 24px;
}
/* 右侧功能按钮样式 */
.hero-right {
  display: flex;
  flex-direction: column;
  align-items: flex-end;
  gap: 20px;
  padding: 20px;
}

.hero-cta .el-button {
  border-radius: 30px;
  font-size: 16px;
  padding: 20px 28px;
  background: linear-gradient(90deg, #0072FF, #00E0FF);
  box-shadow: 0 0 15px rgba(0, 224, 255, 0.5);
  transition: all 0.3s ease;
}
.hero-cta .el-button:hover {
  box-shadow: 0 0 25px rgba(0, 224, 255, 0.9);
  transform: translateY(-2px);
}

/* === 动画方块部分 === */
.home-figure {
  position: relative;
  flex-shrink: 0;
  width: 400px;
  height: 400px;
  perspective: 800px;
}

/* === 方块基底样式 === */
.home-figure-box {
  position: absolute;
  border-radius: 12px;
  backdrop-filter: blur(18px);
  border: 1px solid rgba(255, 255, 255, 0.08);
  box-shadow: 0 4px 24px rgba(0, 224, 255, 0.15);
  transition: all 0.6s ease;
}

/* 悬浮时轻微放大 + 光晕 */
.home-figure-box:not(.home-figure-box-routes):hover {
  transform: scale(1.05);
  box-shadow: 0 0 24px rgba(0, 224, 255, 0.4);
}


.home-figure-box-routes {
  display: flex;
  flex-direction: column;
  gap: 12px;
  padding: 16px;
  align-items: flex-end;
  z-index: 99; /* 确保在所有方块之上 */
  left: 40%;
  top: 40%;
  transform: translate(-50%, -50%);
  min-width: 160px;
  text-align: center;
  animation: fadeIn 0.8s ease forwards 1.2s;
  
  /* 核心：清除继承的方块样式 */
  background: transparent !important;
  border: none !important;
  box-shadow: none !important;
  backdrop-filter: none !important;
}
.hero-btn {
  width: 170px;
  border-radius: 10px;
  font-weight: 600;
  background: linear-gradient(90deg, #0072FF, #00E0FF);
  color: white;
  border: none;
  transition: all 0.3s ease;
}


/* 方块1：浅蓝到更浅蓝渐变，适配背景 */
.home-figure-box-01 {
  width: 180px;
  height: 160px;
  background: rgba(0, 114, 255, 0.15); // 主色渐变
  left: 60%;
  top: 10%;
}

.home-figure-box-02 {
  width: 120px;
  height: 120px;
  background: rgba(0, 224, 255, 0.2); // 深蓝渐变
  left: 5%;
  top: 40%;
}

.home-figure-box-03 {
  width: 120px;
  height: 100px;
  background:rgba(0, 224, 255, 0.25); // 浅蓝，增加层次
  left: 30%;
  top: 55%;
}
// /* 方块4：选取背景中的稍深一点的蓝 */
.home-figure-box-04 {
  width: 80px;
  height: 80px;
  background:rgba(0, 114, 255, 0.3);
  left: 80%;
  top: 60%;
}
// /* 方块5：较浅的蓝，适配背景 */
.home-figure-box-05 {
  width: 200px;
  height: 160px;
  background: rgba(0, 114, 255, 0.1);
  left: 15%;
  top: 20%;
}
// /* 方块6：选取背景中的蓝紫色调，更协调 */
.home-figure-box-06 {
  width: 100px;
  height: 80px;
  background: rgba(0, 224, 255, 0.2);
  left: 85%;
  top: 35%;
}
/* 方块7：选取背景中的蓝，增加层次感 */
.home-figure-box-07 {
  width: 100px;
  height: 80px;
  background: rgba(0, 224, 255, 0.3);
  left: 45%;
  top: 75%;
}

/* === 动画触发 === */
.has-animations .anime-ready .home-figure-box {
  animation: rotateScaleOpacity 1.2s ease forwards;
}

@keyframes rotateScaleOpacity {
  0% {
    transform: rotateZ(0deg) scale(0.5);
    opacity: 0;
  }
  100% {
    transform: rotateZ(360deg) scale(1);
    opacity: 1;
  }
}

/* === Features === */
.features {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(260px, 1fr));
  gap: 24px;
  padding: 80px 40px;
  background: #1E293B; // 深灰背景，与主背景分层
  color: #F8FAFC; // 浅灰文字

  .feature-card {
    background: #0F172A; // 深黑蓝卡片，与背景区分
    border-radius: 16px;
    box-shadow: 0 6px 16px rgba(0, 0, 0, 0.2); // 深色阴影更协调
    padding: 32px 24px;
    transition: all 0.8s ease;

    &:hover {
      transform: translateY(-6px);
      box-shadow: 0 10px 20px rgba(0, 224, 255, 0.3); // 霓虹蓝阴影，强化交互
    }

    .feature-icon {
      font-size: 40px;
      color: #00E0FF; // 霓虹蓝图标，突出功能
      margin-bottom: 16px;
    }
    .feature-title {
      font-size: 22px;
      font-weight: 600;
      color: #F8FAFC;
      margin-bottom: 10px;
    }
    .feature-desc {
      color: #CBD5E1; // 中灰描述，次要信息弱化
      font-size: 15px;
    }
  }
}
</style>
