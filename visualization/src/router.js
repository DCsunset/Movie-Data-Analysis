import Vue from 'vue';
import Router from 'vue-router';
import store from './store';

const Main = () => import(/* webpackChunkName: "main" */ './pages/Main.vue');
const Home = () => import(/* webpackChunkName: "home" */ './pages/Home.vue');
const Error = () => import(/* webpackChunkName: "error" */ './pages/Error.vue');
const Graph = () => import(/* webpackChunkName: "graph" */ './pages/graph/MovieGraph.vue');

Vue.use(Router);

const router = new Router({
	base: __dirname,
	routes: [
		{
			exact: true,
			path: '/',
			component: Main,
			children: [
				{
					exact: true,
					path: '/',
					component: Home,
					name: 'Home',
					beforeEnter: (to, from, next) => {
						store.commit('setPageTitle', 'Home');
						next();
					}
				},
				{
					exact: true,
					path: '/error',
					component: Error,
					name: 'Error',
					beforeEnter: (to, from, next) => {
						store.commit('setPageTitle', 'Error');
						next();
					}
				},
				{
					exact: true,
					path: '/graph',
					component: Graph,
					beforeEnter: (to, from, next) => {
						store.commit('setPageTitle', 'Graph');
						next();
					}
				}
			]
		},
		{
			path: '*',
			redirect: '/error'
		}
	]
});

export default router;
