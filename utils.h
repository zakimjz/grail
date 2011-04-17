/* Copyright (c) Hilmi Yildirim 2010,2011.

The software is provided on an as is basis for research purposes.
There is no additional support offered, nor are the author(s) 
or their institutions liable under any circumstances.
*/
/**
 *
 *
 */

#ifndef _UTILS_H_
#define _UTILS_H_

#include<iostream>
#include<iomanip>
#include<cmath>

using namespace std;

/**
 * Functor for reading delimited words from input line.
 */
struct parse_word 
{

  /**
   * Reads in a delimited set of characsters from line into word, 
   * and returns pointer to next char that needs to be read
   */
  char* operator() (char* line, char* word, char delim=' ') const {

    while(*line && *line!=delim) {
      *word=*line;
      word++;
      line++;
    }
    *word='\0';

    if(*line==delim) {
      line++;
    }

    return line;
  }
};

/**
 * Print mem used by the program.
 * This will only work on Linux machine.
 */
float print_mem_usage() {

    unsigned size; //       total program size
  char buf[30];
  snprintf(buf, 30, "/proc/%u/statm", (unsigned)getpid());
  FILE* pf = fopen(buf, "r");
  if (pf) {
    unsigned resident;//   resident set size
    unsigned share;//      shared pages
    unsigned text;//       text (code)
    unsigned lib;//        library
    unsigned data;//       data/stack
    unsigned dt;//         dirty pages (unused in Linux 2.6)
    // fscanf(pf, "%u" %u %u %u %u %u", &size, &resident, &share, &text, &lib, &data);
    fscanf(pf, "%u", &size);
    cout << std::setprecision(4) << size/1024.0 << " MB mem used" << endl;
  }
	return size/1024.0;	
  fclose(pf);
}

#endif
