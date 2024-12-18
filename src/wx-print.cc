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

#include "wx-print.h"
#include "wx-global.h"
#include "wx-settings.h"

#define PRINT_MARGINS 20


wxPrint::wxPrint()  	
{
	//m_refPageSetup = Gtk::PageSetup::create();
	//m_refSettings = Gtk::PrintSettings::create();
	CreatePrintWindow();
}
wxPrint::~wxPrint()
{
	delete printWindow;
	
	wxGLOBAL->DebugPrint("wxPrint released");
}

void wxPrint::CreatePrintWindow()
{
	printWindow = new Gtk::Window();

	Gtk::VBox* vbox = Gtk::manage(new Gtk::VBox());
	labelInfo = Gtk::manage(new Gtk::Label("Printing ..."));
	
	printWindow->set_border_width(2);
	vbox->pack_start(*labelInfo, TRUE, TRUE, 0);
	vbox->set_size_request(200,-1);
	printWindow->add(*vbox);
	
	printWindow->set_size_request(300,50);
	printWindow->set_title("WinXound - Print");
	printWindow->set_resizable(FALSE);

	
}

void wxPrint::showWindow()
{
	//Show window
	printWindow->move(0,0);
	printWindow->show_all();
	printWindow->present(); //Deiconify if necessary
	printWindow->grab_focus();
}

void wxPrint::hideWindow()
{
	printWindow->hide();
}

bool wxPrint::on_key_press_event(GdkEventKey* event)
{
	//wxGLOBAL->DebugPrint("KEY", "PRESSED");
	if (event->keyval == GDK_Escape)
	{
		this->hideWindow();
	}
	return false;
}






void wxPrint::PrintText(Glib::ustring text, bool isPreview, Glib::ustring jobname)
{

	mText = text;

	m_refPageSetup = Gtk::PageSetup::create();
    //m_refSettings = Gtk::PrintSettings::create();
	m_refPageSetup->set_left_margin(PRINT_MARGINS, Gtk::UNIT_MM);
	m_refPageSetup->set_right_margin(PRINT_MARGINS, Gtk::UNIT_MM);
	m_refPageSetup->set_top_margin(PRINT_MARGINS, Gtk::UNIT_MM);
	m_refPageSetup->set_bottom_margin(PRINT_MARGINS, Gtk::UNIT_MM);
	
	
	this->set_job_name(jobname);
	//this->set_show_progress(TRUE);
	//this->set_track_print_status();
	this->set_embed_page_setup(TRUE);
	//this->set_allow_async(TRUE);
	this->set_default_page_setup(m_refPageSetup);
	//this->set_print_settings(m_refSettings);

	this->signal_done().connect(sigc::mem_fun(*this, &wxPrint::on_print_done));


	Gtk::PrintOperationAction print_action;

	if(isPreview)
		print_action = Gtk::PRINT_OPERATION_ACTION_PREVIEW;
	else
		print_action = Gtk::PRINT_OPERATION_ACTION_PRINT_DIALOG;

	
	//START PRINTING OPERATION
	try
	{
		this->run(print_action);
	}
	catch (...)
	{
		wxGLOBAL->DebugPrint("wxPrint: an error occurred while trying to run a print operation!");
	}

}



void wxPrint::on_begin_print(const Glib::RefPtr<Gtk::PrintContext>& print_context)
{
	//Create and set up a Pango layout for PrintData based on the passed
	//PrintContext: We then use this to calculate the number of pages needed, and
	//the lines that are on each page.
	m_refLayout = print_context->create_pango_layout();


	//Pango::FontDescription font_desc("Monospace 10");
	Glib::ustring font = 
		Glib::ustring::compose("%1 %2",
		                       wxSETTINGS->EditorProperties.DefaultFontName,
		                       wxSETTINGS->EditorProperties.DefaultFontSize);
	Pango::FontDescription font_desc(font);

	
	m_refLayout->set_font_description(font_desc);
	m_refLayout->set_wrap(Pango::WRAP_WORD_CHAR);

	const double width = print_context->get_width();
	const double height = print_context->get_height();

	m_refLayout->set_width(static_cast<int>(width * Pango::SCALE));

	//Set and mark up the text to print:
	//Glib::ustring marked_up_form_text = mText;

	//m_refLayout->set_markup(marked_up_form_text);
	m_refLayout->set_text(mText);

	
	labelInfo->set_text("Printing ...");
	showWindow();
	pageIndex = 0;

	
	//Set the number of pages to print by determining the line numbers
	//where page breaks occur:
	const int line_count = m_refLayout->get_line_count();

	Glib::RefPtr<Pango::LayoutLine> layout_line;
	double page_height = 0;

	for (int line = 0; line < line_count; ++line)
	{
		Pango::Rectangle ink_rect, logical_rect;

		layout_line = m_refLayout->get_line(line);
		layout_line->get_extents(ink_rect, logical_rect);

		const double line_height = logical_rect.get_height() / 1024.0;

		if (page_height + line_height > height)
		{
			m_PageBreaks.push_back(line);
			page_height = 0;
		}

		page_height += line_height;
	}

	set_n_pages(m_PageBreaks.size() + 1);
}

void wxPrint::on_draw_page(const Glib::RefPtr<Gtk::PrintContext>& print_context, int page_nr)
{
	//Decide which lines we need to print in order to print the specified page:
	int start_page_line = 0;
	int end_page_line = 0;

	if(page_nr == 0)
	{
		start_page_line = 0;
	}
	else
	{
		start_page_line = m_PageBreaks[page_nr - 1];
	}

	if(page_nr < static_cast<int>(m_PageBreaks.size()))
	{
		end_page_line = m_PageBreaks[page_nr];
	}
	else
	{
		end_page_line = m_refLayout->get_line_count();
	}

	//Get a Cairo Context, which is used as a drawing board:
	Cairo::RefPtr<Cairo::Context> cairo_ctx = print_context->get_cairo_context();

	//We'll use black letters:
	cairo_ctx->set_source_rgb(0, 0, 0);

	//Render Pango LayoutLines over the Cairo context:
	Pango::LayoutIter iter;
	m_refLayout->get_iter(iter);

	double start_pos = 0;
	int line_index = 0;

	pageIndex++;
	labelInfo->set_text(Glib::ustring::compose("Printing page %1", pageIndex));
	
	do
	{
		Gtk::Main::iteration(false);
		if (line_index >= start_page_line)
		{
			Glib::RefPtr<Pango::LayoutLine> layout_line = iter.get_line();
			Pango::Rectangle logical_rect = iter.get_line_logical_extents();
			int baseline = iter.get_baseline();

			if (line_index == start_page_line)
			{
				start_pos = logical_rect.get_y() / 1024.0;
			}

			cairo_ctx->move_to(logical_rect.get_x() / 1024.0,
			                   baseline / 1024.0 - start_pos);

			layout_line->show_in_cairo_context(cairo_ctx);
		}

		line_index++;
			                 
	}
	while (line_index < end_page_line && iter.next_line());
}

void wxPrint::on_print_done(Gtk::PrintOperationResult result)
{
	hideWindow();
}



