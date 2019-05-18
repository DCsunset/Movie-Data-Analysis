<template>
	<v-app :dark="dark">
		<v-navigation-drawer
			:mini-variant="mini"
			v-model="drawer"
			app
			>
			<div v-show="!mini" class="pa-2 text-xs-center">
				<div class="display-2 py-4">
					离散数学
				</div>
				<p>
					数据可视化与交互
				</p>
			</div>
			<v-list expand>
				<div v-for="item in menu">
					<v-list-group
						v-if="item.items"
						:prepend-icon="item.icon"
						active-class="orange--text"
						:title="item.title"
						>
						<v-list-tile slot="activator">
							<v-list-tile-content>
								<v-list-tile-title>
									{{ item.title }}
								</v-list-tile-title>
							</v-list-tile-content>
						</v-list-tile>

						<v-list-tile
							v-for="subItem in item.items"
							:key="subItem.href"
							:title="subItem.title"
							:to="subItem.href"
							>
							<v-list-tile-action>
								<v-icon>{{ subItem.icon }}</v-icon>
							</v-list-tile-action>
							<v-list-tile-content>
								<v-list-tile-title>
									{{ subItem.title }}
								</v-list-tile-title>
							</v-list-tile-content>
						</v-list-tile>
					</v-list-group>

					<v-list-tile
						v-else
						:to="item.href"
						:title="item.title"
						>
						<v-list-tile-action>
							<v-icon>{{ item.icon }}</v-icon>
						</v-list-tile-action>
						<v-list-tile-content>
							<v-list-tile-title>
								{{ item.title }}
							</v-list-tile-title>
						</v-list-tile-content>
					</v-list-tile>
				</div>
			</v-list>
		</v-navigation-drawer>

		<v-toolbar
			app
			dark
			:color="color"
			>
			<v-toolbar-side-icon @click.stop="drawer = !drawer" />
			<v-btn icon @click.stop="mini = !mini">
				<v-icon v-html="mini ? 'chevron_right' : 'chevron_left'" />
			</v-btn>
			<v-toolbar-title>
				{{ pageTitle }}
			</v-toolbar-title>
			<v-spacer />
			<v-btn icon :title="dark ? 'Dark' : 'Bright'" @click.stop="dark = !dark">
				<v-icon v-html="dark ? 'brightness_4' : 'brightness_5'" />
			</v-btn>
		</v-toolbar>

		<v-content>
			<router-view />
		</v-content>
	</v-app>
</template>

<script>
import { mapState } from 'vuex';

export default {
	data()
	{
		return {
			drawer: true,
			dark: false,
			mini: false,
			color: 'deep-purple lighten-1',
			userMenu: [
				{
					icon: 'settings',
					title: 'Settings',
					action: () => {
						this.$router.push('/settings');
					}
				},
				{
					icon: 'exit_to_app',
					title: 'Logout',
					action: this.logout
				}
			],
			menu: [
				{
					href: '/',
					icon: 'home',
					title: '主页'
				},
				{
					icon: 'domain',
					title: '图的可视化',
					href: '/graph'
				}
			]
		};
	},
	computed: {
		...mapState(['pageTitle'])
	},
	methods: {
		errorHandle(err)
		{
			this.$router.push({
				path: '/error',
				query: {
					status: err.status,
					statusText: err.statusText,
					data: err.data
				}
			});
		},
	}
};
</script>
