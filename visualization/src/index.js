import Vue from 'vue';
import Vuex from 'vuex';
import Vuetify from 'vuetify';
import App from './App.vue';
import router from './router.js';
import store from './store.js';
import 'vuetify/dist/vuetify.min.css';

Vue.use(Vuetify);
Vue.config.productionTip = false;

new Vue({
	el: '#app',
	router,
	store,
	render: h => h(App)
});
