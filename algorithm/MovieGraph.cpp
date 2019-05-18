#include "MovieGraph.hpp"
#include <boost/multi_array.hpp>
#include <boost/heap/fibonacci_heap.hpp>
#include <boost/dynamic_bitset.hpp>
#include <set>
#include <stack>
#include <json.hpp>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <random>

void MovieGraph::read_data(const std::string &filename)
{
	io::CSVReader<3> data(filename);
	data.read_header(io::ignore_extra_column, "movie name", "user name", "comment");

	std::string movie, user, comment;
	while(data.read_row(movie, user, comment))
	{
		if(movie_names_.empty() || movie_names_.back() != movie)
			movie_names_.push_back(movie);
		data_.push_back(std::make_pair(user, movie_names_.size() - 1));
	}
	std::sort(data_.begin(), data_.end());
	std::unique(data_.begin(), data_.end());
}

void MovieGraph::build_graph()
{
	associated_.resize(movie_names_.size());
	unsigned i = 0, j = 0;
	boost::multi_array<unsigned, 2> adj(boost::extents[movie_names_.size()][movie_names_.size()]);
	std::fill(adj.data(), adj.data() + adj.num_elements(), 0);
	// Used for mapping edge number 
	boost::multi_array<int, 2> adj_map(boost::extents[movie_names_.size()][movie_names_.size()]);
	std::fill(adj_map.data(), adj_map.data() + adj_map.num_elements(), -1);
	while(i < data_.size())
	{
		// Find the end of this user
		while(j < data_.size() && data_[i].first == data_[j].first)
			++j;
		// All pairs within (i, j)
		for(unsigned u = i; u < j; ++u)
			for(unsigned v = u + 1; v < j; ++v)
			{
				++adj[data_[u].second][data_[v].second];
				++adj[data_[v].second][data_[u].second];
			}
		i = j;
	}

	// Use KNN to build edge
	unsigned counter = 0;
	// pair (weight, movie number)
	std::vector<std::pair<unsigned, unsigned>> knn(movie_names_.size());
	unsigned K = 5;
	for(unsigned u = 0; u < adj.size(); ++u)
	{
		for(unsigned v = 0; v < adj[u].size(); ++v) knn[v] = std::make_pair(adj[u][v], v);
		std::sort(knn.begin(), knn.end(), std::greater<decltype(knn)::value_type>());

		for(unsigned v = 0; v < K; ++v)
		{
			if(u < knn[v].second)
			{
				// It must appear earlier than u > knn[v].second
				edges_.push_back(std::make_tuple(u, knn[v].second, adj[u][knn[v].second]));
				associated_[u].push_back(edges_.size() - 1);
				edges_.push_back(std::make_tuple(knn[v].second, u, adj[knn[v].second][u]));
				associated_[knn[v].second].push_back(edges_.size() - 1);
				// Map edge number
				adj_map[knn[v].second][u] = adj_map[u][knn[v].second] = counter++;
			}
			else
				if(adj_map[knn[v].second][u] == -1)
				{
					edges_.push_back(std::make_tuple(knn[v].second, u, adj[knn[v].second][u]));
					associated_[knn[v].second].push_back(edges_.size() - 1);
					edges_.push_back(std::make_tuple(u, knn[v].second, adj[u][knn[v].second]));
					associated_[u].push_back(edges_.size() - 1);
					// Map edge number
					adj_map[u][knn[v].second] = adj_map[knn[v].second][u] = counter++;
				}
		}
	}

	// Make important edges have less weight
	unsigned max_weight = std::get<2>(*std::max_element(edges_.begin(), edges_.end(), [](const auto &lhs, const auto &rhs) {
		return std::get<2>(lhs) < std::get<2>(rhs);
	}));
	for(auto &u : edges_)
		std::get<2>(u) = max_weight + 1 - std::get<2>(u);

	// Map edge number
	edge_number_map_.resize(edges_.size());
	for(unsigned u = 0; u < edges_.size(); ++u)
		edge_number_map_[u] = adj_map[std::get<0>(edges_[u])][std::get<1>(edges_[u])];

	// Serialize
	nlohmann::json data;
	for(auto u : movie_names_)
		data["nodes"].push_back({{"id", u}});
	for(const auto &v : edges_)
	{
		if(std::get<0>(v) < std::get<1>(v))
			data["links"].push_back({
				{"source", movie_names_[std::get<0>(v)]},
					{"target", movie_names_[std::get<1>(v)]},
					{"weight", std::get<2>(v)}
			});
	}
	std::ofstream fout("movie_graph.json");
	fout << data;
}

void MovieGraph::shortest_path() const
{
	// Store the shortest path
	boost::multi_array<std::vector<unsigned>, 2> path(boost::extents[movie_names_.size()][movie_names_.size()]);
	for(unsigned i = 0; i < movie_names_.size(); ++i)
	{
		// Dijkstra, pair (weight, edge number)
		boost::heap::fibonacci_heap<std::pair<unsigned, unsigned>, boost::heap::compare<std::greater<std::pair<unsigned, unsigned>>>> heap;
		heap.push(std::make_pair(0, ~0u));
		while(!heap.empty())
		{
			auto t = heap.top();
			heap.pop();
			if(t.second != ~0u)
			{
				if(path[i][std::get<1>(edges_[t.second])].size())
					continue;
				path[i][std::get<1>(edges_[t.second])] = path[i][std::get<0>(edges_[t.second])];
				path[i][std::get<1>(edges_[t.second])].push_back(edge_number_map_[t.second]);
			}
			for(auto j : associated_[t.second == ~0u ? i : std::get<1>(edges_[t.second])])
				if(!path[i][std::get<1>(edges_[j])].size())
					heap.push(std::make_pair(t.first + std::get<2>(edges_[j]), j));
		}
	}

	// Serialize
	nlohmann::json data;
	data = path;

	std::ofstream fout("shortest_path.json");
	fout << data;
}

void MovieGraph::minimum_spanning_tree() const
{
	// Find the minimum spanning tree in every connected component
	// Record the already used points
	boost::dynamic_bitset<> flags(movie_names_.size());
	flags.flip();
	boost::dynamic_bitset<>::size_type i;
	std::vector<std::set<unsigned>> trees;
	while((i = flags.find_first()) != boost::dynamic_bitset<>::npos)
	{
		// Prim
		// tuple (weight, node number, edge number)
		boost::heap::fibonacci_heap<std::tuple<unsigned, unsigned, unsigned>, boost::heap::compare<std::greater<std::tuple<unsigned, unsigned, unsigned>>>> heap;
		std::set<unsigned> edges;
		heap.push(std::make_tuple(0, i, ~0u));
		while(!heap.empty())
		{
			auto t = heap.top();
			heap.pop();
			if(!flags.test(std::get<1>(t)))
				continue;
			flags.reset(std::get<1>(t));
			if(std::get<2>(t) != ~0u)
				edges.insert(edge_number_map_[std::get<2>(t)]);
			for(auto j : associated_[std::get<1>(t)])
				if(flags.test(std::get<1>(edges_[j])))
					heap.push(std::make_tuple(
								std::get<2>(edges_[j]),
								std::get<1>(edges_[j]),
								j
								));
		}
		if(!edges.empty())
			trees.push_back(edges);
	}

	// Serialize
	nlohmann::json data;
	data = trees;

	std::ofstream fout("minimum_spanning_tree.json");
	fout << data;
}

void MovieGraph::betweenness_centrality() const
{
	std::vector<double> CB(movie_names_.size(), 0);
	for(unsigned s = 0; s < movie_names_.size(); ++s)
	{
		std::stack<unsigned> S;
		std::vector<std::vector<unsigned>> P(movie_names_.size());
		std::vector<unsigned> sigma(movie_names_.size(), 0);
		sigma[s] = 1;
		// Distance
		std::vector<unsigned> d(movie_names_.size(), ~0u);
		// tuple (distance, current node, previous edge)
		boost::heap::fibonacci_heap<std::tuple<unsigned, unsigned, unsigned>, boost::heap::compare<std::greater<std::tuple<unsigned, unsigned, unsigned>>>> Q;
		Q.push(std::make_tuple(0, s, ~0u));
		while(!Q.empty())
		{
			auto v = Q.top();
			Q.pop();
			// Another shortest path
			if(d[std::get<1>(v)] == std::get<0>(v))
			{
				P[std::get<1>(v)].push_back(std::get<0>(edges_[std::get<2>(v)]));
				sigma[std::get<1>(v)] += sigma[std::get<0>(edges_[std::get<2>(v)])];
			}
			if(d[std::get<1>(v)] != ~0u)
				continue;
			d[std::get<1>(v)] = std::get<0>(v);
			S.push(std::get<1>(v));
			if(std::get<2>(v) != ~0u)
			{
				P[std::get<1>(v)].push_back(std::get<0>(edges_[std::get<2>(v)]));
				sigma[std::get<1>(v)] += sigma[std::get<0>(edges_[std::get<2>(v)])];
			}
			for(auto j : associated_[std::get<1>(v)])
			{
				if(std::get<0>(v) + std::get<2>(edges_[j]) <= d[std::get<1>(edges_[j])])
					Q.push(std::make_tuple(
								std::get<0>(v) + std::get<2>(edges_[j]),
								std::get<1>(edges_[j]),
								j
								));
			}
		}
		std::vector<double> delta(movie_names_.size(), 0);
		while(!S.empty())
		{
			unsigned w = S.top();
			S.pop();
			for(auto v : P[w])
				delta[v] += static_cast<double>(sigma[v]) / sigma[w] * (1 + delta[w]);
			if(w != s)
				CB[w] += delta[w];
		}
	}
	// This is an undirected graph, must divide by 2
	for(auto &i : CB)
		i /= 2;

	// Serialize
	nlohmann::json data;
	data = CB;

	std::ofstream fout("betweenness_centrality.json");
	fout << data;
}

void MovieGraph::closeness_centrality() const
{
	std::vector<double> CD(movie_names_.size(), 0);
	for(unsigned i = 0; i < movie_names_.size(); ++i)
	{
		// Dijkstra
		boost::heap::fibonacci_heap<std::pair<unsigned, unsigned>, boost::heap::compare<std::greater<std::pair<unsigned, unsigned>>>> heap;
		std::vector<int> dist(movie_names_.size(), -1);
		heap.push(std::make_pair(0, ~0u));
		while(!heap.empty())
		{
			auto t = heap.top();
			heap.pop();
			if(t.second != ~0u)
			{
				if(dist[std::get<1>(edges_[t.second])] != -1)
					continue;
				dist[std::get<1>(edges_[t.second])] = t.first;
			}
			for(auto j : associated_[t.second == ~0u ? i : std::get<1>(edges_[t.second])])
				if(dist[std::get<1>(edges_[j])] == -1)
					heap.push(std::make_pair(t.first + std::get<2>(edges_[j]), j));
		}
		for(auto j : dist)
			CD[i] += j;
		CD[i] = 1 / CD[i];
	}

	// Serialize
	nlohmann::json data;
	data = CD;

	std::ofstream fout("closeness_centrality.json");
	fout << data;
}

void MovieGraph::label_propagation() const
{
	std::vector<unsigned> label(movie_names_.size());
	for(unsigned i = 0; i < label.size(); ++i)
		label[i] = i;
	std::vector<unsigned> label_pre[2] = {label, label};
	std::vector<unsigned> tmp_label(movie_names_.size()), table(movie_names_.size(), 0), max_label;
	std::random_device rd;
	while(1)
	{
		for(unsigned i = 0; i < movie_names_.size(); ++i)
		{
			for(const auto &j : associated_[i])
				++table[label[std::get<1>(edges_[j])]];
			unsigned largest = *std::max_element(table.begin(), table.end());
			for(unsigned u = 0; u < table.size(); ++u)
				if(table[u] == largest)
					max_label.push_back(u);
			std::uniform_int_distribution<unsigned> dist(0, max_label.size() - 1);
			tmp_label[i] = max_label[dist(rd)];
			max_label.clear();
			std::fill(table.begin(), table.end(), 0);
		}
		label = tmp_label;
		if(label == label_pre[0] || label == label_pre[1])
			break;
		label_pre[1] = label_pre[0];
		label_pre[0] = label;
	}

	// Renumber label
	std::map<unsigned, unsigned> final_label;
	for(auto i : label)
		if(!final_label.count(i))
			final_label[i] = final_label.size();

	// Serialize
	nlohmann::json data;
	for(unsigned i = 0; i < movie_names_.size(); ++i)
		data.push_back(final_label[label[i]]);

	std::ofstream fout("label_propagation.json");
	fout << data;
}

void MovieGraph::run()
{
	read_data("movie_data.csv");
	//read_data("test.csv");
	std::cout << "Building graph...\n";
	build_graph();
	std::cout << "Generating shortest path...\n";
	shortest_path();
	std::cout << "Generating minimum spanning tree...\n";
	minimum_spanning_tree();
	std::cout << "Calculating betweenness centrality...\n";
	betweenness_centrality();
	std::cout << "Calculating closeness centrality...\n";
	closeness_centrality();
	std::cout << "Calculating label propagation...\n";
	label_propagation();
}
