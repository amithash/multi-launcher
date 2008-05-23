/**************************************************************************
 * Copyright 2008 Amithash Prasad                                         *
 *                                                                        *
 * This file is part of multi-launcher                                    *
 *                                                                        *
 * multi-launcher is free software: you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by   *
 * the Free Software Foundation, either version 3 of the License, or      *
 * (at your option) any later version.                                    *
 *                                                                        *
 * This program is distributed in the hope that it will be useful,        *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 * GNU General Public License for more details.                           *
 *                                                                        *
 * You should have received a copy of the GNU General Public License      *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 **************************************************************************/

#ifndef _WRAPPER_H_
#define _WRAPPER_H_ 1


//change the keyword to something else if you want to
#ifndef KEYWORD
	#define KEYWORD "core"
#endif
//#define DEBUG 1

//this is the macro to call sched_setaffinity()
#define SET_AFFINITY(x) do { \
cpu_set_t cpuset; \
unsigned long *aff = (unsigned long *)&cpuset; \
memset(&cpuset, 0, sizeof(cpuset)); \
*aff = x; \
assert(sched_setaffinity(0, sizeof(const cpu_set_t), \
                          (const cpu_set_t *)&cpuset) == 0); \
} while (0)


const char CORE[20] = KEYWORD; 


void print_usage(void);
void help(void);

#endif
