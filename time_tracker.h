/* Copyright (c) Hilmi Yildirim 2010,2011.

The software is provided on an as is basis for research purposes.
There is no additional support offered, nor are the author(s) 
or their institutions liable under any circumstances.
*/
/*
 *  Copyright (C) 2005 M.J. Zaki <zaki@cs.rpi.edu> Rensselaer Polytechnic Institute
 *  Written by parimi@cs.rpi.edu
 *  Updated by chaojv@cs.rpi.edu, alhasan@cs.rpi.edu, salems@cs.rpi.edu
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 */
#ifndef _TT_H
#define _TT_H

#include <sys/time.h>
#include <unistd.h>

#define microsec 1000000.0

/**
 * \brief An auxilary class to keep track of the running time statistics.
 */

class time_tracker {
 private:
  struct timeval start_time;
  struct timeval stop_time;
  bool  running;
  double curr_time;
 public:
  time_tracker() {
    running=false;
    curr_time=0;
  }

  /** Start the timer */
  void start() {
    gettimeofday(&start_time, (struct timezone *)0);
    running=true;
  }

  /** Stop the timer and return the total time taken */
  void stop() {
    double st, en;
    if (!running) return;
    else {
      gettimeofday(&stop_time, (struct timezone *)0);
      st = start_time.tv_sec + (start_time.tv_usec/microsec);
      en = stop_time.tv_sec + (stop_time.tv_usec/microsec);
      running=false;
      curr_time+=en-st;
    }
  }

  double print() const
  { return curr_time; }
};

#endif
