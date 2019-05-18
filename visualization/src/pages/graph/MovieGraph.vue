<template>
	<v-layout row fill-height wrap>
		<v-flex md12 lg4>
			<v-layout column wrap>
				<v-flex md4 lg12>
					<v-card class="ma-2 text-xs-center">
						<v-card-text>
							<h2 class="my-3">
								<v-icon style="vertical-align: text-bottom" color="primary">build</v-icon>
								算法面板
							</h2>
							<v-tabs>
								<v-tab ripple>
									最短路
								</v-tab>
								<v-tab-item class="mt-3">
									<v-form @submit="shortestPath">
										<v-text-field
											v-model="startMovie"
											label="起点电影名"
											type="text"
											clearable
											required
											/>
										<v-text-field
											v-model="endMovie"
											label="终点电影名"
											type="text"
											clearable
											required
											/>
										<v-btn dark color="primary" @click="clear">
											清除最短路
										</v-btn>
										<v-btn dark color="primary" type="submit">
											显示最短路
										</v-btn>
									</v-form>
								</v-tab-item>

								<v-tab ripple>
									最小生成树
								</v-tab>
								<v-tab-item class="mt-3">
									<v-btn dark color="primary" @click="clear">
										清除
									</v-btn>
									<v-btn dark color="primary" @click="minimumSpanningTree">
										显示
									</v-btn>
								</v-tab-item>

								<v-tab ripple>
									介数中心度
								</v-tab>
								<v-tab-item class="mt-3">
									<v-btn dark color="primary" @click="clear">
										清除
									</v-btn>
									<v-btn dark color="primary" @click="betweennessCentrality">
										显示
									</v-btn>
								</v-tab-item>

								<v-tab ripple>
									紧密中心度
								</v-tab>
								<v-tab-item class="mt-3">
									<v-btn dark color="primary" @click="clear">
										清除
									</v-btn>
									<v-btn dark color="primary" @click="closenessCentrality">
										显示
									</v-btn>
								</v-tab-item>

								<v-tab ripple>
									标签传播LPA算法
								</v-tab>
								<v-tab-item class="mt-3">
									<v-btn dark color="primary" @click="clear">
										清除
									</v-btn>
									<v-btn dark color="primary" @click="labelPropagation">
										显示
									</v-btn>
								</v-tab-item>
							</v-tabs>
						</v-card-text>
					</v-card>
				</v-flex>
				<v-flex md6 lg12>
					<v-card class="ma-2 text-xs-center">
						<v-card-text>
							<h2 class="my-3">
								<v-icon style="vertical-align: text-bottom" color="primary">movie</v-icon>
								电影信息
							</h2>
							<v-data-table
								:items="movieInfo"
								hide-headers
								hide-actions
								>
								<template slot="items" slot-scope="props">
									<td>{{ props.item.name }}</td>
									<td>{{ props.item.info }}</td>
								</template>
							</v-data-table>
						</v-card-text>
					</v-card>
				</v-flex>
			</v-layout>
		</v-flex>
		<v-flex md12 lg8>
			<svg width="100%" height="100%"></svg>
		</v-flex>
	</v-layout>
</template>

<script>
import * as d3 from 'd3';
import graph from './movie_graph.json';
import movieInfoData from './movies.json';
import shortestPathData from './shortest_path.json';
import minimumSpanningTreeData from './minimum_spanning_tree.json';
import betweennessCentralityData from './betweenness_centrality.json';
import closenessCentralityData from './closeness_centrality.json';
import labelPropagationData from './label_propagation.json';

export default {
	data()
	{
		return {
			radius: 7,
			startMovie: '',
			endMovie: '',
			movieInfo: [],
			color: d3.scaleOrdinal(d3.schemeCategory10)
		};
	},
	methods: {
		minimumSpanningTree()
		{
			this.clear();
			function inTree(link)
			{
				return minimumSpanningTreeData.findIndex(i => i.includes(link.index));
			}

			d3.selectAll('line')
				.transition()
				.duration(1500)
				.attr('stroke-width', link => inTree(link) !== -1 ? 2 : 1)
				.attr('stroke-opacity', link => inTree(link) !== -1 ? 1 : 0.7)
				.attr('stroke', link => inTree(link) !== -1 ? 'blue' : '#999');
		},
		shortestPath()
		{
			this.clear();
			const start = graph.nodes.findIndex(i => i.id === this.startMovie);
			const end = graph.nodes.findIndex(i => i.id === this.endMovie);

			function inPath(node)
			{
				if(start === -1 || end === -1)
					return -1;
				return shortestPathData[start][end].findIndex(i => graph.links[i].target.id === node.id || graph.links[i].source.id === node.id);
			}

			function isPath(link)
			{
				if(start === -1 || end === -1)
					return -1;
				return shortestPathData[start][end].indexOf(link.index);
			}

			d3.selectAll('circle')
				.transition()
				.delay(node => node.index === start ? 0 : (inPath(node) + 1) * 1000)
				.duration(500)
				.ease(d3.easeBounce)
				.attr('fill', node => inPath(node) >= 0 ? this.color(3) : this.color(node.index % 3))
				.attr('r', node => inPath(node) >= 0 ? this.radius * 1.5 : this.radius)
				.attr('stroke', node => inPath(node) >= 0 ? 'blue' : '#fff')
				.attr('stroke-width', node => inPath(node) >= 0 ? 3 : 2);

			d3.selectAll('line')
				.transition()
				.delay(link => (isPath(link) + 1) * 1000 - 500)
				.duration(500)
				.attr('stroke-width', link => isPath(link) >= 0 ? 3 : 1)
				.attr('stroke', link => isPath(link) >= 0 ? 'blue' : '#999');
		},
		betweennessCentrality()
		{
			this.clear();
			d3.selectAll('circle')
				.transition()
				.duration(1500)
				.attr('r', node => Math.pow(this.radius * this.radius + betweennessCentralityData[node.index], 1 / 3) * 1.3);
		},
		closenessCentrality()
		{
			this.clear();
			const minValue = Math.min(...closenessCentralityData);
			d3.selectAll('circle')
				.transition()
				.duration(1500)
				.attr('r', node => this.radius * Math.pow(closenessCentralityData[node.index] / minValue, 2));
		},
		labelPropagation()
		{
			this.clear();
			d3.selectAll('circle')
				.transition()
				.duration(1500)
				.attr('fill', node => this.color(labelPropagationData[node.index] % 10));
		},
		clear()
		{
			d3.selectAll('circle')
				.attr('fill', node => this.color(node.index % 3))
				.attr('r', this.radius)
				.attr('stroke', '#fff')
				.attr('stroke-width', 2);

			d3.selectAll('line')
				.attr('stroke-width', 1)
				.attr('stroke-opacity', 0.7)
				.attr('stroke', '#999');
		}
	},
	mounted()
	{
		const svg  = d3.select('svg');
		let width  = parseFloat(svg.style('width'));
		let height = parseFloat(svg.style('height'));

		const simulation = d3.forceSimulation()
			.force('forceX', d3.forceX().strength(110 / width).x(width / 2))
			.force('forceY', d3.forceY().strength(110 / height).y(height / 2))
			.force('charge', d3.forceManyBody().strength(-40))
			.force('center', d3.forceCenter(width / 2, height / 2))
			.force('link', d3.forceLink()
				.id(link => link.id)
				.distance(80)
			);

		const links = svg.append('g')
			.selectAll('line')
			.data(graph.links)
			.enter().append('line')
			.attr('stroke-width', link => 1)
			.attr('stroke-opacity', 0.7)
			.attr('stroke', '#999');

		const nodes = svg.append('g')
			.selectAll('circle')
			.data(graph.nodes)
			.enter().append('circle')
			.attr('r', this.radius)
			.attr('stroke', '#fff')
			.attr('stroke-width', 2)
			.on('click', node => {
				this.movieInfo.length = 0;
				const res = movieInfoData.findIndex(i => i['影片'] === node.id);
				if(res !== -1)
					for(const i in movieInfoData[res])
						this.movieInfo.push({
							name: i,
							info: movieInfoData[res][i]
						});
			})
			.call(d3.drag()
				.on('start', function(d) {
					if (!d3.event.active) simulation.alphaTarget(0.3).restart();
					d.fx = d.x;
					d.fy = d.y;
				})
				.on('drag', function(d) {
					d.fx = d3.event.x;
					d.fy = d3.event.y;
				})
				.on('end', function(d) {
					if (!d3.event.active) simulation.alphaTarget(0);
					d.fx = null;
					d.fy = null;
				}));

		nodes.append('title')
			.text(node => node.id);

		simulation
			.nodes(graph.nodes)
			.on('tick', () => {
				links
					.attr("x1", link => link.source.x)
					.attr("y1", link => link.source.y)
					.attr("x2", link => link.target.x)
					.attr("y2", link => link.target.y);
				nodes
					.attr("cx", node => node.x)
					.attr("cy", node => node.y)
			});

		simulation.force('link')
			.links(graph.links);

		// Index will be generated after the simulation
		nodes.attr('fill', node => this.color(node.index % 3));

		function resize()
		{
			width  = parseFloat(svg.style('width'));
			height = parseFloat(svg.style('height'));
			simulation
				.force('center', d3.forceCenter(width / 2, height / 2))
				.force('forceX', d3.forceX().strength(110 / width).x(width / 2))
				.force('forceY', d3.forceY().strength(110 / height).y(height / 2));
		}

		// Replace center based on the new size whenever the browser window is resized.
		d3.select(window).on('resize', resize);
	}
};
</script>
