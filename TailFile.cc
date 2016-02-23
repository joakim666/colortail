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
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#include "TailFile.h"

using namespace std;

TailFile::TailFile()
{
   m_filename = NULL;
   m_file = NULL;
   m_colorizer = NULL;
   m_position = 0;
   m_follow_buffer = NULL;
   memset(&m_file_stats, 0, sizeof(struct stat));
   reopening = 0;
}

TailFile::~TailFile()
{
   if (m_filename)
   {
      delete[] m_filename;
   }
   
   if (m_file)
   {
      fclose(m_file);
   }
   
   if (m_colorizer)
   {
      delete m_colorizer;
   }

   if (m_follow_buffer)
   {
      delete m_follow_buffer;
   }
}

int TailFile::reopen()
{
  if (m_file)
    fclose(m_file);
  m_file = NULL;

  if (m_follow_buffer)
    delete m_follow_buffer;
  m_follow_buffer = NULL;
  m_position = 0;
  memset(&m_file_stats, 0, sizeof(struct stat));
  reopening = 1;
  char *tmp = m_filename;
  int ret = open(m_filename, m_colorizer);
  delete tmp;
  reopening = 0;
  return ret;
}

int TailFile::open(char *filename, Colorizer *colorizer)
{
   // Opens the file to tail. And sets the colorizer
   // Prints error message if failed to open file.
   // returns: 0 - success
   //          1 - failure

   if (filename == NULL)
   {
      cout << "colortail: Filename was NULL" << endl;
      return 1;
   }
   else
   {
      // save filename
      m_filename = new char[strlen(filename) + 1];
      strcpy(m_filename, filename);

      // tries to open the file
      if (strcmp (filename, "-") == 0)
      {
         m_file = stdin;
      }
      else
         m_file = fopen(filename, "r");

      if (m_file == NULL)
      {
	 // open failed
	 cout << "colortail: Failed to open file: " << filename << endl;
	 return 1;
      }

      // set the colorizer
      m_colorizer = colorizer;

      // set the saved stream position used to see if the file has
      // changed size to the end of the file
      m_position = reopening ? 0 : end_of_file_position();
   }
   return 0;
}

void TailFile::print(int n)
{
   // print and colorize last n rows of the file

   // is a file open
   if (m_file == NULL)
   {
      // no file open
      return;
   }

   find_position(n);

//   const bufSize = 1024;
   char buf[MAX_CHARS_READ];

   // print file
   int loop = 1;
   while (loop)
   {
      // empty buffer
      // not nessesary??
      for (int i = 0 ; i < MAX_CHARS_READ ; i++)
      {
	 buf[i] = '\0';
      }
      
      // read line
      char *ret = fgets(buf, MAX_CHARS_READ-1, m_file);
      
      // read more than zero chars
      if (ret != NULL)
      {
		// remove trailing newline. TODO only needed on OSX?
/*		char last = buf[strlen(buf)-1];
		if (last == '\n') {
			buf[strlen(buf)-1] = '\0';
		}*/
	
	 // print the line
	 print_to_stdout(buf);
      }
      else
      {
	 // eof
	 // stop looping
	 loop = 0;
      }
   }   
}

void TailFile::printFilename()
{
   // prints the filename to stdout

   cout << "==> " << m_filename << " <==" << endl;
}

char* TailFile::get_filename()
{
   // returns the filename
   return m_filename;
}

void TailFile::printAll()
{
   // print the complete file, for debugging
   const int bufSize = 1024;
   char buf[bufSize];
}


void TailFile::find_position(int n)
{
   // sets the file to the position after the n:th return
   // from the end of the file
   
   // check if file is not open
   if (m_file == NULL)
   {
      // no file open
      return;
   }

   // check if zero rows
   if (n == 0)
   {
      // zero rows
      // set position to end of file
      fseek(m_file, 0, SEEK_END);
      return;
   }
   
   const int bufSize (2048);
   // buffer for chars
   char buf[bufSize];

   // bytes to read
   int bytesToRead = bufSize;

   // position in file from beginning
   long pos = 0;

   // go to end of file
   fseek(m_file, 0, SEEK_END);

   // set pos to last position
   pos = ftell(m_file);

   while (pos != 0)
   {   
      // sub bufSize-1 from position
      pos -= (bufSize-1);

      // check if pos < 0
      if (pos < 0)
      {
	 // pos below zero
	 
	 // calc new bytesToRead (pos is < 0)
	 bytesToRead += pos;
	 
	 // pos below zero, set pos to zero
	 pos = 0;
      }
   
      // goto that position
      fseek(m_file, pos, SEEK_SET);

      // read bytesToRead bytes from the file
      // note: use fgetc, because fgets stops at newlines
      for (int j = 0 ; j < bytesToRead-1 ; j++)
      {
	 buf[j] = fgetc(m_file);
      }
      // null terminate the string
      buf[bytesToRead-1] = '\0';

      // search through the character buffer
      for (int i = bytesToRead-1 ; i >= 0 ; i--)
      {
	 // is it a CR
	 if (buf[i] == '\n')
	 {
	    // found return
	    // decrease n
	    n--;
	    // enough returns
	    if (n < 0)
	    {
	       // goto position after the return
	       fseek(m_file, pos + i + 1, SEEK_SET);
	       return;
	    }
	 }
      }
   }

   // if execution gets here, then all the returns wasn't found
   // set position to beginning of file
   fseek(m_file, 0, SEEK_SET);
}

long TailFile::end_of_file_position()
{
   // returns the position for the end of the current file ie
   // the size of the file.
   // doesn't modify the current position
   
   // check if file isn't open
  if (m_file == NULL && reopen())
   {
      // no file open, return 0
      return 0;
   }

   struct stat file_stats;

   // get the stats for the current file
   int ret = stat(m_filename, &file_stats);

   if (ret != 0)
   {
     // some error, return 0
     return 0;
   }

   if ((m_file_stats.st_ino && file_stats.st_ino != m_file_stats.st_ino) ||
       (m_file_stats.st_ino && file_stats.st_ino == m_file_stats.st_ino &&
        m_file_stats.st_size > file_stats.st_size))
     {
       if (!reopen())
	 {
	   int ret = stat(m_filename, &file_stats);
	   if (ret)
	     return 0;
	 }
     }

   memcpy(&m_file_stats, &file_stats, sizeof(struct stat));

   // return the size
   return file_stats.st_size;
}

int TailFile::more_to_read()
{
   // returns the number of bytes that can be read from the file
   // or 0 if nothing has been added to the file
   // updates the saved position if the file has been truncated.
   // RETURNS: number of bytes or 0

   // check if no file open
   if (m_file == NULL)
   {
      // no file open, return 0
      return 0;
   }

   // get the end of file position
   long end = end_of_file_position();

   // check if file is truncated
   if (end < m_position)
   {
      // yes, truncated
      // set the saved position to the new (lesser) stream position
      m_position = end;
      // nothing added to the file, just removed
      return 0;
   }

   return end - m_position;
}

void TailFile::follow_print(int n, int verbose, char *last_filename)
{
   // Reads n characters from the file and search from the beginning after a
   // '\n'. If it is found that line is colorized and printed, and the
   // current stream position is uppdated. If a line is found and the
   // verbose flag is set. The filename is printed in ==> <== :s if it
   // isn't the same file as the last line was printed from.
   // If it isn't found, nothing is printed and the stream position
   // isn't changed.

   // check if a file isn't open
   if (m_file == NULL)
   {
      // no file open
      // just return
      return;
   }

   // check if there isn't a follow buffer
   if (m_follow_buffer == NULL)
   {
      m_follow_buffer = new ostringstream();
   }

   // reset saved EOF result, see man fgetc(3)
   clearerr(m_file);
   
   // read n characters
   //char *ret = fgets(buf, (n + 1), m_file);
   for (int i = 0 ; i < (n-1) ; i++)
   {
      int ch = fgetc(m_file);

      // add the character to the string
      m_follow_buffer->put(ch);
      
      // check if return
      if (ch == '\n')
      {
	 // a return

	 // chack if the verbose flag is set
	 if (verbose)
	 {
	    // check if last_filename is NULL
	    if (last_filename == NULL)
	    {
	       // print the name of this file
	       printFilename();
	       }
	    else
	    {
	       // last_filename isn't NULL
	       // check if same filename
	       if (strcmp(last_filename, m_filename) != 0)
	       {
		  // not same file
		  // print the name of this file
		  printFilename();
	       }
	    }
	 }

	string source_str = m_follow_buffer->str();

	 // get the string
	 const char *str = source_str.c_str();
	 
	 // print the line
	 print_to_stdout(str);

	 // free the mem
	 delete m_follow_buffer;
	 m_follow_buffer = NULL;
	 
	 // update the saved stream position with the no of characters
	 // until and including the '\n'

	 m_position = ftell(m_file);

	 // break the loop
	 break;
      }
   }   
}

void TailFile::print_to_stdout(const char *str)
{
   // checks if there is a colorizer. If so the string is colorized
   // before it's printed. If not the string isn't colorized.

   // check that str is something
   if (str == NULL)
   {
      return;
   }

   // check if we have a colorizer
   if (m_colorizer)
   {
      // colorize the string
      const string& res = m_colorizer->colorize(str);
      // print the new colorized string
      cout << res;
   }
   else
   {
      // don't colorize

      // print the line
      cout << str;
   }
}




