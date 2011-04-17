/* Copyright (c) Hilmi Yildirim 2010,2011.

The software is provided on an as is basis for research purposes.
There is no additional support offered, nor are the author(s) 
or their institutions liable under any circumstances.
*/
#include "GraphUtil.h"
// depth first search given a start node
void GraphUtil::dfs(Graph& g, int vid, vector<int>& preorder, vector<int>& postorder, vector<bool>& visited) {
	visited[vid] = true;
	preorder.push_back(vid);
	EdgeList el = g.out_edges(vid);
	EdgeList::iterator eit;
	int nextid = -1;
	// check whether all child nodes are visited
	for (eit = el.begin(); eit != el.end(); eit++) {
		if (!visited[*eit]) {
			nextid = *eit;
			dfs(g, nextid, preorder, postorder, visited);
		}
	}
	postorder.push_back(vid);
}

void GraphUtil::topo_leveler(Graph& g){
	int N = g.num_vertices();
	vector<int>::iterator sit;
	// depth-first-search whole graph
	for (int i=0; i < N ; i++) 
			topo_level(g, i);
}

int GraphUtil::topo_level(Graph& g, int vid){
	if(g[vid].top_level != -1){
		return g[vid].top_level;
	}
	int min = g.num_vertices();	
	int max = -1;
	g[vid].top_level = 0;
	EdgeList el = g.in_edges(vid);
	EdgeList::iterator eit;
	for(eit = el.begin(); eit != el.end(); eit++){
		max = max > topo_level(g,*eit) ? max : g[*eit].top_level;
		min = min < g[*eit].top_level ? min : g[*eit].top_level;
	}
	g[vid].top_level = max + 1;
	g[vid].min_parent_level = (min == g.num_vertices() ? -1 : min );
	return g[vid].top_level;
}

// topological sorting by depth-first search
// return reverse order of topological_sorting list
void GraphUtil::topological_sort(Graph g, vector<int>& ts) {
	vector<bool> visited(g.num_vertices(), false);
	vector<int> preorder;
	vector<int> postorder;
	vector<int> roots = g.getRoots();
	vector<int>::iterator sit;
	// depth-first-search whole graph
	for (sit = roots.begin(); sit != roots.end(); sit++) 
		if (!visited[*sit])
			dfs(g, (*sit), preorder, postorder, visited);
	
	ts = postorder;

	// for test
/*
	cout << "Topo sorting(GraphUtil): ";
	vector<int>::iterator vit;
	for (vit = ts.begin(); vit != ts.end(); vit++)
		cout << (*vit) << " ";
	cout << endl;
*/
}

// implement tarjan's algorithm to find Strongly Connected Component from a given start node
void GraphUtil::tarjan(Graph& g, int vid, int& index, hash_map< int, pair<int,int> >& order, 
	vector<int>& sn, multimap<int,int>& sccmap, int& scc) {
//	cout << " inside tarjan " << endl;	
	order[vid].first = index;
	order[vid].second = index;
	index++;
//	cout << " before pushing vid = " << vid << endl;
	sn.push_back(vid);
//	cout << " after pushing vid = " << vid << endl;
	g[vid].visited = true;
	EdgeList el = g.out_edges(vid);
	EdgeList::iterator eit;
	for (eit = el.begin(); eit != el.end(); eit++) {
		if (!g[*eit].visited) {
			tarjan(g, *eit, index, order, sn, sccmap, scc);
			order[vid].second = min(order[*eit].second, order[vid].second);
		}
		else if (find(sn.begin(), sn.end(), (*eit)) != sn.end()) {
			order[vid].second = min(order[*eit].first, order[vid].second);
		}
	}

//	cout << " in the middle of tarjan "<< endl;	
	if (order[vid].first == order[vid].second) {
		vector<int>::reverse_iterator rit;
		for (rit = sn.rbegin(); rit != sn.rend(); rit++) {
			if ((*rit) != vid) {
				sccmap.insert(make_pair(scc, *rit));
			//	sccmap[*rit] = scc;
				sn.pop_back();
			}
			else {
				sccmap.insert(make_pair(scc, *rit));
			//	sccmap[*rit] = scc;
				sn.pop_back();
				break;
			}
		}
		scc++;
	}

//	cout << " outside tarjan "<< endl;	
}

// merge Strongly Connected Component
// return vertex map between old vertex and corresponding new merged vertex
void GraphUtil::mergeSCC(Graph& g, int* on, vector<int>& reverse_topo_sort) {
	vector<int> sn;
	hash_map< int, pair<int, int> > order;
	int ind = 0;
	multimap<int, int> sccmap;	// each vertex id correspond with a scc num 
	int scc = 0;
	int vid;
	int origsize = g.num_vertices();
//	cout << " inside MergeSCC "<< endl;	
	for (int i = 0; i < origsize; i++) {
		vid = i;
		if (g[vid].visited)
			continue;
		tarjan(g, vid, ind, order, sn, sccmap, scc);
	}
//	cout << " inside MergeSCC after tarjan "<< endl;	
	// no component need to merge
	if (scc == origsize) {
		for (int i = 0; i < origsize; i++)
			on[i] = i;
		// topological sort
		topological_sort(g, reverse_topo_sort);
		// update graph's topological id
		for (int i = 0; i < reverse_topo_sort.size(); i++)
			g[reverse_topo_sort[i]].topo_id = reverse_topo_sort.size()-i-1;

		return;
	}


	hash_map<int, vector<int> > inlist, outlist;
	g.extract(inlist, outlist);
	
	multimap<int,int>::iterator mit;
	mit = sccmap.begin();
	int num_comp;
	int maxid = g.num_vertices()-1;
	while (mit != sccmap.end()) {
		num_comp = mit->first;
		
		if (++sccmap.lower_bound(num_comp) == sccmap.upper_bound(num_comp)) {
			on[mit->second] = mit->second;
			++mit;
			continue;
		}

		maxid++;
		inlist[maxid] = vector<int>();
		outlist[maxid] = vector<int>();
		
		for (; mit != sccmap.upper_bound(num_comp); ++mit) {
			on[mit->second] = maxid;

			vector<int> vec = inlist[mit->second];
			vector<int>::iterator vit, vit1;
			vector<int> vec1;
			bool hasEdge = false;

			// copy all incoming edges
			for (vit = vec.begin(); vit != vec.end(); vit++) {
				hasEdge = false;
				vec1 = outlist[*vit];
				for (vit1 = vec1.begin(); vit1 != vec1.end(); vit1++) {
					if (*vit1 == maxid) {
						hasEdge = true;
						break;
					}
				}
				if (!hasEdge && (*vit != maxid)) {
					inlist[maxid].push_back(*vit);
					outlist[*vit].push_back(maxid);
				}
			}

			// copy all outgoing edges
			vec = outlist[mit->second];
			for (vit = vec.begin(); vit != vec.end(); vit++) {
				hasEdge = false;
				vec1 = inlist[*vit];
				for (vit1 = vec1.begin(); vit1 != vec1.end(); vit1++)
					if (*vit1 == maxid) {
						hasEdge = true;
						break;
					}
				if (!hasEdge && (*vit != maxid)) {
					outlist[maxid].push_back(*vit);
					inlist[*vit].push_back(maxid);
				}
			}
			
			// delete old vertex
			vec = inlist[mit->second];
			for (vit = vec.begin(); vit != vec.end(); vit++) {
				for (vit1 = outlist[*vit].begin(); vit1 != outlist[*vit].end(); )
					if (*vit1 == mit->second)
						outlist[*vit].erase(vit1);
					else
						vit1++;
			}
			vec = outlist[mit->second];
			for (vit = vec.begin(); vit != vec.end(); vit++) {
				for (vit1 = inlist[*vit].begin(); vit1 != inlist[*vit].end(); )
					if (*vit1 == mit->second)
						inlist[*vit].erase(vit1);
					else
						vit1++;
			}
			outlist.erase(mit->second);
			inlist.erase(mit->second);
		}
	}			

	g = Graph(inlist, outlist);
	
	// topological sort
	topological_sort(g, reverse_topo_sort);
	// update graph's topological id
	for (int i = 0; i < reverse_topo_sort.size(); i++)
		g[reverse_topo_sort[i]].topo_id = reverse_topo_sort.size()-i-1;

	// update index map
	hash_map<int,int> indexmap;
	hash_map<int, vector<int> >::iterator hit;
	int k;
	for (hit = outlist.begin(), k=0; hit != outlist.end(); hit++, k++) {
		indexmap[hit->first] = k;
	}
	for (k = 0; k < origsize; k++)
		on[k] = indexmap[on[k]];

/*
	cout << "Index Map" << endl;
	for (int i = 0; i < origsize; i++)
		cout << on[i] << " ";
	cout << endl;
	cout << "roots: " << g.getRoots().size() << endl;
*/
}

// traverse tree to label node with pre and post order by giving a start node
// using GRIPP's labeling method
void GraphUtil::traverse(Graph& tree, int vid, int& pre_post, vector<bool>& visited) {
	visited[vid] = true;
	EdgeList el = tree.out_edges(vid);
	EdgeList::iterator eit;
	int pre_order;
	for (eit = el.begin(); eit != el.end(); eit++) {
		pre_order = pre_post;
		pre_post++;
		if (!visited[*eit])
			traverse(tree, *eit, pre_post, visited);
		tree[*eit].pre_order = pre_order;
		tree[*eit].post_order = pre_post;
		pre_post++;
	}
}

// compute interval label for each node of tree (pre_order, post_order)
void GraphUtil::pre_post_labeling(Graph& tree) {
	vector<int> roots = tree.getRoots();
	vector<int>::iterator sit;
	int pre_post = 0;
	int pre_order = 0;
	vector<bool> visited(tree.num_vertices(), false);
	
	for (sit = roots.begin(); sit != roots.end(); sit++) {
		pre_order = pre_post;
		pre_post++;
		traverse(tree, *sit, pre_post, visited);
		tree[*sit].pre_order = pre_order;
		tree[*sit].post_order = pre_post;
		pre_post++;
	}
}


// for test
void GraphUtil::genRandomGraph(int n, double c, char* filename) {
	int threshold = int(c*1.00/(n*1.00)*10000);
	Graph g;
	int i, j;
	int rand_num;
	for (i = 0; i < n; i++) 
		g.addVertex(i);

	srand(time(NULL));
	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++)
			if (i != j) {
				rand_num = rand() % 10000;
				if (rand_num < threshold)
					g.addEdge(i,j);
			}
	}

	ofstream out(filename);
	g.writeGraph(out);
}
