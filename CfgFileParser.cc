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

#include "CfgFileParser.h"

#include <iostream>
#include <assert.h>
#include <cstring>
#include <stdlib.h>

using namespace std;

// ## class SearchData ##

SearchData::SearchData()
{
   // default constructor
   m_preg = NULL;
   m_ansi_color_code = NULL;
   m_pf = NULL;
   m_param_to_callback_fkn = NULL;
}

SearchData::~SearchData()
{
   if (m_preg)
   {
      // is allocated with malloc
      free(m_preg);
   }

   if (m_ansi_color_code)
   {
      delete m_ansi_color_code;
   }

   if (m_param_to_callback_fkn)
   {
      delete m_param_to_callback_fkn;
   }
}

void SearchData::set_color(char *color)
{
   // allocates memory and sets the m_ansi_color_code string to color

   if (m_ansi_color_code)
   {
      delete m_ansi_color_code;
   }

   m_ansi_color_code = new char[strlen(color)+1];
   strcpy(m_ansi_color_code, color);
}

CfgFileParser::CfgFileParser()
{
   m_items_list = NULL;
   m_filename = NULL;
   m_line = 0;
}

CfgFileParser::~CfgFileParser()
{
   free_items();
   if (m_filename)
   {
      delete m_filename;
   }
}

void CfgFileParser::free_items()
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

char* CfgFileParser::read_line()
{
   // reads the next line from the file and increases the line counter
   // prints error message if no characters were read.
   // returns: string or NULL if error.

   char *str = new char[MAX_CFG_LINE_LENGTH];
   assert (str != NULL);
   
   // read next line
   m_infile.getline(str, MAX_CFG_LINE_LENGTH);

   // increase line counter
   m_line++;
   
   // read less than zero chars?
   if (m_infile.gcount() <= 0)
   {
      // found EOF
      delete str;
      return NULL;
   }

   return str;
}

int CfgFileParser::read_item()
{
   // reads the color for the reg exps, then eads the reg exps and makes
   // SearchData instances for them and adds to the m_items_list
   // only reads and processes ONE item
   // returns number of SearchData instances added to the list

   int nr_items_added = 0;
   
   // is there a list?
   assert (m_items_list != NULL);

   // is there a open file?
   assert (m_infile);

   // read color
   char *color = read_color();

   if (color == NULL)
   {
      // didn't get a color

      // quit, return 0
      return 0;
   }
   
   // TODO: add action reading
   // read action

   if (read_left() == 0)
   {
      // found '{'

      // loop until '}'
      while (1)
      {
	 // read string
	 char *regexp = read_regexp();
	 if (regexp)
	 {
	    // check if it's a '}'
	    if (regexp[0] == '}')
	    {
	       // free mem
	       delete regexp;
	       // stop looping
	       break;
	    }
	    
	    // has a regexp.. make a SearchData item
	    
	    SearchData *searchdata = new SearchData();
	    // check allocation
	    assert (searchdata != NULL);
	    
	    // set color
	    searchdata->set_color(color);
	    
	    // allocate memory to the pattern storage buffer
	    searchdata->m_preg = (regex_t*) malloc(sizeof(regex_t));
	    // check allocation
	    assert (searchdata->m_preg != NULL);
	    
	    // make compiled pattern buffer
	    if (regcomp(searchdata->m_preg, regexp, REG_EXTENDED) != 0)
	    {
	       // failed to make compiled reg exp pattern
	       cout << "colortail: Failed to make compiled reg exp pattern for "
		    << "reg exp in config file " << m_filename << " at line "
		    << m_line << ". Skipping this line." << endl;
	       
	       // free mem
	       delete searchdata;
	    }
	    else
	    {
	       // TODO: set callback fkn
	       // TODO: set param to callback fkn
	       
	       // add the search data item to the items list
	       m_items_list->add(searchdata);
	       // increase items added counter
	       nr_items_added++;
	    }
	    // free mem
	    delete regexp;
	 }
	 else
	 {
	    // error reading string, eof maybe..
	    // stop looping
	    break;
	 }
      }
   }
   return nr_items_added;
}

char* CfgFileParser::read_color()
{
   // reads and skips lines until a 'COLOR' statement is found,
   // it then extracts the color after 'COLOR' and returns a new string
   // containing the ANSI color code for that string.
   // RETURNS: the new string, NULL on error.

   // is there a open file?
   assert (m_infile);

   while (1)
   {
      // read line
      char *tmp = read_line();
      if (!tmp)
      {
	 	// found EOF
	 	return NULL;
      }

      // got a line to look at

      // process line if it's not empty or doesn't starts with a '#'
      if (tmp[0] != '\0' && tmp[0] != '#')
      {
	 if (strncmp(tmp, "COLOR", 5) != 0)
	 {
	    cout << "colortail: Error in config file: " << m_filename
		 << " at line " << m_line << ". Skipping this line." << endl;
	 }
	 else 
	 {
	    int linepos = 5;
	    char *color = new char[20];

	    // skip all spaces
	    while (tmp[linepos] == ' ')
	    {
	       linepos++;
	    }

	    // read which color it is
	    int found_color = 0;

	    if (strncmp(&tmp[linepos], "black", 5) == 0) {
	       strcpy(color, BLACK);
	       found_color = 1;
	    }

	    if (strncmp(&tmp[linepos], "red", 3) == 0) {
	       strcpy(color, RED);
	       found_color = 1;
	    }
	    
	    if (strncmp(&tmp[linepos], "green", 5) == 0) {
	       strcpy(color, GREEN);
	       found_color = 1;
	    }

	    if (strncmp(&tmp[linepos], "yellow", 6) == 0) {
	       strcpy(color, YELLOW);
	       found_color = 1;
	    }

	    if (strncmp(&tmp[linepos], "blue", 4) == 0) {
	       strcpy(color, BLUE);
	       found_color = 1;
	    }

	    if (strncmp(&tmp[linepos], "magenta", 7) == 0) {
	       strcpy(color, MAGENTA);
	       found_color = 1;
	    }

	    if (strncmp(&tmp[linepos], "cyan", 4) == 0) {
	       strcpy(color, CYAN);
	       found_color = 1;
	    }

	    if (strncmp(&tmp[linepos], "white", 6) == 0) {
	       strcpy(color, WHITE);
	       found_color = 1;
	    }

	    if (strncmp(&tmp[linepos], "brightblack", 6+5) == 0) {
	       strcpy(color, BRIGHTBLACK);
	       found_color = 1;
	    }

	    if (strncmp(&tmp[linepos], "brightred", 6+3) == 0) {
	       strcpy(color, BRIGHTRED);
	       found_color = 1;
	    }
	    
	    if (strncmp(&tmp[linepos], "brightgreen", 6+5) == 0) {
	       strcpy(color, BRIGHTGREEN);
	       found_color = 1;
	    }

	    if (strncmp(&tmp[linepos], "brightyellow", 6+6) == 0) {
	       strcpy(color, BRIGHTYELLOW);
	       found_color = 1;
	    }

	    if (strncmp(&tmp[linepos], "brightblue", 6+4) == 0) {
	       strcpy(color, BRIGHTBLUE);
	       found_color = 1;
	    }

	    if (strncmp(&tmp[linepos], "brightmagenta", 6+7) == 0) {
	       strcpy(color, BRIGHTMAGENTA);
	       found_color = 1;
	    }
	    
            if (strncmp(&tmp[linepos], "brightcyan", 6+4) == 0) {
	       strcpy(color, BRIGHTCYAN);
	       found_color = 1;
	    }

	    if (strncmp(&tmp[linepos], "brightwhite", 6+5) == 0) {
	       strcpy(color, BRIGHTWHITE);
	       found_color = 1;
	    }

	    if (found_color == 0)
	    {
	       // didn't found color
	       cout << "colortail: Don't recognize color in config file: "
		    << m_filename << " at line " << m_line << endl;

	       // free mem
	       delete tmp;
	       delete color;
	       // error, return NULL
	       return NULL;
	    }
	    else
	    {
	       // free mem
	       delete tmp;
	       // found color
	       return color;
	    }
	 }
	 // free memory for read line
	 delete tmp;
      }
   }
   // should never execute this
   return NULL;
}

int CfgFileParser::read_left()
{
   // reads lines (skips empty and comments lines) until a '{' is found
   // RETURNS: 0 if a '{' is found, 1 if not found, 2 if error

   // is there a open file?
   assert (m_infile);

   while (1)
   {
      // read line
      char *tmp = read_line();
      if (!tmp)
      {
	 // error reading line
	 cout << "colortail: Error reading line in config file: "
	      << m_filename << " at line " << m_line << "." << endl;
	 // error, return 2
	 return 2;
      }

      // got a line to look at

      // process line if it's not empty or doesn't starts with a '#'
      if (tmp[0] != '\n' && tmp[0] != '#')
      {
	 if (tmp[0] == '{')
	 {
	    // free mem
	    delete tmp;

	    return 0;
	 }
	 else
	 {
	    // not a '{'
	    cout << "colortail: Error, expected '{' but found '"
		 << tmp[0] << "' in config file: " << m_filename
		 << " at line " << m_line << "." << endl;
	    // free mem
	    delete tmp;

	    return 1;
	 }
      }

      // free mem
      delete tmp;
   }

   // should never get this far
   return 1;
}

char* CfgFileParser::read_regexp()
{
   // reads and skips empty and comments lines until a regexp is found
   // RETURNS: a new string containing the reg exp, NULL on error

   // is there a open file?
   assert (m_infile);

   while (1)
   {
      // read line
      char *tmp = read_line();

      if (!tmp)
      {
	 // error reading line
	 cout << "colortail: Error reading line in config file: "
	      << m_filename << " at line " << m_line << "." << endl;

	 // error, return NULL
	 return NULL;
      }

      // got a line

      // process line if it's not empty or doesn't starts with a '#'
      if (tmp[0] != '\n' && tmp[0] != '#')
      {
	 // found a string, return it
	 return tmp;
      }

      // this is a empty line or a comment, skip it

      // free mem
      delete tmp;
   }

   // should never get this far
   return NULL;
}
	 

int CfgFileParser::parse(const char *filename)
{
   // parses the cfg file and sets up the list of SearchData elements
   // returns number of SearchData items created

   // is there a list?
   if (m_items_list)
   {
      // delete it
      free_items();
   }

   // make a new list
   m_items_list = new List<SearchData*>;

   // check the allocation
   assert (m_items_list != NULL);
  
   // try to open the file
   m_infile.open(filename, ios::in);

   if (!m_infile)
   {
      // open failed
      cout << "colortail: Failed to open config file: " << filename << endl;
      return 0;
   }

   // save filename of config file
   if (m_filename)
   {
      delete m_filename;
   }

   m_filename = new char[strlen(filename) + 1];
   strcpy(m_filename, filename);
   

   int items_counter = 0;
   
   // read the items
   while (!m_infile.eof())
   {
      // read in a item
      int n = read_item();

      items_counter += n;
   }
   
   return items_counter;
}
      
List<SearchData*>* CfgFileParser::get_items_list()
{
   // returns the items list, and sets this class item list to NULL

   List<SearchData*>* tmp = m_items_list;
   m_items_list = NULL;

   return tmp;
}

	 
      





