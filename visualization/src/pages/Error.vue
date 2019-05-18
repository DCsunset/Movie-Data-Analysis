<template>
	<div class="text-center">
		<v-card class="blue darken-1 white--text ma-4">
			<v-card-text>
				<div class="display-2">{{ status }}</div>
				<p>{{ statusText }}</p>
				<p>{{ data }}</p>
			</v-card-text>
		</v-card>
	</div>
</template>

<script>
export default {
	data()
	{
		return {
			status: this.$route.query.status || 404,
			statusText: this.$route.query.statusText || 'Not Found',
			data: this.$route.query.data || 'Page Not Found.'
		};
	}
};
</script>
