/* Copyright (c) Hilmi Yildirim 2010,2011.

The software is provided on an as is basis for research purposes.
There is no additional support offered, nor are the author(s) 
or their institutions liable under any circumstances.
*/
#include "Graph.h"

Graph::Graph() {
	graph = GRA();
	vl = VertexList();
}
Graph::Graph(int size) {
	vsize = size;
	vl = VertexList(size);
	graph = GRA(size, In_OutList());
}

Graph::Graph(GRA& g, VertexList& vlist) {
	vsize = vlist.size();
	graph = g;
	vl = vlist;
}

Graph::Graph(istream& in) {
	readGraph(in);
}

Graph::~Graph() {}

void Graph::printGraph() {
	writeGraph(cout);
}

void Graph::clear() {
	vsize = 0;
	graph.clear();
	vl.clear();
}

bool Graph::contains(int src,int trg,int dim){
	int i;
	for(i=0;i<dim;i++){
		if(vl[src].pre->at(i) > vl[trg].pre->at(i))
			return false;
		if(vl[src].post->at(i) < vl[trg].post->at(i))
			return false;
	}
	return true;
}

bool Graph::incrementalContains(int src,int trg,int cur){
	int i;
	for(i=0;i<cur;i++){
		if(vl[src].pre->at(i) > vl[trg].pre->at(i))
			return false;
		if(vl[src].post->at(i) < vl[trg].post->at(i))
			return false;
	}
	return true;
}

void Graph::strTrimRight(string& str) {
	string whitespaces(" \t");
	int index = str.find_last_not_of(whitespaces);
	if (index != string::npos) 
		str.erase(index+1);
	else
		str.clear();
}

void Graph::readGraph(istream& in) {
	string buf;
	getline(in, buf);

	strTrimRight(buf);
	if (buf != "graph_for_greach") {
		cout << "BAD FILE FORMAT!" << endl;
		exit(0);
	}
	
	int n;
	getline(in, buf);
	istringstream(buf) >> n;
	// initialize
	vsize = n;
	vl = VertexList(n);
	graph = GRA(n, In_OutList());	

	for (int i = 0; i < n; i++)
		addVertex(i);

	string sub;
	int idx;
	int sid = 0;
	int tid = 0;
	while (getline(in, buf)) {
		strTrimRight(buf);
		idx = buf.find(":");
		buf.erase(0, idx+2);
		while (buf.find(" ") != string::npos) {
			sub = buf.substr(0, buf.find(" "));
			istringstream(sub) >> tid;
			buf.erase(0, buf.find(" ")+1);
			if(sid == tid) 
				cout << "Self-edge " << sid << endl;
			if(tid < 0 || tid > n)
				cout << "Wrong tid " << tid << endl;

			addEdge(sid, tid);
		}
		++sid;
	}
}	

void Graph::writeGraph(ostream& out) {
	cout << "Graph size = " << graph.size() << endl;
	out << "graph_for_greach" << endl;
	out << vl.size() << endl;

	GRA::iterator git;
	EdgeList el;
	EdgeList::iterator eit;
	VertexList::iterator vit;
	int i = 0;
	for (i = 0; i < vl.size(); i++) {
		out << i << ": ";
		el = graph[i].outList;
		for (eit = el.begin(); eit != el.end(); eit++)
			out << (*eit) << " ";
		out << "#" << endl;
	}
/*
	cout << "** In List for graph **" << endl;
	for (i = 0; i < vl.size(); i++) {
		out << i << ": ";
		el = graph[i].inList;
		for (eit = el.begin(); eit != el.end(); eit++)
			out << (*eit) << " ";
		out << "#" << endl;
	}
*/
}

void Graph::addVertex(int vid) {
	if (vid >= vl.size()) {
		int size = vl.size();
		for (int i = 0; i < (vid-size+1); i++) {
			graph.push_back(In_OutList());
			vl.push_back(Vertex(vid+i));
		}
		vsize = vl.size();
	}

	Vertex v;
	v.id = vid;
	v.top_level = -1;
	v.visited = false;
	vl[vid] = v;

	EdgeList il = EdgeList();
	EdgeList ol = EdgeList();
	In_OutList oil = In_OutList();
	oil.inList = il;
	oil.outList = ol;
	graph[vid] = oil;	
}

void Graph::addEdge(int sid, int tid) {
	if (sid >= vl.size())
		addVertex(sid);
	if (tid >= vl.size())
		addVertex(tid);
	// update edge list
	graph[tid].inList.push_back(sid);
	graph[sid].outList.push_back(tid);
}

int Graph::num_vertices() {
	return vl.size();
}

int Graph::num_edges() {
	EdgeList el;
	GRA::iterator git;
	int num = 0;
	for (git = graph.begin(); git != graph.end(); git++) {
		el = git->outList;
		num += el.size();
	}
	return num;
}

// return out edges of specified vertex
EdgeList& Graph::out_edges(int src) {
	return graph[src].outList;
}

// return in edges of specified vertex
EdgeList& Graph::in_edges(int trg) {
	return graph[trg].inList;
}

int Graph::out_degree(int src) {
	return graph[src].outList.size();
}

int Graph::in_degree(int trg) {
	return graph[trg].inList.size();
}

// get roots of graph (root is zero in_degree vertex)
vector<int> Graph::getRoots() {
	vector<int> roots;
	GRA::iterator git;
	int i = 0;
	for (git = graph.begin(), i = 0; git != graph.end(); git++, i++) {
		if (git->inList.size() == 0)
			roots.push_back(i);
	}
	
	return roots;
}

// check whether the edge from src to trg is in the graph
bool Graph::hasEdge(int src, int trg) {
	EdgeList el = graph[src].outList;
	EdgeList::iterator ei;
	for (ei = el.begin(); ei != el.end(); ei++)
		if ((*ei) == trg)
			return true;
	return false;

}

// return vertex list of graph
VertexList& Graph::vertices() {
	return vl;
}

Graph& Graph::operator=(const Graph& g) {
	if (this != &g) {
		graph = g.graph;
		vl = g.vl;
		vsize = g.vsize;
	}
	return *this;
}

// get a specified vertex property
Vertex& Graph::operator[](const int vid) {
	return vl[vid];
}

const double Graph::actualgap(const int vid){
		return vl[vid].mingap;
//	return vl[vid].mingap - vl[vid].tcs;
}

const double Graph::tcs(const int vid){
	return vl[vid].tcs;
}
Graph::Graph(hash_map<int,vector<int> >& inlist, hash_map<int,vector<int> >& outlist) {
	vsize = inlist.size();
	vl = VertexList(vsize);
	graph = GRA(vsize, In_OutList());
	for (int i = 0; i < vsize; i++)
		addVertex(i);
	hash_map<int,vector<int> >::iterator hit, hit1;
	hash_map<int,int> indexmap;
	vector<int> vec;
	vector<int>::iterator vit;
	int k;
	for (hit = inlist.begin(), k = 0; hit != inlist.end(); hit++,k++) {
		indexmap[hit->first] = k;
	}
	for (hit = inlist.begin(), hit1 = outlist.begin(), k = 0; hit != inlist.end(); hit++, hit1++, k++) {
		vec = hit->second;
		for (vit = vec.begin(); vit != vec.end(); vit++)
			graph[k].inList.push_back(indexmap[*vit]);
		vec = hit1->second;
		for (vit = vec.begin(); vit != vec.end(); vit++)
			graph[k].outList.push_back(indexmap[*vit]);
	}
}

void Graph::extract(hash_map<int,vector<int> >& inlist, hash_map<int,vector<int> >& outlist) {
	for (int i = 0; i < vl.size(); i++) {
		inlist[i] = graph[i].inList;
		outlist[i] = graph[i].outList;
	}
//	printMap(inlist,outlist);
}

// for test
void Graph::printMap(hash_map<int,vector<int> >& inlist, hash_map<int,vector<int> >& outlist) {
	cout << "==============================================" << endl;
	hash_map<int, vector<int> >::iterator hit;
	vector<int>::iterator vit;
	for (hit = outlist.begin(); hit != outlist.end(); hit++) {
		cout << hit->first << ": ";
		vector<int> vec = hit->second;
		for (vit = vec.begin(); vit != vec.end(); vit++)
			cout << *vit << " ";
		cout << "#" << endl;
	}
	cout << "In List for graph" << endl;
	for (hit = inlist.begin(); hit != inlist.end(); hit++) {
		cout << hit->first << ": ";
		vector<int> vec = hit->second;
		for (vit = vec.begin(); vit != vec.end(); vit++)
			cout << *vit << " ";
		cout << "#" << endl;
	}
	cout << "================================================" << endl;
}
