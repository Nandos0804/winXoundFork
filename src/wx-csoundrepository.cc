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


#include <iostream>
#include <fstream>

#include "wx-csoundrepository.h"
#include "wx-global.h"




wxCSoundRepository::wxCSoundRepository()
{

	gint indexT = -1;
	gint indexI = -1;
	Glib::ustring itemText = "";
	Glib::ustring oldItemText = "";
	Glib::ustring itemTextInside = "";
	Glib::ustring oldItemTextInside = "";
	
	Gtk::TreeModel::Row parent;
	Gtk::TreeModel::Row inside;
	
	std::string line;
	//Glib::ustring file = wxGLOBAL->getSettingsPath();
	Glib::ustring file = wxGLOBAL->getSrcPath();
	file.append("/opcodes.txt");

	if(!Glib::file_test(file, Glib::FILE_TEST_EXISTS)) return;

	
	std::ifstream myfile (file.c_str());
	if (!myfile.is_open()) return;


	Glib::ustring name;
	Glib::ustring category;
	Glib::ustring description;
	Glib::ustring synopsis;

	Glib::ustring temp;

	
	//Create the Tree model:
	if(csRefTreeModel != NULL)
		csRefTreeModel.clear();	
	csRefTreeModel = Gtk::TreeStore::create(csColumns);

	
	if (myfile.is_open())
	{
		while (!myfile.eof() )
		{
			name="";
			category="";
			description="";
			synopsis="";

			//std::cout << line << std::endl;

			//mString = reader.ReadLine();			
			getline (myfile,line);
			//if (string.IsNullOrEmpty(mString)) continue;
			if(line == "") continue;


			gchar* t = strtok((gchar*)line.c_str(),";");
			if (t != NULL)
			{
				temp = t;
				name = temp.substr(1,temp.length()-2);
			}

			t = strtok (NULL, ";");
			if (t != NULL)
			{
				temp = t;
				category = temp.substr(1,temp.length()-2);
			}

			t = strtok (NULL, ";");
			if (t != NULL)
			{
				temp = t;
				description = temp.substr(1,temp.length()-2);
			}

			t = strtok (NULL, ";");
			if (t != NULL)
			{
				temp = t;
				synopsis = temp.substr(1,temp.length()-3);
			}

			
			//gchar* k = new gchar[key.size()+1];
			//strcpy (k, key.c_str());

			//gchar* v = new gchar[value.size()+1];
			//strcpy (v, value.c_str());

			/*DEBUG
			std::cout << "name: " << name << " - " 
					  << "category: " << category << " - " 
					  << "description: " << description << " - " 
				      << "synopsis: " << synopsis << std::endl; 
			*/

			//Fill treeViewOpcodes
			//itemText = split[1].Split(":".ToCharArray())[0];
			gchar** split = g_strsplit(category.c_str(), ":", 0);
			Glib::ustring itemText;
			if(wxGLOBAL->ArrayLength(split) <= 1)
				itemText = category;
			else
				itemText = split[0];

			
			//if (itemText.ToLower() == "utilities") continue;
			if(itemText.lowercase() == "utilities") continue;


			if (oldItemText != itemText)
			{
				//TreeNode t = treeViewOpcodes.Nodes.Add(itemText);
				parent = *(csRefTreeModel->append());
				parent[csColumns.name] = itemText;
				parent[csColumns.category] = "NODE";
				parent[csColumns.description] = "NODE";
				parent[csColumns.synopsis] = "NODE";
				//index = t.Index;
				indexT++;
				oldItemText = itemText;
			}

			//if (split[1].Split(":".ToCharArray()).Length > 1)
			if(wxGLOBAL->ArrayLength(split) > 1)
			{
				//string itemTextInside = split[1].Split(":".ToCharArray())[1];
				itemTextInside = split[1];

				gint i = 0;

				/*
				 Gtk::TreeModel::Row inside = *(MyRefTreeModel->append(rowOptions.children()));
				 childrow[MyColumns.key] = mFsKey.c_str();
				 childrow[MyColumns.value] = mFsValue.c_str();
				 childrow[MyColumns.image] = pixbufGreen;

				 //Retrieve row parent text (if any)
				 Gtk::TreeModel::Row rowParent = *row.parent();
				 if(rowParent)
				 {
					 rowParent.get_value(0, ParentName);
				 }
				*/
				
				//if (treeViewOpcodes.Nodes[index].Nodes[itemTextInside] == null)
				if(itemTextInside != oldItemTextInside)
				{
					//TreeNode inside = treeViewOpcodes.Nodes[index].Nodes.Add(itemTextInside);
					//inside.Name = itemTextInside;
					//i = inside.Index;
					inside = *(csRefTreeModel->append(parent.children()));
					inside[csColumns.name] = itemTextInside;
					inside[csColumns.category] = "NODE";
					inside[csColumns.description] = "NODE";
					inside[csColumns.synopsis] = "NODE";
					oldItemTextInside = itemTextInside;
					indexI++;
				}
				//else
					//i = treeViewOpcodes.Nodes[index].Nodes[itemTextInside].Index;
					

				//treeViewOpcodes.Nodes[index].Nodes[i].Nodes.Add(split[0]);
				Gtk::TreeModel::Row childrow = *(csRefTreeModel->append(inside.children()));
				childrow[csColumns.name] = name;
				childrow[csColumns.category] = category;
				childrow[csColumns.description] = description;
				childrow[csColumns.synopsis] = synopsis;

			}
			else
			{
				//treeViewOpcodes.Nodes[index].Nodes.Add(split[0]);
				Gtk::TreeModel::Row childrow = *(csRefTreeModel->append(parent.children()));
				childrow[csColumns.name] = name;
				childrow[csColumns.category] = category;
				childrow[csColumns.description] = description;
				childrow[csColumns.synopsis] = synopsis;
			}

	
		}
		myfile.close();
	}


	CreateNewCSoundRepository();


	//FILL TREEVIEW:

	//treeViewStructure->set_model(RefTreeModel);	
	treeStructure->set_model(csRefTreeModel);
	
	//Add the TreeView's view columns:
	treeStructure->remove_all_columns();
	treeStructure->append_column("Info: Drag and Drop to insert opcode name / "
	                             "Ctrl + Drag to insert opcode synopsis", 
	                             csColumns.name);
	//treeStructure->expand_all();


	//SELECT THE FIRST ROW
	Glib::RefPtr<Gtk::TreeSelection> sel = treeStructure->get_selection();
	Gtk::TreeModel::Row row = csRefTreeModel->children()[0]; //The fifth row.
	if (row) sel->select(row);


}

wxCSoundRepository::~wxCSoundRepository()
{
	wxGLOBAL->DebugPrint("wxCSoundRepository released");
}



void wxCSoundRepository::CreateNewCSoundRepository()
{
	
	Gtk::VBox* vbox = Gtk::manage(new Gtk::VBox());
	//vbox->pack_start(*new Gtk::Label("Info: Drag and Drop to insert opcode name / "
	//                                "Ctrl + Drag to insert opcode synopsis", 0.0, 0.50, false), 
	//                 FALSE, FALSE, 0);
	vbox->set_size_request(200,-1);
	

	//Add explorer pane:
	//TreeViewStructure
	treeStructure = Gtk::manage(new Gtk::TreeView());
	//treeStructure->set_can_focus(FALSE);
	mScrolledWindow = Gtk::manage(new Gtk::ScrolledWindow());
	mScrolledWindow->add(*treeStructure);
	//Only show the scrollbars when they are necessary:
	mScrolledWindow->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

	
	//SET SIGNALS
	treeStructure->signal_cursor_changed().connect(
		sigc::mem_fun(*this, &wxCSoundRepository::on_my_cursor_changed), false);

	//SET DRAG SOURCE
	std::list<Gtk::TargetEntry> listTargets;
	listTargets.push_back( Gtk::TargetEntry("STRING") );
	listTargets.push_back( Gtk::TargetEntry("text/plain") );
	//listTargets.push_back( Gtk::TargetEntry("text/uri-list") );
	
	treeStructure->drag_source_set(listTargets);

	treeStructure->signal_drag_data_get().connect(
		sigc::mem_fun(*this, &wxCSoundRepository::on_tree_drag_data_get),false);

	
	
	labelDescription = Gtk::manage(new Gtk::Label(" Description: ", 0.0, 0.50, false));
	labelSynopsis = Gtk::manage(new Gtk::Label(" Synopsis: ", 0.0, 0.50, false));
	vbox->pack_start(*labelDescription, FALSE, FALSE, 0);
	vbox->pack_start(*labelSynopsis, FALSE, FALSE, 0);
	
	
	vbox->pack_start(*mScrolledWindow, TRUE, TRUE, 0);

	
	//RESIZE, SHRINK
	this->add(*vbox);

	this->signal_size_allocate().connect(
		sigc::mem_fun(*this, &wxCSoundRepository::on_check_resize));


	
	show_all_children(); 
	show();

}


void wxCSoundRepository::on_check_resize(Gdk::Rectangle& rect)
{
	//std::cout << rect.get_height() << std::endl;

	if (rect.get_height() < 80)
	{

		//if(gtk_widget_get_visible(mScrolledWindow) == FALSE) return;
		if(mScrolledWindow->get_visible() == FALSE) return;

		mScrolledWindow->set_policy(Gtk::POLICY_NEVER,Gtk::POLICY_NEVER);
	}
	else
	{
		mScrolledWindow->set_policy(Gtk::POLICY_AUTOMATIC,Gtk::POLICY_ALWAYS);
	}
}


//bool wxCSoundRepository::on_treeStructure_clicked(GdkEventButton* event)
//void wxCSoundRepository::on_treeStructure_row_activated(const Gtk::TreeModel::Path& path, 
//                                                        Gtk::TreeViewColumn* column)
void wxCSoundRepository::on_my_cursor_changed()
{
	Glib::ustring name = "";
	Glib::ustring category = "";
	Glib::ustring description = "";
	Glib::ustring synopsis = "";

	
	//Gtk::TreePath path;
	//Gtk::TreeViewColumn col;
	//treeViewStructure->get_path_at_pos((int)event->x, (int)event->y, path);
	//if(!path)return false;
	
	//Gtk::TreeModel::iterator iter = treeStructure->get_model()->get_iter(path);
	Glib::RefPtr<Gtk::TreeSelection> sel = treeStructure->get_selection();
	Gtk::TreeModel::iterator iter = sel->get_selected();
	if(iter) //If anything is selected
	{
		//Retrieve row text
		Gtk::TreeModel::Row row = *iter;
		
		//Do something with the row.
		row.get_value(0, name);
		row.get_value(1, category);
		row.get_value(2, description);
		row.get_value(3, synopsis);

		/*
		std::cout << "name: " << name << " - " 
			<< "category: " << category << " - " 
			<< "description: " << description << " - " 
			<< "synopsis: " << synopsis << std::endl; 
		 */

		if(category != "NODE")
		{
			labelDescription->set_text(
				Glib::ustring::compose(" [%1] - %2",
				                       name, description));
			labelSynopsis->set_text(
			    Glib::ustring::compose(" %1", synopsis));
			
		}
		

	}

	return;
}






void wxCSoundRepository::on_tree_drag_data_get(
        const Glib::RefPtr<Gdk::DragContext>& dragcontext,
        Gtk::SelectionData& selection_data, guint info, guint time)
{
	Glib::ustring name = "";
	Glib::ustring category = "";
	Glib::ustring description = "";
	Glib::ustring synopsis = "";

	
	//Gtk::TreeModel::iterator iter = treeStructure->get_model()->get_iter(path);
	Glib::RefPtr<Gtk::TreeSelection> sel = treeStructure->get_selection();
	Gtk::TreeModel::iterator iter = sel->get_selected();
	if(iter) //If anything is selected
	{
		//Retrieve row text
		Gtk::TreeModel::Row row = *iter;
		
		//Do something with the row.
		row.get_value(0, name);
		row.get_value(1, category);
		row.get_value(2, description);
		row.get_value(3, synopsis);

		/*
		std::cout << "name: " << name << " - " 
			<< "category: " << category << " - " 
			<< "description: " << description << " - " 
			<< "synopsis: " << synopsis << std::endl; 
		 */

		if(category == "NODE")
		{
			//dragcontext->drag_refuse(time);
			//dragcontext->drag_finish(false, false, time);
			return;
		}
		
	}		 

	//guint modifiers = gtk_accelerator_get_default_mod_mask ();
	//std::cout << dragcontext->get_suggested_action() << std::endl;

	int x, y; 
	Gdk::ModifierType modifiers;


	//editor->textEditor->get_textView1_Widget()->get_window()->get_pointer(x, y, modifiers);
	dragcontext->get_source_window()->get_pointer(x, y, modifiers);




	const guchar* data;

	if(modifiers == Gdk::CONTROL_MASK)
		data = (const guchar*)synopsis.c_str();
	else 
		data = (const guchar*)name.c_str();
		
	
	selection_data.set(selection_data.get_target(), 
	                   8 /* 8 bits format */,
	                   data,
	                   strlen((const char*)data) /* the length of data in bytes */);
}

