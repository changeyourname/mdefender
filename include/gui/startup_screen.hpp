/*
Name: KSD_GUI_STARTUP_WINDOW

Description: GUI Interface for startup window, 

Copyright (C) 2014  Trevor Sibanda, Clifford J Mthemba, ZimCODERZ

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

Date: 21 April 2014

Author: Trevor Sibanda

Email: trevorsibb@gmail.com

*/

#ifndef KSD_GUI_STARTUP_WINDOW

#define KSD_GUI_STARTUP_WINDOW

//As soon as an object is instantiated
//a thread is setup and the window is shown
//the window is shown for a period of time defined 
//by timeout
//after which the window is terminated and may not
//be used again  
//like a startup window should be
//BTW its not blocking, so program execution continues
// immediately after
//a call to isActive will tell you if the window is still 
//being displayed
class StartUPWindow{
public:
	StartUPWindow(int timeout = 5000);
	
	~StartUPWindow();
	
	bool isActive()const;
};


#endif

