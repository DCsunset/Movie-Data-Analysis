import Vue from 'vue';
import Vuex from 'vuex';

Vue.use(Vuex);

const store = new Vuex.Store({
	state: {
		pageTitle: ''
	},
	mutations: {
		setPageTitle(state, data)
		{
			state.pageTitle = data;
		}
	}
});

export default store;
