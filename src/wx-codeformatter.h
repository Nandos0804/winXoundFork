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



class wxTextEditor;

#ifndef _WX_CODE_FORMATTER_H_
#define _WX_CODE_FORMATTER_H_

class wxCodeFormatter
{
public:
	wxCodeFormatter();
	virtual ~wxCodeFormatter();

	void FormatCode(wxTextEditor* editor, GHashTable* keywords);
	void FormatCode(wxTextEditor* editor, GHashTable* keywords, gint start, gint end);
	bool CheckScoreForGoodLine(Glib::ustring& textline);
	Glib::ustring Replace(Glib::ustring text, 
	                      Glib::ustring regex, 
	                      Glib::ustring replaceText);

	void CreateNewFormatterWindow();
	void showWindowAt(gint x, gint y);
	void closeWindow();

	void ConfigureTextView();
	
protected:

	void SciEditSetFontsAndStyles();
	bool on_key_press_event(GdkEventKey* event);
	void on_buttonCancel_Clicked();
	void on_checkbutton_toggled();
	void on_buttonSave_Clicked();
	void FillTextView();
	void on_buttonReset_Clicked();

	
	Gtk::Window*		formatterWindow;

	Gtk::CheckButton*   checkbuttonHeader;
	Gtk::CheckButton*   checkbuttonInstruments;
	Gtk::CheckButton*   checkbuttonFunctions;
	Gtk::CheckButton*   checkbuttonScoreInstruments;
	Gtk::CheckButton*   checkbuttonTempo;
	Gtk::RadioButton*   radiobuttonStyle1;
	Gtk::RadioButton*   radiobuttonStyle2;
	Gtk::Button*		buttonReset;
	Gtk::Button*		buttonCancel;
	Gtk::Button*		buttonSave;
	Gtk::VBox*			vboxScintilla;

	wxTextEditor*	    textView;

	GHashTable*			opcodes;

private:


};

#endif // _WX_CODE_FORMATTER_H_
