/* Copyright (c) Hilmi Yildirim 2010,2011.

The software is provided on an as is basis for research purposes.
There is no additional support offered, nor are the author(s) 
or their institutions liable under any circumstances.
*/
#ifndef INTERVALTREE_H_
#define INTERVALTREE_H_

#include"templatestack.h"
#include<math.h>
#include<limits.h>

//  The interval_tree.h and interval_tree.cc files contain code for 
//  interval trees implemented using red-black-trees as described in 
//  the book _Introduction_To_Algorithms_ by Cormen, Leisserson, 
//  and Rivest.  

#ifndef MAX_INT
#define MAX_INT INT_MAX // some architechturs define INT_MAX not MAX_INT
#endif

// The Interval class is an Abstract Base Class.  This means that no
// instance of the Interval class can exist.  Only classes which
// inherit from the Interval class can exist.  Furthermore any class
// which inherits from the Interval class must define the member
// functions GetLowPoint and GetHighPoint.
//
// The GetLowPoint should return the lowest point of the interval and
// the GetHighPoint should return the highest point of the interval.  

class Interval {
public:
  Interval();
  virtual ~Interval();
  virtual int GetLowPoint() const = 0;
  virtual int GetHighPoint() const = 0;
  virtual void Print() const;
};

class IntervalTreeNode {
  friend class IntervalTree;
public:
  void Print(IntervalTreeNode*,
	     IntervalTreeNode*) const;
  IntervalTreeNode();
  IntervalTreeNode(Interval *);
  ~IntervalTreeNode();
protected:
  Interval * storedInterval;
  int key;
  int high;
  int maxHigh;
  int red; /* if red=0 then the node is black */
  IntervalTreeNode * left;
  IntervalTreeNode * right;
  IntervalTreeNode * parent;
};

struct it_recursion_node {
public:
  /*  this structure stores the information needed when we take the */
  /*  right branch in searching for intervals but possibly come back */
  /*  and check the left branch as well. */

  IntervalTreeNode * start_node;
  unsigned int parentIndex;
  int tryRightBranch;
} ;


class IntervalTree {
public:
  IntervalTree();
  ~IntervalTree();
  void Print() const;
  Interval * DeleteNode(IntervalTreeNode *);
  IntervalTreeNode * Insert(Interval *);
  IntervalTreeNode * GetPredecessorOf(IntervalTreeNode *) const;
  IntervalTreeNode * GetSuccessorOf(IntervalTreeNode *) const;
  TemplateStack<void *> * Enumerate(int low, int high) ;
  void CheckAssumptions() const;
protected:
  /*  A sentinel is used for root and for nil.  These sentinels are */
  /*  created when ITTreeCreate is caled.  root->left should always */
  /*  point to the node which is the root of the tree.  nil points to a */
  /*  node which should always be black but has aribtrary children and */
  /*  parent and no key or info.  The point of using these sentinels is so */
  /*  that the root and nil nodes do not require special cases in the code */
  IntervalTreeNode * root;
  IntervalTreeNode * nil;
  void LeftRotate(IntervalTreeNode *);
  void RightRotate(IntervalTreeNode *);
  void TreeInsertHelp(IntervalTreeNode *);
  void TreePrintHelper(IntervalTreeNode *) const;
  void FixUpMaxHigh(IntervalTreeNode *);
  void DeleteFixUp(IntervalTreeNode *);
  void CheckMaxHighFields(IntervalTreeNode *) const;
  int CheckMaxHighFieldsHelper(IntervalTreeNode * y, 
			const int currentHigh,
			int match) const;
private:
  unsigned int recursionNodeStackSize;
  it_recursion_node * recursionNodeStack;
  unsigned int currentParent;
  unsigned int recursionNodeStackTop;
};


#endif



