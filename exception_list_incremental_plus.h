/* Copyright (c) Hilmi Yildirim 2010,2011.

The software is provided on an as is basis for research purposes.
There is no additional support offered, nor are the author(s) 
or their institutions liable under any circumstances.
*/
#ifndef _EXCEPTION_LIST_INCREMENTAL_PLUS_H
#define _EXCEPTION_LIST_INCREMENTAL_PLUS_H

#include"Graph.h"
//#include"topological_sort.h"
#include"time_tracker.h"
#include"utils.h"
#include"interval_tree.h"
#include"exception_list.h"
#include"exception_list_incremental_base.h"
#include<vector>
#include<map>

class ExceptionListIncrementalPlus : public ExceptionListIncrementalBase{
	
	public :
		map<int,int> * in_count;
		int DIM;
	ExceptionListIncrementalPlus(Graph& org, int dim, bool lf){
		int i;
		DIM = dim;
		LEVEL_FILTER = lf;
		size = org.num_vertices();
		lists = new hash_set<int>[size];
		directs = new hash_set<int>[size];
		indirects = new hash_set<int>[size];
		in_count = new map<int,int>[size];		
		time_tracker tt;  		
		tt.start();
		getIntervals(org,0);
		cout << " starting " << endl;
		
		obtainDirectExceptions(org);
		obtainIndirectExceptions(org);
  		tt.stop();
		cout << "Initial Step completed in " << tt.print() << endl;
		times.push_back(tt.print());	
		for(i=1;i<DIM; i++){
		time_tracker tt3;  		
			tt3.start();
			cout << " Step " << i << " started" << endl;
			getIntervals(org,i);
			obtainIncrementalExceptions(org,i);
  			tt3.stop();
			cout << " Step " << i << " completed in "  << tt3.print() << " seconds " << endl;
			times.push_back(tt3.print());	
		}
		for(i=0;i<size;i++){
			set_union(directs[i].begin() , directs[i].end(), indirects[i].begin(), indirects[i].end(),
				std::insert_iterator<hash_set<int> >(lists[i],lists[i].end()));

		}
		//printExceptions();
		printStats(DIM);		
	}


/*	
	void obtainInitialExceptions(const Graph& g, GraphIndex& gi){
		int i,j,k,count=0,count2=0;
		int opencount =0;
		time_tracker tt;
		IntervalTree intervalTree;
		TemplateStack<VertexInterval*> * results;
		TemplateStack<VertexInterval*> * results2;
		AdjItr t1,t2;
		std::pair<AdjItr, AdjItr> out;
		vector<pair<int, int> >  openIntervals;
		tt.start();
		vector<int> extras;
		for(i=0; i<vlist.size(); i++){
			extras.clear();
			openIntervals = findOpenIntervals(g,vlist[i],extras);
			for(j=0;j<openIntervals.size()-1;j++){
				opencount++;
				results = (TemplateStack<VertexInterval*>*) intervalTree.Enumerate(
									openIntervals[j].first*2+1, openIntervals[j].second*2-1);		
	
				for(k=0; k < results->Size() ; k++){
					if((*results)[k]->GetLowPoint()/2 >= intervals[vlist[i]].first && 
						(*results)[k]->GetHighPoint()/2 <= intervals[vlist[i]].second )
		//			if(gi.contains(vlist[i],(*results)[k]->_v))
						lists[vlist[i]].insert((*results)[k]->_v);
				}
			}
		// The following is seperated from the loop, since some openIntervals arrays contain a dummy interval at the end	
			j = openIntervals.size()-1;
			if(openIntervals[j].first !=openIntervals[j].second-1){
				opencount++;
				results = (TemplateStack<VertexInterval*>*) intervalTree.Enumerate(
									openIntervals[j].first*2+1, openIntervals[j].second*2-1);		
	
				for(k=0; k < results->Size() ; k++){
					if((*results)[k]->GetLowPoint()/2 >= intervals[vlist[i]].first && 
							(*results)[k]->GetHighPoint()/2 <= intervals[vlist[i]].second )
		//			if(gi.contains(vlist[i],(*results)[k]->_v))
						lists[vlist[i]].insert((*results)[k]->_v);
				}
			}
			
			IntervalTree it;
			for(t1=out.first ; t1!=out.second; t1++){
				it.Insert(new VertexInterval((Vertex&)t1));
			}
			int t;
			out = adjacent_vertices(vlist[i],g);
			for(j=0;j<extras.size();j++){
				results = (TemplateStack<VertexInterval*>*) intervalTree.Enumerate(
									extras[j]*2+1, extras[j]*2+1);		
				results2 = (TemplateStack<VertexInterval*>*) it.Enumerate(
									extras[j]*2+1, extras[j]*2+1);		
				for(k=0; k < results->Size() ; k++){
					if((*results)[k]->GetLowPoint()/2 >= intervals[vlist[i]].first && 
							(*results)[k]->GetHighPoint()/2 <= intervals[vlist[i]].second ){
						for(t=0; t < results2->Size() ; t++){
						//for(t1=out.first ; t1!=out.second; t1++){
						//	if((*results)[k]->GetLowPoint()/2 >= intervals[*t1].first && 
						//			(*results)[k]->GetHighPoint()/2 <= intervals[*t1].second )
							if((*results)[k]->GetLowPoint() >= (*results2)[t]->GetLowPoint() && 
									(*results)[k]->GetHighPoint() <= (*results2)[t]->GetHighPoint())
								break;
						}
						if(t==results2->Size()){
							lists[vlist[i]].insert((*results)[k]->_v);
						}
					}
				}
			}

			count+= lists[vlist[i]].size();
			intervalTree.Insert(new VertexInterval(vlist[i]));
		}
		tt.stop();
		cout << "Direct Exceptions " << tt.print() << endl;
		int *used = new int[num_vertices(g)];
		fill(used, used + num_vertices(g), -1);
		Vertex child;
		set<int>::iterator setItr;
		bool flag;
		for(i=0;i<vlist.size();i++){
			out = adjacent_vertices(vlist[i],g);
			for(t1 = out.first; t1 != out.second ; t1++){
				for(setItr = lists[*t1].begin() ; setItr != lists[*t1].end(); setItr++){
		//			if(lists[vlist[i]].find(*setItr)!=lists[vlist[i]].end()) break;
					if(used[*setItr]==i || lists[vlist[i]].find(*setItr)!=lists[vlist[i]].end()) continue;
					used[*setItr]=i;
					flag = true;
					for(t2 = out.first; t2 != out.second ; t2++){
						if(t1!=t2 && (intervals[*t2].first <= intervals[*setItr].first 
										&& intervals[*t2].second >= intervals[*setItr].second)
										&& lists[*t2].find(*setItr) == lists[*t2].end()){
							flag = false;
							break;
						}
					}
					if(flag){
						count2++;
						lists[vlist[i]].insert(*setItr);
					}
				}			
			}
		}

		cout << "count 0: " << count << endl;
		count =0;
		for(i=0;i<vlist.size();i++)
			count += lists[i].size();
		cout << "count 1: " << count << endl;
		cout << "count 2:  " << count2 << endl;
		counts.push_back(count);
		cout << "opencount " << opencount << endl;
	}
*/

	void obtainIndirectExceptions(Graph& g){
	
		int i,count2=0;
		int opencount =0;
//		AdjItr t1,t2;
//		std::pair<AdjItr, AdjItr> out;
		EdgeList::iterator t1,t2;
		EdgeList el;
	
		int *used = new int[g.num_vertices()];
		fill(used, used + g.num_vertices(), -1);
		Vertex child;
		hash_set<int>::iterator setItr;
		set<int> merged;
		bool flag;
		int numberOfCopy;  
		for(i=0;i<vlist.size();i++){
//			std::pair<AdjItr, AdjItr> out = adjacent_vertices(vlist[i],g);
			el = g.out_edges(vlist[i].id);
//			for(t1 = out.first; t1 != out.second ; t1++){
			for(t1 = el.begin(); t1 != el.end() ; t1++){
				for(setItr = directs[*t1].begin() ; setItr != directs[*t1].end(); setItr++){
					if(used[*setItr]==i || indirects[vlist[i].id].find(*setItr)!=indirects[vlist[i].id].end()
											|| directs[vlist[i].id].find(*setItr)!=directs[vlist[i].id].end()) 
							continue;
					used[*setItr]=i;
					flag = true;
					numberOfCopy = 1;
//					for(t2 = out.first; t2 != out.second ; t2++){
					for(t2 = el.begin(); t2 != el.end() ; t2++){
						if(t1!=t2 && (intervals[g[*t2]].first <= intervals[g[*setItr]].first 
									  && intervals[g[*t2]].second >= intervals[g[*setItr]].second)){
						   if  (directs[*t2].find(*setItr) == directs[*t2].end()
									&& indirects[*t2].find(*setItr) == indirects[*t2].end()){
								flag = false;
								break;
							}
							else{
								numberOfCopy++;
							}
						}
					}
					if(flag){
						count2++;
						indirects[vlist[i].id].insert(*setItr);
						in_count[vlist[i].id][*setItr]=numberOfCopy;
					}
				}

				for(setItr = indirects[*t1].begin() ; setItr != indirects[*t1].end(); setItr++){
					if(used[*setItr]==i || indirects[vlist[i].id].find(*setItr)!=indirects[vlist[i].id].end()
											|| directs[vlist[i].id].find(*setItr)!=directs[vlist[i].id].end()) 
							continue;
					used[*setItr]=i;
					flag = true;
					numberOfCopy=1;
//					for(t2 = out.first; t2 != out.second ; t2++){
					for(t2 = el.begin(); t2 != el.end() ; t2++){
						if(t1!=t2 && (intervals[g[*t2]].first <= intervals[g[*setItr]].first 
									  && intervals[g[*t2]].second >= intervals[g[*setItr]].second)){
						   if  (directs[*t2].find(*setItr) == directs[*t2].end()
									&& indirects[*t2].find(*setItr) == indirects[*t2].end()){
								flag = false;
								break;
							}
							else{
								numberOfCopy++;
							}
						}
					}
					if(flag){
						count2++;
						indirects[vlist[i].id].insert(*setItr);
						in_count[vlist[i].id][*setItr]=numberOfCopy;
					}

				}
			}
		}
		
		int count = 0, count1 =0;
		for(i=0;i<vlist.size();i++){
			count += directs[i].size();
			count1 += indirects[i].size();
		}
		cout << "# Directs  : " << count << endl;
		cout << "# Indirects: " << count1<< endl;
		cout << "# Total    : " << count + count1<< endl;

		counts.push_back(count+count1);
		cout << "opencount " << opencount << endl;
	}
	




	void obtainIncrementalExceptions(Graph& g, int cur){
		int i,j,k,count=0,count2=0;
					
//		std::pair<InvAdjItr, InvAdjItr> ie;
		EdgeList el;
		EdgeList::iterator t1,t2;
//		InvAdjItr t1,t2;
		Vertex child;
		vector<int> toBeRemoved;
		hash_set<int>::iterator setItr;
		bool flag;
		for(i=0;i<vlist.size();i++){
			toBeRemoved.clear();
//			ie = inv_adjacent_vertices(vlist[i],g);
			el = g.in_edges(vlist[i].id);

			for(setItr = directs[vlist[i].id].begin() ; setItr != directs[vlist[i].id].end(); setItr++){
				if(intervals[vlist[i]].first > intervals[g[*setItr]].first 
						|| intervals[vlist[i]].second < intervals[g[*setItr]].second){
					toBeRemoved.push_back(*setItr);
				}
/*				else{
					for(t1 = out.first; t1 != out.second ; t1++){
						if(gi.incrementalContains(*t1,*setItr,cur)
										&& lists[*t1].find(*setItr) == lists[*t1].end()){
							toBeRemoved.push_back(*setItr);
							break;
						}
					}
				}
*/
			}
			int removethis;
			for(j=0;j<toBeRemoved.size();j++){
				removethis = toBeRemoved[j];
//				for(t1=ie.first ; t1 != ie.second; t1++){
				for(t1=el.begin() ; t1 != el.end(); t1++){
					if(indirects[*t1].find(removethis) != indirects[*t1].end()){
						if(--in_count[*t1][removethis]==0){
							indirects[*t1].erase(indirects[*t1].find(removethis));
							directs[*t1].insert(removethis);
						}
					}
				}
				directs[vlist[i].id].erase(directs[vlist[i].id].find(removethis));
			}
		}
		count =0;
		for(i=0;i<vlist.size();i++)
			count += directs[i].size() + indirects[i].size();
		counts.push_back(count);
		cout << "count 1: " << count << endl;

	}

};

#endif
