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

#include <iostream>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "ColorTail.h"
#include "TailFile.h"
#include "OptionsParser.h"
#include "Colorizer.h"


using namespace std;

// the constructor
ColorTail::ColorTail()
{
   // nothing yet
}

// the destructor
ColorTail::~ColorTail()
{
   // free the memory for the TailFile objects in the list
   free_list_items();
}

void ColorTail::free_list_items()
{
   // removes all the elements from the list and frees the memory the objects
   // uses

   TailFile *tmp;

   // go through all the elements in the list,
   // and free each TailFile object
   while (!m_tailfiles.is_empty())
   {
      tmp = m_tailfiles.first_element();
      m_tailfiles.remove_first();
      delete tmp;
   }
}

// the starting method
int ColorTail::start(int argc, char **argv)
{
   // parse options
   OptionsParser optParser;
   Options *options = optParser.parse(argc, argv);

   assert (options != NULL);

   // the options has been stripped from argc and argv, just the
   // files to tail and the program namn left

   // tailfile counter
   int tailfile_counter = 0;

   // iterate through all the files to tail
   for (int i = optind ; i < argc ; i++)
   {
      // new TailFile object
      TailFile *new_tailfile = new TailFile();

      Colorizer *colorizer = NULL;

      // check if colors
      if (options->color)
      {
	 // color output
	 // check if global config file
	 if (options->global_cfg_file)
	 {
	    // global config file
	    // check if there is a config file
	    if (options->nr_cfg_files > 0)
	    {
	       // yes there is a first config file
	       colorizer = new Colorizer(options->cfg_filenames[0]);

	       // open the tailfile
	       new_tailfile->open(argv[i], colorizer);
	    }
	    else
	    {
	       // no config file
	       // print error
	       cout << "colortail: Couldn't open global color config file. Skipping colors for the " << argv[i] << " file." << endl;
               // open the tailfile without colorizer
	       new_tailfile->open(argv[i], NULL);
	    }
	 }
	 else
	 {
	    // individual config files
	    // check if there is a config file
	    if (options->nr_cfg_files > tailfile_counter
		&& options->cfg_filenames[tailfile_counter] != NULL)
	    {
	       // there is a config file
	       colorizer = new Colorizer
		  (options->cfg_filenames[tailfile_counter]);

	       // open the tailfile
	       new_tailfile->open(argv[i], colorizer);
	    }
	    else
	    {
	       // no config file
	       // no error message because it can be cfgfile1,,cfgfile2
         // check for configuration file in home dir and global
         string cade = getenv("HOME");
         cade += "/.colortail/conf.colortail";
         if(!fopen(cade.c_str(), "r")){
           // open failed
           cade = "/etc/colortail/conf.colortail";
           if(!fopen(cade.c_str(), "r")){
             cade.clear();
           }
         }
		char* ccade = new char[cade.length()+1];
		strcpy(ccade, cade.c_str());
		colorizer = new Colorizer(ccade);
		new_tailfile->open(argv[i], colorizer);
	    }
	 }
      }
      else
      {
	 // no colors

	 // open the tailfile without colorizer
	 new_tailfile->open(argv[i], NULL);
      }

      // add the tailfile to the end of the tailfile list
      m_tailfiles.add_last(new_tailfile);

      // increase the tailfile counter
      tailfile_counter++;
   }

   // check if not follow-mode
   if (options->follow == 0)
   {
      // not follow-mode, just print the tails of the files in the list

      // make an iterator
      ListIterator<TailFile*> itr(m_tailfiles);
      TailFile *current_file;

      // iterate through the file list
      for (itr.init() ; !itr ; ++itr)
      {
	 current_file = itr();

	 // check if verbose mode
	 if (options->verbose)
	 {
	    // print filename
	    current_file->printFilename();
	 }

	 // print the specified number of rows
	 current_file->print(options->rows);
      }
   }
   else
   {
      // follow mode

      // make an iterator
      ListIterator<TailFile*> itr(m_tailfiles);
      TailFile *current_file;

      // iterate through the file list and print the no of rows wanted
      for (itr.init() ; !itr ; ++itr)
      {
	 current_file = itr();

	 // check if more than zero rows
	 if (options->rows > 0)
	 {
	    // check if verbose mode
	    if (options->verbose)
	    {
	       // print filename
	       current_file->printFilename();
	    }
	 }

	 // print the specified number of rows
	 current_file->print(options->rows);
      }

      // the "forever" part

      // keeps track of the last file a line was printed from
      char *last_filename = NULL;
      int changed = 0;
      while (1)
      {
	 // iterate through the file list and check if the
	 // file size has changed.

	 for (itr.init() ; !itr ; ++itr)
	 {
	    current_file = itr();

	    int n = current_file->more_to_read();
	    if (n != 0)
	    {
	       // file size has changed
	       // print the change, if there is a '\n'
	       current_file->follow_print(n, options->verbose,last_filename);

	       // get the filename of this file
	       last_filename = current_file->get_filename();

	       // set the changed boolean
	       changed = 1;
	    }
	 }

	 // check if changed
	 if (changed)
	 {
	    // something changed

	    // clear the flag
	    changed = 0;
	 }
	 else
	 {
	    // nothing changed

	    // sleep one second
	    sleep(1);
	 }
      }
   }
   return 0;
}
