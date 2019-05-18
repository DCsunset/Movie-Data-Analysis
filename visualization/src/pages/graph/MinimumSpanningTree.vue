<template>
	<svg width="100%" height="100%"></svg>
</template>

<script>
import * as d3 from 'd3';
import graph from './minimum_spanning_tree.json';

export default {
	mounted()
	{
		const color = d3.scaleOrdinal(d3.schemeCategory10);

		const svg  = d3.select('svg'),
			width  = parseFloat(svg.style('width')),
			height = parseFloat(svg.style('height'));

		const simulation = d3.forceSimulation()
			.force('charge', d3.forceManyBody().strength(-10))
			.force('center', d3.forceCenter(width / 2, height / 2))
			.force('link', d3.forceLink()
				.id(link => link.id));

		const links = svg.append('g')
			.attr('class', 'links')
			.selectAll('line')
			.data(graph.links)
			.enter().append('line')
			.attr('stroke-width', 2)
			.attr('stroke-opacity', 0.7)
			.attr('stroke', '#999');

		const nodes = svg.append('g')
			.attr('class', 'nodes')
			.selectAll('circle')
			.data(graph.nodes)
			.enter().append('circle')
			.attr('r', 10)
			.attr('stroke', '#fff')
			.attr('stroke-width', 2)
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
					.attr("cy", node => node.y);
			});

		simulation.force('link')
			.links(graph.links);

		// Index will be generated after the simulation
		nodes.attr('fill', node => color(node.index % 3));
	}
};
</script>
