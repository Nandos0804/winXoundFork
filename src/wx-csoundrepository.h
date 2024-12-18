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


#ifndef _WX_CSOUND_REPOSITORY_H_
#define _WX_CSOUND_REPOSITORY_H_

class wxCSoundRepository : public Gtk::EventBox //public Gtk::Frame
{
public:
	wxCSoundRepository();
	virtual ~wxCSoundRepository();



protected:

	//Tree model columns:
	class csoundColumns : public Gtk::TreeModel::ColumnRecord
	{
	public:
		Gtk::TreeModelColumn<Glib::ustring> name;
		Gtk::TreeModelColumn<Glib::ustring> category;
		Gtk::TreeModelColumn<Glib::ustring> description;
		Gtk::TreeModelColumn<Glib::ustring> synopsis;

		csoundColumns()
		{
			add(name); 
			add(category);
			add(description);
			add(synopsis);
		}

	};	

	csoundColumns					csColumns;
	Glib::RefPtr<Gtk::TreeStore>	csRefTreeModel;

	Gtk::TreeView*					treeStructure;
	Gtk::Label*						labelDescription;
	Gtk::Label*						labelSynopsis;
	Gtk::ScrolledWindow*			mScrolledWindow;

	void CreateNewCSoundRepository();

	//void on_treeStructure_row_activated(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* column);
	//bool on_treeStructure_clicked(GdkEventButton* event);
	void on_my_cursor_changed();
	
	void on_check_resize(Gdk::Rectangle& rect);
	

	void on_tree_drag_data_get(
        const Glib::RefPtr<Gdk::DragContext>& dragcontext,
        Gtk::SelectionData& selection_data, guint info, guint time);



	
private:



};

#endif // _WX_CSOUND_REPOSITORY_H_
