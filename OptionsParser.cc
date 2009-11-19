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

#include <iostream>
#include <sstream>
#include <cstring>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "OptionsParser.h"
#include "Info.h"
#include "Usage.h"

using namespace std;
// methods for class Options

Options::Options()
{
   // sets default values for attributes

   // don't follow by default
   follow = 0;
   // color output by default
   color = 1;
   // 10 rows by default
   rows = 10;
   // verbose by default
   verbose = 1;
   // no cfg files by default
   nr_cfg_files = 0;
   for (int i = 0 ; i < MAX_FILES ; i++)
   {
      cfg_filenames[i] = NULL;
   }
   // individual cfg files as default
   global_cfg_file = 0;
   
}

Options::~Options()
{
   // destructor, frees memory allocated for filenames

   for (int i = 0 ; i < MAX_FILES ; i++)
   {
      if (cfg_filenames[i])
      {
	 delete[] cfg_filenames[i];
      }
   }
}

// methods for class OptionsParser
OptionsParser::OptionsParser()
{
   // constructor

   // set default error msg
   sprintf(m_error, "No error");
}

OptionsParser::~OptionsParser()
{
   // destructor
}

Options* OptionsParser::parse(int argc, char **argv)
{
   // parses options and sets the attributes in the Options class
   // which is then returned. Returns NULL on error and m_error is set

   // make Options instance
   Options *o = new Options();

   // prevent error messages from getopt_long
   opterr = 0;
   
   int c;
   Info info;
   Usage usage;
   
   while (1)
   {
      c = getopt_long(argc, argv, "qvn:fk:l", long_options, NULL);
      if (c == -1)
      {
	 break;
      }

      switch (c)
      {
	 case 'v':
	    // version
	    info.print();
	    break;

	 case 'h':
	    // help
	    usage.print();
	    break;

	 case 'f':
	    // follow
	    o->follow = 1;
	    break;

	 case 'k':
	    // color config file
	    
	 {

	    int pos = 0; // position in optarg
	    int loop = 1;
	    while (loop)
	    {
	       ostringstream filename;

	       while (1)
	       {
		  if (optarg[pos] == ',')
		  {
		     // found seperator
		     // set filename in options class
			string str = filename.str();
			 char* cs = new char[str.length()+1];
			std::strcpy(cs, str.c_str());
			
			o->cfg_filenames[o->nr_cfg_files] = cs;

		     // increase the nr_cfg_files counter
		     (o->nr_cfg_files)++;

		     // increase the position
		     pos++;
		     
		     // break the inner loop
		     break;
		  }

		  if (optarg[pos] == '\0')
		  {
		     // found end of string
		     // set filename in options class
			string str = filename.str();
			 char* cs = new char[str.length()+1];
  			 std::strcpy(cs, str.c_str());

		     o->cfg_filenames[o->nr_cfg_files] = cs;

		     // increase the nr_cfg_files counter
		     (o->nr_cfg_files)++;

		     // check if only one cfg file
		     if (o->nr_cfg_files == 1)
		     {
			// only one cfg file
			o->global_cfg_file = 1;
		     }

		     // stop the big loop
		     loop = 0;

		     // break the inner loop
		     break;
		  }

		  // insert the current character into the filename string
		  filename.put(optarg[pos]);

		  // increase the position
		  pos++;
	       }
	    }
	 }
	 break;

	 case 'l':
	    // no colors
	    o->color = 0;
	    break;
	    
	 case 'n':
	    // numberg of lines
	    
	    // check if only digits
	    if (check_digits(optarg) == 0)
	    {
	       o->rows = atoi(optarg);
	    }
	    else
	    {
	       sprintf(m_error,
		       "One or more non-digits were given as number of rows");
	       // free memory for options class
	       delete o;
	       // return NULL to show that an error occured
	       return NULL;
	    }
	    break;
	    
	 case 'q':
	    // quiet mode
	    o->verbose = 0;
	    break;

	 case '?':
	    // unrecognized option
	    
	    // TODO: fix to handle -45 that means -n 45

	    cout << "colortail: invalid option -- " << (char)c << endl;
	    cout << "Try 'colortail --help' for more information." << endl;
	    
	    break;
	    
	 default:

	    cout << "Unrecognized option " << (char)c << endl;

	    // print usage
	    usage.print();
	    break;
      }
   }
   
   return o;
}


int OptionsParser::check_digits(char *str)
{
   // checks if str only contains digits

   int i = 0, len = strlen(str);

   for (i = 0 ; i < len ; i++)
   {
      if (str[i] < '0' || str[i] > '9')
      {
	 // not a digit
	 return 1;
      }
   }
   return 0;
}

char* OptionsParser::get_error_msg()
{
   // returns the current set error msg

   return m_error;
}






