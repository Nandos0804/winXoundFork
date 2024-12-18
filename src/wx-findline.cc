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

#include "wx-findline.h"
#include "wx-main.h"
#include "wx-global.h"



wxFindLine::wxFindLine(wxMain* owner)
{

	mOwner = owner;
	
	Glib::RefPtr<Gtk::Builder> builder;
	try
	{
		builder = Gtk::Builder::create_from_file(
			Glib::ustring::compose("%1/winxound_findline.ui",wxGLOBAL->getSrcPath()));
	}
	catch (const Glib::FileError & ex)
	{
		wxGLOBAL->DebugPrint("wxFindLine constructor - Builder Error - Critical",
		                     ex.what().c_str());
		return;
	}
	catch (...)
	{
		wxGLOBAL->DebugPrint("wxFindLine constructor - Builder Error - Critical");
		return;
	}

	//Locate and Associate wxMainWindow
	builder->get_widget("windowFindLine", FindLineWindow);
	associateWidgets(builder);
	FindLineWindow->set_border_width(2);
	FindLineWindow->signal_key_press_event().connect(
		sigc::mem_fun(*this, &wxFindLine::on_key_press_event));

	Glib::ustring iconfile = Glib::ustring::compose("%1/winxound_48.png",wxGLOBAL->getIconsPath());
	if(Glib::file_test(iconfile, Glib::FILE_TEST_EXISTS))
		FindLineWindow->set_icon_from_file(iconfile);

	spinbuttonLineNumber->signal_key_press_event().connect(
		sigc::mem_fun(*this, &wxFindLine::on_key_press_event));

	//findWindow->hide();
	//findWindow->set_title(TITLE);
	//findWindow->move(0, 0);
	//findWindow->resize(400,300);
}

wxFindLine::~wxFindLine()
{
	delete FindLineWindow;

	wxGLOBAL->DebugPrint("wxFindLine released");
}

void wxFindLine::associateWidgets(Glib::RefPtr<Gtk::Builder> builder)
{
	
	//WINDOW WIDGETS
	builder->get_widget("spinbuttonLineNumber",spinbuttonLineNumber);
	builder->get_widget("buttonFindLine",buttonFindLine);
	builder->get_widget("buttonExit",buttonExit);

	spinbuttonLineNumber->set_update_policy(Gtk::UPDATE_ALWAYS);
	spinbuttonLineNumber->set_numeric(TRUE);

	buttonExit->signal_clicked().connect(
		sigc::mem_fun(*this, &wxFindLine::on_buttonExit_Clicked));

	buttonFindLine->signal_clicked().connect(
		sigc::mem_fun(*this, &wxFindLine::on_buttonFindLine_Clicked));


}

void wxFindLine::on_buttonExit_Clicked()
{
	closeWindow();
}

void wxFindLine::on_buttonFindLine_Clicked()
{
	spinbuttonLineNumber->update();
	mOwner->GoToLineNumber(spinbuttonLineNumber->get_value_as_int());
	on_buttonExit_Clicked();
}

bool wxFindLine::on_key_press_event(GdkEventKey* event)
{
	//wxGLOBAL->DebugPrint("KEY", "PRESSED");
	if (event->keyval == GDK_Escape)
	{
		on_buttonExit_Clicked();
	}
	if (event->keyval == GDK_Return)
	{
		on_buttonFindLine_Clicked();
	}
	return false;
}


void wxFindLine::showWindowAt(gint x, gint y)
{               
	//Show window
	FindLineWindow->show_all();
	FindLineWindow->present(); //Deiconify if necessary

	FindLineWindow->move(x - (FindLineWindow->get_width() / 2), 
	                     y - (FindLineWindow->get_height() / 2));


	FindLineWindow->grab_focus();
}

void wxFindLine::closeWindow()
{	
	FindLineWindow->hide();
}