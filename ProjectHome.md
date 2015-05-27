This software is a C++ implementation of the methods proposed in the papers:

  1. Hilmi Yildirim, Vineet Chaoji and Mohammad J. Zaki : "GRAIL: Scalable Reachability Index for Large Graphs", Proceedings of the VLDB Endowment, 3(1):276-284. 2010.
  1. Hilmi Yildirim, Vineet Chaoji and Mohammad J. Zaki : ["GRAIL: A Scalable Index for Reachability Queries in Very Large Graphs"](http://www.springerlink.com/content/r7400086p658h68q/), VLDB Journal.

Methods proposed in the second paper improves the first paper. ([see changes](NewPaper.md)) However both methods are available in the package. Please read [README](http://code.google.com/p/grail/source/browse/trunk/README) for the proper usage.

Please cite the papers if you use the software for academic purposes. You can download the whole package from [Downloads](http://code.google.com/p/grail/downloads/list) or svn checkout from [Source](http://code.google.com/p/grail/source/checkout) tab.

## Disclaimer: ##
The software is provided on an **as is** basis for **research purposes**. There is no additional support offered, nor are the author(s) or their institutions liable under any circumstances.

## Acknowledgements: ##

  * This project was supported in part by NSF Grants EMT-0829835, and CNS-0103708, and NIH Grant 1R01EB0080161-01A1.

The project contains some code from the following software:
  * PathTree: http://www.cs.kent.edu/~nruan/data/3HOP.zip
    1. Graph representation
    1. Some Graph utility functions such as tarjan algorithm
    1. The main PathTree algorithm (its reachability index) is not included in this package.
  * Interval tree implementation from http://www.cs.montana.edu/~bohannan/intervaltree/index.htm
    1. Interval tree structure is used in exception list construction of GRAIL
