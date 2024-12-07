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

#include <gtkmm.h>


#ifndef _WX_INFO_H_
#define _WX_INFO_H_

class wxInfo: public Gtk::Window //public Gtk::Dialog//public Gtk::Window 
{
public:
	wxInfo();
	virtual ~wxInfo();

	void CreateNewInfoWindow();
	void showWindow();
	void showWindowAt(gint x, gint y);
	void hideWindow();
	void setLabelText(Glib::ustring text);
	virtual bool on_key_press_event(GdkEventKey* event);

protected:
	Gtk::Label*		labelInfo;

private:

};

#endif // _WX_INFO_H_
