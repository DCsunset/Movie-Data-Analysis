#include "MovieUserGraph.hpp"
#include <boost/dynamic_bitset.hpp>
#include <boost/heap/fibonacci_heap.hpp>
#include <boost/multi_array.hpp>
#include <json.hpp>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <queue>

void MovieUserGraph::read_data(const std::string &filename)
{
	io::CSVReader<3> data(filename);
	data.read_header(io::ignore_extra_column, "movie name", "user name", "comment");

	std::string movie, user, comment;
	while(data.read_row(movie, user, comment))
	{
		if(movie_names_.empty() || movie_names_.back() != movie)
			movie_names_.push_back(movie);
		data_.push_back(std::make_tuple(user, movie_names_.size() - 1,
					comment == "力荐" ? 1 :
					comment == "推荐" ? 2 :
					comment == "还行" ? 3 :
					comment == "较差" ? 4 :
					comment == "很差" ? 5 : 6));
	}
	std::sort(data_.begin(), data_.end());
	data_.erase(std::unique(data_.begin(), data_.end()), data_.end());
}

void MovieUserGraph::build_graph()
{
	associated_.resize(movie_names_.size());
	edges_.reserve(data_.size());
	for(const auto &i : data_)
	{
		if(usernames_.empty() || usernames_.back() != std::get<0>(i))
		{
			usernames_.push_back(std::get<0>(i));
			associated_.emplace_back();
		}
		edges_.push_back(std::make_tuple(std::get<1>(i), usernames_.size() - 1, std::get<2>(i)));
		associated_[std::get<1>(i)].push_back(edges_.size() - 1);
		associated_.back().push_back(edges_.size() - 1);
	}

	// Serialize
	nlohmann::json data;
	data["edges"] = edges_;
	data["movie_names"] = movie_names_;
	data["usernames"] = usernames_;

	std::ofstream fout("movie_user_graph.json");
	fout << data;
}

void MovieUserGraph::shortest_path() const
{
}

void MovieUserGraph::minimum_spanning_tree() const
{
	// Find the minimum spanning tree in every connected component
	// Record the already used points
	boost::dynamic_bitset<> flags(movie_names_.size() + usernames_.size());
	flags.flip();
	boost::dynamic_bitset<>::size_type i;
	std::vector<std::set<unsigned>> trees;
	while((i = flags.find_first()) != boost::dynamic_bitset<>::npos)
	{
		// Prim
		// tuple (weight, node number, edge number
		boost::heap::fibonacci_heap<std::tuple<unsigned, unsigned, unsigned>, boost::heap::compare<std::greater<std::tuple<unsigned, unsigned, unsigned>>>> heap;
		std::set<unsigned> edges;
		heap.push(std::make_tuple(0, i, -1u));
		while(!heap.empty())
		{
			auto t = heap.top();
			heap.pop();
			if(!flags.test(std::get<1>(t)))
				continue;
			flags.reset(std::get<1>(t));
			if(std::get<2>(t) != -1u)
				edges.insert(std::get<2>(t));
			for(auto j : associated_[std::get<1>(t)])
			{
				if(std::get<1>(t) >= movie_names_.size())
				{
					if(flags.test(std::get<0>(edges_[j])))
						heap.push(std::make_tuple(std::get<2>(edges_[j]), std::get<0>(edges_[j]), j));
				}
				else
					if(flags.test(std::get<1>(edges_[j])))
						heap.push(std::make_tuple(std::get<2>(edges_[j]), movie_names_.size() + std::get<1>(edges_[j]), j));
			}
		}
		trees.push_back(edges);
	}

	// Serialize
	nlohmann::json data;
	data["data"] = trees;

	std::ofstream fout("minimum_spanning_tree.json");
	fout << data;
}

bool MovieUserGraph::find_path_(unsigned x, std::set<unsigned> &path, std::set<unsigned> &match, std::set<unsigned> &matched_users) const
{
	for(auto i : associated_[x])
		if(std::get<2>(edges_[i]) <= 3 && !path.count(i))
		{
			path.insert(i);
			if(!matched_users.count(std::get<1>(edges_[i])))
				return true;
			else
				for(auto j : match)
					if(std::get<1>(edges_[j]) == std::get<1>(edges_[i]) && !path.count(j))
					{
						path.insert(j);
						if(find_path_(std::get<0>(edges_[j]), path, match, matched_users))
							return true;
						path.erase(j);
					}
			path.erase(i);
		}
	return false;
}

void MovieUserGraph::maximum_matching() const
{
	boost::dynamic_bitset<> remaining_movies(movie_names_.size());
	std::set<unsigned> match;
	remaining_movies.flip();
	std::set<unsigned> matched_users;
	while(remaining_movies.any())
	{
		auto x = remaining_movies.find_first();
		remaining_movies.reset(x);
		std::set<unsigned> path;
		if(find_path_(x, path, match, matched_users))
			for(auto i : path)
			{
				if(match.count(i))
					match.erase(i);
				else
				{
					match.insert(i);
					matched_users.insert(std::get<1>(edges_[i]));
				}
			}
	}

	// Serialize
	nlohmann::json data;
	for(auto i : match)
	{
		data["nodes"].push_back({
			{"id", movie_names_[std::get<0>(edges_[i])]},
			{"type", 0}
		});
		data["nodes"].push_back({
			{"id", usernames_[std::get<1>(edges_[i])]},
			{"type", 1}
		});
		data["links"].push_back({
			{"source", movie_names_[std::get<0>(edges_[i])]},
			{"target", usernames_[std::get<1>(edges_[i])]},
			{"comment", std::get<2>(edges_[i])}
		});
	}

	std::ofstream fout("maximum_matching.json");
	fout << data;
}

void MovieUserGraph::run()
{
	read_data("../data/movie_data.csv");
	//read_data("test.csv");
	std::cout << "Building graph...\n";
	build_graph();
	/*
	std::cout << "Calculating minimum spanning tree...\n";
	minimum_spanning_tree();
	*/
	std::cout << "Calculating maximum matching...\n";
	maximum_matching();
}
