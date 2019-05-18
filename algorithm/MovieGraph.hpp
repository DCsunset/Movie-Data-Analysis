#ifndef MOVIEGRAPH_HPP
#define MOVIEGRAPH_HPP

#include <csv.h>
#include <tuple>
#include <iostream>

class MovieGraph
{
	private:
		std::vector<std::string> movie_names_;
		/** @brief Data
		 * (username, movie number)
		 */
		std::vector<std::pair<std::string, unsigned>> data_;
		/** @brief Edges
		 * (movie 1, movie 2, number of users)
		 */
		std::vector<std::tuple<unsigned, unsigned, unsigned>> edges_;
		/// Associated edges
		std::vector<std::vector<unsigned>> associated_;
		/// Map from directed edge number to undirected ones
		std::vector<unsigned> edge_number_map_;

	public:
		void read_data(const std::string &filename);
		void build_graph();
		void shortest_path() const;
		void minimum_spanning_tree() const;
		void betweenness_centrality() const;
		void closeness_centrality() const;
		void label_propagation() const;
		void run();
};

#endif
