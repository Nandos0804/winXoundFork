/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * winxound_gtkmm
 * Copyright (C) Stefano Bonetti 2010 <stefano_bonetti@tin.it>
 * 
 * winxound_gtkmm is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * winxound_gtkmm is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "wx-position.h"
#include "wx-global.h"

#define CAPACITY 20

wxPosition::wxPosition()  	
{
	mIndex = 0;
	oldPos = -1;
}

wxPosition::~wxPosition()
{
	ClearAll();
	//wxGLOBAL->DebugPrint("wxPosition released");
}


void wxPosition::StoreCursorPos(gint position)
{
	try
	{
		gint newPos = position;

		if (newPos != oldPos)
		{
			//this.AddValue(newPos);
			AddValue(newPos);
			oldPos = newPos;
		}
		
	}
	catch(...)
	{
		wxGLOBAL->DebugPrint("wxPosition - StoreCursorPos Error");
	}

}


gint wxPosition::PreviousPos()
{
	try
	{
		//if (this.Count == 0) return 0;
		if(pos_list.size() == 0) return 0;

		mIndex++;
		
		//if (mIndex >= this.Count) mIndex = this.Count - 1;
		if(mIndex >= (gint)pos_list.size()) mIndex = pos_list.size() - 1;

		//if (mIndex >= mCAPACITY) mIndex = mCAPACITY - 1;
		if (mIndex >= CAPACITY) mIndex = CAPACITY - 1;

		//return (int)this[mIndex];
		return pos_list[mIndex];
		
	}
	catch (...)
	{
		wxGLOBAL->DebugPrint("wxPosition - PreviousPos Error");
	}

	return 0;
}

gint wxPosition::NextPos()
{
	try
	{
		//if (this.Count == 0) return 0;
		if(pos_list.size() == 0) return 0;

		mIndex--;
		
		if (mIndex < 0) mIndex = 0;

		//return (int)this[mIndex];
		return pos_list[mIndex];
		
	}
	catch (...)
	{
		wxGLOBAL->DebugPrint("wxPosition - NextPos Error");
	}

	return 0;
}

void wxPosition::AddValue(gint Value)
{
	try
	{
		//if (this.Count <= mCAPACITY)
		if(pos_list.size() <= CAPACITY)
		{
			//this.Insert(0, Value);
			pos_list.insert(pos_list.begin(), Value);
			
			mIndex = 0;
		}
		else
		{
			//this.RemoveAt(mCAPACITY);
			//pos_list.erase(pos_list.begin() + mCAPACITY);
			pos_list.pop_back();
			
			//this.Insert(0, Value);
			pos_list.insert(pos_list.begin(), Value);
			
			mIndex = 0;
		}
	}
	catch (...)
	{
		wxGLOBAL->DebugPrint("wxPosition - AddValue Error");
	}
}

gint wxPosition::Position()
{
	return mIndex;
}

void wxPosition::ClearAll()
{
	try
	{
		//this.Clear();
		pos_list.clear();
		
		mIndex = 0;
		oldPos = -1;
		
	}
	catch (...)
	{
		wxGLOBAL->DebugPrint("wxPosition - ClearAll Error");
	}
}



