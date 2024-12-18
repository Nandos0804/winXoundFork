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

#include "wx-intellitip.h"
//#include <iostream>


wxIntellitip::wxIntellitip()  	
{

	labelTitle = new Gtk::Label();
	labelTitle->set_text(" Opcode:");
	labelTitle->set_justify(Gtk::JUSTIFY_LEFT);
	labelTitle->set_alignment(0,0);
	////gtk_widget_set_usize(GTK_WIDGET(labelTitle), -1, 20);
	
	labelParams = new Gtk::Label();
	labelParams->set_text(" Params:");
	labelParams->set_justify(Gtk::JUSTIFY_LEFT);
	labelParams->set_alignment(0,0);
	////gtk_widget_set_usize(GTK_WIDGET(labelParams), -1, 20);


	Gtk::VBox* vbox = new Gtk::VBox();
	//gtk_box_pack_start                    (GtkBox *box,
    //                                       GtkWidget *child,
    //                                       gboolean expand,
    //                                       gboolean fill,
    //                                       guint padding);
	vbox->pack_start(*labelTitle, TRUE, TRUE, 0);
	vbox->pack_start(*labelParams, TRUE, TRUE, 0);

	//GdkColor color;
	//Yellow: FromArgb(255, 255, 220); #ffffdd
	//Grey: FromArgb(230, 230, 230); #e6e6e6
	this->modify_bg(Gtk::STATE_NORMAL, Gdk::Color("#ffffdd"));

	//set_border_width(2);
	add(*vbox);

	show();

	//std::cout << "wxIntellitip created" << std::endl;
	
}
wxIntellitip::~wxIntellitip(void)
{
	delete labelTitle;
	delete labelParams;

	//std::cout << "wxIntellitip released" << std::endl;
}

void wxIntellitip::ShowTip(const gchar* Title, const gchar* Params)
{
	labelTitle->set_text(g_strconcat(" ", Title, NULL));
	labelParams->set_text(g_strconcat(" ", Params, NULL));
}



