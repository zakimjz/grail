/* Copyright (c) Hilmi Yildirim 2010,2011.

The software is provided on an as is basis for research purposes.
There is no additional support offered, nor are the author(s) 
or their institutions liable under any circumstances.
*/
#ifndef _GRAPH_UTIL_H_
#define _GRAPH_UTIL_H_

#include "Graph.h"
#include <sys/time.h>

class GraphUtil {
	public:
	static void dfs(Graph& g, int vid, vector<int>& preorder, vector<int>& postorder, vector<bool>& visited);
		static void topo_leveler(Graph& g);
		static int topo_level(Graph& g, int vid);
		static void topological_sort(Graph g, vector<int>& ts);
		static void tarjan(Graph& g, int vid, int& index, hash_map< int, pair<int,int> >& order, vector<int>& sn, 
			multimap<int, int>& sccmap, int& scc);
		static void mergeSCC(Graph& g, int* on, vector<int>& ts);
		static void traverse(Graph& tree, int vid, int& pre_post, vector<bool>& visited);
		static void pre_post_labeling(Graph& tree);

		static void genRandomGraph(int n, double c, char* filename);
};
#endif
