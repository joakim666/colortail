/*
    colortail -- output last part of file(s) in color.
    Copyright (C) 1999  Joakim Andersson <pt98jan@student.hk-r.se>

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

#include "Info.h"
#include "config.h"

using namespace std;

Info::Info()
{
}

Info::~Info()
{
}

// prints info about colortail
void Info::print()
{
   cout << "Colortail version <" << COLORTAIL_VERSION << ">" << endl;
   cout << "Copyright (C) 2009 Joakim Andersson <ja@joakimandersson.se>" << endl;
   cout << "Colortail comes with ABSOLUTELY NO WARRANTY; for details see the COPYING file." << endl;
   cout << "This is free software, and you are welcome to redistribute it" << endl;
   cout << "under certain conditions; see the COPYING file for details." << endl;
}

   







