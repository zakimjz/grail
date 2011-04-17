/* Copyright (c) Hilmi Yildirim 2010,2011.

The software is provided on an as is basis for research purposes.
There is no additional support offered, nor are the author(s) 
or their institutions liable under any circumstances.
*/
#ifndef _EXCEPTION_LIST_INCREMENTAL_BASE_H
#define _EXCEPTION_LIST_INCREMENTAL_BASE_H

#include"Graph.h"
#include"time_tracker.h"
#include"utils.h"
#include"exception_list.h"
#include"interval_tree.h"
#include<vector>
#include<map>

class ExceptionListIncrementalBase : public ExceptionList{
	
public:
	
//	std::pair<VertexItr, VertexItr> vp;
	vector<Vertex > vlist;
	
	hash_set<int> * directs;
	hash_set<int> * indirects;
	
	vector<int> counts;
	vector<double > times;
	static map<Vertex, pair<int,int >, VertexCompare > intervals;
	VertexList vl;
	
	class VertexInterval : public Interval
	{
		public :
		Vertex& _v;
		VertexInterval(Vertex &v) 
			:_v(v){}
		int GetLowPoint()  const {return intervals[_v].first*2;}
		int GetHighPoint() const {return intervals[_v].second*2;}
	};
	
	class ChildInterval : public Interval
	{
		public :
		int x,y;
		ChildInterval(int _x,int _y) 
			:x(_x),y(_y){}
		int GetLowPoint() {return x*2;}
		int GetHighPoint() {return y*2;}
	};
	//bool operator<(const Vertex k1, const Vertex k2) { return k1.id < k2.id; };

	
	
	struct vertex_compare : public binary_function<Vertex, Vertex, bool> {
		bool operator()(const Vertex& x, const Vertex& y)
	{ return intervals[x].second -intervals[x].first < intervals[y].second - intervals[y].first ; }
	}vc;
	
	struct children_compare : public binary_function<Vertex, Vertex, bool> {
		bool operator()(const Vertex& x, const Vertex& y){ 
			if(intervals[x].first == intervals[y].first)
				return intervals[x].second > intervals[y].second;
			return intervals[x].first < intervals[y].first ; 
		}
	}cc;
	
	void printStats(int dim){
		for(int i=0;i<dim; i++)
			cout << counts[i] << "\t" ;
		cout << endl;
		for(int i=0;i<dim; i++)
			cout << times[i] << "\t" ;
		cout << endl;
		cout << "Total Number of Exceptions : "  << counts[dim-1] << endl;
		cout << endl;
	}

	// TODO : Can be optimized? ()
	void getIntervals(Graph& org, int index){
		int N = org.num_vertices();
		vlist.clear();
		for(int i=0; i< N;i++){
			intervals[org[i]] = make_pair(org[i].pre->at(index),org[i].post->at(index));
//			cout << "interval["<<i<<"]= ["<< intervals[org[i]].first << "," << intervals[org[i]].second << "]"<< endl; 
			vlist.push_back(org[i]);
		}
//		cout << endl;

//		for(int i=0; i< N;i++){
//			cout << "interval["<<i<<"]= ["<< intervals[vlist[i]].first << "," << intervals[vlist[i]].second << endl; 
//		}
		sort(vlist.begin(), vlist.end(), vc);
//		for(int i=0; i< N;i++){
//			cout << "interval["<<i<<"]= ["<< intervals[vlist[i]].first << "," << intervals[vlist[i]].second << endl; 
//		}
	}
	
	
	vector<pair<int, int> > findOpenIntervals(Graph& g, const Vertex& v, vector<int>& extra){
		vector<pair<int, int> > result;
		vector<Vertex > vlist;
		vector<Vertex >::iterator vit;
		EdgeList::iterator eit;
		EdgeList el;
//		std::pair<AdjItr, AdjItr> out = adjacent_vertices(v, g);
		el = g.out_edges(v.id);

		
		result.push_back(make_pair(intervals[v].first,intervals[v].second));
//		cout << " Parent [ " << intervals[v].first << " " << intervals[v].second << "]" << endl; 	
//		for(; out.first != out.second ; out.first++){
		for(eit=el.begin(); eit!=el.end(); eit++){
	//		cout << "[ " << intervals[g[*eit]].first << " " << intervals[g[*eit]].second << " ] " << endl;
			vlist.push_back(g[*eit]);
		}
		//cout << endl << endl;
		
		sort(vlist.begin(),vlist.end(),cc);
		/*// Print children intervals	
			for(vit = vlist.begin(); vit != vlist.end() ; vit++){
				//cout << "[ " << intervals[*vit].first << " " << intervals[*vit].second << " ] " << endl;
			}
		*/
		int index = 0;
		for(vit = vlist.begin(); vit != vlist.end() ; vit++){
			// if children start from the same point, just process the fist one..
			
			//if(vit!=vlist.begin() && intervals[*vit].first == intervals[*(vit-1)].first){
			// Trying something, if not work use the line above
			if(intervals[*vit].second <= result[index].first){
				continue;
			}
			
			if(intervals[*vit].second < result[index].first){
				extra.push_back(intervals[*vit].second);
				continue;
			}
			//cout << intervals[*vit].first << " " <<  result[index].first << endl;
			
			/*  Modification to resolve the bug : Original Version */
			if(intervals[*vit].first <= result[index].first){
				if(intervals[*vit].first != result[index].first)
					extra.push_back(result[index].first);
				result[index].first =  intervals[*vit].second;
			}
			
			/*  Modification to resolve the bug : New Version
			if(intervals[*vit].first <= result[index].first){
				result.push_back(make_pair(intervals[*vit].second,result[index].second));
				result[index].second = result[index].first+1; 
				index++;
			}*/
			else{
				result.push_back(make_pair(intervals[*vit].second,result[index].second));
				result[index].second = intervals[*vit].first;
				index++;
			}
		}
		//cout << " Open Intervals " << endl;
		
			// Print open intervals	
//			for(index = 0;index < result.size(); index++){
//				cout << " [ " << result[index].first << " " << result[index].second << "] " << endl;
//			}
	
		return result;
		}
	
	void obtainDirectExceptions(Graph& g){
		int i,j,k,count=0,count2=0;
		int opencount =0;
		time_tracker tt;
		IntervalTree intervalTree;
		TemplateStack<VertexInterval*> * results;
		TemplateStack<VertexInterval*> * results2;
		EdgeList::iterator t1,t2;
		EdgeList el;
//		std::pair<AdjItr, AdjItr> out;
		vector<pair<int, int> >  openIntervals;
		tt.start();
		vector<int> extras;
		for(i=0; i<vlist.size(); i++){
			extras.clear();
			openIntervals = findOpenIntervals(g,vlist[i],extras);
			for(j=0;j<openIntervals.size()-1;j++){
		//		cout << "j = " << j << endl;
				opencount++;
				results = (TemplateStack<VertexInterval*>*) intervalTree.Enumerate(
																				   openIntervals[j].first*2+1, openIntervals[j].second*2-1);		
				
				for(k=0; k < results->Size() ; k++){
					if((*results)[k]->GetLowPoint()/2 >= intervals[vlist[i]].first && 
					   (*results)[k]->GetHighPoint()/2 <= intervals[vlist[i]].second )
						//			if(gi.contains(vlist[i],(*results)[k]->_v))
						if(!LEVEL_FILTER || vlist[i].min_parent_level <=  (*results)[k]->_v.top_level)
							directs[vlist[i].id].insert((*results)[k]->_v.id);
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
						if(!LEVEL_FILTER || vlist[i].min_parent_level <=  (*results)[k]->_v.top_level)
							directs[vlist[i].id].insert((*results)[k]->_v.id);
				}
			}
			
			el = g.out_edges(vlist[i].id);	
//			out = adjacent_vertices(vlist[i],g);

			// Old Version
			for(j=0;j<extras.size();j++){
				results = (TemplateStack<VertexInterval*>*) intervalTree.Enumerate(
																				   extras[j]*2+1, extras[j]*2+1);		
				for(k=0; k < results->Size() ; k++){
					if((*results)[k]->GetLowPoint()/2 >= intervals[vlist[i]].first && 
					   (*results)[k]->GetHighPoint()/2 <= intervals[vlist[i]].second ){
//						for(t1=out.first ; t1!=out.second; t1++){
						for(t1=el.begin() ; t1!=el.end(); t1++){
							if((*results)[k]->GetLowPoint()/2 >= intervals[g[*t1]].first && 
							   (*results)[k]->GetHighPoint()/2 <= intervals[g[*t1]].second )
								break;
						}
//						if(t1==out.second){
						if(t1==el.end()){
							if(!LEVEL_FILTER || vlist[i].min_parent_level <=  (*results)[k]->_v.top_level)		// it is "<=" instead of "<" to be able to capture indirects 
								directs[vlist[i].id].insert((*results)[k]->_v.id);
						}
					}
				}
			}
			
			/*	
				int t;
			 IntervalTree it;
			 for(t1=out.first ; t1!=out.second; t1++){
				 it.Insert(new ChildInterval(intervals[*t1].first,intervals[*t1].second));
			 }
			 bool flag;	
			 // Efficient Version ???
			 for(j=0;j<extras.size();j++){
				 results = (TemplateStack<VertexInterval*>*) intervalTree.Enumerate(
																					extras[j]*2+1, extras[j]*2+1);		
				 results2 = (TemplateStack<VertexInterval*>*) it.Enumerate(
																		   extras[j]*2+1, extras[j]*2+1);		
				 for(k=0; k < results->Size() ; k++){
					 flag = true;
					 if((*results)[k]->GetLowPoint()/2 >= intervals[vlist[i]].first && 
						(*results)[k]->GetHighPoint()/2 <= intervals[vlist[i]].second ){
						 for(t=0; t < results2->Size() ; t++){
							 if((*results)[k]->GetLowPoint() >= (*results2)[t]->GetLowPoint() && 
								(*results)[k]->GetHighPoint() <= (*results2)[t]->GetHighPoint()){
								 flag = false;
								 break;
							 }
						 }
						 if(flag){
							 lists[vlist[i]].insert((*results)[k]->_v);
						 }
					 }
				 }
			 }
			 */
			count+= directs[vlist[i].id].size();
			intervalTree.Insert(new VertexInterval(vlist[i]));
		}
		tt.stop();
		cout << "Completed Direct Exceptions in " << tt.print() << endl;
	}
		
	void obtainIndirectExceptions(Graph& g){
	
		int i,count2=0;
		int opencount =0;
		EdgeList::iterator t1,t2;
//		std::pair<AdjItr, AdjItr> out;
		EdgeList el;
	
		int *used = new int[g.num_vertices()];
		fill(used, used + g.num_vertices(), -1);
		Vertex child;
		hash_set<int>::iterator setItr;
		set<int> merged;
		bool flag;
		for(i=0;i<vlist.size();i++){
//			std::pair<AdjItr, AdjItr> out = adjacent_vertices(vlist[i],g);
			el = g.out_edges(vlist[i].id);
//			for(t1 = out.first; t1 != out.second ; t1++){
			for(t1 = el.begin(); t1 != el.end() ; t1++){
				for(setItr = directs[*t1].begin() ; setItr != directs[*t1].end(); setItr++){
					if(used[*setItr]==i 
											|| (LEVEL_FILTER && vlist[i].top_level >= g[*setItr].top_level) 
											|| indirects[vlist[i].id].find(*setItr)!=indirects[vlist[i].id].end()
											|| directs[vlist[i].id].find(*setItr)!=directs[vlist[i].id].end()) 
							continue;
					used[*setItr]=i;
					flag = true;
//					for(t2 = out.first; t2 != out.second ; t2++){
					for(t2 = el.begin(); t2 != el.end() ; t2++){
						if(t1!=t2 && (intervals[g[*t2]].first <= intervals[g[*setItr]].first 
									  && intervals[g[*t2]].second >= intervals[g[*setItr]].second)
						   && (directs[*t2].find(*setItr) == directs[*t2].end()
								&& indirects[*t2].find(*setItr) == indirects[*t2].end())){
							flag = false;
							break;
						}
					}
					if(flag){
						count2++;
						indirects[vlist[i].id].insert(*setItr);
					}
				}

				for(setItr = indirects[*t1].begin() ; setItr != indirects[*t1].end(); setItr++){
					if(used[*setItr]==i 
											|| (LEVEL_FILTER && vlist[i].top_level >= g[*setItr].top_level) 
											|| indirects[vlist[i].id].find(*setItr)!=indirects[vlist[i].id].end()
											|| directs[vlist[i].id].find(*setItr)!=directs[vlist[i].id].end()) 
							continue;
					used[*setItr]=i;
					flag = true;
//					for(t2 = out.first; t2 != out.second ; t2++){
					for(t2 = el.begin(); t2 != el.end() ; t2++){
						if(t1!=t2 && (intervals[g[*t2]].first <= intervals[g[*setItr]].first 
									  && intervals[g[*t2]].second >= intervals[g[*setItr]].second)
						   && (directs[*t2].find(*setItr) == directs[*t2].end()
								&& indirects[*t2].find(*setItr) == indirects[*t2].end())){
							flag = false;
							break;
						}
					}
					if(flag){
						count2++;
						indirects[vlist[i].id].insert(*setItr);
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
	
public :
		
	ExceptionListIncrementalBase(){
	}
	
	ExceptionListIncrementalBase(Graph& org){

	}
	
/*	
		time_tracker tt;  		
		tt.start();
		getIntervals(org,gi,0);
		
		obtainInitialExceptions(org,gi,lists);
  		tt.stop();
		cout << "Initial Step completed in " << tt.print() << endl;
		times.push_back(tt.print());	
		for(i=1;i<gi.K; i++){
			time_tracker tt3;  		
			tt3.start();
			cout << " Step " << i << " started" << endl;
			getIntervals(org,gi,i);
			obtainIncrementalExceptions(org,gi,i);
  			tt3.stop();
			cout << " Step " << i << " completed in "  << tt3.print() << " seconds " << endl;
			times.push_back(tt3.print());	
		}
		//printExceptions();
		printStats(gi);		
	}
*/	
private:
		
		void obtainIncrementalExceptions(Graph& g, int cur){
			int i,j,k,count=0,count2=0;
			
//			std::pair<AdjItr, AdjItr> out;
//			AdjItr t1,t2;
			EdgeList::iterator t1,t2;
			EdgeList el;
			Vertex child;
			vector<int> toBeRemoved;
			hash_set<int>::iterator setItr;
			bool flag;
			for(i=0;i<vlist.size();i++){
				toBeRemoved.clear();
//				out = adjacent_vertices(vlist[i],g);
//				out = adjacent_vertices(vlist[i],g);
				el = g.out_edges(vlist[i].id);
				for(setItr = lists[vlist[i].id].begin() ; setItr != lists[vlist[i].id].end(); setItr++){
					if(intervals[vlist[i]].first > intervals[g[*setItr]].first 
					   || intervals[vlist[i]].second < intervals[g[*setItr]].second){
						toBeRemoved.push_back(*setItr);
					}
					else{
//						for(t1 = out.first; t1 != out.second ; t1++){
						for(t1 = el.begin(); t1 != el.end() ; t1++){
							if(g.incrementalContains(*t1,*setItr,cur)
							   && lists[*t1].find(*setItr) == lists[*t1].end()){
								toBeRemoved.push_back(*setItr);
								break;
							}
						}
					}
				}
				for(j=0;j<toBeRemoved.size();j++){
					lists[vlist[i].id].erase(lists[vlist[i].id].find(toBeRemoved[j]));
					count2++;
				}
			}
			cout << "count 0: " << count << endl;
			count =0;
			for(i=0;i<vlist.size();i++)
				count += lists[i].size();
			counts.push_back(count);
			cout << "count 1: " << count << endl;
			cout << "count 2:  " << count2 << endl;
			
		}
	
};
map<Vertex, pair<int, int>, VertexCompare > ExceptionListIncrementalBase::intervals;

#endif
