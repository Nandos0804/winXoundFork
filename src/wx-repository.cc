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



#include "wx-repository.h"
#include "wx-global.h"
#include "wx-settings.h"


#define SSM(sci, m, w, l) scintilla_send_message(sci, m, w, l)


wxRepository::wxRepository()  	
{

	CreateNewRepository();

	UpdateUdoList();

	CreateTreePopupMenu();

	//Select Instructions Node
	Glib::RefPtr<Gtk::TreeSelection> sel = repStructure->get_selection();
	Gtk::TreeModel::Row row = rRefTreeModel->children()[0]->children()[0];
	if (row) sel->select(row);

	on_rep_cursor_changed();

}

wxRepository::~wxRepository()
{
	////NOT NEEDED: Already done by Gtk::manage !!!
	////g_object_unref(G_OBJECT(textView));
	////textView = NULL;
	
	wxGLOBAL->DebugPrint("wxRepository released");
}

void wxRepository::UpdateUdoList()
{
	Glib::ustring itemText = "";
	Glib::ustring oldItemText = "";
	
	Gtk::TreeModel::Row parent;

	
	//Create the Tree model:
	if(rRefTreeModel != NULL)
		rRefTreeModel.clear();	
	rRefTreeModel = Gtk::TreeStore::create(rColumns);
	
	Glib::ustring directory_path = wxGLOBAL->getRepositoryPath();
	
	Glib::Dir dir (directory_path);

	if (Glib::file_test(directory_path, Glib::FILE_TEST_IS_DIR) == FALSE) return;


	

	mOpcodes = "";

	std::vector<std::string> dirList;
	for(Glib::Dir::iterator p = dir.begin(); p != dir.end(); ++p)
	{
		//const std::string path_inside = *p;
		Glib::ustring temp = *p;
		Glib::ustring path_inside = Glib::ustring::compose("%1/%2",
		                                                   directory_path,
		                                                   temp);

		dirList.push_back(path_inside);
	}
	std::sort (dirList.begin(), dirList.end());

	for (uint index=0; index < dirList.size(); index++)
	{
		parent = *(rRefTreeModel->append());
		parent[rColumns.category] = Glib::path_get_basename(dirList[index]);
		parent[rColumns.name] = "NODE";

		std::vector<std::string> fileList;
		Glib::Dir dir_inside (dirList[index]);
		for(Glib::Dir::iterator i = dir_inside.begin(); i != dir_inside.end(); ++i)
		{
			const std::string filename = Glib::build_filename(dirList[index], *i);
			itemText = Glib::path_get_basename(filename);
			
			fileList.push_back(itemText);
		}			
		std::sort (fileList.begin(), fileList.end());

		for (uint index2=0; index2 < fileList.size(); index2++)
		{	
			Gtk::TreeModel::Row childrow = *(rRefTreeModel->append(parent.children()));
			childrow[rColumns.category] = fileList[index2];
			childrow[rColumns.name] = dirList[index];

			if(fileList[index2] != "Instructions.udo")
			{
				mOpcodes.append(wxGLOBAL->getFileNameWithoutExtension(fileList[index2]));
				mOpcodes.append(" ");
			}
			
		}
		
	}



	
	

	//Glib::RefPtr<Gtk::TreeModelSort> sorted_model =
	//		Gtk::TreeModelSort::create(rRefTreeModel);
	//sorted_model->set_sort_column(rColumns.category, Gtk::SORT_ASCENDING);
	//repStructure->set_model(sorted_model);
	
	
	//FILL TREEVIEW:
	repStructure->set_model(rRefTreeModel);
	//Add the TreeView's view columns:
	repStructure->remove_all_columns();
	repStructure->append_column("Repository", rColumns.category);
	repStructure->expand_all();
	
}

Glib::ustring wxRepository::GetUdoOpcodesList()
{
	return mOpcodes;
}

void wxRepository::CreateNewRepository()
{

	Gtk::VBox* mainframe = Gtk::manage(new Gtk::VBox());
	
	Gtk::HBox* hbox = Gtk::manage(new Gtk::HBox());
	hbox->set_size_request(200,200); //y=-1
	hbox->set_border_width(1);
	

	//Add explorer pane:
	//TreeViewStructure
	repStructure = Gtk::manage(new Gtk::TreeView());
	//repStructure->set_can_focus(FALSE);
	mScrolledWindow = Gtk::manage(new Gtk::ScrolledWindow());
	mScrolledWindow->add(*repStructure);
	//Only show the scrollbars when they are necessary:
	mScrolledWindow->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

	
	//SET SIGNALS
	repStructure->signal_cursor_changed().connect(
		sigc::mem_fun(*this, &wxRepository::on_rep_cursor_changed), false);

	//SET DRAG SOURCE
	std::list<Gtk::TargetEntry> listTargets;
	listTargets.push_back( Gtk::TargetEntry("STRING") );
	listTargets.push_back( Gtk::TargetEntry("text/plain") );
	//listTargets.push_back( Gtk::TargetEntry("text/uri-list") );
	
	repStructure->drag_source_set(listTargets);
	repStructure->drag_dest_set(listTargets);

	repStructure->signal_drag_data_get().connect(
		sigc::mem_fun(*this, &wxRepository::on_rep_drag_data_get),false);

	repStructure->signal_button_press_event().connect(
		sigc::mem_fun(*this, &wxRepository::on_tree_press_event),false);

	repStructure->signal_drag_data_received().connect(sigc::mem_fun(*this,
              &wxRepository::on_drop_data_received) );

	//repStructure->enable_model_drag_source (listTargets,
	//                                         Gdk::CONTROL_MASK,
	//										 Gdk::ACTION_COPY);
	

	mScrolledWindow->set_size_request(250,-1);
	hbox->pack_start(*mScrolledWindow, FALSE, FALSE, 0);

	

	//TEXTVIEW
	textView = SCINTILLA(scintilla_new());
   	scintilla_set_id(textView, 0);
   	gtk_widget_set_usize(GTK_WIDGET(textView), 300, 100);
	//SET CODEPAGE TO UTF8
	SSM(textView, SCI_SETCODEPAGE, SC_CP_UTF8, 0);
	textView_Widget = Glib::wrap(GTK_WIDGET(textView));
	//Various Scintilla Settings:
	//[self setTextEditorFont: [NSFont fontWithName:@"Andale Mono" size: 12]];
	SSM(textView, SCI_STYLESETFONT, 32, (sptr_t)"!Monospace");
	SSM(textView, SCI_STYLESETSIZE, 32, 10);	
	//Assign margin size for Line Numbers and other margins
	SSM(textView, SCI_SETMARGINWIDTHN, 0, 0);
	SSM(textView, SCI_SETMARGINWIDTHN, 1, 0); 
	SSM(textView, SCI_SETMARGINWIDTHN, 2, 0);
	//SCI_SetAdditionalCaretsVisible = 0 (false)
	SSM(textView, SCI_SETVIRTUALSPACEOPTIONS, 1, 0);
	SSM(textView, SCI_SETADDITIONALCARETSVISIBLE, 0, 0);	
	//Disable popup menu
	SSM(textView, SCI_USEPOPUP, 0, 0);

	Gtk::VBox* temp = Gtk::manage(new Gtk::VBox());
	buttonSave = Gtk::manage(new Gtk::Button("Save changes ..."));
	buttonSave->signal_clicked().connect(
		sigc::mem_fun(*this, &wxRepository::on_button_save_Clicked));
	temp->set_border_width(2);
	temp->pack_start(*buttonSave, FALSE, FALSE, 0);
	temp->pack_start(*textView_Widget, TRUE, TRUE, 0);
	hbox->pack_start(*temp, TRUE, TRUE, 0);


	g_signal_connect(textView, 
	                 SCINTILLA_NOTIFY, 
	                 GtkSignalFunc(&wxRepository::on_SCI_NOTIFY),
	                 this);




	//RESIZE, SHRINK
	mainframe->add(*hbox);
	//this->add(*hbox);
	this->add(*mainframe);

	this->signal_size_allocate().connect(
		sigc::mem_fun(*this, &wxRepository::on_rep_check_resize));



	//SHOW WIDGETS
	show_all_children(); 
	show();

	buttonSave->hide();

}


void wxRepository::on_SCI_NOTIFY(GtkWidget *widget, gint wParam, gpointer lParam, gpointer data)
{
	//wxTextEditor* _textEditor = reinterpret_cast<wxTextEditor*>(widget);
	ScintillaObject* _textView = reinterpret_cast<ScintillaObject*>(widget);

	wxRepository* _this = reinterpret_cast<wxRepository*>(data);
	SCNotification *nt = reinterpret_cast<SCNotification*>(lParam);

	//std::cerr << "TextEditor:" << tempEditor << std::endl;

	switch (nt->nmhdr.code) //  nmhdr.code)
	{
		case SCN_SAVEPOINTLEFT:
		{
			_this->SavePointLeft();
			break;
		}
		case SCN_SAVEPOINTREACHED:
		{
			_this->SavePointReached();
			break;
		}
		case SCN_MODIFYATTEMPTRO:
		{
			break;
		}
		case SCN_MARGINCLICK:
			break;

		case SCN_UPDATEUI:
			break;

 		case SCN_MODIFIED:
		{	
			/*
			if(nt->modificationType & (SC_MOD_INSERTTEXT | SC_MOD_DELETETEXT))
			{
				if(_textView == _this->textEditor->getPrimaryView())
				{
					//_this->on_SCITextEditor_TextChanged();
					_this->Modified(_this);
					//std::cerr << "SCN_MODIFIED" << std::endl;
				}
			}
			*/
			break;
		}
		case SCN_CHARADDED:
			break;

		case SCN_USERLISTSELECTION:
		{
			break;
		}
		case SCN_AUTOCSELECTION:
			break;
		case SCN_AUTOCCANCELLED:
		{
			break;
		}
		case SCN_NEEDSHOWN:
		{
			break;
		}
		case SCN_URIDROPPED:
		{
			break;
		}
		case SCN_CALLTIPCLICK:
		{
			break;
		}
		case SCN_ZOOM:
		{
			break;
		}
			
	}

}

void wxRepository::SavePointLeft()
{
	buttonSave->show();
}

void wxRepository::SavePointReached()
{
	buttonSave->hide();
}

void wxRepository::on_button_save_Clicked()
{
	Glib::ustring name = "";
	Glib::ustring category = "";

	
	Glib::RefPtr<Gtk::TreeSelection> sel = repStructure->get_selection();
	Gtk::TreeModel::iterator iter = sel->get_selected();
	if(iter) //If anything is selected
	{
		//Retrieve row text
		Gtk::TreeModel::Row row = *iter;
		
		//Do something with the row.
		row.get_value(0, category);
		row.get_value(1, name);
	}		 

	Glib::ustring filename = Glib::ustring::compose("%1/%2",
	                                                name,
	                                                category);


	if(Glib::file_test(filename, Glib::FILE_TEST_EXISTS))
	{
		gint len = SSM(textView, SCI_GETLENGTH, 0, 0);
		gchar* text = new gchar[len + 1];
		SSM(textView, SCI_GETTEXT, len + 1, (sptr_t) text);

		Glib::file_set_contents(filename, text);

		delete[] text;
	}
}



void wxRepository::on_rep_check_resize(Gdk::Rectangle& rect)
{
	//std::cout << rect.get_height() << std::endl;

	if (rect.get_height() < 63)
	{
		if(mScrolledWindow->get_visible() == FALSE) return;

		mScrolledWindow->set_policy(Gtk::POLICY_NEVER,Gtk::POLICY_NEVER);
		//SSM(textView, SCI_SETHSCROLLBAR, 0, 0);
		SSM(textView, SCI_SETVSCROLLBAR, 0, 0); 
	}
	else
	{
		mScrolledWindow->set_policy(Gtk::POLICY_AUTOMATIC,Gtk::POLICY_ALWAYS);
		//SSM(textView, SCI_SETHSCROLLBAR, 1, 0);
		SSM(textView, SCI_SETVSCROLLBAR, 1, 0); 
		
	}
}


void wxRepository::on_rep_cursor_changed()
{
	Glib::ustring category = "";
	Glib::ustring name = "";
	
	Glib::RefPtr<Gtk::TreeSelection> sel = repStructure->get_selection();
	Gtk::TreeModel::iterator iter = sel->get_selected();
	if(iter) //If anything is selected
	{
		//Retrieve row text
		Gtk::TreeModel::Row row = *iter;
		
		//Do something with the row.
		row.get_value(0, category);
		row.get_value(1, name);
	
	}


	if(name == "NODE" || name == "") return;
	
	Glib::ustring filename = name;
	filename.append("/");
	filename.append(category);
	//std::cout << "Filename: " << filename << std::endl;


	if(!Glib::file_test(filename, Glib::FILE_TEST_EXISTS)) return;


	Glib::ustring contents = Glib::file_get_contents(filename);

	//if (contents != "") 
	{
		SSM(textView, SCI_SETTEXT, 0, (sptr_t) contents.c_str());
		SSM(textView, SCI_EMPTYUNDOBUFFER, 0, 0);
		SSM(textView, SCI_SETSAVEPOINT, 0, 0);

		if(category == "Instructions.udo")
			SSM(textView, SCI_SETLEXERLANGUAGE, 0, (sptr_t)"none");
		else
			SSM(textView, SCI_SETLEXERLANGUAGE, 0, (sptr_t)"winxound");
		//SSM(textView, SCI_COLOURISE, 0, -1);
	}

}










void wxRepository::ConfigureEditor(GHashTable* Opcodes)
{
	try
	{

		if (Opcodes != NULL)
		{
			//Set Keywords list
			//Glib::ustring KeyWordList = "";
			std::string KeyWordList = "";

			//foreach (DictionaryEntry de in Opcodes)
			GHashTableIter iter;
			gpointer key, value;
			g_hash_table_iter_init (&iter, Opcodes);
			while (g_hash_table_iter_next (&iter, &key, &value)) 
			{
				/* do something with key and value */
				//if (de.Key.ToString() != "instr" && de.Key.ToString() != "endin")
				if(!g_str_equal(key, "instr") &&
				   !g_str_equal(key, "endin"))
				{
					KeyWordList.append((gchar*)key);
					KeyWordList.append(" ");
				}
			}

			//this->textEditor->setKeyWords(0, KeyWordList.c_str());
			SSM(textView, SCI_SETKEYWORDS, 0, (sptr_t)KeyWordList.c_str());

			std::string KeyWordList2 = 
				"<CsVersion> </CsVersion> "
				"<CsoundSynthesizer> </CsoundSynthesizer> "
				"<CsOptions> </CsOptions> "
				"<CsInstruments> </CsInstruments> "
				"<CsScore> </CsScore> "
				"<CsVersion> </CsVersion> "
				"<CsLicence> </CsLicence> ";
			
			std::string KeyWordList3 = " instr endin ";

			SSM(textView, SCI_SETKEYWORDS, 1, (sptr_t)KeyWordList2.c_str());
			SSM(textView, SCI_SETKEYWORDS, 3, (sptr_t)KeyWordList3.c_str());
			
		}

		//this->textEditor->setWordChars("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.$_");
		Glib::ustring chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.$_";
		SSM(textView, SCI_SETWORDCHARS, 0, (sptr_t)chars.c_str());

		SciEditSetFontsAndStyles();

	}
	catch (...)
	{
		wxGLOBAL->DebugPrint("wxRepository::ConfigureEditor Error");
	}
}


void wxRepository::SciEditSetFontsAndStyles()
{

	//Set Default Font (name and size) to styles 0 -> 32
	Glib::ustring fontN = wxSETTINGS->EditorProperties.DefaultFontName;
	fontN.insert(0, "!");
	for (gint f = 0; f < 34; f++)
	{
		SSM(textView, SCI_STYLESETFONT, f, (sptr_t)fontN.c_str());
		SSM(textView, SCI_STYLESETSIZE, f, 10);
	}
	

	//textEditor->setZoom(0);
	SSM(textView, SCI_SETZOOM, 0, 0);

	
	//textEditor->setTabIndent(wxSETTINGS->EditorProperties.DefaultTabSize);
	SSM(textView, SCI_SETTABWIDTH, wxSETTINGS->EditorProperties.DefaultTabSize, 0);


	
	//Set styles from 0 to 33
	for (gint i = 0; i < 34; i++)
	{
		//const gchar* temp = wxSETTINGS->StyleGetForeColor(mLanguage, i).c_str();
		SSM(textView, SCI_STYLESETFORE, i, getColorFromString(wxSETTINGS->StyleGetForeColor("csound", i).c_str()));
		SSM(textView, SCI_STYLESETBACK, i, getColorFromString(wxSETTINGS->StyleGetBackColor("csound", i).c_str()));
		SSM(textView, SCI_STYLESETBOLD, i, wxSETTINGS->StyleGetBold("csound", i));
		SSM(textView, SCI_STYLESETITALIC, i, wxSETTINGS->StyleGetItalic("csound", i));
	}
	
	
	//TEXT SELECTION (style 256)
	//textEditor->setSelFore(true, wxSETTINGS->StyleGetForeColor(mLanguage, 256).c_str());
	SSM(textView, SCI_SETSELFORE, true, getColorFromString(wxSETTINGS->StyleGetForeColor("csound", 256).c_str()));


	//textEditor->setSelBack(true,wxSETTINGS->StyleGetBackColor(mLanguage, 256).c_str());
	SSM(textView, SCI_SETSELBACK, true, getColorFromString(wxSETTINGS->StyleGetBackColor("csound", 256).c_str()));



	//CARET COLOR (Same as Text Fore Color)
	//mSciEdit.SetCaretFore(mSciEdit.StyleGetFore(32));
	//textEditor->setCaretFore(wxSETTINGS->StyleGetForeColor(mLanguage, 32).c_str());
	SSM(textView, SCI_SETCARETFORE, getColorFromString(wxSETTINGS->StyleGetForeColor("csound", 32).c_str()), 0);
	
}


gint wxRepository::getColorFromString(const gchar* stringColor)
{
	GdkColor color;

	//color range = 65535;
	gdk_color_parse (stringColor, &color);

	int r = color.red * 255 / 65535;
	int g = color.green * 255 / 65535;
	int b = color.blue * 255 / 65535;
	
	return r | (g << 8) | (b << 16);
	
    //std::stringstream ss(color);
    //int i;
    //ss >> std::hex >> i;
    //return i;
}



void wxRepository::Delete()
{

	Glib::ustring category;
	Glib::ustring name;

	gint indexParent = 0;
	gint index = 0;
	
	Glib::RefPtr<Gtk::TreeSelection> sel = repStructure->get_selection();
	Gtk::TreeModel::iterator iter = sel->get_selected();
	if(iter) //If anything is selected
	{
		//Retrieve row text
		Gtk::TreeModel::Row row = *iter;
		
		//Do something with the row.
		row.get_value(0, category);
		row.get_value(1, name);

		
		//Get index
		Gtk::TreePath path = repStructure->get_model()->get_path(iter);
		//std::vector<int> indexes = path.get_indices();
		//index = indexes[0];
		if(path != NULL)
		{
			if(path.size() > 1)
			{
				indexParent = path[0];
				index = path[1] - 1;
				if(index < 0) index = 0;
			}
		}
	}




	Glib::ustring filename = Glib::ustring::compose("%1/%2",
	                                                name,
	                                                category);


	if(Glib::file_test(filename, Glib::FILE_TEST_EXISTS))
	{
		gint ret = wxGLOBAL->ShowMessageBox("Are you sure?",
		                                    "Delete UDO",
		                                    Gtk::BUTTONS_YES_NO);
		//if (resp == DialogResult.No)
		if(ret != Gtk::RESPONSE_YES) return;

		//unlink???
		std::remove(filename.c_str());
		UpdateUdoList();

		Gtk::TreeModel::Row r = rRefTreeModel->children()[indexParent]->children()[index];
		if (r) sel->select(r);

		on_rep_cursor_changed();
		
	}
	
}

void wxRepository::Rename()
{

	Glib::ustring category;
	Glib::ustring name;
	gint index = 0;
	gint indexParent = 0;
	
	Glib::RefPtr<Gtk::TreeSelection> sel = repStructure->get_selection();
	Gtk::TreeModel::iterator iter = sel->get_selected();
	
	if(iter) //If anything is selected
	{
		//Retrieve row text
		Gtk::TreeModel::Row row = *iter;
		
		//Do something with the row.
		row.get_value(0, category);
		row.get_value(1, name);

		//Get index
		Gtk::TreePath path = repStructure->get_model()->get_path(iter);
		//std::vector<int> indexes = path.get_indices();
		//index = indexes[0];
		if(path != NULL)
		{
			if(path.size() > 1)
			{
				indexParent = path[0];
				index = path[1];
			}
		}

	}

	Glib::ustring filename = Glib::ustring::compose("%1/%2",
	                                                name,
	                                                category);


	if(Glib::file_test(filename, Glib::FILE_TEST_EXISTS))
	{
		Gtk::Dialog dialog("Rename Udo as ...", TRUE, FALSE);

		Gtk::VBox* vbox = dialog.get_vbox();


		//FILENAME
		Gtk::Label* labelFilename = Gtk::manage(new Gtk::Label("New name:"));
		Gtk::Entry* entryFilename = Gtk::manage(new Gtk::Entry());
		vbox->pack_start(*labelFilename, FALSE, FALSE, 0);
		vbox->pack_start(*entryFilename, FALSE, FALSE, 0);


		vbox->set_size_request(300,-1);

		dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
		dialog.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);


		dialog.show_all_children();
		dialog.set_resizable(FALSE);
		dialog.set_icon_from_file(Glib::ustring::compose("%1/winxound_48.png",wxGLOBAL->getIconsPath()));
		dialog.show_all();

		/*
		 //SET DIALOG POSITION (CENTER PARENT)
		 gint x,y;
		 wxMainWindow->get_position(x, y);
		 x += wxMainWindow->get_width() / 2;
		 y += wxMainWindow->get_height() / 2;
		 dialog.move(x - dialog.get_width() / 2,
		 y - dialog.get_height() / 2);
		 */


		int ret = dialog.run();

		//if (resp == DialogResult.No)
		if(ret != Gtk::RESPONSE_OK) return;

		Glib::ustring Newfilename = Glib::ustring::compose("%1/%2.udo",
		                                                   name,
		                                                   entryFilename->get_text());

		//unlink???
		//std::cout << "Oldname: \n" << filename <<
		//	         "Newname: \n" << Newfilename << std::endl;

		//int rename ( const char * oldname, const char * newname )
		std::rename(filename.c_str(), Newfilename.c_str());
		UpdateUdoList();

		
		Gtk::TreeModel::Row r = rRefTreeModel->children()[indexParent]->children()[index];
		if (r) sel->select(r);
	}


	
}


void wxRepository::CreateTreePopupMenu()
{
	/*
	Gtk::Menu		treePopupMenu;
	Gtk::MenuItem   treeDelete;
	Gtk::MenuItem   treeRemove;
	*/

	Gtk::Menu::MenuList& menulist = treePopupMenu.items();

	//RENAME
	menulist.push_back(Gtk::Menu_Helpers::MenuElem("Rename",
		sigc::mem_fun(*this, &wxRepository::Rename)));

	//SEPARATOR
	menulist.push_back(Gtk::Menu_Helpers::SeparatorElem());

	//DELETE
	menulist.push_back(Gtk::Menu_Helpers::MenuElem("Delete",
		sigc::mem_fun(*this, &wxRepository::Delete)));

	treePopupMenu.show_all();

	//treePopupMenu.accelerate(*repStructure);

	
}

bool wxRepository::on_tree_press_event(GdkEventButton* event)
{
  //Call base class, to allow normal handling,
  //such as allowing the row to be selected by the right-click:
  //bool return_value = Gtk::TreeView::on_button_press_event(event);

  //Then do our custom stuff:
  if( (event->type == GDK_BUTTON_PRESS) && (event->button == 3) )
  {
    treePopupMenu.popup(event->button, event->time);
  }

  return false;
}


void wxRepository::on_rep_drag_data_get(
        const Glib::RefPtr<Gdk::DragContext>& dragcontext,
        Gtk::SelectionData& selection_data, guint info, guint time)
{

	gint len = SSM(textView, SCI_GETLENGTH, 0, 0);
	gchar* contents = new gchar[len + 1];
	SSM(textView, SCI_GETTEXT, len + 1, (sptr_t) contents);

	Glib::ustring text = contents;
	oldDrag = text;
	
	
	selection_data.set(selection_data.get_target(), 
	                   8 /* 8 bits format */,
	                   (const guchar*)text.c_str(),
	                   strlen((const char*)text.c_str()) /* the length of data in bytes */);

	delete[] contents;
}



void wxRepository::on_drop_data_received(const Glib::RefPtr<Gdk::DragContext>& context, int, int,
                                         const Gtk::SelectionData& selection_data, guint, guint time)
{
	const int length = selection_data.get_length();
	if((length >= 0) && (selection_data.get_format() == 8))
	{
		//std::cout << "Received: \n" << selection_data.get_data_as_string()
		//	<< std::endl;

		Glib::ustring data = selection_data.get_data_as_string();

		if(oldDrag != data)
		{
			InsertText(data);
		}
	}

	context->drag_finish(false, false, time);

}


void wxRepository::InsertText(Glib::ustring& text)
{

	Gtk::Dialog dialog("Save Code as ...", TRUE, FALSE);

	Gtk::VBox* vbox = dialog.get_vbox();


	//FILENAME
	Gtk::Label* labelFilename = Gtk::manage(new Gtk::Label("Filename:"));
	Gtk::Entry* entryFilename = Gtk::manage(new Gtk::Entry());
	vbox->pack_start(*labelFilename, FALSE, FALSE, 0);
	vbox->pack_start(*entryFilename, FALSE, FALSE, 0);


	//NODES
	Gtk::ListViewText* listboxNodes = Gtk::manage(new Gtk::ListViewText(1, false, Gtk::SELECTION_SINGLE));
	//listboxNodes->set_can_focus(FALSE);
	listboxNodes->set_column_title(0,"Nodes");
	Gtk::ScrolledWindow* mScrolledWindow = Gtk::manage(new Gtk::ScrolledWindow());
	mScrolledWindow->add(*listboxNodes);
	//Only show the scrollbars when they are necessary:
	mScrolledWindow->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	Glib::ustring directory_path = wxGLOBAL->getRepositoryPath();	
	Glib::Dir dir (directory_path);

	//Sort dirs
	std::vector<std::string> dirList;
	for(Glib::Dir::iterator p = dir.begin(); p != dir.end(); ++p)
	{
		//const std::string path_inside = *p;
		Glib::ustring temp = *p;
		dirList.push_back(temp);
	}
	std::sort (dirList.begin(), dirList.end());

	for (uint index=0; index < dirList.size(); index++)
	{
		listboxNodes->append_text(dirList[index]);
	}

	//Select First Node
	Glib::RefPtr<Gtk::TreeSelection> sel = listboxNodes->get_selection();
	Gtk::TreeModel::Row row = listboxNodes->get_model()->children()[0];
	if (row) sel->select(row);


	vbox->pack_start(*mScrolledWindow, TRUE, TRUE, 0);
	vbox->set_size_request(400,300);

	dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	dialog.add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_OK);


	dialog.show_all_children();
	dialog.set_resizable(FALSE);
	dialog.set_icon_from_file(Glib::ustring::compose("%1/winxound_48.png",wxGLOBAL->getIconsPath()));
	dialog.show_all();

	/*
	//SET DIALOG POSITION (CENTER PARENT)
	gint x,y;
	wxMainWindow->get_position(x, y);
	x += wxMainWindow->get_width() / 2;
	y += wxMainWindow->get_height() / 2;
	dialog.move(x - dialog.get_width() / 2,
	            y - dialog.get_height() / 2);
	*/


	int result = dialog.run();

	if(result == Gtk::RESPONSE_OK)
	{
		//Save code as filename
		Glib::RefPtr<Gtk::TreeSelection> sel = listboxNodes->get_selection();
		Gtk::TreeModel::iterator iter = sel->get_selected();
		Glib::ustring nodename = "";
		if(iter) //If anything is selected
		{
			//Retrieve row text
			Gtk::TreeModel::Row row = *iter;

			//Do something with the row.
			row.get_value(0, nodename);
		}

		if(nodename == "") return;
		
		Glib::ustring path_inside = Glib::ustring::compose("%1/%2",
		                                                   directory_path,
		                                                   nodename);
		path_inside.append("/");
		path_inside.append(entryFilename->get_text());
		path_inside.append(".udo");

		//Glib::ustring output = text;


		if(Glib::file_test(path_inside, Glib::FILE_TEST_EXISTS))
		{
			gint ret = wxGLOBAL->ShowMessageBox("Udo code already exists. Do you want to continue?",
			                                    "File already exists",
			                                    Gtk::BUTTONS_YES_NO);
			//if (resp == DialogResult.No)
			if(ret != Gtk::RESPONSE_YES) return;
		}


		Glib::file_set_contents(path_inside, text);

		UpdateUdoList();


		//ITERATE THROUGH ALL TREE ELEMENTS
		Glib::ustring udoname = Glib::path_get_basename(path_inside);
		Glib::ustring category = "";
		Glib::ustring name = "";
		bool found = false;
		for (Gtk::TreeIter iter = repStructure->get_model()->children().begin(); 
		     iter != repStructure->get_model()->children().end();
		     ++iter) //iter++)
		{

			//Retrieve row text
			Gtk::TreeModel::Row row = *iter;

			row.get_value(0, category);
			row.get_value(1, name);

			std::cout << "Nodename: " << nodename <<
				         " - Category: " << category << 
						 " - Name: " << name << std::endl;

			if(category == nodename)
			{
				//Iter through children
				for (Gtk::TreeIter iterChild = row.children().begin(); 
				     iterChild != row.children().end();
				     ++iterChild) //iter++)
				{
					//Retrieve row text
					Gtk::TreeModel::Row rowChild = *iterChild;

					rowChild.get_value(0, category);
					rowChild.get_value(1, name);

					std::cout << "UDOname: " << udoname <<
				         " - Category: " << category << 
						 " - Name: " << name << std::endl;

					if(category == udoname)
					{
						repStructure->get_selection()->select(rowChild);
						found = true;
						break;
					}
				}
			}	
			if(found) break;
		}

		on_rep_cursor_changed();
		
	}	

	
}

	