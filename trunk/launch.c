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

#include <stdio.h>
#include <sched.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/signal.h>

#include "launch.h"


int main(int argc, char* argv[]){

	int i, number_apps=0,app_index=0, app_number=0;
	char *return_gr;
	int just=0;
	pid_t pid;
        int *pids;
        char temp[20];
        char out[100];
	pid_t wpid;
	pid_t parent_pid;
	pid_t my_pid;
	int child_status;
	int kill_slow_children = 0;

	typedef struct{
		int core;
		char prog[300];
	}app;
	app *app_data;

	// Too few apps, print usage and terminate
	if(argc < 2){
		print_usage();
	}
	
	// Help requested, display help and terminate.
	if(strcmp(argv[1],"-h") == 0){
		help();
	}

	// This is a killing wrapper, set the kill_slow_children flag
	if(strcmp(argv[1],"-k") == 0){
		kill_slow_children = 1;
	}

	// There must be atleast 4 arguments! (wrapper core x app)
	if(argc < 4){
		print_usage();
	}
        
	//calculate the number of applications to launch
	for(i=1 + kill_slow_children;i<argc;i++){
		if(strcmp(argv[i],CORE) == 0){
			number_apps++;
		}
	}

	//create an array to hold the app data
	app_data = (app *)calloc(number_apps, sizeof(app));

       
  	//for each argument other than the first one
	app_index = -1;
	for(i=1 + kill_slow_children,app_index=-1;i<argc; i++){
		//if the string matches to core, set just to 1
		//and increment app_index
		//app_index is the application number
		if(strcmp(argv[i],CORE) == 0){
			just = 1;
			app_index++;
		}
		//if just == 1, then take the cpumask
		else if(just == 1){
			just++;
			//shift 1 left cpu id number of times to form the mask
			app_data[app_index].core = 1 << atoi(argv[i]);
		}
		//else collect the command line options for the 
		//app to run
		else if (just >= 2){
			just++;
			//core not provided first, print usage and exit
			if(app_index == -1){
				print_usage();
			}
			//copy the data structure into the array
			return_gr = strcat(app_data[app_index].prog, argv[i]);
			return_gr = strcat(app_data[app_index].prog, " ");	
		}
		else{
			print_usage();
		}
	}
	if(just <= 2){
		print_usage();
	}
        pids = (int *)calloc(number_apps,sizeof(int));

	//app number set to -1, the first child
	app_number = -1;

	// Get the parent's PID. This will be the children's GID.
	parent_pid = getpid();
	setpgid(parent_pid,parent_pid-1);

	// For each app (app-1 as there is already 1 child because of the main
	for(i=0;i<number_apps;i++){
		//if parent
		//increment app_number for the ith child
		app_number++;
		//fork to create the ith child
		pid = fork();
			// parent logs the childs pid.
		if(pid != 0){
			pids[app_number] = pid;
		}
		// child sets its GID to be the the parent's PID
		else{
			my_pid = getpid();
			setpgid(my_pid,my_pid);
			break;
		}
	}	
	
	//for every child....
	if(pid == 0){
		//the child has the latest copy of app_number, its id.
		#ifdef DEBUG
			printf("I am child number %d\n",app_number);
		#endif
		//get the mask from array and set affinity
		SET_AFFINITY(app_data[app_number].core);
		//execute the application
		system(app_data[app_number].prog);
		#ifdef DEBUG
		printf("running: %s on core %d\n",app_data[app_number].prog,app_data[app_number].core);
		#endif
		exit(0);
	}
	//else if parent,
	else{
		wpid = wait(&child_status);
		if(WIFEXITED(child_status)){
			printf("process with pid = %d just finished successfully with status = %d\n",(int)wpid, WEXITSTATUS(child_status));
		}
		// If the -k flag is provided, then kill all the children as soon as the first process exits!
		if(kill_slow_children){
			for(i=0;i<number_apps;i++){
				if(pids[i] != wpid){
					printf("Killing all process with gid = %d\n",pids[i]);
					killpg(pids[i],SIGTERM);
				}
			}
		}
		// normal execution, wait for all other the processes to finish (dying if they were killed)
		
		for(i=0;i<number_apps-1;i++){
			wpid = wait(&child_status);
			if(WIFEXITED(child_status)){
				printf("process with pid = %d just finished successfully with status = %d\n",(int)wpid, WEXITSTATUS(child_status));
			}
		}
		
	}

	// All children are dead (Either natural or murder) Gracefully exit
		
	return 0;
}


void print_usage(void){
	printf("Usage: wrapper [-k] %s <cpu_id_1> <application_1> [%s <cpu_id_2> <application_2> ....]\n", CORE, CORE);
	printf("try wrapper -h for help\n");
	exit(1);
}

void help(void){
	printf("Usage: wrapper [-k] %s <cpu_id_1> <application_1> [%s <cpu_id_2> <application_2> ....]\n\n", CORE, CORE);
	printf("-k \t\t\t\t[Optional] Kill all apps as soon as any one of them exits, wrapper waits for all by default\n");
	printf("%s \t\t\t\tKeyword seperating the applications\n",CORE);
	printf("cpu_id_x ->\t\t\tThe cpu on which you want to execute application_x\n");
	printf("application_x ->\t\tThe applicaton you want to execute\n");
	printf("[ -- ] means optional\n\n");
	printf("Note that wrapper cannot handle I/O Redirects (<, >, &>) cleanly\n");
	printf("enclose the application with the I/O Redirects in single quotes\n\n");
	exit(1);
}

