
---

> ## MULTI-LAUNCHER ##
> Allows the user to launch multiple jobs on multiple processors
> > Copyright 2008 Amithash Prasad


> multi-launcher is free software: you can redistribute it and/or modify
> it under the terms of the GNU General Public License as published by
> the Free Software Foundation, either version 3 of the License, or
> (at your option) any later version.

> This program is distributed in the hope that it will be useful,
> but WITHOUT ANY WARRANTY; without even the implied warranty of
> MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
> GNU General Public License for more details.

> You should have received a copy of the GNU General Public License
> along with this program.  If not, see <http://www.gnu.org/licenses/>.

---


# DESCRIPTION #

---


LAUNCHER functions to force apps to run on particular cpu's

# COMPILATION #

---


make to compile both the single thread and the 2 thread modules.
'make'
'make clean' to clean build files.

if you do not want the default keyword "core", you can use the optional make command

make opt OPTI='your\_keyword'

example:
make opt OPTI='proc'

'make install'
> copies the binary to the /usr/bin folder and hence you do not need to set the
> path to find wrapper and you can directly use wrapper

'make uninstall'
> removes the copy in /usr/bin

NOTE: You need root permission to do a 'make install; or a 'make uninstall'
> So either do this as a root, or use 'sudo make install' or 'sudo make uninstall'

# SETUP #

---


create an enviornment variable called WRAPPER\_HOME
pointing to the path where the binary files exist,
and also include this in the PATH enviornment variable.

# USAGE #

---


launch [-k] core 

<cpu\_id\_1>

 

<application\_1>

 [<cpu\_id\_2> <application\_2> ......](core.md)

core is a keyword, if your application which either:
> a. is named "core"
> b. takes in an argument named "core"
then you can expect unexpected execution!
if this is your case, try compiling with make opt OPTI='your\_keyword'
where your\_keyword is the the keyword of your choice



<cpu\_id\_x>

 is the processor on which you want to execute applicaiton\_x (0,1,2,....)



<application\_x>

 - is the application along with its input parameters you want to execute on cpu\_id\_x

-k      If this option is provided, wrapper kills all executing apps as soon as one of them completes


example:
launch core 0 echo prasad core 1 echo mac core 2 echo colorado
launch -k core 0 'some\_infinitly\_running\_app' core 1 'sleep 10'
(These is nice feature if you have some app which runs infinitely! -> example are buggy software,
> and synthetic benchmarks!)

there is NO test if your cpu\_id is valid or not.

# ISSUES & WORK AROUNDS #

---


the launch cannot cleanly handle io redirects. If such exists, then
put the entire thing in single quotes.

Example:

launch core 0 '/bin/echo blah > blah.out 2> blah.err' core 1 '/bin/echo blahblah > 2blah.out 2> 2blah.err'

same thing for input redirects, but i did not show that in the example,
as i could not think of an example which expects input. And my brain is
fried enough.

# BEWARE #

---


This is for singled threaded applicaitons only! I do not know why would anyone want to force
all the threads in their multi threaded application to run on a single core.

If you thought that this app will automatically detect multi-threaded applicaitons and then
intelligently force each thread to a particular core, well you are wrong! (It would be nice though!)
If this is what you want, then check out the macro function. This is what you want to use in your
multithreaded code to pin the thread to a particular core

If you do not have to code for the multi threaded app you have, then too bad. Opensource rules!!!

# AUTHORS #

---


Amithash Prasad, University of colorado, Boulder,
ECE Department.
{amithash.prasad}