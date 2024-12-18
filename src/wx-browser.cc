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

#include "wx-browser.h"
#include "wx-settings.h"
#include "wx-global.h"

#define DEFAULT_ZOOM_VALUE 1.0


wxBrowser::wxBrowser(bool history)  	
{
	History = history;
	this->CreateNewBrowser();
	//std::cerr << "wxTextEditor Created" << std::endl;
}
wxBrowser::~wxBrowser(void)
{
	gtk_widget_destroy(GTK_WIDGET(web_view));
	gtk_widget_destroy(frame);
	
	wxGLOBAL->DebugPrint("wxBrowser released");
}

void wxBrowser::CreateNewBrowser()
{

	zoomValue = DEFAULT_ZOOM_VALUE;
	
	web_view = WEBKIT_WEB_VIEW (webkit_web_view_new ());
	if(History)
		webkit_web_view_set_maintains_back_forward_list(web_view, true);

	
	scrolled_window = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window), 
                                    GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_container_add (GTK_CONTAINER (scrolled_window), GTK_WIDGET (web_view));
	gtk_widget_set_size_request(GTK_WIDGET(web_view), 10, 10); //IMPORTANT
	//gtk_widget_set_has_tooltip(GTK_WIDGET(web_view), FALSE);
	
	
	vbox = gtk_vbox_new(FALSE, 1);

	if(History)
	{
		GtkWidget* hbox = gtk_hbox_new(FALSE, 1);
		buttonBackward = gtk_button_new_with_label("<<"); //gtk_button_new_from_stock(GTK_STOCK_GO_BACK);
		buttonForward = gtk_button_new_with_label(">>"); //gtk_button_new_from_stock(GTK_STOCK_GO_FORWARD);
		GtkWidget* buttonHome = gtk_button_new_with_label("Home"); //gtk_button_new_from_stock(GTK_STOCK_HOME);
		//gtk_button_set_relief(GTK_BUTTON(buttonHome) ,GTK_RELIEF_NONE);
		entryFind = gtk_entry_new();
		//GtkWidget* labelEsc = gtk_label_new("(Press 'Esc' to return to the CodeView)");
		//gtk_label_set_justify(GTK_LABEL(labelEsc), GTK_JUSTIFY_RIGHT);

		gtk_box_pack_start(GTK_BOX(hbox), buttonBackward, FALSE, FALSE, 0);
		gtk_box_pack_start(GTK_BOX(hbox), buttonForward, FALSE, FALSE, 0);
		gtk_box_pack_start(GTK_BOX(hbox), buttonHome, FALSE, FALSE, 0);
		gtk_box_pack_start(GTK_BOX(hbox), gtk_label_new("   "), FALSE, FALSE, 0);
		gtk_box_pack_start(GTK_BOX(hbox), gtk_label_new("Find:"), FALSE, FALSE, 10);
		gtk_box_pack_start(GTK_BOX(hbox), entryFind, FALSE, FALSE, 0);
		gtk_box_pack_start(GTK_BOX(hbox), gtk_label_new("   "), FALSE, FALSE, 0);

		//Zoom buttons:
		GtkWidget* buttonMinus = gtk_button_new_with_label("<");
		GtkWidget* buttonPlus = gtk_button_new_with_label(">");
		GtkWidget* buttonZoomReset = gtk_button_new_with_label("Reset");
		gtk_box_pack_start(GTK_BOX(hbox), gtk_label_new("Zoom:"), FALSE, FALSE, 10);
		gtk_box_pack_start(GTK_BOX(hbox), buttonMinus, FALSE, FALSE, 0);
		gtk_box_pack_start(GTK_BOX(hbox), buttonPlus, FALSE, FALSE, 0);
		gtk_box_pack_start(GTK_BOX(hbox), buttonZoomReset, FALSE, FALSE, 0);

		//Label Escape:
		gtk_box_pack_start(GTK_BOX(hbox), gtk_label_new(""), TRUE, TRUE, 0);
		//gtk_box_pack_start(GTK_BOX(hbox), labelEsc, FALSE, FALSE, 15);
		gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

		g_signal_connect(entryFind, 
		                 "activate",
		                 G_CALLBACK(&wxBrowser::on_entry_activate),
		                 this);

		g_signal_connect(entryFind, 
		                 "changed",
		                 G_CALLBACK(&wxBrowser::on_entry_activate),//G_CALLBACK(&wxBrowser::on_insert_at_cursor),
		                 this);

		g_signal_connect(buttonBackward, 
		                 "clicked",//"cursor-moved",//"contents-changed", //"child-exited",
		                 G_CALLBACK(&wxBrowser::on_buttonBackward_clicked),
		                 this);

		g_signal_connect(buttonForward, 
		                 "clicked",//"cursor-moved",//"contents-changed", //"child-exited",
		                 G_CALLBACK(&wxBrowser::on_buttonForward_clicked),
		                 this);

		g_signal_connect(buttonHome, 
		                 "clicked",//"cursor-moved",//"contents-changed", //"child-exited",
		                 G_CALLBACK(&wxBrowser::on_buttonHome_clicked),
		                 this);

		g_signal_connect(buttonMinus,
		                 "clicked",//"cursor-moved",//"contents-changed", //"child-exited",
		                 G_CALLBACK(&wxBrowser::on_buttonZoomMinus_clicked),
		                 this);

		g_signal_connect(buttonPlus, 
		                 "clicked",//"cursor-moved",//"contents-changed", //"child-exited",
		                 G_CALLBACK(&wxBrowser::on_buttonZoomPlus_clicked),
		                 this);		
		
		g_signal_connect(buttonZoomReset, 
		                 "clicked",//"cursor-moved",//"contents-changed", //"child-exited",
		                 G_CALLBACK(&wxBrowser::on_buttonZoomReset_clicked),
		                 this);	
		
	}
	
	gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);

	
	frame = gtk_event_box_new();
	//gtk_widget_set_events(frame, GDK_ALL_EVENTS_MASK);
	gtk_container_add(GTK_CONTAINER(frame), vbox);


	
	//gtk_container_add (GTK_CONTAINER (vbox), scrolled_window);
	//gtk_container_add (GTK_CONTAINER (hbox), scrolled_window);


	//gtk_widget_show_all(scrolled_window);
	gtk_widget_show_all(frame);

	//wid = Glib::wrap(scrolled_window);
	wid = Glib::wrap(frame);

	//CONNECT_SIGNALS
	//didFinishLoadForFrame
	//load-finished
	
	g_signal_connect(web_view, 
	                 "notify::load-status",
	                 G_CALLBACK(&wxBrowser::on_load_status),
	                 this);

	g_signal_connect(frame, 
	                 "size-allocate",//"size-request", //"check-resize",
	                 G_CALLBACK(&wxBrowser::on_check_resize_browser),
	                 this);

	SetBrowserZoom(zoomValue);
	
}


void wxBrowser::on_buttonZoomMinus_clicked(GtkWidget *widget, gpointer data)
{
	wxBrowser* _this = reinterpret_cast<wxBrowser*>(data);
	_this->zoomOut();
}
void wxBrowser::zoomOut()
{
	zoomValue -= 0.1;
	if(zoomValue < 0.5) zoomValue = 0.5;
	
	webkit_web_view_set_zoom_level(web_view, zoomValue);
}

void wxBrowser::on_buttonZoomPlus_clicked(GtkWidget *widget, gpointer data)
{
	wxBrowser* _this = reinterpret_cast<wxBrowser*>(data);
	_this->zoomIn();
}
void wxBrowser::zoomIn()
{
	zoomValue += 0.1;
	if(zoomValue > 2) zoomValue = 2;

	webkit_web_view_set_zoom_level(web_view, zoomValue);
}

void wxBrowser::on_buttonZoomReset_clicked(GtkWidget *widget, gpointer data)
{
	wxBrowser* _this = reinterpret_cast<wxBrowser*>(data);
	_this->SetBrowserZoom(DEFAULT_ZOOM_VALUE);
}


void  wxBrowser::on_check_resize_browser (GtkWidget      *widget,
                                  GtkRequisition *requisition,
                                  gpointer        data) 
{

	wxBrowser* _this = reinterpret_cast<wxBrowser*>(data);
	_this->check_resize();
}
void wxBrowser::check_resize()
{

	GdkRectangle pippo;
	gtk_widget_get_allocation(vbox, &pippo);
	//wxGLOBAL->DebugPrint("WXBROWSER on_check_resize", wxGLOBAL->IntToString(pippo.height).c_str()); 


	if (pippo.height < 74)
	{
		//if(scrolled_window == NULL) return;
		//if(gtk_widget_get_visible(scrolled_window) == FALSE) return;

		if(gtk_widget_get_visible(scrolled_window) == FALSE) return;

		//g_object_ref(web_view);
		//gtk_container_remove(GTK_CONTAINER(vbox), scrolled_window);
		//gtk_box_pack_end(GTK_BOX(vbox), GTK_WIDGET(web_view), TRUE, TRUE, 0);
		//g_object_unref(web_view);

		//gtk_widget_reparent(GTK_WIDGET(web_view), vbox);
		
		//gtk_widget_hide(scrolled_window);
		gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), 
		                               GTK_POLICY_NEVER,
		                               GTK_POLICY_NEVER);
	}
	else
	{

		//gtk_widget_reparent(GTK_WIDGET(web_view), scrolled_window);
		
		//g_object_ref(web_view);
		//gtk_container_remove(GTK_CONTAINER(vbox), GTK_WIDGET(web_view));
		
		//scrolled_window = gtk_scrolled_window_new (NULL, NULL);
		//gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window), 
		//                                GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
		//gtk_container_add (GTK_CONTAINER (scrolled_window), GTK_WIDGET (web_view));
		//gtk_box_pack_end(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);
		//g_object_unref(web_view);
		
		//gtk_widget_show(scrolled_window);

		gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), 
		                               GTK_POLICY_AUTOMATIC,
		                               GTK_POLICY_ALWAYS);
	}
}




Gtk::Widget* wxBrowser::getBrowserWidget()
{
	return wid;
}

void wxBrowser::LoadUri(const gchar* uri)
{
	webkit_web_view_load_uri (web_view, uri);
}



void wxBrowser::on_entry_activate(GtkWidget *widget, gpointer data)
{
	//wxGLOBAL->DebugPrint("wxBrowser", "on_entry_activate");
	GtkEntry* _entry = reinterpret_cast<GtkEntry*>(widget);
	wxBrowser* _this = reinterpret_cast<wxBrowser*>(data);
	_this->Find(gtk_entry_get_text(_entry));
}

void wxBrowser::Find(const gchar* text)
{
	//wxGLOBAL->DebugPrint("wxBrowser", "Find");
	bool ret;
	if(g_str_equal(text, ""))
	{
		webkit_web_view_execute_script(web_view, "document.execCommand('Unselect')");
	}
	else
		ret = webkit_web_view_search_text(web_view, text, FALSE, TRUE, TRUE);

}

void wxBrowser::on_buttonBackward_clicked(GtkWidget *widget, gpointer data)
{
	//wxGLOBAL->DebugPrint("wxBrowser", "on_buttonBackward_clicked");
	wxBrowser* _this = reinterpret_cast<wxBrowser*>(data);
	_this->goBack();
}

void wxBrowser::goBack()
{
	//wxGLOBAL->DebugPrint("wxBrowser", "goBack");
	if(webkit_web_view_can_go_back (web_view))
		webkit_web_view_go_back(web_view);
}

void wxBrowser::on_buttonForward_clicked(GtkWidget *widget, gpointer data)
{
	wxBrowser* _this = reinterpret_cast<wxBrowser*>(data);
	_this->goForward();
}

void wxBrowser::goForward()
{
	//wxGLOBAL->DebugPrint("wxBrowser", "goForward");
	if(webkit_web_view_can_go_forward(web_view))
		webkit_web_view_go_forward(web_view);
}


void wxBrowser::on_buttonHome_clicked(GtkWidget *widget, gpointer data)
{
	wxBrowser* _this = reinterpret_cast<wxBrowser*>(data);
	_this->goHome();
}

void wxBrowser::goHome()
{
	/*
	Glib::ustring filename; // = wxGLOBAL->getHelpPath();
	//filename.append("/winxound_help.html");
	filename.append("/usr/share/doc/csound-doc/html/index.html");
	//filename.append(wxSETTINGS->Directory.CSoundHelpHTML);
	LoadUri(g_filename_to_uri(filename.c_str(), NULL, NULL));
	*/

	Glib::ustring filename = wxSETTINGS->Directory.CSoundHelpHTML;
	if(Glib::file_test(filename, Glib::FILE_TEST_EXISTS))
		LoadUri(g_filename_to_uri(filename.c_str(), NULL, NULL));
	else
	{
		filename = wxGLOBAL->getHelpPath();
		filename.append("/winxound_help.html");
		if(Glib::file_test(filename, Glib::FILE_TEST_EXISTS))
			LoadUri(g_filename_to_uri(filename.c_str(), NULL, NULL));
	}
	
	//LoadUri("http://www.google.com/");
}

void wxBrowser::on_load_status(WebKitWebView  *web_view,
                               WebKitWebFrame *frame,
                               gpointer        user_data)
{
	WebKitLoadStatus stat = webkit_web_view_get_load_status(web_view);

	/*
	if (stat == WEBKIT_LOAD_COMMITTED) //WEBKIT_LOAD_PROVISIONAL
	{
		//wxGLOBAL->DebugPrint("wxBrowser", webkit_web_view_get_uri(web_view));
		if(g_str_has_suffix(webkit_web_view_get_uri(web_view), ".csd"))
		{
			wxGLOBAL->DebugPrint("wxBrowser", "FILE_CSD");
			webkit_web_view_stop_loading (web_view);
		}
	}
	*/
	
	
	//WEBKIT_LOAD_FINISHED
	if (stat == WEBKIT_LOAD_FINISHED)
	{
		wxBrowser* _this = reinterpret_cast<wxBrowser*>(user_data);
		if(_this->History == false) return;
		
		_this->refreshButtonState();

		////if(g_str_has_suffix(webkit_web_view_get_uri(web_view), ".csd"))
		Glib::ustring temp = webkit_web_view_get_uri(web_view);
		if(Glib::str_has_suffix(temp.lowercase(), ".csd"))
		{
			//wxGLOBAL->DebugPrint("wxBrowser", "FILE_CSD");
			_this->m_signal_csound_file_clicked.emit(webkit_web_view_get_uri(web_view));
		}
	}
}

void wxBrowser::refreshButtonState()
{
	if(webkit_web_view_can_go_back (web_view))
		gtk_widget_set_sensitive (GTK_WIDGET (buttonBackward), TRUE);
	else
		gtk_widget_set_sensitive (GTK_WIDGET (buttonBackward), FALSE);


	if(webkit_web_view_can_go_forward(web_view))
		gtk_widget_set_sensitive (GTK_WIDGET (buttonForward), TRUE);
	else
		gtk_widget_set_sensitive (GTK_WIDGET (buttonForward), FALSE);

}

void wxBrowser::SetBrowserZoom(gfloat zoom)
{
	zoomValue = zoom;
	webkit_web_view_set_zoom_level(web_view, zoom); 	
}
