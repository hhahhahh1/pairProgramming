Overview
=========

Ned is a simple, graphical, multiplatform editor written in C++ using the
Qt4 graphical framework.

It can run on many platforms including:

 * Windows
 * GNU/Linux
 * FreeBSD

These binaries can gathered from 
[Sourceforge](www.sourceforge.net/projects/ned "Sourceforge")

Installation
=============

Just download the binaries from the Sourceforge and install in your platform's specific way.

Compilation
============

Download the sourcecode tarball from
[Github](http://github.com/akoskovacs/Ned/downloads "Github downloads") or
[Gitorious](http://gitorious.org/ned/ned/trees/unstable "Gitorious, Ned unstable repo")

The third way to clone the whole git repository: (If you have preinstalled git)
http://git.gitorious.org/ned/ned.git

*To compile it, you need the Qt libraries and the CMake tool!*

## Instructions ##

0. If you download the tarball, first unzip it

1. Go to the source directory

2. $ cmake Ned.pro

3. $ make

4. If everything works only start the
  executable with the 
       $ ./Ned
  command
