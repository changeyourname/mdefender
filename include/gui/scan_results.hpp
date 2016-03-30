/*
Name: KSD_GUI_SCAN_RESULTS_WINDOW

Description: GUI Interface for scan progress, Self contained Class

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


Date: 6 April 2014

Author: Trevor Sibanda

Email: trevorsibb@gmail.com

*/

 
 #ifndef KSD_GUI_SCAN_RESULTS
 
 #define KSD_GUI_SCAN_RESULTS
 
#include <windows.h> 
#include <string>


 
 //we must have the detected scanObjects reference

  
 //Actions to be carried out on detected files
 #define KSD_DELETE_DETECTED 					0
 #define KSD_QUARANTINE_DETECTED 			1
 #define KSD_REPAIR_DETECTED					2 //NOT YET IMPLEMENTED
 #define KSD_IGNORE_DETECTED					3
 
 namespace mdf{
 
 namespace gui{
 
 //blocking function
 //first hides parent window, then displays window
 //returns false if user clicks on cancel
 
 std::vector<SROB> scanResults; 
 
 int actionToApply;
 
 bool ScanResultsWindow(HWND _parent ,  std::vector<SROB>& refSROB ,  int& actionToTake );
 
 }
 
 
 
 
 };
 
#endif 
