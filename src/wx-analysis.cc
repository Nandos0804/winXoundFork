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

#include "wx-analysis.h"
#include "wx-main.h"
#include "wx-global.h"
#include "wx-terminal.h"
#include "wx-browser.h"
#include "wx-settings.h"



wxAnalysis::wxAnalysis(wxMain* owner)
{
	
	mOwner = owner;

	mCurrentIndex = 0;
    mStopBatch = true;


	Glib::RefPtr<Gtk::Builder> builder;
	try
	{
		//builder = Gtk::Builder::create_from_file(UI_ANALYSIS_FILE);
		builder = Gtk::Builder::create_from_file(
			Glib::ustring::compose("%1/winxound_analysis.ui",wxGLOBAL->getSrcPath()));
	}
	catch (const Glib::FileError & ex)
	{
		wxGLOBAL->DebugPrint("wxAnalysis constructor - Builder Error - Critical",
		                     ex.what().c_str());
		return;
	}
	catch (...)
	{
		wxGLOBAL->DebugPrint("wxAnalysis constructor - Builder Error - Critical");
		return;
	}

	//Locate and Associate wxMainWindow
	builder->get_widget("windowAnalysis", analysisWindow);
	builder->get_widget("notebook1", notebook1);
	builder->get_widget("notebook2", notebook2);

	
	//Gtk::VBox* vboxCompiler;
	//Gtk::VBox* vboxHelp;
	builder->get_widget("vboxCompiler", vboxCompiler);
	builder->get_widget("vboxHelp", vboxHelp);
	compiler = new wxTerminal(true);
	help = new wxBrowser(false);
	vboxCompiler->pack_start(*compiler->getTerminalWidget(), TRUE, TRUE, 0);
	vboxHelp->pack_start(*help->getBrowserWidget(), TRUE, TRUE, 0);
	vboxCompiler->set_size_request(200,200);
	vboxHelp->set_size_request(200,200);



	Glib::ustring filename = wxGLOBAL->getHelpPath();
	filename.append("/atsa.html");
	if(Glib::file_test(filename, Glib::FILE_TEST_EXISTS))
		help->LoadUri(g_filename_to_uri(filename.c_str(), NULL, NULL));


	
	help->SetBrowserZoom(0.9);
	notebook2->set_current_page(0);

	associateWidgets(builder);


	
	//analysisWindow->set_border_width(2);
	analysisWindow->signal_key_press_event().connect(
		sigc::mem_fun(*this, &wxAnalysis::on_key_press_event));


	//analysisWindow->hide();
	//analysisWindow->set_title(TITLE);
	//analysisWindow->move(0, 0);
	//analysisWindow->resize(0,600);

	//Prepare data tabs 
	SetDefaultValues("ATSA");
	SetDefaultValues("CVANAL");
	SetDefaultValues("HETRO");
	SetDefaultValues("LPANAL");
	SetDefaultValues("PVANAL");

	//m_refTreeModel = Gtk::ListStore::create(inputColumns);
	//comboboxentryInputFile->set_model(m_refTreeModel);


	//Set icon
	Glib::ustring iconfile = Glib::ustring::compose("%1/winxound_48.png",wxGLOBAL->getIconsPath());
	if(Glib::file_test(iconfile, Glib::FILE_TEST_EXISTS))
		analysisWindow->set_icon_from_file(iconfile);

	buttonStopBatch->set_sensitive(FALSE);

	compiler->signal_compiler_completed().connect(
		sigc::mem_fun(*this, &wxAnalysis::on_Compiler_Completed));

	wxGLOBAL->DebugPrint("wxAnalysis created");

}

wxAnalysis::~wxAnalysis()
{
	try
	{
		//Before to delete analysisWindow we must remove terminal
		//and browser widgets (after, we call directly the destructors)
		vboxCompiler->remove(*compiler->getTerminalWidget());
		vboxHelp->remove(*help->getBrowserWidget());
		delete analysisWindow;

		delete compiler;
		delete help;
	}
	catch(...){}


	wxGLOBAL->DebugPrint("wxAnalysis released");
}

void wxAnalysis::associateWidgets(Glib::RefPtr<Gtk::Builder> builder)
{

	//WINDOW WIDGETS
	//builder->get_widget("comboboxentryInputFile",comboboxentryInputFile);
	builder->get_widget("vboxInput",vboxInput);
	comboboxentryInputFile = Gtk::manage(new Gtk::ComboBoxText());//new Gtk::ComboBoxEntryText();
	vboxInput->pack_start(*comboboxentryInputFile,TRUE,TRUE,0);
	                      
	
	builder->get_widget("entryOutputPath",entryOutputPath);
	builder->get_widget("notebook1",notebook1);
	builder->get_widget("statusbar1",statusbar1);
	
	builder->get_widget("b_Atsa",b_Atsa);
	builder->get_widget("e_Atsa",e_Atsa);
	builder->get_widget("l_Atsa",l_Atsa);
	builder->get_widget("Max_Atsa",Max_Atsa);
	builder->get_widget("d_Atsa",d_Atsa);
	builder->get_widget("c_Atsa",c_Atsa);
	builder->get_widget("T_SMR_Atsa",T_SMR_Atsa);
	builder->get_widget("h_Atsa",h_Atsa);
	builder->get_widget("m_Atsa",m_Atsa);
	builder->get_widget("t_Atsa",t_Atsa);
	builder->get_widget("M_SMR_Atsa",M_SMR_Atsa);
	builder->get_widget("s_Atsa",s_Atsa);
	builder->get_widget("S_SMR_Atsa",S_SMR_Atsa);
	builder->get_widget("g_Atsa",g_Atsa);
	builder->get_widget("P_Peak_Atsa",P_Peak_Atsa);
	builder->get_widget("s_Cvanal",s_Cvanal);
	builder->get_widget("c_Cvanal",c_Cvanal);
	builder->get_widget("b_Cvanal",b_Cvanal);
	builder->get_widget("d_Cvanal",d_Cvanal);
	builder->get_widget("s_Hetro",s_Hetro);
	builder->get_widget("f_Hetro",f_Hetro);
	builder->get_widget("b_Hetro",b_Hetro);
	builder->get_widget("n_Hetro",n_Hetro);
	builder->get_widget("Max_Hetro",Max_Hetro);
	builder->get_widget("d_Hetro",d_Hetro);
	builder->get_widget("c_Hetro",c_Hetro);
	builder->get_widget("h_Hetro",h_Hetro);
	builder->get_widget("min_Hetro",min_Hetro);
	builder->get_widget("l_Hetro",l_Hetro);

	builder->get_widget("s_Lpanal",s_Lpanal);
	builder->get_widget("p_Lpanal",p_Lpanal);
	builder->get_widget("b_Lpanal",b_Lpanal);
	builder->get_widget("Max_Lpanal",Max_Lpanal);
	builder->get_widget("Min_Lpanal",Min_Lpanal);
	builder->get_widget("d_Lpanal",d_Lpanal);
	builder->get_widget("c_Lpanal",c_Lpanal);
	builder->get_widget("h_Lpanal",h_Lpanal);
	builder->get_widget("a_Lpanal",a_Lpanal);
	builder->get_widget("Comments_Lpanal",Comments_Lpanal);

	builder->get_widget("s_Pvanal",s_Pvanal);
	builder->get_widget("b_Pvanal",b_Pvanal);
	builder->get_widget("d_Pvanal",d_Pvanal);
	builder->get_widget("c_Pvanal",c_Pvanal);


	builder->get_widget("buttonBrowseFile",buttonBrowseFile);
	builder->get_widget("buttonBrowsePath",buttonBrowsePath);
	builder->get_widget("buttonClear",buttonClear);
	builder->get_widget("buttonReset",buttonReset);
	builder->get_widget("buttonStopBatch",buttonStopBatch);
	//builder->get_widget("buttonPause",buttonPause);
	builder->get_widget("buttonStartAnalysis",buttonStartAnalysis);


	builder->get_widget("checkbuttonSDIF",checkbuttonSDIF);
	builder->get_widget("checkbuttonPvocEx",checkbuttonPvocEx);



	//COMBOBOXWIDGETS
	Gtk::VBox* w_Atsa_vbox; builder->get_widget("w_Atsa_vbox",w_Atsa_vbox);
	Gtk::VBox* F_File_Atsa_vbox; builder->get_widget("F_File_Atsa_vbox",F_File_Atsa_vbox);
	Gtk::VBox* v_Lpanal_vbox; builder->get_widget("v_Lpanal_vbox",v_Lpanal_vbox);
	Gtk::VBox* n_Pvanal_vbox; builder->get_widget("n_Pvanal_vbox",n_Pvanal_vbox);
	Gtk::VBox* w_Pvanal_vbox; builder->get_widget("w_Pvanal_vbox",w_Pvanal_vbox);
	Gtk::VBox* h_Pvanal_vbox; builder->get_widget("h_Pvanal_vbox",h_Pvanal_vbox);
	Gtk::VBox* window_Pvanal_vbox; builder->get_widget("window_Pvanal_vbox",window_Pvanal_vbox);
	
	w_Atsa = Gtk::manage(new Gtk::ComboBoxText());
	F_File_Atsa = Gtk::manage(new Gtk::ComboBoxText());
	v_Lpanal = Gtk::manage(new Gtk::ComboBoxText());
	n_Pvanal = Gtk::manage(new Gtk::ComboBoxText());
	w_Pvanal = Gtk::manage(new Gtk::ComboBoxText());
	h_Pvanal = Gtk::manage(new Gtk::ComboBoxText());
	window_Pvanal = Gtk::manage(new Gtk::ComboBoxText());

	w_Atsa_vbox->pack_start(*w_Atsa,TRUE,TRUE,0);
	F_File_Atsa_vbox->pack_start(*F_File_Atsa,TRUE,TRUE,0);
	v_Lpanal_vbox->pack_start(*v_Lpanal,TRUE,TRUE,0);
	n_Pvanal_vbox->pack_start(*n_Pvanal,TRUE,TRUE,0);
	w_Pvanal_vbox->pack_start(*w_Pvanal,TRUE,TRUE,0);
	h_Pvanal_vbox->pack_start(*h_Pvanal,TRUE,TRUE,0);
	window_Pvanal_vbox->pack_start(*window_Pvanal,TRUE,TRUE,0);

	w_Atsa->append_text("Blackmann");
	w_Atsa->append_text("Blackmann_H");
	w_Atsa->append_text("Hamming");
	w_Atsa->append_text("Von Hann");

	F_File_Atsa->append_text("1=amp and freq only");
	F_File_Atsa->append_text("2=amp, freq and phase");
	F_File_Atsa->append_text("3=amp, freq and residual");
	F_File_Atsa->append_text("4=amp, freq, phase and residual");
	
	v_Lpanal->append_text("none");
	v_Lpanal->append_text("verbose");
	v_Lpanal->append_text("debug");
	
	n_Pvanal->append_text("16");
	n_Pvanal->append_text("32");
	n_Pvanal->append_text("64");
	n_Pvanal->append_text("128");
	n_Pvanal->append_text("256");
	n_Pvanal->append_text("512");
	n_Pvanal->append_text("1024");
	n_Pvanal->append_text("2048");
	n_Pvanal->append_text("4096");
	n_Pvanal->append_text("8192");
	n_Pvanal->append_text("16384");
	
	w_Pvanal->append_text("Use Hop Size");
	w_Pvanal->append_text("2");
	w_Pvanal->append_text("4");
	w_Pvanal->append_text("8");
	w_Pvanal->append_text("16");
	
	h_Pvanal->append_text("16");
	h_Pvanal->append_text("32");
	h_Pvanal->append_text("64");
	h_Pvanal->append_text("128");
	h_Pvanal->append_text("256");
	h_Pvanal->append_text("512");
	h_Pvanal->append_text("1024");
	h_Pvanal->append_text("2048");
	h_Pvanal->append_text("4096");
	
	window_Pvanal->append_text("Hamming");
	window_Pvanal->append_text("Von Hann");
	window_Pvanal->append_text("Kaiser");

	
	

	
	buttonStartAnalysis->signal_clicked().connect(
		sigc::mem_fun(*this, &wxAnalysis::on_buttonStartAnalysis_Clicked));

	buttonBrowseFile->signal_clicked().connect(
		sigc::mem_fun(*this, &wxAnalysis::on_buttonInput_Clicked));

	buttonBrowsePath->signal_clicked().connect(
		sigc::mem_fun(*this, &wxAnalysis::on_buttonOutput_Clicked));

	buttonClear->signal_clicked().connect(
		sigc::mem_fun(*this, &wxAnalysis::on_buttonClear_Clicked));

	buttonStopBatch->signal_clicked().connect(
		sigc::mem_fun(*this, &wxAnalysis::on_buttonStopBatch_Clicked));

	buttonReset->signal_clicked().connect(
		sigc::mem_fun(*this, &wxAnalysis::on_buttonReset_Clicked));

	notebook1->signal_switch_page().connect(
		sigc::mem_fun(*this, &wxAnalysis::on_notebook1_switch_page));

}

void wxAnalysis::on_buttonExit_Clicked()
{
	closeWindow();
}


bool wxAnalysis::on_key_press_event(GdkEventKey* event)
{
	//wxGLOBAL->DebugPrint("KEY", "PRESSED");
	if (event->keyval == GDK_Escape)
	{
		//on_buttonExit_Clicked();
		closeWindow();
	}
	return false;
}


void wxAnalysis::showWindowAt(gint x, gint y)
{               
	//Show window
	analysisWindow->show_all();
	analysisWindow->present(); //Deiconify if necessary

	analysisWindow->move(x - (analysisWindow->get_width() / 2), 
	                     y - (analysisWindow->get_height() / 2));


	analysisWindow->grab_focus();
}

void wxAnalysis::closeWindow()
{	
	analysisWindow->hide();
}



////////////////////////////////////////////////////////////////////////////////

gint wxAnalysis::getIndex(Glib::ustring utility)
{
	if(utility == "ATSA")
		return 0;
	else if(utility == "CVANAL")
		return 1;
	else if(utility == "HETRO")
		return 2;
	else if(utility == "LPANAL")
		return 3;
	else if(utility == "PVANAL")
		return 4;
	else //SNDINFO
		return 5;
}

void wxAnalysis::SetDefaultValues(Glib::ustring mUtilityName)
{

	gint i = getIndex(mUtilityName);

	switch (i)
	{
		case 0:
			b_Atsa->set_value(0); 
			e_Atsa->set_value(0);
			l_Atsa->set_value(20);
			Max_Atsa->set_value(20000);
			d_Atsa->set_value(0.1);
			c_Atsa->set_value(4);
			//w_Atsa.SelectedIndex = 1;
			w_Atsa->set_active(1);
			//F_File_Atsa.SelectedIndex = 3;
			F_File_Atsa->set_active(3);
			h_Atsa->set_value(0.25);
			m_Atsa->set_value(-60);
			t_Atsa->set_value(3);
			s_Atsa->set_value(3);
			g_Atsa->set_value(3);
			T_SMR_Atsa->set_value(30);
			S_SMR_Atsa->set_value(60);
			P_Peak_Atsa->set_value(0);
			M_SMR_Atsa->set_value(0.5);
			break;

		case 1:
			s_Cvanal->set_value(10000);
			c_Cvanal->set_value(0);
			b_Cvanal->set_value(0);
			d_Cvanal->set_value(0);
			break;

		case 2:
			s_Hetro->set_value(10000);
			c_Hetro->set_value(1);
			b_Hetro->set_value(0);
			d_Hetro->set_value(0);
			f_Hetro->set_value(100);
			Max_Hetro->set_value(32767);
			n_Hetro->set_value(256);
			h_Hetro->set_value(10);
			min_Hetro->set_value(64);
			l_Hetro->set_value(0);
			checkbuttonSDIF->set_active(FALSE);
			break;

		case 3:
			s_Lpanal->set_value(10000);
			c_Lpanal->set_value(1);
			b_Lpanal->set_value(0);
			d_Lpanal->set_value(0);
			p_Lpanal->set_value(34);
			Min_Lpanal->set_value(70);
			Max_Lpanal->set_value(200);
			h_Lpanal->set_value(200);
			//v_Lpanal.SelectedIndex = 0;
			v_Lpanal->set_active(0);
			a_Lpanal->set_active(FALSE);
			Comments_Lpanal->set_text("");
			break;

		case 4:
			s_Pvanal->set_value(10000);
			c_Pvanal->set_value(1);
			b_Pvanal->set_value(0);
			d_Pvanal->set_value(0);
			//n_Pvanal.Text = "2048";
			n_Pvanal->set_active(7);
			//w_Pvanal.SelectedIndex = 0;
			w_Pvanal->set_active(0);
			//h_Pvanal.Text = "128";
			h_Pvanal->set_active(3);
			//window_Pvanal.Text = "Von Hann";
			window_Pvanal->set_active(1);
			checkbuttonPvocEx->set_active(FALSE);
			break;

	}

}


void wxAnalysis::on_buttonResetAtsa_Clicked()
{
	SetDefaultValues("ATSA");
}

void wxAnalysis::buttonResetCvanal_Clicked()
{
	SetDefaultValues("CVANAL");
}

void wxAnalysis::buttonResetHetro_Clicked()
{
	SetDefaultValues("HETRO");
}

void wxAnalysis::buttonResetLpanal_Clicked()
{
	SetDefaultValues("LPANAL");
}

void wxAnalysis::buttonResetPvanal_Clicked()
{
	SetDefaultValues("PVANAL");
}



void wxAnalysis::on_buttonStartAnalysis_Clicked()
{

	//if (string.IsNullOrEmpty(wxGlobal.Settings.Directory.CSoundConsole) ||
	//    !File.Exists(wxGlobal.Settings.Directory.CSoundConsole))
	
	if(wxSETTINGS->Directory.CSoundConsole == "" ||
	   !Glib::file_test(wxSETTINGS->Directory.CSoundConsole, Glib::FILE_TEST_EXISTS))
	{
		wxGLOBAL->ShowMessageBox("Cannot find CSound binary compiler!\n"
		                         "Please select a valid path in File->Settings->Directories->CSound executable",
		                         "CSound Compiler error",
		                         Gtk::BUTTONS_OK);

		return;
	}

	StartCompiler();

}



Glib::ustring wxAnalysis::ATSA()
{
	Glib::ustring tFlags = "";

	if (b_Atsa->get_value() > 0)
	{
		//tFlags += "-b" + b_Atsa->get_value() + " ";
		tFlags.append(Glib::ustring::compose("-b%1 ",
		                                     b_Atsa->get_value()));	                               
	}
	if (e_Atsa->get_value() > 0)
	{
		//tFlags += "-e" + e_Atsa->get_value() + " ";
		tFlags.append(Glib::ustring::compose("-e%1 ",
		                                     e_Atsa->get_value()));
	}
	if (l_Atsa->get_value() != 20)
	{
		//tFlags += "-l" + l_Atsa->get_value() + " ";
		tFlags.append(Glib::ustring::compose("-l%1 ",
		                                     l_Atsa->get_value()));
	}
	if (Max_Atsa->get_value() != 20000)
	{
		//tFlags += "-H" + Max_Atsa->get_value() + " ";
		tFlags.append(Glib::ustring::compose("-H%1 ",
		                                     Max_Atsa->get_value()));
	}
	if (d_Atsa->get_value() != 0.1)
	{
		//tFlags += "-d" + d_Atsa->get_value() + " ";
		tFlags.append(Glib::ustring::compose("-d%1 ",
		                                     d_Atsa->get_value()));
	}
	if (c_Atsa->get_value() != 4)
	{
		//tFlags += "-c" + c_Atsa->get_value() + " ";
		tFlags.append(Glib::ustring::compose("-c%1 ",
		                                     c_Atsa->get_value()));
	}

	//get_active_row_number //get_active
	if (w_Atsa->get_active_row_number() != 1)
	{
		//tFlags += "-w" + w_Atsa.SelectedIndex + " ";
		tFlags.append(Glib::ustring::compose("-w%1 ",
		                                     w_Atsa->get_active_row_number()));
	}
	
	if (h_Atsa->get_value() != 0.25)
	{
		//tFlags += "-h" + h_Atsa->get_value() + " ";
		tFlags.append(Glib::ustring::compose("-h%1 ",
		                                     h_Atsa->get_value()));
	}
	if (m_Atsa->get_value() != -60)
	{
		//tFlags += "-m" + m_Atsa->get_value() + " ";
		tFlags.append(Glib::ustring::compose("-m%1 ",
		                                     m_Atsa->get_value()));
	}
	if (t_Atsa->get_value() != 3)
	{
		//tFlags += "-t" + t_Atsa->get_value() + " ";
		tFlags.append(Glib::ustring::compose("-t%1 ",
		                                     t_Atsa->get_value()));
	}
	if (s_Atsa->get_value() != 3)
	{
		//tFlags += "-s" + s_Atsa->get_value() + " ";
		tFlags.append(Glib::ustring::compose("-s%1 ",
		                                     s_Atsa->get_value()));
	}
	if (g_Atsa->get_value() != 3)
	{
		//tFlags += "-g" + g_Atsa->get_value() + " ";
		tFlags.append(Glib::ustring::compose("-g%1 ",
		                                     g_Atsa->get_value()));
	}
	if (T_SMR_Atsa->get_value() != 30)
	{
		//tFlags += "-T" + T_SMR_Atsa->get_value() + " ";
		tFlags.append(Glib::ustring::compose("-T%1 ",
		                                     T_SMR_Atsa->get_value()));
	}
	if (S_SMR_Atsa->get_value() != 60)
	{
		//tFlags += "-S" + S_SMR_Atsa->get_value() + " ";
		tFlags.append(Glib::ustring::compose("-S%1 ",
		                                     S_SMR_Atsa->get_value()));
	}
	if (P_Peak_Atsa->get_value() > 0)
	{
		//tFlags += "-P" + P_Peak_Atsa->get_value() + " ";
		tFlags.append(Glib::ustring::compose("-P%1 ",
		                                     P_Peak_Atsa->get_value()));
	}
	if (M_SMR_Atsa->get_value() != 0.5)
	{
		//tFlags += "-M" + M_SMR_Atsa->get_value() + " ";
		tFlags.append(Glib::ustring::compose("-M%1 ",
		                                     M_SMR_Atsa->get_value()));
	}

	//get_active_row_number //get_active
	if (F_File_Atsa->get_active_row_number() != 3)
	{
		//tFlags += "-F" + F_File_Atsa.SelectedIndex + 1 + " ";
		tFlags.append(Glib::ustring::compose("-F%1 ",
		                                     (F_File_Atsa->get_active_row_number()+1)));
	}
	

	return tFlags;
}



Glib::ustring wxAnalysis::CVANAL()
{
	Glib::ustring tFlags = "";

	if (s_Cvanal->get_value() != 10000)
	{
		//tFlags += "-s" + s_Cvanal->get_value() + " ";
		tFlags.append(Glib::ustring::compose("-s%1 ",
		                                     s_Cvanal->get_value()));
	}
	if (c_Cvanal->get_value() > 0)
	{
		//tFlags += "-c" + c_Cvanal->get_value() + " ";
		tFlags.append(Glib::ustring::compose("-c%1 ",
		                                     c_Cvanal->get_value()));
	}
	if (b_Cvanal->get_value() > 0)
	{
		//tFlags += "-b" + b_Cvanal->get_value() + " ";
		tFlags.append(Glib::ustring::compose("-b%1 ",
		                                     b_Cvanal->get_value()));
	}
	if (d_Cvanal->get_value() > 0)
	{
		//tFlags += "-d" + d_Cvanal->get_value() + " ";
		tFlags.append(Glib::ustring::compose("-d%1 ",
		                                     d_Cvanal->get_value()));
	}

	return tFlags;
}


Glib::ustring wxAnalysis::HETRO()
{
	Glib::ustring tFlags = "";

	if (s_Hetro->get_value() != 10000)
	{
		//tFlags += "-s" + s_Hetro->get_value() + " ";
		tFlags.append(Glib::ustring::compose("-s%1 ",
		                                     s_Hetro->get_value()));
	}
	if (c_Hetro->get_value() != 1)
	{
		//tFlags += "-c" + c_Hetro->get_value() + " ";
		tFlags.append(Glib::ustring::compose("-c%1 ",
		                                     c_Hetro->get_value()));
	}
	if (b_Hetro->get_value() > 0)
	{
		//tFlags += "-b" + b_Hetro->get_value() + " ";
		tFlags.append(Glib::ustring::compose("-b%1 ",
		                                     b_Hetro->get_value()));
	}
	if (d_Hetro->get_value() > 0)
	{
		//tFlags += "-d" + d_Hetro->get_value() + " ";
		tFlags.append(Glib::ustring::compose("-d%1 ",
		                                     d_Hetro->get_value()));
	}

	if (f_Hetro->get_value() != 100)
	{
		//tFlags += "-f" + f_Hetro->get_value() + " ";
		tFlags.append(Glib::ustring::compose("-f%1 ",
		                                     f_Hetro->get_value()));
	}
	if (h_Hetro->get_value() != 10)
	{
		//tFlags += "-h" + h_Hetro->get_value() + " ";
		tFlags.append(Glib::ustring::compose("-h%1 ",
		                                     h_Hetro->get_value()));
	}
	if (Max_Hetro->get_value() != 32767)
	{
		//tFlags += "-M" + Max_Hetro->get_value() + " ";
		tFlags.append(Glib::ustring::compose("-M%1 ",
		                                     Max_Hetro->get_value()));
	}
	if (min_Hetro->get_value() != 64)
	{
		//tFlags += "-m" + min_Hetro->get_value() + " ";
		tFlags.append(Glib::ustring::compose("-m%1 ",
		                                     min_Hetro->get_value()));
	}
	if (n_Hetro->get_value() != 256)
	{
		//tFlags += "-n" + n_Hetro->get_value() + " ";
		tFlags.append(Glib::ustring::compose("-n%1 ",
		                                     n_Hetro->get_value()));
	}
	if (l_Hetro->get_value() > 0)
	{
		//tFlags += "-l" + l_Hetro->get_value() + " ";
		tFlags.append(Glib::ustring::compose("-l%1 ",
		                                     l_Hetro->get_value()));
	}

	return tFlags;
}


Glib::ustring wxAnalysis::LPANAL()
{
	Glib::ustring tFlags = "";

	if (s_Lpanal->get_value() != 10000)
	{
		//tFlags += "-s" + s_Lpanal->get_value() + " ";
		tFlags.append(Glib::ustring::compose("-s%1 ",
		                                     s_Lpanal->get_value()));
	}
	if (c_Lpanal->get_value() != 1)
	{
		//tFlags += "-c" + c_Lpanal->get_value() + " ";
		tFlags.append(Glib::ustring::compose("-c%1 ",
		                                     c_Lpanal->get_value()));
	}
	if (b_Lpanal->get_value() > 0)
	{
		//tFlags += "-b" + b_Lpanal->get_value() + " ";
		tFlags.append(Glib::ustring::compose("-b%1 ",
		                                     b_Lpanal->get_value()));
	}
	if (d_Lpanal->get_value() > 0)
	{
		//tFlags += "-d" + d_Lpanal->get_value() + " ";
		tFlags.append(Glib::ustring::compose("-d%1 ",
		                                     d_Lpanal->get_value()));
	}

	if (p_Lpanal->get_value() != 34)
	{
		//tFlags += "-p" + p_Lpanal->get_value() + " ";
		tFlags.append(Glib::ustring::compose("-p%1 ",
		                                     p_Lpanal->get_value()));
	}
	if (h_Lpanal->get_value() != 200)
	{
		//tFlags += "-h" + h_Lpanal->get_value() + " ";
		tFlags.append(Glib::ustring::compose("-h%1 ",
		                                     h_Lpanal->get_value()));
	}
	if (Max_Lpanal->get_value() != 200)
	{
		//tFlags += "-Q" + Max_Lpanal->get_value() + " ";
		tFlags.append(Glib::ustring::compose("-Q%1 ",
		                                     Max_Lpanal->get_value()));
	}
	if (Min_Lpanal->get_value() != 70)
	{
		//tFlags += "-P" + Min_Lpanal->get_value() + " ";
		tFlags.append(Glib::ustring::compose("-P%1 ",
		                                     Min_Lpanal->get_value()));
	}
	if (a_Lpanal->get_active())
	{
		tFlags.append("-a ");
	}

	//get_active_row_number //get_active
	if (v_Lpanal->get_active_text() != "none")
	{
		//tFlags += "-v" + v_Lpanal.SelectedIndex + " ";
		tFlags.append(Glib::ustring::compose("-v%1 ",
		                                     v_Lpanal->get_active_row_number()));
	}

	
	//if (Comments_Lpanal.Text.Length > 0)
	if(Comments_Lpanal->get_text().size() > 0)
	{
		//tFlags += "-C" + Comments_Lpanal.Text + " ";
		tFlags.append(Glib::ustring::compose("-C%1 ",
		                                     Comments_Lpanal->get_text()));
	}

	return tFlags;
}


Glib::ustring wxAnalysis::PVANAL()
{
	Glib::ustring tFlags = "";

	if (s_Pvanal->get_value() != 10000)
	{
		//tFlags += "-s" + s_Pvanal->get_value() + " ";
		tFlags.append(Glib::ustring::compose("-s%1 ",
		                                     s_Pvanal->get_value()));
	}
	if (c_Pvanal->get_value() != 1)
	{
		//tFlags += "-c" + c_Pvanal->get_value() + " ";
		tFlags.append(Glib::ustring::compose("-c%1 ",
		                                     c_Pvanal->get_value()));
	}
	if (b_Pvanal->get_value() > 0)
	{
		//tFlags += "-b" + b_Pvanal->get_value() + " ";
		tFlags.append(Glib::ustring::compose("-b%1 ",
		                                     b_Pvanal->get_value()));
	}
	if (d_Pvanal->get_value() > 0)
	{
		//tFlags += "-d" + d_Pvanal->get_value() + " ";
		tFlags.append(Glib::ustring::compose("-d%1 ",
		                                     d_Pvanal->get_value()));
	}

	
	//get_active_row_number //get_active
	//tFlags += "-n" + n_Pvanal.Text + " ";
	tFlags.append(Glib::ustring::compose("-n%1 ",
		                                 n_Pvanal->get_active_text()));
	
										 
	//if (w_Pvanal.Text.Contains("Hop"))
	if (w_Pvanal->get_active_text() == "Use Hop Size")
	{
		//tFlags += "-h" + h_Pvanal.Text + " ";
		tFlags.append(Glib::ustring::compose("-h%1 ",
		                                     h_Pvanal->get_active_text()));
	}
	//else if (w_Pvanal.Text != "4")
	else if(w_Pvanal->get_active_text() != "4")
	{
		//tFlags += "-w" + w_Pvanal.Text + " ";
		tFlags.append(Glib::ustring::compose("-w%1 ",
		                                     w_Pvanal->get_active_text()));
	}
	

	//if (!window_Pvanal.Text.Contains("Hann"))
	if (window_Pvanal->get_active_text() != "Von Hann")
	{
		//if (window_Pvanal.Text.Contains("Hamming"))
		if (window_Pvanal->get_active_text() == "Hamming")
		{
			tFlags += "-H ";
		}
		else
		{
			tFlags += "-K ";
		}
	}
	

	return tFlags;
}



void wxAnalysis::StartCompiler() //string arguments)
{

	setStatubarInfo("");
	
	notebook2->set_current_page(1);

	if(comboboxentryInputFile->get_active_text() == "")
	{
		wxGLOBAL->ShowMessageBox("Input file not specified",
		                         "Error",
		                         Gtk::BUTTONS_OK);
		return;
	}


	if(entryOutputPath->get_text() == "")
	{
		wxGLOBAL->ShowMessageBox("Output path not specified",
		                         "Error",
		                         Gtk::BUTTONS_OK);
		return;
	}


	//CHECK FOR OUTPUT DIRECTORY PERMISSION
	//FileAccessRights rights = new FileAccessRights(output.Text);
	//if (!rights.canWrite())
	if(!wxGLOBAL->FileIsWritable(entryOutputPath->get_text().c_str()))
	{
		wxGLOBAL->ShowMessageBox("Your current user account allows read-only access to the specified 'Output' directory.\n"
		                         "Please select a valid 'Output path' where you have full write permissions.",
		                         "Output path - Write access denied!",
		                         Gtk::BUTTONS_OK);
		return;
	}




	Glib::ustring flags = "";

	Glib::ustring name = notebook1->get_tab_label_text(
	                 		*notebook1->get_nth_page(
	                 			notebook1->get_current_page()));

	name = name.lowercase();
	
	Glib::ustring arguments = Glib::ustring::compose("-U%1 ", name);
	
	
	switch (notebook1->get_current_page())
	{
		case 0: //"Atsa":
			flags = ATSA(); 
			break;
		case 1: //"Cvanal":
			flags = CVANAL();
			break;
		case 2: //"Hetro":
			flags = HETRO();
			break;
		case 3: //"Lpanal":
			flags = LPANAL();
			break;
		case 4: //"Pvanal":
			flags = PVANAL();
			break;
	}

	//arguments += flags + "\"" + input.Text.Trim() + "\"";
	arguments.append(flags);

	//Set current active combobox index
	comboboxentryInputFile->set_active(mCurrentIndex);

	
	//1. INPUT_FILE
	Glib::ustring inputFile = wxGLOBAL->Trim(comboboxentryInputFile->get_active_text());
		//Glib::ustring::compose("\"%1\"", wxGLOBAL->Trim(comboboxentryInputFile->get_active_text()));

	
	//2. OUTPUT_FILE
	Glib::ustring outputFile = ""; //"\"";
	outputFile.append(wxGLOBAL->Trim(entryOutputPath->get_text()));

	//string filename = Path.GetFileName(input.Text.Trim());
	Glib::ustring filename = Glib::path_get_basename(comboboxentryInputFile->get_active_text());

	Glib::ustring o = "";
	//if (filename.Contains("."))
	if((int)filename.find(".") != -1) //-1 //Glib::ustring::npos
	{
		//o = filename.Substring(0, filename.LastIndexOf("."));
		o = filename.substr(0, filename.rfind("."));
		//wxGLOBAL->DebugPrint("filename", filename.c_str());
	}
	else
	{
		o = filename;
	}

	
	//Output filename extension (check for pvanal and hetro tools)
	//o += "." + input.Name.Substring(5, 3).ToLower();   //".ats"
	if(name.substr(0,3) == "pva")
	{
		if(checkbuttonPvocEx->get_active())
			o.append(".pvx"); //Extended format (pvoc-ex)
		else
			o.append(".pv"); //Simple format
	}
	else if(name.substr(0,3) == "het" &&
	        checkbuttonSDIF->get_active()) //Check for sdif extension
	{
		o.append(".sdif");
	}
	else
	{
		o.append(".");
		o.append(name.substr(0,3));
	}
	

	//if (!output.Text.Trim().EndsWith("/"))
	if(!Glib::str_has_suffix(entryOutputPath->get_text(), "/"))
	{
		outputFile.append("/");
	}
	outputFile.append(o);
	//outputFile.append("\"");
	


	//System.Diagnostics.Debug.WriteLine(arguments);
	//wxGLOBAL->DebugPrint("ARGUMENTS", arguments.c_str());
	//wxGLOBAL->DebugPrint("FILENAME1", inputFile.c_str());
	//wxGLOBAL->DebugPrint("FILENAME2", outputFile.c_str());




	//CHECK CONTROLS APPEARANCE
	//TabControl1.Enabled = false;
	notebook1->set_sensitive(FALSE);

	
	//TextModelColumns
	//Gtk::TreeModel::iterator iter = comboboxentryInputFile->get_model()->children().begin();
	//if (input.Items.Count > 1)
	if(comboboxentryInputFile->get_model()->children().size() > 1)
	{
		mStopBatch = false;

		//WxUtilityConsole1.buttonStopBatch.Text = "Stop batch process";
		buttonStopBatch->set_label("Stop batch process");

		//WxUtilityConsole1.buttonStopBatch.Visible = true;
		buttonStopBatch->set_sensitive(TRUE);
		
		//WxUtilityConsole1.Title =
		//	string.Format("Batch processing info:  {0}  [{1} of {2}]",
		//	              input.SelectedItem.ToString(),
		//	              mCurrentIndex + 1, 
		//	              input.Items.Count);
			
		setStatubarInfo(
			Glib::ustring::compose("Batch processing info:  %1  [%2 of %3]",
			                       inputFile,
			                       mCurrentIndex + 1,
			                       comboboxentryInputFile->get_model()->children().size()));
			                     
	}
	else
	{
		setStatubarInfo(Glib::ustring::compose("Processing file:  %1",
			                   					inputFile));
	}

	//START COMPILER
	//Replace "," with "." inside arguments string 
	//arguments = arguments.Replace(",", ".");
	Glib::RefPtr<Glib::Regex> my_regex = Glib::Regex::create(",");
	arguments = my_regex->replace(arguments, 
	                              0, 
	                              ".", 
	                              Glib::REGEX_MATCH_NOTBOL);
	
	   
	//Call csound compiler [Utility -U] 
	//Example: "-U cvanal [FLAGS] inputfile outputfile" 
	if(notebook1->get_current_page() < 5)
	{
		compiler->Compile(wxSETTINGS->Directory.CSoundConsole, 
		                  wxGLOBAL->Trim(arguments),
		                  inputFile,
		                  outputFile,
		                  mCurrentIndex < 1);
	}
	else //SNDINFO
		compiler->Compile(wxSETTINGS->Directory.CSoundConsole, 
		                  "-Usndinfo",
		                  inputFile,
		                  "",
		                  mCurrentIndex < 1);
}




void wxAnalysis::on_Compiler_Completed(Glib::ustring mErrorLine, Glib::ustring mWaveFile)
{
	//wxGLOBAL->DebugPrint("wxAnalysis", "on_Compiler_Completed!!!");
	
	mCurrentIndex++;

	//if (mCurrentIndex < input.Items.Count && mStopBatch == false)
	if(mCurrentIndex < (int)comboboxentryInputFile->get_model()->children().size() &&
	   mStopBatch == false)
	{
		//input.SelectedIndex = mCurrentIndex;
		comboboxentryInputFile->set_active(mCurrentIndex);
		StartCompiler();
		return;
	}
	else
	{
		mStopBatch = true;
		mCurrentIndex = 0;
		//TabControl1.Enabled = true;
		notebook1->set_sensitive(TRUE);
		//WxUtilityConsole1.buttonStopBatch.Visible = false;
		buttonStopBatch->set_sensitive(FALSE);
		setStatubarInfo("Process completed");
	}

}

void wxAnalysis::on_buttonStopBatch_Clicked()
{
	//WxUtilityConsole1.buttonStopBatch.Text = "Stopping batch (Wait current) ...";
	setStatubarInfo("Stopping batch (Wait current) ...");
	mStopBatch = true;	
}



//INPUT BUTTONS
void wxAnalysis::on_buttonInput_Clicked()
{
	//Fill the TreeView's model
	//Gtk::TreeModel::Row rowSynthesizer = *(MyRefTreeModel->append());
	//rowSynthesizer[MyColumns.key] = "<CsoundSynthesizer>";
	//rowSynthesizer[MyColumns.value] = "<CsoundSynthesizer>";


	//Notify user that there are other files into the list
	//if (input.Items.Count > 0)
	//if(!comboboxentryInputFile->get_active_row_number() > -1)
	if(comboboxentryInputFile->get_active_text() != "" &&
	   notebook1->get_current_page() < 5)
	{
		gint r =
			wxGLOBAL->ShowMessageBox("There are one or more files in the input list.\n"
			                         "Do you want to keep them (new files will be added)?",
			                         "WinXound Analysis Info",
			                         Gtk::BUTTONS_YES_NO);

		if(r == Gtk::RESPONSE_NO)
		{
			comboboxentryInputFile->clear_items ();
			entryOutputPath->set_text(""); //output.Clear();
		}
	}




	Gtk::FileChooserDialog dialog("Analysis Input File",
	                              Gtk::FILE_CHOOSER_ACTION_OPEN);
	dialog.set_transient_for(*analysisWindow);

	//Add response buttons the the dialog:
	dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	dialog.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);

	if(notebook1->get_current_page() == 5)
		dialog.set_select_multiple(FALSE);
	else
		dialog.set_select_multiple(TRUE);
	
	dialog.set_current_folder(wxSETTINGS->Directory.LastUsedPath);

	Gtk::FileFilter filter_any;
	filter_any.set_name("Any files");
	filter_any.add_pattern("*");
	dialog.add_filter(filter_any);

	//If the WorkingDir is not empty and exists add it to the Open Dialog Box:
	if(Glib::file_test(wxSETTINGS->Directory.WorkingDir, 
	                   (Glib::FILE_TEST_EXISTS | Glib::FILE_TEST_IS_DIR)))
	{
		dialog.add_shortcut_folder(wxSETTINGS->Directory.WorkingDir);
	}
	

	int result = dialog.run();

	//Handle the response:
	if(result != Gtk::RESPONSE_OK) return;

	
	if(notebook1->get_current_page() == 5) //SNDINFO
	{
		comboboxentryInputFile->clear_items ();
		entryOutputPath->set_text(""); //output.Clear();
	}
	
	wxSETTINGS->Directory.LastUsedPath = dialog.get_current_folder();

	
	// Add file(s) to list
	Gtk::TreeModel::Row row;
	std::vector<std::string> files = dialog.get_filenames();
	for (uint i = 0; i < files.size(); i++)
	{
		comboboxentryInputFile->append_text(files[i]);
	}

	if(entryOutputPath->get_text() == "" &&
	   files.size() > 0)
	{
		entryOutputPath->set_text(Glib::path_get_dirname(files[0]));
	}

	comboboxentryInputFile->set_active_text(files[0]);

}




//OUTPUT BUTTONS
void wxAnalysis::on_buttonOutput_Clicked()
{
	Gtk::FileChooserDialog dialog("Analysis Output Path",
	                              Gtk::FILE_CHOOSER_ACTION_SELECT_FOLDER);
	dialog.set_transient_for(*analysisWindow);

	//Add response buttons the the dialog:
	dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	dialog.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);

	//dialog.set_select_multiple(TRUE);
	dialog.set_current_folder(wxSETTINGS->Directory.LastUsedPath);

	Gtk::FileFilter filter_any;
	filter_any.set_name("Any files");
	filter_any.add_pattern("*");
	dialog.add_filter(filter_any);

	//If the WorkingDir is not empty and exists add it to the Open Dialog Box:
	if(Glib::file_test(wxSETTINGS->Directory.WorkingDir, 
	                   (Glib::FILE_TEST_EXISTS | Glib::FILE_TEST_IS_DIR)))
	{
		dialog.add_shortcut_folder(wxSETTINGS->Directory.WorkingDir);
	}

	int result = dialog.run();

	//Handle the response:
	if(result != Gtk::RESPONSE_OK) return;

	wxSETTINGS->Directory.LastUsedPath = dialog.get_current_folder();

	entryOutputPath->set_text(dialog.get_current_folder());
}

void wxAnalysis::on_notebook1_switch_page(GtkNotebookPage* page, guint page_num)
{
	notebook2->set_current_page(0);


	if(notebook1->get_current_page() == 5)
	{
		entryOutputPath->set_sensitive(FALSE);
		buttonBrowsePath->set_sensitive(FALSE);
		buttonReset->set_sensitive(FALSE);
		
	}
	else
	{
		entryOutputPath->set_sensitive(TRUE);
		buttonBrowsePath->set_sensitive(TRUE);
		buttonReset->set_sensitive(TRUE);
	}

	
	Glib::ustring mFile = wxGLOBAL->getHelpPath();
	mFile.append("/");
	Glib::ustring name = "";
	
	switch (notebook1->get_current_page())
	{
		case 0: //"Atsa":
			name = "atsa.html"; 
			break;
		case 1: //"Cvanal":
			name = "cvanal.html"; 
			break;
		case 2: //"Hetro":
			name = "hetro.html"; 
			break;
		case 3: //"Lpanal":
			name = "lpanal.html"; 
			break;
		case 4: //"Pvanal":
			name = "pvanal.html"; 
			break;
		case 5:
			name = "sndinfo.html";
			break;
	}

	mFile.append(name);
	if(Glib::file_test(mFile, Glib::FILE_TEST_EXISTS))
		help->LoadUri(g_filename_to_uri(mFile.c_str(), NULL, NULL));
}

void wxAnalysis::on_buttonClear_Clicked()
{
	comboboxentryInputFile->clear_items();
	//entryOutputPath->set_text("");
}


void wxAnalysis::clearStatubarInfo()
{
	statusbar1->pop(1);
	statusbar1->push("",1);
}

void wxAnalysis::setStatubarInfo(Glib::ustring text)
{
	statusbar1->pop(1);
	statusbar1->push(text,1);
}

void wxAnalysis::on_buttonReset_Clicked()
{
	Glib::ustring name = notebook1->get_tab_label_text(
		*notebook1->get_nth_page(notebook1->get_current_page()));

	name = name.uppercase();
	SetDefaultValues(name);
}






