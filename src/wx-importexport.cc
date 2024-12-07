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

#include "wx-importexport.h"
#include "wx-global.h"
#include "wx-settings.h"
#include "wx-textEditor.h"


wxImportExport::wxImportExport() 	
{

}
wxImportExport::~wxImportExport(void)
{	

}







Glib::ustring wxImportExport::ImportORCSCO(const gchar* filename)
{
	try
	{
		Glib::ustring tempOrc = "";
		Glib::ustring tempSco = "";
		Glib::ustring SciEditFileName = filename;

		//string OrcFileName = SciEditFileName.Remove(SciEditFileName.Length - 4, 4) + ".orc";
		//string ScoFileName = SciEditFileName.Remove(SciEditFileName.Length - 4, 4) + ".sco";
		Glib::ustring OrcFileName = SciEditFileName.substr(0, SciEditFileName.size() - 4);
		OrcFileName.append(".orc");
		Glib::ustring ScoFileName = SciEditFileName.substr(0, SciEditFileName.size() - 4);
		ScoFileName.append(".sco");

		
		//Verify that files exists and load them 
		if (Glib::file_test(OrcFileName, Glib::FILE_TEST_EXISTS))
		{
			//tempOrc = File.ReadAllText(OrcFileName, System.Text.Encoding.Default);
			tempOrc = Glib::file_get_contents(OrcFileName);
		}
		if (Glib::file_test(ScoFileName, Glib::FILE_TEST_EXISTS))
		{
			//tempSco = File.ReadAllText(ScoFileName, System.Text.Encoding.Default);
			tempSco = Glib::file_get_contents(ScoFileName);
		}

		Glib::ustring tempString = 
			Glib::ustring::compose("<CsoundSynthesizer>\n\n"
			                       "<CsOptions>\n"
			                       "</CsOptions>\n\n"
			                       "<CsInstruments>\n\n"
			                       "%1\n\n" //tempOrc + newline + newline +
			                       "</CsInstruments>\n\n"
			                       "<CsScore>\n\n"
			                       "%2\n\n" //tempSco + newline + newline +
			                       "</CsScore>\n\n"
			                       "</CsoundSynthesizer>\n\n", tempOrc, tempSco);

		
		////SciEditFileName = SciEditFileName.Remove(SciEditFileName.Length - 4, 4) + ".csd";
		//SciEditFileName = Path.GetFileNameWithoutExtension(SciEditFileName) + ".csd";
		//Glib::ustring ret = SciEditFileName.substr(0, SciEditFileName.size() - 4);
		//ret.append(".csd");

			
		return tempString;
		
	}
	catch (...)
	{
		wxGLOBAL->DebugPrint("wxImportExport - ImportORCSCO Error ");
	}

	return "";

}



//ORC
void wxImportExport::ImportORC(wxTextEditor* textEditor, const gchar* OrcFileName)
{
	try
	{
		Glib::ustring tempOrc = Glib::file_get_contents(OrcFileName);
		if (tempOrc == "") return;

		//tempOrc = tempOrc.Insert(0, newline + newline);
		tempOrc.insert(0, "\n\n");
		tempOrc.append("\n");

		//wxGLOBAL->DebugPrint("", tempOrc.c_str());
		
		gint startInstr = 0;
		gint endInstr = 0;

		//Search for <CsInstruments> section: start and end
		//Find <CsInstruments> 
		startInstr = 
			textEditor->FindText("<CsInstruments>", true, true, false, false, false, true, 0, -1);
		if (startInstr == -1)
		{
			wxGLOBAL->ShowMessageBox("<CsInstruments> tag not found!\n"
			                         "Please insert it in the code and retry.", 
			                         "Import Orc Error!",
			                         Gtk::BUTTONS_OK);
			return;
		}

		//Find </CsInstruments> 
		endInstr = 
			textEditor->FindText("</CsInstruments>", true, true, false, false, false, true, 0, -1);
		if (endInstr == -1)
		{
			wxGLOBAL->ShowMessageBox("</CsInstruments> tag not found!\n"
			                         "Please insert it in the code and retry.",
			                         "Import Orc Error!",
			                         Gtk::BUTTONS_OK);
			return;
		}

		textEditor->setSelection(startInstr + strlen("<CsInstruments>"), endInstr);

		//Replace/Insert text
		Glib::ustring temp = textEditor->ConvertEolOfString(tempOrc.c_str());
		textEditor->setSelectedText(temp.c_str());
	}
	catch (...)
	{
		wxGLOBAL->DebugPrint("wxImportExport - ImportORC Error ");
	}

}


//SCO
void wxImportExport::ImportSCO(wxTextEditor* textEditor, const gchar* ScoFileName)
{


	try
	{
		Glib::ustring tempSco = Glib::file_get_contents(ScoFileName);
		if (tempSco == "") return;

		//tempSco = tempSco.Insert(0, newline + newline);
		tempSco.insert(0, "\n\n");
		tempSco.append("\n");

		//wxGLOBAL->DebugPrint("", tempSco.c_str());
		
		gint startScore = 0;
		gint endScore = 0;

		//Search for <CsScore> section: start and end
		//Find <CsScore> 
		startScore = 
			textEditor->FindText("<CsScore>", true, true, false, false, false, true, 0, -1);
		if (startScore == -1)
		{
			wxGLOBAL->ShowMessageBox("<CsScore> tag not found!\n"
			                         "Please insert it in the code and retry.", 
			                         "Import Sco Error!",
			                         Gtk::BUTTONS_OK);
			return;
		}

		//Find </CsScore> 
		endScore =
			textEditor->FindText("</CsScore>", true, true, false, false, false, true, 0, -1);
		if (endScore == -1)
		{
			wxGLOBAL->ShowMessageBox("</CsScore> tag not found!\n"
			                         "Please insert it in the code and retry.",
			                         "Import Sco Error!",
			                         Gtk::BUTTONS_OK);
			return;
		}

		textEditor->setSelection(startScore + strlen("<CsScore>"), endScore);

		//Replace/Insert text
		Glib::ustring temp = textEditor->ConvertEolOfString(tempSco.c_str());
		textEditor->setSelectedText(temp.c_str());
	}
	catch (...)
	{
		wxGLOBAL->DebugPrint("wxImportExport - ImportSCO Error ");
	}

}




////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////
// EXPORT CLASSES
void wxImportExport::ExportOrcSco(wxTextEditor* textEditor, const gchar* filename)
{

	gint startInstr = 0;
	gint endInstr = 0;

	Glib::ustring tempOrc = "";
	Glib::ustring tempSco = "";

	//Search for <CsInstruments> section: start and end
	//Find <CsInstruments> 
	startInstr = 
		textEditor->FindText("<CsInstruments>", true, true, false, false, false, true, 0, -1);
	if (startInstr == -1)
	{
		wxGLOBAL->ShowMessageBox("<CsInstruments> tag not found!\n"
		                         "Please insert it in the code and retry.", 
		                         "Export Orc/Sco Error!",
		                         Gtk::BUTTONS_OK);
		return;
	}

	//Find </CsInstruments> 
	endInstr = 
		textEditor->FindText("</CsInstruments>", true, true, false, false, false, true, 0, -1);
	if (endInstr == -1)
	{
		wxGLOBAL->ShowMessageBox("</CsInstruments> tag not found!\n"
		                         "Please insert it in the code and retry.",
		                         "Export Orc/Sco Error!",
		                         Gtk::BUTTONS_OK);
		return;
	}



	
	gint startScore = 0;
	gint endScore = 0;

	//Search for <CsScore> section: start and end
	//Find <CsScore> 
	startScore = 
		textEditor->FindText("<CsScore>", true, true, false, false, false, true, 0, -1);
	if (startScore == -1)
	{
		wxGLOBAL->ShowMessageBox("<CsScore> tag not found!\n"
		                         "Please insert it in the code and retry.", 
		                         "Export Orc/Sco Error!",
		                         Gtk::BUTTONS_OK);
		return;
	}

	//Find </CsScore> 
	endScore =
		textEditor->FindText("</CsScore>", true, true, false, false, false, true, 0, -1);
	if (endScore == -1)
	{
		wxGLOBAL->ShowMessageBox("</CsScore> tag not found!\n"
		                         "Please insert it in the code and retry.",
		                         "Export Orc/Sco Error!",
		                         Gtk::BUTTONS_OK);
		return;
	}


	//Retrieve text
	tempOrc = textEditor->getTextRange(startInstr + strlen("<CsInstruments>"), endInstr);
	tempSco = textEditor->getTextRange(startScore + strlen("<CsScore>"), endScore);
	


	Gtk::FileChooserDialog dialog("Export CSD File to Orc/Sco",
	                              Gtk::FILE_CHOOSER_ACTION_SAVE);
	//dialog.set_transient_for(*this);
	dialog.set_modal(TRUE);
	dialog.set_current_folder(wxSETTINGS->Directory.LastUsedPath);

	//Add response buttons the the dialog:
	dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	dialog.add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_OK);

	/*
	Gtk::FileFilter filter_supported_files;
	filter_supported_files.set_name("CSD files");
	//filter_supported_files.add_mime_type("text/plain");
	filter_supported_files.add_pattern("*.csd");
	dialog.add_filter(filter_supported_files);
	*/
	 

	Glib::ustring t1 = Glib::path_get_basename(filename);
	Glib::ustring t2 = t1.substr(0, t1.size() - 4);
	dialog.set_current_name(t2);

	//If the WorkingDir is not empty and exists add it to the Open Dialog Box:
	if(Glib::file_test(wxSETTINGS->Directory.WorkingDir, 
	                   (Glib::FILE_TEST_EXISTS | Glib::FILE_TEST_IS_DIR)))
	{
		dialog.add_shortcut_folder(wxSETTINGS->Directory.WorkingDir);
	}
	

	int result = dialog.run();

	if (result != Gtk::RESPONSE_OK)
	{
		return;
	}

	//ActiveEditor.SaveFile(ActiveEditor.FileName);
	std::string file = dialog.get_filename();
	wxSETTINGS->Directory.LastUsedPath = dialog.get_current_folder();

	
	Glib::file_set_contents(Glib::ustring::compose("%1.orc",file), wxGLOBAL->Trim(tempOrc));
	Glib::file_set_contents(Glib::ustring::compose("%1.sco",file), wxGLOBAL->Trim(tempSco));

	wxGLOBAL->ShowMessageBox("Export done!",
	                         "Export Orc/Sco",
	                         Gtk::BUTTONS_OK);

}



void wxImportExport::ExportORC(wxTextEditor* textEditor, const gchar* filename)
{

	gint startInstr = 0;
	gint endInstr = 0;

	Glib::ustring tempOrc = "";

	//Search for <CsInstruments> section: start and end
	//Find <CsInstruments> 
	startInstr = 
		textEditor->FindText("<CsInstruments>", true, true, false, false, false, true, 0, -1);
	if (startInstr == -1)
	{
		wxGLOBAL->ShowMessageBox("<CsInstruments> tag not found!\n"
		                         "Please insert it in the code and retry.", 
		                         "Export Orc/Sco Error!",
		                         Gtk::BUTTONS_OK);
		return;
	}

	//Find </CsInstruments> 
	endInstr = 
		textEditor->FindText("</CsInstruments>", true, true, false, false, false, true, 0, -1);
	if (endInstr == -1)
	{
		wxGLOBAL->ShowMessageBox("</CsInstruments> tag not found!\n"
		                         "Please insert it in the code and retry.",
		                         "Export Orc/Sco Error!",
		                         Gtk::BUTTONS_OK);
		return;
	}


	//Retrieve text
	tempOrc = textEditor->getTextRange(startInstr + strlen("<CsInstruments>"), endInstr);
	
	Gtk::FileChooserDialog dialog("Export Orchestra Section",
	                              Gtk::FILE_CHOOSER_ACTION_SAVE);
	//dialog.set_transient_for(*this);
	dialog.set_modal(TRUE);
	dialog.set_current_folder(wxSETTINGS->Directory.LastUsedPath);

	//Add response buttons the the dialog:
	dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	dialog.add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_OK);

	/*
	Gtk::FileFilter filter_supported_files;
	filter_supported_files.set_name("CSD files");
	//filter_supported_files.add_mime_type("text/plain");
	filter_supported_files.add_pattern("*.csd");
	dialog.add_filter(filter_supported_files);
	*/


	Glib::ustring t1 = Glib::path_get_basename(filename);
	Glib::ustring t2 = t1.substr(0, t1.size() - 4);
	dialog.set_current_name(t2);

	//If the WorkingDir is not empty and exists add it to the Open Dialog Box:
	if(Glib::file_test(wxSETTINGS->Directory.WorkingDir, 
	                   (Glib::FILE_TEST_EXISTS | Glib::FILE_TEST_IS_DIR)))
	{
		dialog.add_shortcut_folder(wxSETTINGS->Directory.WorkingDir);
	}

	
	
	int result = dialog.run();

	if (result != Gtk::RESPONSE_OK)
	{
		return;
	}

	//ActiveEditor.SaveFile(ActiveEditor.FileName);
	std::string file = dialog.get_filename();
	wxSETTINGS->Directory.LastUsedPath = dialog.get_current_folder();

	Glib::file_set_contents(Glib::ustring::compose("%1.orc",file), wxGLOBAL->Trim(tempOrc));


	wxGLOBAL->ShowMessageBox("Export done!",
	                         "Export Orc",
	                         Gtk::BUTTONS_OK);	

}


void wxImportExport::ExportSCO(wxTextEditor* textEditor, const gchar* filename)
{
	gint startInstr = 0;
	gint endInstr = 0;

	Glib::ustring tempSco = "";
	
	gint startScore = 0;
	gint endScore = 0;

	//Search for <CsScore> section: start and end
	//Find <CsScore> 
	startScore = 
		textEditor->FindText("<CsScore>", true, true, false, false, false, true, 0, -1);
	if (startScore == -1)
	{
		wxGLOBAL->ShowMessageBox("<CsScore> tag not found!\n"
		                         "Please insert it in the code and retry.", 
		                         "Export Orc/Sco Error!",
		                         Gtk::BUTTONS_OK);
		return;
	}

	//Find </CsScore> 
	endScore =
		textEditor->FindText("</CsScore>", true, true, false, false, false, true, 0, -1);
	if (endScore == -1)
	{
		wxGLOBAL->ShowMessageBox("</CsScore> tag not found!\n"
		                         "Please insert it in the code and retry.",
		                         "Export Orc/Sco Error!",
		                         Gtk::BUTTONS_OK);
		return;
	}


	//Retrieve text
	tempSco = textEditor->getTextRange(startScore + strlen("<CsScore>"), endScore);
	
	Gtk::FileChooserDialog dialog("Export Score Section",
	                              Gtk::FILE_CHOOSER_ACTION_SAVE);
	//dialog.set_transient_for(*this);
	dialog.set_modal(TRUE);
	dialog.set_current_folder(wxSETTINGS->Directory.LastUsedPath);

	//Add response buttons the the dialog:
	dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	dialog.add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_OK);

	/*
	Gtk::FileFilter filter_supported_files;
	filter_supported_files.set_name("CSD files");
	//filter_supported_files.add_mime_type("text/plain");
	filter_supported_files.add_pattern("*.csd");
	dialog.add_filter(filter_supported_files);
	*/
	 

	Glib::ustring t1 = Glib::path_get_basename(filename);
	Glib::ustring t2 = t1.substr(0, t1.size() - 4);
	dialog.set_current_name(t2);

	//If the WorkingDir is not empty and exists add it to the Open Dialog Box:
	if(Glib::file_test(wxSETTINGS->Directory.WorkingDir, 
	                   (Glib::FILE_TEST_EXISTS | Glib::FILE_TEST_IS_DIR)))
	{
		dialog.add_shortcut_folder(wxSETTINGS->Directory.WorkingDir);
	}

	
	
	int result = dialog.run();

	if (result != Gtk::RESPONSE_OK)
	{
		return;
	}

	//ActiveEditor.SaveFile(ActiveEditor.FileName);
	std::string file = dialog.get_filename();
	wxSETTINGS->Directory.LastUsedPath = dialog.get_current_folder();

	Glib::file_set_contents(Glib::ustring::compose("%1.sco",file), wxGLOBAL->Trim(tempSco));

	wxGLOBAL->ShowMessageBox("Export done!",
	                         "Export Sco",
	                         Gtk::BUTTONS_OK);	

}

