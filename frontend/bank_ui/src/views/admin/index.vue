<template>
  <div class="admin-container">
    <h1 class="page-title"><i class="bx bxs-cog"></i> 管理员控制台</h1>

    <!-- 统计图表区域 -->
    <div class="stats-container">
      <a-card class="chart-card" bordered={false}>
        <template #title>
          <span class="card-title">账户状态统计</span>
        </template>
        <div ref="pieChartRef" class="chart-container"></div>
      </a-card>

      <a-card class="chart-card" bordered={false}>
        <template #title>
          <span class="card-title">每日交易统计</span>
        </template>
        <div ref="lineChartRef" class="chart-container"></div>
      </a-card>
    </div>

    <!-- 待审核申请卡 -->
    <a-card class="pending-card" bordered={false}>
      <template #title>
        <span class="card-title">待审核申请</span>
      </template>
      
      <!-- 搜索和筛选区域 -->
      <div class="search-filters">
        <a-input 
          v-model:value="searchUsername" 
          placeholder="搜索用户名" 
          style="width: 200px; margin-right: 12px;"
          @pressEnter="handleSearch"
        />
        <a-select 
          v-model:value="filterType" 
          placeholder="申请类型" 
          style="width: 150px; margin-right: 12px;"
          allow-clear
        >
          <a-select-option value="挂失">挂失</a-select-option>
          <a-select-option value="销户">销户</a-select-option>
          <a-select-option value="冻结">冻结</a-select-option>
        </a-select>
        <a-range-picker 
          v-model:value="dateRange" 
          style="margin-right: 12px;"
          placeholder="选择日期范围"
        />
        <a-button type="primary" @click="handleSearch" style="margin-right: 8px;">搜索</a-button>
        <a-button @click="resetFilters">重置</a-button>
      </div>

      <a-table
        :data-source="filteredPendingApplies"
        :columns="pendingColumns"
        row-key="apply_id"
        bordered
        :pagination="{
          current: currentPage,
          pageSize: pageSize,
          total: filteredPendingApplies.length,
          showSizeChanger: true,
          showQuickJumper: true,
          pageSizeOptions: ['5', '10', '20', '50'],
          onChange: handlePageChange,
          onShowSizeChange: handlePageSizeChange
        }"
        :loading="loadingPending"
        style="margin-top: 16px;"
      >
        <!-- 账户余额列：自定义渲染 -->
        <template #bodyCell="props">
          <template v-if="props.column.dataIndex === 'balance'">
            ¥{{ props.record.balance.toFixed(2) }}
          </template>
          <!-- 申请时间列 -->
          <template v-else-if="props.column.dataIndex === 'apply_time'">
            {{ formatDate(props.record.apply_time) }}
          </template>
          <!-- 操作列：自定义渲染 -->
          <template v-else-if="props.column.dataIndex === 'operation'">
            <div class="operate-buttons">
              <a-button 
                type="primary" 
                size="small" 
                @click="handleAudit(props.record, '通过')"
                style="margin-right: 8px;"
              >
                同意
              </a-button>
              <a-button 
                danger 
                size="small" 
                @click="handleAudit(props.record, '驳回')"
              >
                驳回
              </a-button>
            </div>
          </template>
        </template>
      </a-table>

      <!-- 批量操作按钮组 -->
      <div class="batch-operation-section">
        <div class="button-group">
          <a-button type="primary" shape="round" @click="handleBatchLoss">
            批量同意挂失
          </a-button>
          <a-button danger shape="round" @click="handleBatchClose">
            批量同意销户
          </a-button>
          <a-button type="dashed" shape="round" @click="handleBatchFreeze">
            批量同意冻结
          </a-button>
          <a-button type="default" shape="round" @click="refreshPending">
            刷新待审核列表
          </a-button>
        </div>
      </div>
    </a-card>

    <!-- 用户信息表 -->
    <a-card class="table-card" bordered={false}>
      <template #title>
        <span class="card-title">用户信息列表</span>
      </template>
      <a-table
        :data-source="users"
        :columns="userColumns"
        row-key="id"
        bordered
        :pagination="{ pageSize: 5 }"
        :loading="loadingUsers"
      >
        <!-- 自定义列渲染 -->
        <template #bodyCell="props">
          <!-- 账户状态列 -->
          <template v-if="props.column.dataIndex === 'isvalid'">
            <span :style="getStatusColor(props.record.isvalid)">{{ props.record.isvalid }}</span>
          </template>
          <!-- 账户余额列 -->
          <template v-else-if="props.column.dataIndex === 'balance'">
            ¥{{ props.record.balance.toFixed(2) }}
          </template>
          <!-- 最后登录时间列 -->
          <template v-else-if="props.column.dataIndex === 'lasttime'">
            {{ formatDate(props.record.lasttime) }}
          </template>
        </template>
      </a-table>
    </a-card>
  </div>
</template>

<script setup>
import { ref, onMounted, computed, nextTick } from "vue";
import { message, Modal, Spin } from "ant-design-vue";
import axios from "axios";
import dayjs from 'dayjs';
import * as echarts from 'echarts';

// 状态管理
const pendingApplies = ref([]); // 待审核申请列表
const users = ref([]); // 所有用户列表
const adminName = ref(localStorage.getItem('adminName') || 'admin'); // 管理员用户名
const loadingPending = ref(false); // 待审核列表加载状态
const loadingUsers = ref(false); // 用户列表加载状态

// 图表引用
const pieChartRef = ref(null);
const lineChartRef = ref(null);
let pieChart = null;
let lineChart = null;

// 分页状态
const currentPage = ref(1);
const pageSize = ref(5);

// 搜索和筛选状态
const searchUsername = ref('');
const filterType = ref('');
const dateRange = ref([]);

// 待审核申请表格列配置
const pendingColumns = [
  { title: "申请ID", dataIndex: "apply_id", width: 80 },
  { title: "用户名", dataIndex: "username", width: 120 },
  { title: "银行卡号", dataIndex: "card", width: 180 },
  { title: "申请类型", dataIndex: "apply_type", width: 100 },
  { title: "申请时间", dataIndex: "apply_time", width: 180 },
  { title: "当前状态", dataIndex: "current_status", width: 100 },
  { title: "账户余额", dataIndex: "balance", width: 100 },
  { title: "操作", dataIndex: "operation", width: 220 }
];

// 用户列表表格列配置
const userColumns = [
  { title: "用户ID", dataIndex: "id", width: 80 },
  { title: "用户名", dataIndex: "username", width: 120 },
  { title: "银行卡号", dataIndex: "card", width: 180 },
  { title: "账户状态", dataIndex: "isvalid", width: 120 },
  { title: "账户余额", dataIndex: "balance", width: 120 },
  { title: "最后登录时间", dataIndex: "lasttime", width: 180 }
];

// 格式化日期
const formatDate = (dateString) => {
  if (!dateString) return '';
  return dayjs(dateString).format('YYYY-MM-DD HH:mm:ss');
};

// 计算筛选后的待审核申请列表
const filteredPendingApplies = computed(() => {
  let result = pendingApplies.value;

  // 按用户名搜索
  if (searchUsername.value) {
    result = result.filter(item => 
      item.username.toLowerCase().includes(searchUsername.value.toLowerCase())
    );
  }

  // 按申请类型筛选
  if (filterType.value) {
    result = result.filter(item => item.apply_type === filterType.value);
  }

  // 按日期范围筛选
  if (dateRange.value && dateRange.value.length === 2) {
    const start = dayjs(dateRange.value[0]);
    const end = dayjs(dateRange.value[1]);
    result = result.filter(item => {
      const applyTime = dayjs(item.apply_time);
      return applyTime.isAfter(start) && applyTime.isBefore(end);
    });
  }

  return result;
});

// 账户状态颜色计算
const getStatusColor = (status) => {
  switch(status) {
    case '使用中': return { color: '#00ffb7' };
    case '处理中': return { color: '#999' };
    case '挂失': return { color: '#ffcc00' };
    case '冻结': return { color: '#00c2ff' };
    case '已销户': return { color: '#ff4d4f' };
    default: return { color: '#fff' };
  }
};

// 初始化图表
const initCharts = () => {
  nextTick(() => {
    // 初始化饼图
    if (pieChartRef.value) {
      pieChart = echarts.init(pieChartRef.value);
      const pieOption = {
        tooltip: {
          trigger: 'item'
        },
        legend: {
          top: '5%',
          left: 'center'
        },
        series: [
          {
            name: '账户状态',
            type: 'pie',
            radius: ['40%', '70%'],
            avoidLabelOverlap: false,
            itemStyle: {
              borderRadius: 10,
              borderColor: '#fff',
              borderWidth: 2
            },
            label: {
              show: false,
              position: 'center'
            },
            emphasis: {
              label: {
                show: true,
                fontSize: 20,
                fontWeight: 'bold'
              }
            },
            labelLine: {
              show: false
            },
            data: [
              { value: calculateStatusCount('使用中'), name: '使用中' },
              { value: calculateStatusCount('挂失'), name: '挂失' },
              { value: calculateStatusCount('冻结'), name: '冻结' },
              { value: calculateStatusCount('已销户'), name: '已销户' }
            ]
          }
        ]
      };
      pieChart.setOption(pieOption);
    }

    // 初始化折线图
    if (lineChartRef.value) {
      lineChart = echarts.init(lineChartRef.value);
      const lineOption = {
        title: {
          text: '每日交易统计'
        },
        tooltip: {
          trigger: 'axis'
        },
        legend: {
          data: ['存款', '取款']
        },
        grid: {
          left: '3%',
          right: '4%',
          bottom: '3%',
          containLabel: true
        },
        toolbox: {
          feature: {
            saveAsImage: {}
          }
        },
        xAxis: {
          type: 'category',
          boundaryGap: false,
          data: ['周一', '周二', '周三', '周四', '周五', '周六', '周日']
        },
        yAxis: {
          type: 'value',
          axisLabel: {
            formatter: '¥{value}'
          }
        },
        series: [
          {
            name: '存款',
            type: 'line',
            stack: '交易',
            data: [12000, 13200, 10100, 13400, 9000, 23000, 21000],
            itemStyle: {
              color: '#52c41a'
            }
          },
          {
            name: '取款',
            type: 'line',
            stack: '交易',
            data: [8000, 9200, 11100, 8400, 12000, 13000, 10000],
            itemStyle: {
              color: '#f5222d'
            }
          }
        ]
      };
      lineChart.setOption(lineOption);
    }
  });
};

// 计算状态数量
const calculateStatusCount = (status) => {
  return users.value.filter(user => user.isvalid === status).length;
};

// 页面加载时初始化数据
onMounted(() => {
  fetchPendingApplies(); // 获取待审核申请
  fetchAllUsers(); // 获取所有用户信息
  initCharts(); // 初始化图表
});

// 处理分页变化
const handlePageChange = (page, pageSize) => {
  currentPage.value = page;
  pageSize.value = pageSize;
};

// 处理每页大小变化
const handlePageSizeChange = (current, size) => {
  currentPage.value = current;
  pageSize.value = size;
};

// 处理搜索
const handleSearch = () => {
  currentPage.value = 1; // 搜索时重置到第一页
};

// 重置筛选条件
const resetFilters = () => {
  searchUsername.value = '';
  filterType.value = '';
  dateRange.value = [];
  currentPage.value = 1;
};

// 刷新待审核列表
const refreshPending = () => {
  fetchPendingApplies();
  message.info('正在刷新待审核列表...');
  // 重新初始化图表
  setTimeout(() => {
    if (pieChart) {
      pieChart.dispose();
    }
    if (lineChart) {
      lineChart.dispose();
    }
    initCharts();
  }, 500);
};

// 获取待审核申请列表
const fetchPendingApplies = async () => {
  loadingPending.value = true;
  try {
    const res = await axios.get('http://127.0.0.1:5003/get_pending_applies', {
      timeout: 5000
    });
    pendingApplies.value = res.data || [];
    if (pendingApplies.value.length === 0) {
      message.success('暂无待审核申请');
    }
  } catch (err) {
    message.error(err.response?.data?.message || '获取待审核申请失败');
    pendingApplies.value = [];
  } finally {
    loadingPending.value = false;
  }
};

// 获取所有用户信息
const fetchAllUsers = async () => {
  loadingUsers.value = true;
  try {
    const res = await axios.get('http://127.0.0.1:5003/get_all_users', {
      timeout: 5000
    });
    users.value = res.data || [];
  } catch (err) {
    message.error(err.response?.data?.message || '获取用户列表失败');
    users.value = [];
  } finally {
    loadingUsers.value = false;
  }
};

// 单个申请审核（同意/驳回）
const handleAudit = async (record, auditResult) => {
  const applyId = record.apply_id;
  const applyType = record.apply_type;
  const username = record.username;

  Modal.confirm({
    title: auditResult === '通过' ? '确认同意申请' : '确认驳回申请',
    content: `你确定要${auditResult}用户【${username}】的${applyType}申请吗？`,
    okText: '确认',
    cancelText: '取消',
    okButtonProps: auditResult === '通过' ? {} : { danger: true },
    onOk: async () => {
      try {
        const res = await axios.post('http://127.0.0.1:5003/audit_apply', {
          apply_id: applyId,
          audit_result: auditResult,
          auditor: adminName.value
        });
        message.success(res.data.message);
        // 审核后刷新列表
        fetchPendingApplies();
        fetchAllUsers();
        // 重新初始化图表
        setTimeout(() => {
          if (pieChart) {
            pieChart.dispose();
          }
          if (lineChart) {
            lineChart.dispose();
          }
          initCharts();
        }, 500);
      } catch (err) {
        message.error(err.response?.data?.message || `${auditResult}申请失败`);
      }
    }
  });
};

// 批量同意挂失
const handleBatchLoss = async () => {
  const lossApplies = pendingApplies.value.filter(item => 
    item.apply_type === '挂失' && 
    (!searchUsername.value || item.username.toLowerCase().includes(searchUsername.value.toLowerCase())) &&
    (!filterType.value || item.apply_type === filterType.value) &&
    (!dateRange.value || dateRange.value.length !== 2 || 
      (dayjs(item.apply_time).isAfter(dayjs(dateRange.value[0])) && 
       dayjs(item.apply_time).isBefore(dayjs(dateRange.value[1]))))
  );
  
  if (lossApplies.length === 0) {
    message.warning('暂无符合条件的待审核挂失申请');
    return;
  }

  Modal.confirm({
    title: '批量同意挂失',
    content: `共发现${lossApplies.length}条符合条件的待审核挂失申请，确定全部同意吗？`,
    okText: '确认批量同意',
    cancelText: '取消',
    onOk: async () => {
      try {
        // 批量审核（循环调用审核接口，实际项目可优化为批量接口）
        let successCount = 0;
        for (const apply of lossApplies) {
          await axios.post('http://127.0.0.1:5003/audit_apply', {
            apply_id: apply.apply_id,
            audit_result: '通过',
            auditor: adminName.value
          });
          successCount++;
        }
        message.success(`批量同意成功！共处理${successCount}条申请`);
        fetchPendingApplies();
        fetchAllUsers();
        // 重新初始化图表
        setTimeout(() => {
          if (pieChart) {
            pieChart.dispose();
          }
          if (lineChart) {
            lineChart.dispose();
          }
          initCharts();
        }, 500);
      } catch (err) {
        message.error('批量审核失败，请重试');
      }
    }
  });
};

// 批量同意销户
const handleBatchClose = async () => {
  const closeApplies = pendingApplies.value.filter(item => 
    item.apply_type === '销户' && 
    (!searchUsername.value || item.username.toLowerCase().includes(searchUsername.value.toLowerCase())) &&
    (!filterType.value || item.apply_type === filterType.value) &&
    (!dateRange.value || dateRange.value.length !== 2 || 
      (dayjs(item.apply_time).isAfter(dayjs(dateRange.value[0])) && 
       dayjs(item.apply_time).isBefore(dayjs(dateRange.value[1]))))
  );
  
  if (closeApplies.length === 0) {
    message.warning('暂无符合条件的待审核销户申请');
    return;
  }

  Modal.confirm({
    title: '批量同意销户',
    content: `⚠️ 警告：销户为不可恢复操作！共发现${closeApplies.length}条符合条件的待审核销户申请，确定全部同意吗？`,
    okText: '确认批量同意',
    cancelText: '取消',
    okButtonProps: { danger: true },
    onOk: async () => {
      try {
        let successCount = 0;
        for (const apply of closeApplies) {
          await axios.post('http://127.0.0.1:5003/audit_apply', {
            apply_id: apply.apply_id,
            audit_result: '通过',
            auditor: adminName.value
          });
          successCount++;
        }
        message.success(`批量同意成功！共处理${successCount}条申请`);
        fetchPendingApplies();
        fetchAllUsers();
        // 重新初始化图表
        setTimeout(() => {
          if (pieChart) {
            pieChart.dispose();
          }
          if (lineChart) {
            lineChart.dispose();
          }
          initCharts();
        }, 500);
      } catch (err) {
        message.error('批量审核失败，请重试');
      }
    }
  });
};

// 批量同意冻结
const handleBatchFreeze = async () => {
  const freezeApplies = pendingApplies.value.filter(item => 
    item.apply_type === '冻结' && 
    (!searchUsername.value || item.username.toLowerCase().includes(searchUsername.value.toLowerCase())) &&
    (!filterType.value || item.apply_type === filterType.value) &&
    (!dateRange.value || dateRange.value.length !== 2 || 
      (dayjs(item.apply_time).isAfter(dayjs(dateRange.value[0])) && 
       dayjs(item.apply_time).isBefore(dayjs(dateRange.value[1]))))
  );
  
  if (freezeApplies.length === 0) {
    message.warning('暂无符合条件的待审核冻结申请');
    return;
  }

  Modal.confirm({
    title: '批量同意冻结',
    content: `共发现${freezeApplies.length}条符合条件的待审核冻结申请，确定全部同意吗？`,
    okText: '确认批量同意',
    cancelText: '取消',
    onOk: async () => {
      try {
        let successCount = 0;
        for (const apply of freezeApplies) {
          await axios.post('http://127.0.0.1:5003/audit_apply', {
            apply_id: apply.apply_id,
            audit_result: '通过',
            auditor: adminName.value
          });
          successCount++;
        }
        message.success(`批量同意成功！共处理${successCount}条申请`);
        fetchPendingApplies();
        fetchAllUsers();
        // 重新初始化图表
        setTimeout(() => {
          if (pieChart) {
            pieChart.dispose();
          }
          if (lineChart) {
            lineChart.dispose();
          }
          initCharts();
        }, 500);
      } catch (err) {
        message.error('批量审核失败，请重试');
      }
    }
  });
};

// 监听窗口大小变化，调整图表大小
window.addEventListener('resize', () => {
  if (pieChart) {
    pieChart.resize();
  }
  if (lineChart) {
    lineChart.resize();
  }
});
</script>

<style scoped>
.admin-container {
  padding: 30px;
  color: #fff;
  background: linear-gradient(145deg, #0f172a, #1e293b);
  min-height: 100vh;
  border-radius: 16px;
  box-shadow: 0 0 20px rgba(0, 255, 255, 0.15);
}

/* 页面标题 */
.page-title {
  color: #00e0ff;
  font-size: 26px;
  font-weight: 600;
  margin-bottom: 20px;
  display: flex;
  align-items: center;
  gap: 10px;
}

/* 统计图表容器 */
.stats-container {
  display: flex;
  gap: 24px;
  margin-bottom: 24px;
}

.chart-card {
  background: rgba(255, 255, 255, 0.05);
  color: #fff;
  border-radius: 12px;
  backdrop-filter: blur(10px);
  flex: 1;
}

.chart-card :deep(.ant-card-head) {
  border-bottom: 1px solid rgba(255, 255, 255, 0.1);
}

.card-title {
  color: #00d8ff;
  font-weight: 600;
}

.chart-container {
  height: 300px;
  width: 100%;
}

/* 卡片基础样式 */
.pending-card,
.table-card {
  background: rgba(255, 255, 255, 0.05);
  color: #fff;
  margin-bottom: 24px;
  border-radius: 12px;
  backdrop-filter: blur(10px);
}

/* 搜索和筛选区域 */
.search-filters {
  display: flex;
  flex-wrap: wrap;
  gap: 12px;
  align-items: center;
  padding: 15px 0;
}

/* 批量操作区域 */
.batch-operation-section {
  margin-top: 20px;
  padding-top: 20px;
  border-top: 1px solid rgba(255, 255, 255, 0.1);
}

/* 按钮组布局 */
.button-group {
  display: flex;
  flex-wrap: wrap;
  gap: 12px;
  justify-content: flex-start;
}

/* 表格整体样式 */
:deep(.ant-table) {
  background: rgba(255, 255, 255, 0.05);
  color: #fff;
}

:deep(.ant-table-thead > tr > th) {
  background: rgba(0, 224, 255, 0.15);
  color: #00e0ff;
  font-weight: 600;
  border-bottom: 1px solid rgba(0, 224, 255, 0.3);
}

:deep(.ant-table-tbody > tr > td) {
  border-color: rgba(255, 255, 255, 0.15);
}

:deep(.ant-table-tbody > tr:hover > td) {
  background: rgba(0, 224, 255, 0.15) !important;
  color: #fff !important;
  cursor: pointer;
  transition: background 0.2s ease;
}

/* 操作按钮组 */
.operate-buttons {
  display: flex;
  justify-content: center;
}

/* 加载状态样式 */
:deep(.ant-spin-nested-loading) {
  min-height: 200px;
}

:deep(.ant-spin-dot-item) {
  background-color: #00e0ff !important;
}

/* 图标 */
.bx {
  font-size: 26px;
}

/* 分页样式 */
:deep(.ant-pagination) {
  text-align: center;
  margin-top: 20px;
}

/* 适配小屏幕 */
@media (max-width: 768px) {
  .admin-container {
    padding: 15px;
  }
  
  .stats-container {
    flex-direction: column;
  }
  
  .search-filters {
    flex-direction: column;
    align-items: stretch;
  }
  
  .button-group {
    justify-content: center;
    flex-direction: column;
    align-items: stretch;
  }
  
  .batch-operation-section {
    padding-top: 10px;
  }
}
</style>