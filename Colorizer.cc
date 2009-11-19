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

#include "Colorizer.h"
#include "CfgFileParser.h"
#include "TailFile.h"

#include <assert.h>
#include <iostream>
#include <string.h>

using namespace std;

Colorizer::Colorizer()
{
   // default constructor
   
   m_items_list = NULL;
}

void Colorizer::free_items()
{
   // frees the memory that the items uses

   // check if there is a list
   if (m_items_list)
   {
      SearchData *tmp;

      // go through all the elements in the list,
      // and free each SearchData instance
      while (!m_items_list->is_empty())
      {
         tmp = m_items_list->first_element();
         m_items_list->remove_first();
         delete tmp;
      }

      // delete the list
      delete m_items_list;
      m_items_list = NULL;
   }
}


Colorizer::Colorizer(const char *cfg_file)
{
   // other constructor
   
   // sends the cfg_file to the CfgFileParser
   // gets the items_list from the CfgFileParser and saves it

   // check that cfg_file isn't NULL
   assert (cfg_file != NULL);

   CfgFileParser parser;

   // parse the cfg file
   int n = parser.parse(cfg_file);

   // get the items list
   m_items_list = parser.get_items_list();
}

Colorizer::~Colorizer()
{
   // destructor

   // free the memory the search data items use
   free_items();
}

string Colorizer::colorize(const char *str)
{
   // colorize the string, returns a new string containing
   // the colorized version of str
   // RETURN: new string with result

   // check that a list exists
   assert (m_items_list != NULL);
   
   regmatch_t pmatch[10];
   int found = 0, submatch = 0, j;
   char color[MAX_CHARS_READ][20];

   // set all the color strings to empty strings
   for (int f = 0 ; f < MAX_CHARS_READ ; f++)
   {
      color[f][0] = '\0';
   }
   
   // make an iterator
   ListIterator<SearchData*> itr(*m_items_list);

   // will contain the new string
   ostringstream newstr;
   
   SearchData *current;
   int i = 0;
   // go through all the elements in the list
   for (itr.init() ; !itr ; ++itr, i++)
   {
      current = itr();

      // check for match
      if (regexec(current->m_preg, str, 10, pmatch, 0) == 0)
      {
	 // TODO: check for callback function
	 // TODO: call callback function
	 
	 found = 1;

	 // Check to see if this is a substring match
	 if (pmatch[1].rm_so != -1)
	 {
	    submatch = 1;
	    // Note: start at offset 1 to get only submatches

	    for (j = 1 ; pmatch[j].rm_so != -1 && j <= 10 ; j++)
	    {
	       // go through all the character positions
	       // that the submatch is for
	       for (int k = pmatch[j].rm_so ; k < (pmatch[j].rm_eo) ; k++)
	       {
		  
		  // set the color in the color string array
		  strcpy(color[k], current->m_ansi_color_code);
	       }
	    }
	 }
	 else
	 {
	    // not a substring match, colorize whole string.
	    // stop looking for other matches as well

	    submatch = 0;

	    // write ansi color code
	    newstr << current->m_ansi_color_code;
	    
	    // check if str ends in '\n'
	    int len = strlen(str);	
	    if (str[len-1] == '\n')
	    {
	       for (int a = 0 ; a < len-1 ; a++)
	       {
		  newstr.put(str[a]);
	       }
	    }
	    else
	    {
	       // doesn't end in '\n'
	       newstr << str;
	    }

	    // write ansi reset str and a newline
	    newstr << ANSI_RESET_STR << endl << ends;
	    // return the new string
	    return newstr.str();
	 }
      }
   }

   // did we get a match
   if (found == 0)
   {
      // no we didn't
      // print without color
      // check if str ends in '\n'
      if (str[strlen(str)-1] == '\n')
      {
	 newstr << str << ends;
      }
      else
      {
	 // doesn't end in '\n'
	 newstr << str << endl << ends;
      }

      // return the new string
      return newstr.str().c_str();
   }

   // did we find submatches?
   if (submatch == 1)
   {
      int last_was_reset_str = 1;
      // iiterate through all the characters
      int l = strlen(str);
      for (i = 0 ; i < l ; i++)
      {
	 if (color[i][0] == '\0')
	 {
	    // no color for this position
	    if (last_was_reset_str != 1)
	    {
	       // write reset string
	       newstr << ANSI_RESET_STR;
	       // remeber that we wrote it
	       last_was_reset_str = 1;
	    }
	 }
	 else
	 {
	    // a color for this position
	    // reset ansi reset string checker
	    last_was_reset_str = 0;
	    // write color
	    newstr << color[i];
	 }

	 // write current character
	 newstr.put(str[i]);
      }
      
      // check if last wasn't the ansi reset string
      if (last_was_reset_str != 1)
      {
	 // write reset string
	 newstr << ANSI_RESET_STR;
      }
      
      // write newline and null
      //newstr << endl << ends;
      newstr << ends;
      	 
      return newstr.str().c_str();
      
   }
   
      
   // should never get here
   return NULL;
}
