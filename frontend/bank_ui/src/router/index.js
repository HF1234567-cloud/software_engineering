import { createRouter, createWebHistory } from 'vue-router'


import LoginPage from '../views/login/index.vue'

import LayoutPage from '../views/layout/index.vue'
import HomePage from '../views/home/index.vue'
import AccountPage from '../views/account/index.vue'
import UserPage from '../views/user/index.vue'
import TransactionPage from '../views/transaction/index.vue'



const routes = [
  // open root path to login page
  {
    path: '/',
    redirect: '/login'
  },
  {
    path: '/login',
    name: 'login',
    component: LoginPage
  },
  {
    // move layout off root so root shows login first
    path: '/',
    name: 'layout',
    component: LayoutPage,
    redirect: '/home',
    children: [
      {
        path: '/home',
        name: 'home',
        component: HomePage
      },
      {
        path: '/account',
        name: 'account',
        component: AccountPage
      },
   
      {
        path: '/user',
        name: 'user',
        component: UserPage
      },
      {
        path: '/transaction',
        name: 'transaction',
        component: TransactionPage
      }
    


    ]
  },
  {
    path: '/', // 404 路由（可选）
    redirect: '/home'
  }
]

const router = createRouter({
  history: createWebHistory(import.meta.env.BASE_URL),
  routes
})

export default router



