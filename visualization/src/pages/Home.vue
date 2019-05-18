<template>
	<div>
		<section>
			<v-jumbotron height="auto" color="deep-purple" dark>
				<v-container class="mt-3 mb-3" fill-height>
					<v-layout column align-center justify-center wrap>
						<h1 class="text-xs-center mb-3 display-3">离散数学</h1>
						<span class="text-xs-center mb-5 subheading">
							本网页将电影数据进行可视化，
							并且制作了动态交互。
						</span>
					</v-layout>
				</v-container>
			</v-jumbotron>
		</section>
		<section>
			<v-layout column align-center wrap class="my-4">
				<v-flex xs12>
					<v-container grid-list-lg>
						<v-layout row justify-center wrap>
							<v-flex xs12 sm6 md4>
								<v-card class="text-xs-center" style="height: 100%">
									<v-icon class="mt-5" color="primary" x-large>build</v-icon>
									<v-card-text>
										<h2 class="my-3" >最短路</h2>
										<p>
											使用Dijkstra算法实现
										</p>
									</v-card-text>
								</v-card>
							</v-flex>
							<v-flex xs12 sm6 md4>
								<v-card class="text-xs-center" style="height: 100%">
									<v-icon class="mt-5" color="primary" x-large>cloud_queue</v-icon>
									<v-card-text>
										<h2 class="my-3" >最小生成树</h2>
										<p>
											使用Prim算法
										</p>
									</v-card-text>
								</v-card>
							</v-flex>
							<v-flex xs12 sm6 md4>
								<v-card class="text-xs-center" style="height: 100%">
									<v-icon class="mt-5" color="primary" x-large>library_books</v-icon>
									<v-card-text>
										<h2 class="my-3" >介数中心度</h2>
										<p>
											使用Brandes算法，时间复杂度为O(nm + n 2 log n)
										</p>
									</v-card-text>
								</v-card>
							</v-flex>
							<v-flex xs12 sm6 md4>
								<v-card class="text-xs-center" style="height: 100%">
									<v-icon class="mt-5" color="primary" x-large>code</v-icon>
									<v-card-text>
										<h2 class="my-3" >紧密中心度</h2>
										<p>
											利用了最短路的结果，
											根据定义计算
										</p>
									</v-card-text>
								</v-card>
							</v-flex>
							<v-flex xs12 sm6 md4>
								<v-card class="text-xs-center" style="height: 100%">
									<v-icon class="mt-5" color="primary" x-large>code</v-icon>
									<v-card-text>
										<h2 class="my-3" >标签传播</h2>
										<p>
											使用了LPA算法
										</p>
									</v-card-text>
								</v-card>
							</v-flex>
						</v-layout>
					</v-container>
				</v-flex>
			</v-layout>
		</section>
	</div>
</template>

<script>
export default {
	data()
	{
		return {
			color: 'deep-purple'
		};
	}
};
</script>
