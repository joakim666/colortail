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

#include "Usage.h"

using namespace std;

Usage::Usage()
{
}

Usage::~Usage()
{
}

// prints usage
void Usage::print()
{
   cout << endl;
   cout << "Usage: colortail [OPTION]... [FILE]..." << endl;
   cout << "Print last 10 lines of each FILE to standard output." << endl;
   cout << "With more than one FILE, precede each with a header giving the file name." << endl;
   cout << endl;
   cout << "  -f, --follow\t\t\toutput appended data as the file grows" << endl;
   cout << "  -h, --help\t\t\tdisplay this help and exit" << endl;
   cout << "  -k, --config=file" << endl;
   cout << "      --config=file1,file2,...\tcolor config files for the tail files." << endl;
   cout << "\t\t\t\tIf only one config file it's global otherwise" << endl;
   cout << "\t\t\t\tone config file for each tail file" << endl;
   cout << "  -n, --lines=N\t\t\toutput the last N lines, instead of last 10" << endl;
   cout << "  -q, --quiet, --silent\t\tnever output headers giving file names" << endl;
   cout << "  -v, --version\t\t\toutput version information and exit" << endl;
   cout << endl;
   cout << "Report bugs to ja@joakimandersson.se" << endl;
}

   
