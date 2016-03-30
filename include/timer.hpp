/*

Name: KSD_AV_TIMER

Description: Timer 

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

Date: 13 March 2014

Author: Trevor Sibanda

Email: trevorsibb@gmail.com

*/
#ifndef KSD_AV_TIMER

#define KSD_AV_TIMER

#include <ctime>

class Timer{
public:
	Timer( bool auto_start = true );
	
	~Timer();
	
	//start the timer
	void Start();
	
	///stop the timer
	void Stop();
	
	//pause the timer
	void Pause();
	
	//reset the timer
	void Reset();
	
	//Retrieval
	
	//get time elapsed in ms since last start
	int GetMs();
	
	//get time in seconds since last start
	int GetSeconds();

private:
		time_t* startTime;
		time_t* pauseTime;
		

};

#endif

