/* Copyright (c) Hilmi Yildirim 2010,2011.

The software is provided on an as is basis for research purposes.
There is no additional support offered, nor are the author(s) 
or their institutions liable under any circumstances.
*/
#ifndef _GRAPH_H
#define _GRAPH_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <map>
#include <list>
#include <deque>
#include <algorithm>
#include <utility>
#include <cmath>
#include <ext/hash_map>

namespace std {using namespace __gnu_cxx;}
using namespace std;

#define MAX_VAL 10000000
#define MIN_VAL -10000000

struct Vertex {
	int id;
	int top_level;	// topological level
	int min_parent_level;	// level of the highest parent in top_order
	int topo_id;	// topological order
	int min_int;
	long volume;
	double adj_vol;
	bool visited;
	int dfs_order;
	int pre_order;	
	int post_order;
	double tcs;
	int mingap;
	vector<int> *pre;
	vector<int> *post;
	vector<int> *middle;
	Vertex(int ID) : id(ID) {
		top_level = -1;
		visited = false;
	}
	Vertex(){
		top_level = -1;
		visited = false;
	};
};

struct VertexCompare {
  bool operator() ( const Vertex p1, const Vertex p2 ) const
  {
    return p1.id < p2.id;
  }
};

typedef vector<int> EdgeList;	// edge list represented by vertex id list
typedef vector<Vertex> VertexList;	// vertices list (store real vertex property) indexing by id

struct In_OutList {
	EdgeList inList;
	EdgeList outList;
};
typedef vector<In_OutList> GRA;	// index graph

class Graph {
	private:
		GRA graph;
		VertexList vl;
		int vsize;
		
	public:
		Graph();
		Graph(int);
		Graph(GRA&, VertexList&);
		Graph(istream&);
		~Graph();
		bool contains(int src, int trg,int dim);
		bool incrementalContains(int src, int trg, int cur);
		void readGraph(istream&);
		void writeGraph(ostream&);
		void printGraph();
		void addVertex(int);
		void addEdge(int, int);
		int num_vertices();
		int num_edges();
		VertexList& vertices();
		EdgeList& out_edges(int);
		EdgeList& in_edges(int);
		int out_degree(int);
		int in_degree(int);
		vector<int> getRoots();
		bool hasEdge(int, int);	
		Graph& operator=(const Graph&);
		Vertex& operator[](const int);
		const double actualgap(const int);
		const double tcs(const int);
		
		void clear();
		void strTrimRight(string& str);

		Graph(hash_map<int,vector<int> >& inlist, hash_map<int,vector<int> >& outlist);
		void extract(hash_map<int,vector<int> >& inlist, hash_map<int,vector<int> >& outlist);
		void printMap(hash_map<int,vector<int> >& inlist, hash_map<int,vector<int> >& outlist);

};	

#endif
