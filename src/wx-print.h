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


#ifndef _WX_PRINT_H_
#define _WX_PRINT_H_


class wxPrint: public Gtk::PrintOperation
{
public:
	wxPrint();
	virtual ~wxPrint();

	void PrintText(Glib::ustring text, bool isPreview, Glib::ustring jobname);


protected:

	Gtk::Window*		printWindow;
	Gtk::Label*			labelInfo;
	void showWindow();
	void hideWindow();
	bool on_key_press_event(GdkEventKey* event);
	void CreatePrintWindow();
	void on_print_done(Gtk::PrintOperationResult result);
	bool on_timer_tick();

	gint				pageIndex;

	
	
	//PrintOperation default signal handler overrides:
	virtual void on_begin_print(const Glib::RefPtr<Gtk::PrintContext>& context);
	virtual void on_draw_page(const Glib::RefPtr<Gtk::PrintContext>& context, int page_nr);

	Glib::RefPtr<Pango::Layout> m_refLayout;
	std::vector<int> m_PageBreaks; // line numbers where a page break occurs

	Glib::ustring mText;

	//Printing-related objects:
	Glib::RefPtr<Gtk::PageSetup> m_refPageSetup;
	Glib::RefPtr<Gtk::PrintSettings> m_refSettings;



private:

};

#endif // _WX_PRINT_H_
