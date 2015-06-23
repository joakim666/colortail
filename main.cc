/*
    colortail -- output last part of file(s) in color.
    Copyright (C) 2009  Joakim Ek <joakim@pjek.se>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <cstdlib>
#include <stdio.h>
#include <signal.h>

#include "main.h"
#include "ColorTail.h"

// the main function
int main(int argc, char **argv)
{
   colortail = NULL;
   
   // make a ColorTail instance
   colortail = new ColorTail();


   // setup signal catchers
   signal (SIGINT,  clean_up);
   signal (SIGHUP,  clean_up);
   signal (SIGKILL, clean_up);

   // start the program
   int ret = colortail->start(argc, argv);

   delete colortail;

   return ret;
}


// callback function for signals
void clean_up(int i)
{
   cout << ANSI_RESET_STR << endl << ends;
   if (colortail)
      delete colortail;
   exit(1);
}


