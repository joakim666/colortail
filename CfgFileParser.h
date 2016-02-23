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

#ifndef _CfgFileParser_h_
#define _CfgFileParser_h_

#include "List.h"
#include "config.h"

#include <sys/types.h>
#include <regex.h>
#include <iostream>
#include <fstream>
#include <string>

#ifdef HAVE_GNUREGEX_H
# include <gnuregex.h>
#else
# ifdef HAVE_REGEX_H
#  include <regex.h>
# else
#  error "You don't have regex.h or gnuregex.h"
# endif
#endif


class SearchData
{
  public:
   SearchData();
   ~SearchData();
   void set_color(char *color);
   
   regex_t m_preg;                // pattern storage buffer
   char *m_ansi_color_code;       // color for row if match
   void (*m_pf)(char*);           // callback fkn for action
   char *m_param_to_callback_fkn; // parameters to callback fkn
};


// ###- color defines -###
// foreground
#define BLACK         "\033[0;30m"
#define RED           "\033[0;31m"                
#define GREEN         "\033[0;32m"
#define YELLOW        "\033[0;33m"
#define BLUE          "\033[0;34m"
#define MAGENTA       "\033[0;35m"
#define CYAN          "\033[0;36m"
#define WHITE         "\033[0;37m"
#define BRIGHTBLACK   "\033[1;30m"
#define BRIGHTRED     "\033[1;31m"
#define BRIGHTGREEN   "\033[1;32m"
#define BRIGHTYELLOW  "\033[1;33m"
#define BRIGHTBLUE    "\033[1;34m"
#define BRIGHTMAGENTA "\033[1;35m"
#define BRIGHTCYAN    "\033[1;36m"
#define BRIGHTWHITE   "\033[1;37m"

// background
// TODO: fix this

#define MAX_CFG_LINE_LENGTH 200

class CfgFileParser
{
  private:
   List<SearchData*> *m_items_list;
   std::ifstream m_infile;
   char *m_filename;
   int m_line;

   void free_items();
   char* read_line();

   int read_item();
   char* read_color();
   int read_left();
   char* read_regexp();
   
   
  public:
   CfgFileParser();
   ~CfgFileParser();

   int parse(const char *filename);
   List<SearchData*>* get_items_list();
};

#endif
