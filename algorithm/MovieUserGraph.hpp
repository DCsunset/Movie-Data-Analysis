#ifndef MOVIEUSERGRAPH_HPP
#define MOVIEUSERGRAPH_HPP

#include <csv.h>
#include <tuple>
#include <set>
#include <iostream>

class MovieUserGraph
{
	private:
		std::vector<std::string> movie_names_;
		std::vector<std::string> usernames_;
		/** @brief Data
		 * (user number, movie number, comment)
		 * 1 for 力荐
		 * 2 for 推荐
		 * 3 for 还行
		 * 4 for 较差
		 * 5 for 很差
		 */
		std::vector<std::tuple<std::string, unsigned, unsigned>> data_;
		/** @brief Edges
		 * (movie, username, comment)
		 */
		std::vector<std::tuple<unsigned, unsigned, unsigned>> edges_;
		/// Associated edges (movie is 0 - movie_number, user is movie_number - movie_number + user number)
		std::vector<std::vector<unsigned>> associated_;
		/// Find unsaturated path
		bool find_path_(unsigned x, std::set<unsigned> &path, std::set<unsigned> &match, std::set<unsigned> &matched_users) const;

	public:
		void read_data(const std::string &filename);
		void build_graph();
		void shortest_path() const;
		void minimum_spanning_tree() const;
		void maximum_matching() const;
		void run();
};

#endif
