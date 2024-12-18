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

#ifndef _WX_FINDCODESTRUCTURE_H_
#define _WX_FINDCODESTRUCTURE_H_


#include <gtkmm.h>

class wxEditor;
class wxTextEditor;


class wxFindCodeStructure
{
public:
	wxFindCodeStructure(wxEditor* owner, wxTextEditor* textEditor);
	virtual ~wxFindCodeStructure();

	void Start();
	void Stop();


	//Tree model columns:
	class ModelColumns : public Gtk::TreeModel::ColumnRecord
	{
	public:
		Gtk::TreeModelColumn<Glib::ustring> key;
		Gtk::TreeModelColumn<Glib::ustring> value;
		Gtk::TreeModelColumn< Glib::RefPtr < Gdk::Pixbuf > > image;

		ModelColumns()
		{
			add(key); 
			add(value);
			add(image);
		}

		//row[m_Columns.m_col_image] = Gdk::Pixbuf::create_from_file("./skin/cerrar.png"); 	
		//Gtk::TreeModelColumn< Glib::RefPtr < Gdk::Pixbuf > > m_col_image; 
		//Gtk::TreeModelColumn<int> m_col_id;
		//Gtk::TreeModelColumn<Glib::ustring> m_col_name;

	};	
	ModelColumns	MyColumns;
	Glib::RefPtr<Gtk::TreeStore> MyRefTreeModel;
	Glib::Mutex mutex;
	Glib::Dispatcher sig_done;


protected:
	wxEditor*		mOwner;
	wxTextEditor*   mTextEditor;
	bool			mStop;

	void FindStructure();
	Glib::ustring ParseLine(Glib::ustring Text);
	bool findString(const gchar* stringToFind,
            		Gtk::TreeModel::Row passedNode,
            		bool isScore);
	bool findStringInScore(const gchar* stringToFind,
	                       Gtk::TreeModel::Row passedNode);

	void CreatePixbufs();

	Glib::RefPtr<Gdk::Pixbuf>   pixbufBlue;
	Glib::RefPtr<Gdk::Pixbuf>   pixbufGreen;
	Glib::RefPtr<Gdk::Pixbuf>   pixbufGrey;
	Glib::RefPtr<Gdk::Pixbuf>   pixbufOrange;
	Glib::RefPtr<Gdk::Pixbuf>   pixbufRed;
	Glib::RefPtr<Gdk::Pixbuf>   pixbufSand;
	Glib::RefPtr<Gdk::Pixbuf>   pixbufViolet;
	

private:
	
	

};

#endif // _WX_FINDCODESTRUCTURE_H_
