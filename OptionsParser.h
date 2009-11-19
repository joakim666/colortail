/*
    colortail -- output last part of file(s) in color.
    Copyright (C) 2009  Joakim Andersson <ja@joakimandersson.se>

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

#ifndef _OptionsParser_h_
#define _OptionsParser_h_

#include <getopt.h>

#define MAX_FILES 100


static struct option long_options[] =
{
   {
      "follow",   0, 0, 'f'
   }
   ,
   {
      "config",   1, 0, 'k'
   }
   ,
   {
      "nocolors", 0, 0, 'l'
   }
   ,
   {
      "lines",    1, 0, 'n'
   }
   ,
   {
      "version",  0, 0, 'v'
   }
   ,
   {
      "quiet",    0, 0, 'q'
   }
   ,
   {
      "silent",   0, 0, 'q'
   }
   ,
   {
      "help",     0, 0, 'h'
   }
   ,
   {
      0, 0, 0, 0
   }
};


class Options
{
  public:
   // attributes
   int follow;
   int color;
   int rows;
   int verbose;
   char *cfg_filenames[MAX_FILES];
   int nr_cfg_files;
   int global_cfg_file;

   // methods
   Options();
   ~Options();
};

class OptionsParser
{
  private:
   int check_digits(char *str);
   char m_error[512];

  public:
   OptionsParser();
   ~OptionsParser();

   Options* parse(int argc, char **argv);

   char* get_error_msg();
};

#endif
