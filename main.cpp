/* Copyright (c) Hilmi Yildirim 2010,2011.

The software is provided on an as is basis for research purposes.
There is no additional support offered, nor are the author(s) 
or their institutions liable under any circumstances.
*/
#include "Graph.h"
#include "GraphUtil.h"
#include "Grail.h"
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <cstring>
#include "exception_list.h"
#include "exception_list_incremental_base.h"
#include "exception_list_incremental_plus.h"

#include "utils.h"


bool SKIPSCC = false;
bool BIDIRECTIONAL = false;
int LABELINGTYPE = 0;
bool UseExceptions = false;
bool UsePositiveCut = false;
bool POOL = false;
int POOLSIZE = 100;
	int DIM = 2;
	int query_num = 100000;
	char* filename = NULL;
	char* testfilename = NULL;
	bool debug = false;
	bool LEVEL_FILTER = false;
	bool LEVEL_FILTER_I = false;

	float labeling_time, query_time, query_timepart,exceptionlist_time;
int alg_type = 1;

void handle(int sig) {
 	char *alg_name;

	switch(alg_type){
		case 1: alg_name= "GRAIL";  break;
		case 2: alg_name= "GRAILLF";  break;
		case 3: alg_name= "GRAILBI";  break;
		case 6: alg_name= "GRAILBILF";  break;
		case -1: alg_name= "GRAIL*";  break;
		case -2: alg_name= "GRAIL*LF";  break;
		case -3: alg_name= "GRAIL*BI";  break;
		case -6: alg_name= "GRAIL*BILF";  break;
	}

	cout << "COMPAR: " << alg_name << DIM << "\t" << labeling_time << "\t" << "TOut" << "\t" <<  endl;

	exit(1);
}

static void usage() {
	cout << "\nUsage:\n"
		" grail [-h] <filename> -test <testfilename> [-dim <DIM>] [-skip] [-ex] [-ltype <labelingtype>] [-t <alg_type>]\n"
		"Description:\n"
		"	-h			Print the help message.\n"
		"  <filename> is the name of the input graph in gra format.\n"
		"	-test		Set the queryfile which contains a line of <source> <target> <reachability> for each query. \n"
		"	-dim 		Set the number of traversals to be performed. The default value is 5.\n"
		"	-ex 		Use exception lists method instead of pruning. Default is not using exception lists.\n"
		"	-skip		Skip the phase converting the graph into a dag. Use only when the input is already a DAG.\n"
		"	-t <alg_type>		alg_type can take 8 different values.  Default value is 1.\n"
		" \t\t\t 1  : Basic Search (used in the original VLDB'10 paper) \n"
		" \t\t\t 2  : Basic Search + Level Filter \n"
		" \t\t\t 3  : Bidirectional Search \n"
		" \t\t\t 6  : Bidirectional Search + Level Filter \n"
		" \t\t\t -1 : Positive Cut + Basic Search\n"
		" \t\t\t -2 : Positive Cut + Basic Search + Level Filter (usually provides the best query times) \n"
		" \t\t\t -3 : Positive Cut + Bidirectional Search \n"
		" \t\t\t -6 : Positive Cut + Bidirectional Search + Level Filter\n"
		"	-ltype <labeling_type>		labeling_type can take 6 different values.  Default value is 0.\n"
		" \t\t\t 0  : Completely randomized traversals.  \n"
		" \t\t\t 1  : Randomized Reverse Pairs Traversals (usually provides the best quality) \n"
		" \t\t\t 2  : Heuristicly Guided Traversal: Maximum Volume First \n"
		" \t\t\t 3  : Heuristicly Guided Traversal: Maximum of Minimum Interval First \n"
		" \t\t\t 4  : Heuristicly Guided Traversal: Maximum Adjusted Volume First \n"
		" \t\t\t 5  : Heuristicly Guided Traversal: Maximum of Adjusted Minimum Volume First \n"
		<< endl;
}


static void parse_args(int argc, char *argv[]){
	if (argc == 1) {
		usage();
		exit(0);
	}

 	int i = 1;

	while (i < argc) {
		if (strcmp("-h", argv[i]) == 0) {
			usage();
			exit(0);
		}
		if (strcmp("-d", argv[i]) == 0) {
			i++;
			debug = true;
		}		
		if (strcmp("-n", argv[i]) == 0) {
			i++;
			query_num = atoi(argv[i++]);
		}
		else if (strcmp("-dim", argv[i]) == 0) {
			i++;
			DIM = atoi(argv[i++]);
		}
		else if (strcmp("-lfi", argv[i]) == 0) {
			LEVEL_FILTER_I = true;
			i++;
		}
		else if (strcmp("-lf", argv[i]) == 0) {
			LEVEL_FILTER = true;
			alg_type *=2;
			i++;
		}else if (strcmp("-test", argv[i]) == 0) {
			i++;
			testfilename = argv[i++];
		}else if(strcmp("-skip", argv[i])== 0) {
			i++;
			SKIPSCC = true;
		}else if(strcmp("-ltype", argv[i])== 0) {
			i++;
			LABELINGTYPE = atoi(argv[i++]);
		}else if(strcmp("-t", argv[i])== 0) {
			i++;
			alg_type = atoi(argv[i++]);
		}else if(strcmp("-ex", argv[i])== 0) {
			i++;
			UseExceptions = true;
		}else if(strcmp("-pp", argv[i])== 0) {
			i++;
			UsePositiveCut = true;
			alg_type *=-1;
		}else if(strcmp("-bi", argv[i])== 0) {
			i++;
			BIDIRECTIONAL = true;
			alg_type *=3;
		}else if(strcmp("-pool", argv[i])== 0) {
			i++;
			POOL = true;
			POOLSIZE = atoi(argv[i++]);
		}
		else {
			filename = argv[i++];
		}
	}
	if(!testfilename){
		cout << "Please provide a test file : -test <testfilename> " << endl;
		exit(0);
	}
}




int main(int argc, char* argv[]) {
	signal(SIGALRM, handle);	
	parse_args(argc,argv);
		
	/*
		Read Graph from the input file	
	*/
	ifstream infile(filename);
	if (!infile) {
		cout << "Error: Cannot open " << filename << endl;
		return -1;
	}
	
	Graph g(infile);
	cout << "#vertex size:" << g.num_vertices() << "\t#edges size:" << g.num_edges() << endl;
	
	int s, t;
	int left = 0;
	int gsize = g.num_vertices();
	
	bool r;
	struct timeval after_time, before_time, after_timepart, before_timepart;


	int *sccmap;
	if(!SKIPSCC){
		sccmap = new int[gsize];					// store pair of orignal vertex and corresponding vertex in merged graph
		vector<int> reverse_topo_sort;	
	
		// merge strongly connected component
		cout << "merging strongly connected component..." << endl;
		gettimeofday(&before_time, NULL);
		GraphUtil::mergeSCC(g, sccmap, reverse_topo_sort);	
		gettimeofday(&after_time, NULL);
		query_time = (after_time.tv_sec - before_time.tv_sec)*1000.0 + 
			(after_time.tv_usec - before_time.tv_usec)*1.0/1000.0;
		cout << "merging time:" << query_time << " (ms)" << endl;
		cout << "#DAG vertex size:" << g.num_vertices() << "\t#DAG edges size:" << g.num_edges() << endl;

//		g.printGraph();
//		ofstream outSCC("scc.out");
//		g.writeGraph(outSCC);
//		outSCC.close();	
	}

	GraphUtil::topo_leveler(g);

	// prepare queries
	srand48(time(NULL));
	cout << "preparing queries..." << endl;
	vector<int> src;
	vector<int> trg;
	vector<int> labels;
	vector<int>::iterator sit, tit, lit;
	int success=0,fail=0;
	int label;
	if(testfilename==NULL){
		while (left < query_num) {
			s = lrand48() % gsize; 
			t = lrand48() % gsize; 
			if (s == t) continue;
			src.push_back(s);
			trg.push_back(t);
			++left;
		}
	}else{
      std::ifstream fstr(testfilename);
		while(!fstr.eof()) {
         fstr >> s >> t >> label;
			src.push_back(s);
			trg.push_back(t);
			labels.push_back(label);
		}
	}

	cout << "queries are ready" << endl;


	gettimeofday(&before_time, NULL);

	int dimension ;
	Grail grail(g,DIM,LABELINGTYPE,POOL,POOLSIZE);

	grail.set_level_filter(LEVEL_FILTER);
	gettimeofday(&after_time, NULL);

	labeling_time = (after_time.tv_sec - before_time.tv_sec)*1000.0 + 
		(after_time.tv_usec - before_time.tv_usec)*1.0/1000.0;
	cout << "#construction time:" << labeling_time << " (ms)" << endl;
	

	ExceptionList * el = NULL;
	exceptionlist_time = 0;
	if(UseExceptions){
		gettimeofday(&before_time, NULL);
			// el = new ExceptionListIncrementalPlus(g,DIM,LEVEL_FILTER);	 			// filtering by levels doesnot help inside exceptionlists construction
//		cout << "UP TO HERE OK" << endl;
			el = new ExceptionListIncrementalPlus(g,DIM,0);
		gettimeofday(&after_time, NULL);
		 exceptionlist_time = (after_time.tv_sec - before_time.tv_sec)*1000.0 + 
			(after_time.tv_usec - before_time.tv_usec)*1.0/1000.0;
		cout << "exceptionlist time:" << exceptionlist_time << " (ms)" << endl;
	}

	// process queries
	cout << "process queries..." << endl;
	gettimeofday(&before_time, NULL);
	gettimeofday(&before_timepart, NULL);
	int seenpositive = 0;

	int source, target;
	int reachable = 0, nonreachable =0;
		
	for (sit = src.begin(), tit = trg.begin(), lit = labels.begin();sit != src.end(); ++sit, ++tit, ++lit) {
			if(!SKIPSCC){
				s = sccmap[*sit];
				t = sccmap[*tit];
			}else{
				s = *sit;
				t = *tit;
			}

//			if(grail.bidirectionalReach(s,t,el) != grail.reach(s,t,el)){
//					cout << "Conflict 1 " << s <<" " << t <<  endl;
//			}
//			if(grail.bidirectionalReachPP(s,t,el) != grail.reachPP(s,t,el)){
//					cout << "Conflict 2 " << s <<" " << t <<  endl;
//			}

			switch(alg_type){
				case 1: r = grail.reach(s,t,el); break;
				case 2: r = grail.reach_lf(s,t,el); break;
				case 3: r = grail.bidirectionalReach(s,t,el); break;
				case 6: r = grail.bidirectionalReach_lf(s,t,el); break;

				case -1: r = grail.reachPP(s,t,el); break;
				case -2: r = grail.reachPP_lf(s,t,el); break;
				case -3: r = grail.bidirectionalReachPP(s,t,el); break;
				case -6: r = grail.bidirectionalReachPP_lf(s,t,el); break;
			}

			if(r==true) {
				reachable++;
   	      if(*lit == 0) {
//            	cout << "False positive pair = " << s << " " << t << " " << *lit << endl;
//							cout << "Levels : " << s << "->" << g[s].top_level << " " << t << "->" << g[t].top_level << endl;
            	fail++;
         	} else {
						success++;
					}
      	}
      	else {
				nonreachable++;
         	if(*lit == 1) {
//            	cout << "False negative pair = " << s << " " << t << " " << *lit << endl;
            	fail++;
         	}
         	else   success++;
			}
		}	
		cout << "Success Rate " << success << "/" << success+fail << endl;

	gettimeofday(&after_time, NULL);
	query_time = (after_time.tv_sec - before_time.tv_sec)*1000.0 + 
		(after_time.tv_usec - before_time.tv_usec)*1.0/1000.0;
	cout << "#total query running time:" << query_time << " (ms)" << endl;

	cout.setf(ios::fixed);		
	cout.precision(2);
	cout << "GRAIL REPORT " << endl;
	cout << "filename = " << filename << "\t testfilename = " << (testfilename ? testfilename:"Random") << "\t DIM = " << DIM <<endl;  
	cout << "Labeling_time = " << labeling_time + exceptionlist_time << "\t Query_Time = " << query_time << "\t Index_Size = " << gsize*DIM*2  << "\t Mem_Usage = " << print_mem_usage() << " MB"<< endl; 
	
	char *alg_name;

	switch(alg_type){
		case 1: alg_name= "GRAIL";  break;
		case 2: alg_name= "GRAILLF"; LEVEL_FILTER=true; break;
		case 3: alg_name= "GRAILBI";  break;
		case 6: alg_name= "GRAILBILF"; LEVEL_FILTER=true;  break;
		case -1: alg_name= "GRAIL*";  break;
		case -2: alg_name= "GRAIL*LF"; LEVEL_FILTER=true;  break;
		case -3: alg_name= "GRAIL*BI";  break;
		case -6: alg_name= "GRAIL*BILF"; LEVEL_FILTER=true;  break;
	}

	if(grail.PositiveCut==0)
		grail.PositiveCut = 1;	
	
	int totalIndexSize;
	if(alg_type < 0){
		totalIndexSize=gsize*DIM*3;
	}else{
		totalIndexSize=gsize*DIM*2;
	}
	if(LEVEL_FILTER){
		totalIndexSize+=gsize;
	}
	if(UseExceptions){
		totalIndexSize+=el->Size();
	}
	
	
	cout << "COMPAR: " << alg_name << DIM << "\t" << labeling_time + exceptionlist_time << "\t" << query_time << "\t" <<  totalIndexSize << "\t" << print_mem_usage()  << "\t" << grail.TotalCall << "\t" << grail.PositiveCut << "\t" << grail.NegativeCut << "\t" << reachable << "\t AvgCut:" << (grail.TotalDepth/grail.PositiveCut) << endl;
}
