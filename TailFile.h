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

#ifndef _TailFile_h_
#define _TailFile_h_

#include "Colorizer.h"

#include <stdio.h>
#include <sstream>
#include <sys/stat.h>

#define MAX_CHARS_READ 1024

class TailFile
{
  private:
   // attributes
   // the tail file's filename
   char *m_filename;
   // the file
   FILE *m_file;
   // the colorizer
   Colorizer *m_colorizer;
   // the stream position
   long m_position;
   // the follow buffer, used in follow_print
   std::ostringstream *m_follow_buffer;
   // to detect a changed inode for reopening
   struct stat m_file_stats;
   int reopening;

   // private methods
   void find_position(int n);
   long end_of_file_position();
   void print_to_stdout(const char *str);
   
   // methods   
  public:
   TailFile();
   ~TailFile();

   // opens the file
   int open(char *filename, Colorizer *colorizer);

   // reopen the file when the inode has changed (log rotation)
   int reopen();
   
   // prints last n rows
   void print(int n);

   // prints a line if there is a '\n' in the n bytes that can be read
   void follow_print(int n, int verbose, char *last_filename);
   
   // returns if more to read
   int more_to_read();

   // prints the filename
   void printFilename();

   // returns the filename
   char* get_filename();
   
   // prints the complete file, for debugging
   void printAll();

};


#endif
