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

//Class Usage:
//wxSettings::getInstance()->setCurrentMode();


#include <iostream>
#include "wx-settings.h"
#include "wx-global.h"

using namespace Glib;

#define MAX_RECENT_FILES 6



// Static data must be initialized.
wxSettings* wxSettings::instance = NULL;

wxSettings* wxSettings::getInstance(void)
{
	/*
	 * The first time getInstance() is called, create the wxSettings.
		 * Thereafter, just return a pointer to the already created one.
	 */
	if (instance == NULL)
	{
		instance = new wxSettings();
	}

	return instance;
}

wxSettings::wxSettings()
{
	d = "|";
	//MAX_RECENT_FILES = 6;


	//Create WinXound Settings Window
	Glib::RefPtr<Gtk::Builder> builder;
	try
	{
		builder = Gtk::Builder::create_from_file(
			Glib::ustring::compose("%1/winxound_settings.ui",wxGLOBAL->getSrcPath()));
	}
	catch (const Glib::FileError & ex)
	{
		wxGLOBAL->DebugPrint("wxSettings constructor - Builder Error - Critical",
		                     ex.what().c_str());
		return;
	}
	catch (...)
	{
		wxGLOBAL->DebugPrint("wxSettings constructor - Builder Error - Critical");
		return;
	}

	//Locate and Associate wxMainWindow
	builder->get_widget("windowSettings", settingsWindow);
	associateWidgets(builder);
	settingsWindow->set_border_width(2);
	settingsWindow->signal_key_press_event().connect(
		sigc::mem_fun(*this, &wxSettings::on_key_press_event));

	Glib::ustring iconfile = Glib::ustring::compose("%1/winxound_48.png",wxGLOBAL->getIconsPath());
	if(Glib::file_test(iconfile, Glib::FILE_TEST_EXISTS))
		settingsWindow->set_icon_from_file(iconfile);

	settingsWindow->set_title(Glib::ustring::compose("WinXound Settings (%1)",
	                                                 wxGLOBAL->getSettingsPath()));

	//settingsWindow->set_size_request(760,500);
	
	//settingsWindow->hide();
	//settingsWindow->set_title(TITLE);
	//settingsWindow->move(0, 0);
	//settingsWindow->resize(900,600);

	
	//Initialize List's and Create the default settings
	EditorProperties.CSoundStyles = NULL;
	EditorProperties.PythonStyles = NULL;
	EditorProperties.LuaStyles = NULL;
	CreateDefaultWinXoundSettings();


	treeCSoundColors = Gtk::manage(new Gtk::TreeView());
	treePythonColors = Gtk::manage(new Gtk::TreeView());
	treeLuaColors = Gtk::manage(new Gtk::TreeView());

	CreateSyntaxPanelItems(EditorProperties.CSoundStyles);
	CreateSyntaxPanelItems(EditorProperties.PythonStyles);
	CreateSyntaxPanelItems(EditorProperties.LuaStyles);	
	
}

wxSettings::~wxSettings()
{
	FreeSyntaxLists();
	
	delete settingsWindow;
	
	std::cout << "wxSettings released" << std::endl;
}

void wxSettings::TEST()
{

}



////////////////////




//Styles format reminders
//Styles 0-255 for Scintilla
//Style 256 = TextSelection
//Style 257 = Bookmarks
//Style 258 = VerticalRuler
//Style 259 = MarkCaretLine



//Useful methods for styles

//Useful Methods
Glib::ustring wxSettings::StyleGetFriendlyName(Glib::ustring language, gint stylenumber)
{
	tempList = SelectLanguage(language);
	if (tempList != NULL)
	{
		//foreach (string s in tempList)
		for(int i = 0; i < (int)g_list_length(tempList); i++)
		{
			if (GetValueAt((gchar*)g_list_nth(tempList, i)->data,
			               0) == wxGLOBAL->IntToString(stylenumber))
			{
				//return GetValues(s)[7];
				return GetValueAt((gchar*)g_list_nth(tempList, i)->data, 7);
			}
		}
	}
	return "";
}

Glib::ustring wxSettings::StyleGetForeColor(Glib::ustring language, gint stylenumber)
{
	tempList = SelectLanguage(language);
	if (tempList != NULL)
	{
		for(int i = 0; i < (int)g_list_length(tempList); i++)
		{
			if (GetValueAt((gchar*)g_list_nth(tempList, i)->data,
			               0) == wxGLOBAL->IntToString(stylenumber))
			{
				return GetValueAt((gchar*)g_list_nth(tempList, i)->data, 1);
			}
		}
		//32 = Default Scintilla Style
        return StyleGetForeColor(language, 32);
	}
	return "#000000"; //Default Fore color (Black)
}

Glib::ustring wxSettings::StyleGetBackColor(Glib::ustring language, gint stylenumber)
{
	tempList = SelectLanguage(language);
	if (tempList != NULL)
	{
		for(int i = 0; i < (int)g_list_length(tempList); i++)
		{
			if (GetValueAt((gchar*)g_list_nth(tempList, i)->data,
			               0) == wxGLOBAL->IntToString(stylenumber))
			{
				return GetValueAt((gchar*)g_list_nth(tempList, i)->data, 2);
			}
		}
		return StyleGetBackColor(language, 32);
	}
	return "#FFFFFF"; //Default Back color (White);
}

bool wxSettings::StyleGetBold(Glib::ustring language, gint stylenumber)
{
	tempList = SelectLanguage(language);
	if (tempList != NULL)
	{
		for(int i = 0; i < (int)g_list_length(tempList); i++)
		{
			if (GetValueAt((gchar*)g_list_nth(tempList, i)->data,
			               0) == wxGLOBAL->IntToString(stylenumber))
			{
				return wxGLOBAL->StringToBool(
				                              GetValueAt((gchar*)g_list_nth(tempList, i)->data, 3));
			}
		}
		return StyleGetBold(language, 32);
	}
	return false;
}

bool wxSettings::StyleGetItalic(Glib::ustring language, gint stylenumber)
{
	tempList = SelectLanguage(language);
	if (tempList != NULL)
	{
		for(int i = 0; i < (int)g_list_length(tempList); i++)
		{
			if (GetValueAt((gchar*)g_list_nth(tempList, i)->data,
			               0) == wxGLOBAL->IntToString(stylenumber))
			{
				return wxGLOBAL->StringToBool(
				                              GetValueAt((gchar*)g_list_nth(tempList, i)->data, 4));
			}
		}
		return StyleGetItalic(language, 32);
	}
	return false;
}

gint wxSettings::StyleGetAlpha(Glib::ustring language, gint stylenumber)
{
	tempList = SelectLanguage(language);
	if (tempList == NULL) return 40;

	for(int i = 0; i < (int)g_list_length(tempList); i++)
	{
		if (GetValueAt((gchar*)g_list_nth(tempList, i)->data,
		               0) == wxGLOBAL->IntToString(stylenumber))
		{
			return atoi(GetValueAt((gchar*)g_list_nth(tempList, i)->data, 5).c_str());
		}
	}
	return 40; //Default Alpha for Bookmarks;
}

bool wxSettings::StyleGetEolFilled(Glib::ustring language, gint stylenumber)
{
	tempList = SelectLanguage(language);
	if (tempList == NULL) return false;

	for(int i = 0; i < (int)g_list_length(tempList); i++)
	{
		if (GetValueAt((gchar*)g_list_nth(tempList, i)->data,
		               0) == wxGLOBAL->IntToString(stylenumber))
		{
			return wxGLOBAL->StringToBool(
							 GetValueAt((gchar*)g_list_nth(tempList, i)->data, 6));
		}
	}
	return false;
}

gint wxSettings::StyleGetStyleNumber(Glib::ustring text)
{
	gint i = atoi(GetValueAt(text, 0).c_str());
	if (i > -1) return i;
	
	return 0;
}


GList* wxSettings::SelectLanguage(Glib::ustring language)
{
	if(language == "csound")
		return EditorProperties.CSoundStyles;
	else if (language == "python")
		return EditorProperties.PythonStyles;
	else if (language == "lua")
		return EditorProperties.LuaStyles;
	return NULL;
}

Glib::ustring wxSettings::GetValueAt(Glib::ustring s, gint i)
{
	try
	{
		Glib::ustring ret = "";
		
		if(i == 0)
		{
			ret = strtok ((char*)s.c_str(), ",");
		}
		else
		{
			ret = strtok ((char*)s.c_str(), ",");
			for(gint pos = 0; pos < i; pos++)
			{
				ret = strtok(NULL, ",");
			}
		}

		return ret;
	}
	catch (...)
	{
		wxGLOBAL->DebugPrint("wxSettings.GetValueAt");
		return "";
	}

}










///////////////////////////////////////////////////////////////////////////////
// LOAD SETTINGS
///////////////////////////////////////////////////////////////////////////////
bool wxSettings::LoadSettings(bool readOnly)
{
	Glib::ustring temp = wxGLOBAL->getSettingsPath();
	temp.append("/WinXoundSettings.txt");
	return LoadSettings(temp, readOnly);
}

bool wxSettings::LoadSettings(Glib::ustring filename, bool readOnly)
{
	//Read datas from file
	gchar* content = NULL;
	
	try
	{
		if(Glib::file_test(filename, Glib::FILE_TEST_EXISTS))
		{
			GError *err = NULL;
			g_file_get_contents (filename.c_str(), &content, NULL, &err);
		}
		else
		{
			General.FirstStart = true;
			CreateDefaultWinXoundSettings();
			if (readOnly == false)
				SaveSettings();
		}
		
	}
	catch (...)
	{
		wxGLOBAL->DebugPrint("wxSettings.LoadSettings Error - Create default");
		CreateDefaultWinXoundSettings();
	}

	
	
	Glib::ustring input;
	Glib::ustring field;
	Glib::ustring value;


	if (content != NULL)
	{
		FreeSyntaxLists();	
		General.CSoundAdditionalFlags = "";		
		
		input = strtok (content,"\n");
		field = "";
		value = "";

		while (input != "")
		{
			
			field = GetField(input);
			if(field == "") break;
			
			value = GetValue(input);
			if(value.length() < 1)
			{
				input = strtok (NULL, "\n");
				continue;
			}

			//wxGLOBAL->DebugPrint(field.c_str(), value.c_str());

			
			
			try
			{
				//case "VERSION":
				if(field == "VERSION"){}
						//this.VERSION = value;

				//GENERAL
				//case "General.ShowUtilitiesMessage":
				else if(field =="General.ShowUtilitiesMessage")
						General.ShowUtilitiesMessage =
							wxGLOBAL->StringToBool(value);

				//case "General.ShowImportOrcScoMessage":
				else if(field == "General.ShowImportOrcScoMessage")
						General.ShowImportOrcScoMessage =
							wxGLOBAL->StringToBool(value);

				//case "General.ShowReadOnlyFileMessage":
				else if(field == "General.ShowReadOnlyFileMessage")
						General.ShowReadOnlyFileMessage =
							wxGLOBAL->StringToBool(value);

				/*//case "General.BringWinXoundToFrontForCabbage":
				else if(field == "General.BringWinXoundToFrontForCabbage")
						General.BringWinXoundToFrontForCabbage =
							wxGLOBAL->StringToBool(value);*/
				

				//case "General.WindowState":
				else if(field == "General.WindowState"){}
						//this.General.WindowState =
						//	(FormWindowState)Enum.Parse(typeof(FormWindowState),
						//	                            value);

				//case "General.WindowSize":
				else if(field == "General.WindowSize")
				{
					General.WindowSize = 
						wxGLOBAL->StringToPoint(value);
				}

				//case "General.WindowPosition":
				else if(field == "General.WindowPosition")
				{
					General.WindowPosition = 
						wxGLOBAL->StringToPoint(value);
				}

				//case "General.CompilerWindowSize":
				else if(field == "General.CompilerWindowSize")
				{
					General.CompilerWindowSize = 
						wxGLOBAL->StringToPoint(value);
				}

				//case "General.CompilerWindowPosition":
				else if(field == "General.CompilerWindowPosition")
				{
					General.CompilerWindowPosition = 
						wxGLOBAL->StringToPoint(value);
				}

				//case "General.FirstStart":
				else if(field == "General.FirstStart")
						General.FirstStart =
							wxGLOBAL->StringToBool(value);

				//case "General.FindWholeWord":
				else if(field == "General.FindWholeWord")
						General.FindWholeWord =
							wxGLOBAL->StringToBool(value);

				//case "General.FindMatchCase":
				else if(field == "General.FindMatchCase")
						General.FindMatchCase =
							wxGLOBAL->StringToBool(value);

				//case "General.ReplaceFromCaret":
				else if(field == "General.ReplaceFromCaret")
						General.ReplaceFromCaret =
							wxGLOBAL->StringToBool(value);

				//case "General.ShowToolbar":
				else if(field == "General.ShowToolbar")
						General.ShowToolbar =
							wxGLOBAL->StringToBool(value);

				//case "General.StartupAction":
				else if(field == "General.StartupAction")
						General.StartupAction =
							atoi(value.c_str());

				//case "General.OrcScoImport":
				else if(field == "General.OrcScoImport")
						General.OrcScoImport =
							atoi(value.c_str());
				
				//case "General.DefaultWavePlayer":
				else if(field == "General.DefaultWavePlayer")
						General.DefaultWavePlayer =
							atoi(value.c_str());
				


				//TOOLBAR ELEMENTS
				//case "General.ToolBarItems":
				//		General.ToolBarItems.Add(
				//		                              Convert.ToBoolean(value);
				//		break;

				//case "General.CSoundDefaultFlags":
				else if(field == "General.CSoundDefaultFlags")
						General.CSoundDefaultFlags = value;

				//case "General.OpenSoundFileWith":
				else if(field == "General.OpenSoundFileWith")
						General.OpenSoundFileWith =
							atoi(value.c_str());

				//case "General.PythonDefaultFlags":
				else if(field == "General.PythonDefaultFlags")
						General.PythonDefaultFlags = value;

				//case "General.CompilerFontName":
				else if(field == "General.CompilerFontName")
						General.CompilerFontName = value;

				//case "General.CompilerFontSize":
				else if(field == "General.CompilerFontSize")
						General.CompilerFontSize =
							atoi(value.c_str());

				//case "General.LuaDefaultFlags":
				else if(field == "General.LuaDefaultFlags")
						General.LuaDefaultFlags = value;

				//case "General.UseWinXoundFlags":
				else if(field == "General.UseWinXoundFlags")
						General.UseWinXoundFlags =
							wxGLOBAL->StringToBool(value);

				/*//case "General.CheckForCabbageUpdates":
				else if(field == "General.CheckForCabbageUpdates")
						General.CheckForCabbageUpdates =
							wxGLOBAL->StringToBool(value);*/

				//case "General.CSoundAdditionalFlags":
				else if(field == "General.CSoundAdditionalFlags")
				{
					General.CSoundAdditionalFlags.append(value);
					General.CSoundAdditionalFlags.append("\n");
				}
				

				//DIRECTORY
				//case "Directory.WavePlayer":
				else if(field == "Directory.WavePlayer")
						Directory.WavePlayer = value;

				//case "Directory.WaveEditor":
				else if(field == "Directory.WaveEditor")
						Directory.WaveEditor = value;

				//case "Directory.Calculator":
				else if(field == "Directory.Calculator")
						Directory.Calculator = value;

				//case "Directory.WorkingDir":
				else if(field == "Directory.WorkingDir")
						Directory.WorkingDir = value;

				//case "Directory.LastUsedPath":
				else if(field == "Directory.LastUsedPath")
						Directory.LastUsedPath = value;

				//case "Directory.CSoundConsole":
				else if(field == "Directory.CSoundConsole")
						Directory.CSoundConsole = value;

				//case "Directory.Winsound":
				else if(field == "Directory.Winsound")
						Directory.Winsound = value;

				//case "Directory.CSoundAV":
				else if(field == "Directory.CSoundAV")
						Directory.CSoundAV = value;

				//case "Directory.CSoundAVHelp":
				else if(field == "Directory.CSoundAVHelp")
						Directory.CSoundAVHelp = value;

				//case "Directory.CSoundHelpHTML":
				else if(field == "Directory.CSoundHelpHTML")
						Directory.CSoundHelpHTML = value;

				//case "Directory.SFDIR":
				else if(field == "Directory.SFDIR")
						Directory.SFDIR = value;

				//case "Directory.SSDIR":
				else if(field == "Directory.SSDIR")
						Directory.SSDIR = value;

				//case "Directory.SADIR":
				else if(field == "Directory.SADIR")
						Directory.SADIR = value;

				//case "Directory.MFDIR":
				else if(field == "Directory.MFDIR")
						Directory.MFDIR = value;

				//case "Directory.INCDIR":
				else if(field == "Directory.INCDIR")
						Directory.INCDIR = value;

				//case "Directory.OPCODEDIR":
				else if(field == "Directory.OPCODEDIR")
						Directory.OPCODEDIR = value;

				//case "Directory.UseSFDIR":
				else if(field == "Directory.UseSFDIR")
						Directory.UseSFDIR =
							wxGLOBAL->StringToBool(value);

				//case "Directory.PythonConsolePath":
				else if(field == "Directory.PythonConsolePath")
						Directory.PythonConsolePath = value;

				//case "Directory.PythonIdlePath":
				else if(field == "Directory.PythonIdlePath")
						Directory.PythonIdlePath = value;

				//case "Directory.LuaConsolePath":
				else if(field == "Directory.LuaConsolePath")
						Directory.LuaConsolePath = value;

				//case "Directory.LuaGuiPath":
				else if(field == "Directory.LuaGuiPath")
						Directory.LuaGuiPath = value;

				/*//case "Directory.CabbagePath":
				else if(field == "Directory.CabbagePath")
						Directory.CabbagePath = value;*/
				

				
				//EDITORPROPERTIES
				//case "EditorProperties.DefaultFontName":
				else if(field == "EditorProperties.DefaultFontName")
						EditorProperties.DefaultFontName = value;

				//case "EditorProperties.DefaultFontSize":
				else if(field == "EditorProperties.DefaultFontSize")
						EditorProperties.DefaultFontSize =
							atoi(value.c_str());

				//case "EditorProperties.DefaultTabSize":
				else if(field == "EditorProperties.DefaultTabSize")
						EditorProperties.DefaultTabSize =
							atoi(value.c_str());

				//case "EditorProperties.ShowVerticalRuler":
				else if(field == "EditorProperties.ShowVerticalRuler")
						EditorProperties.ShowVerticalRuler =
							wxGLOBAL->StringToBool(value);

				//case "EditorProperties.ShowMatchingBracket":
				else if(field == "EditorProperties.ShowMatchingBracket")
						EditorProperties.ShowMatchingBracket =
							wxGLOBAL->StringToBool(value);

				//case "EditorProperties.ShowLineNumbers":
				else if(field == "EditorProperties.ShowLineNumbers")
						EditorProperties.ShowLineNumbers =
							wxGLOBAL->StringToBool(value);

				//case "EditorProperties.ShowFoldLine":
				else if(field == "EditorProperties.ShowFoldLine")
						EditorProperties.ShowFoldLine =
							wxGLOBAL->StringToBool(value);

				//case "EditorProperties.ShowIntelliTip":
				else if(field == "EditorProperties.ShowIntelliTip")
						EditorProperties.ShowIntelliTip =
							wxGLOBAL->StringToBool(value);

				//case "EditorProperties.ShowExplorer":
				else if(field == "EditorProperties.ShowExplorer")
						EditorProperties.ShowExplorer =
							wxGLOBAL->StringToBool(value);

				//case "EditorProperties.MarkCaretLine":
				else if(field == "EditorProperties.MarkCaretLine")
						EditorProperties.MarkCaretLine =
							wxGLOBAL->StringToBool(value);

				//case "EditorProperties.SaveBookmarks":
				else if(field == "EditorProperties.SaveBookmarks")
						EditorProperties.SaveBookmarks =
							wxGLOBAL->StringToBool(value);

				//case "EditorProperties.UseMixedPython":
				else if(field == "EditorProperties.UseMixedPython")
						EditorProperties.UseMixedPython =
							wxGLOBAL->StringToBool(value);

				//case "EditorProperties.ExplorerShowOptions":
				else if(field == "EditorProperties.ExplorerShowOptions")
						EditorProperties.ExplorerShowOptions =
							wxGLOBAL->StringToBool(value);

				//case "EditorProperties.ExplorerShowInstrMacros":
				else if(field == "EditorProperties.ExplorerShowInstrMacros")
						EditorProperties.ExplorerShowInstrMacros =
							wxGLOBAL->StringToBool(value);

				//case "EditorProperties.ExplorerShowInstrOpcodes":
				else if(field == "EditorProperties.ExplorerShowInstrOpcodes")
						EditorProperties.ExplorerShowInstrOpcodes =
							wxGLOBAL->StringToBool(value);

				//case "EditorProperties.ExplorerShowInstrInstruments":
				else if(field == "EditorProperties.ExplorerShowInstrInstruments")
						EditorProperties.ExplorerShowInstrInstruments =
							wxGLOBAL->StringToBool(value);

				//case "EditorProperties.ExplorerShowScoreFunctions":
				else if(field == "EditorProperties.ExplorerShowScoreFunctions")
						EditorProperties.ExplorerShowScoreFunctions =
							wxGLOBAL->StringToBool(value);

				//case "EditorProperties.ExplorerShowScoreMacros":
				else if(field == "EditorProperties.ExplorerShowScoreMacros")
						EditorProperties.ExplorerShowScoreMacros =
							wxGLOBAL->StringToBool(value);

				//case "EditorProperties.ExplorerShowScoreSections":
				else if(field == "EditorProperties.ExplorerShowScoreSections")
						EditorProperties.ExplorerShowScoreSections =
							wxGLOBAL->StringToBool(value);

				//case "EditorProperties.ExplorerFontSize":
				else if(field == "EditorProperties.ExplorerFontSize")
						EditorProperties.ExplorerFontSize =
							atoi(value.c_str());

				//STYLES
				//case "EditorProperties.CSoundStyles":
				else if(field == "EditorProperties.CSoundStyles")
				{
					EditorProperties.CSoundStyles = 
						g_list_append (EditorProperties.CSoundStyles, 
	           						   //(gpointer)value.c_str());
						               g_strdup(value.c_str()));
				}

				//case "EditorProperties.PythonStyles":
				else if(field == "EditorProperties.PythonStyles")
				{
					EditorProperties.PythonStyles = 
						g_list_append (EditorProperties.PythonStyles, 
						               //(gpointer)value.c_str());
						               g_strdup(value.c_str()));
				}

				//case "EditorProperties.LuaStyles":
				else if(field == "EditorProperties.LuaStyles")
				{
					EditorProperties.LuaStyles = 
						g_list_append (EditorProperties.LuaStyles, 
						               //(gpointer)value.c_str());
						               g_strdup(value.c_str()));
				}
				

				//CODEFORMAT
				//case "CodeFormat.FormatHeader":
				else if(field == "CodeFormat.FormatHeader")
						CodeFormat.FormatHeader =
							wxGLOBAL->StringToBool(value);

				//case "CodeFormat.FormatInstruments":
				else if(field == "CodeFormat.FormatInstruments")
						CodeFormat.FormatInstruments =
							wxGLOBAL->StringToBool(value);

				//case "CodeFormat.FormatFunctions":
				else if(field == "CodeFormat.FormatFunctions")
						CodeFormat.FormatFunctions =
							wxGLOBAL->StringToBool(value);

				//case "CodeFormat.FormatScoreInstruments":
				else if(field == "CodeFormat.FormatScoreInstruments")
						CodeFormat.FormatScoreInstruments =
							wxGLOBAL->StringToBool(value);

				//case "CodeFormat.FormatTempo":
				else if(field == "CodeFormat.FormatTempo")
						CodeFormat.FormatTempo =
							wxGLOBAL->StringToBool(value);

				//case "CodeFormat.InstrumentsType":
				else if(field == "CodeFormat.InstrumentsType")
						CodeFormat.InstrumentsType =
							atoi(value.c_str());

				//case "CodeFormat.TabIndent":
				else if(field == "CodeFormat.TabIndent")
						CodeFormat.TabIndent =
							atoi(value.c_str());
			}

			catch (...)
			{
				wxGLOBAL->DebugPrint("wxSettings.LoadSettings - While loop Error");
			}

			gchar* t = strtok (NULL, "\n");
			if (t != NULL)
				input = t;
			else
				input = "";

		}

	}

	g_free(content);


	
	if (General.CSoundAdditionalFlags == "")
		General.CSoundAdditionalFlags = getDefaultAlternativeCSoundFlags();
	else
		General.CSoundAdditionalFlags = wxGLOBAL->Trim(General.CSoundAdditionalFlags);

	
	//DEBUG:
	//std::cout << "CSound Styles NUM: " << 
	//		g_list_length(EditorProperties.CSoundStyles) << std::endl;
	

	
	
	//LOAD RECENT FILES LIST
	RecentFiles.clear();
	try
	{
		//if (File.Exists(Application.StartupPath + "\\Settings\\RecentFiles.txt"))
		Glib::ustring path = wxGLOBAL->getSettingsPath();
		path.append("/RecentFiles.txt");
		if(Glib::file_test(path, Glib::FILE_TEST_EXISTS))
		{
			Glib::ustring content = "";			
			content = Glib::file_get_contents(path);
	
			if (content != "")
			{
				//TODO:
				//g_strsplit: returns :a newly-allocated NULL-terminated array of strings. 
				//Use g_strfreev() to free it. 
				gchar** files = g_strsplit(content.c_str(), "\n", 0);
				int length = wxGLOBAL->ArrayLength(files);
				for(int i=0; i < length; i++)
				{
					if(Glib::file_test(files[i], Glib::FILE_TEST_EXISTS))
					{
						std::string f = files[i];
						RecentFiles.push_back(f);
					}
				}	
				g_strfreev(files);
			}
		}
		else //Create new RecentList file
		{
			if (readOnly == false)
			{
				//File.WriteAllText(Application.StartupPath + "\\Settings\\RecentFiles.txt","");
				Glib::ustring path = wxGLOBAL->getSettingsPath();
				path.append("/RecentFiles.txt");
				Glib::file_set_contents(path, "");
			}
		}
	}
	catch (...)
	{
		//System.Diagnostics.Debug.WriteLine("wxSettings.LoadSettings - RecentFiles Error : " + ex.Message);
		wxGLOBAL->DebugPrint("wxSettings.LoadSettings(RecentFiles) Error");
	}



	//LOAD LAST SESSION FILES
	LastSessionFiles.clear();
	try
	{
		//if (File.Exists(Application.StartupPath + "\\Settings\\RecentFiles.txt"))
		Glib::ustring pathSession = wxGLOBAL->getSettingsPath();
		pathSession.append("/LastSessionFiles.txt");
		if(Glib::file_test(pathSession, Glib::FILE_TEST_EXISTS))
		{
			Glib::ustring contentSession = "";			
			contentSession = Glib::file_get_contents(pathSession);
	
			if (contentSession != "")
			{
				//g_strsplit: returns :a newly-allocated NULL-terminated array of strings. 
				//Use g_strfreev() to free it. 
				gchar** filesSession = g_strsplit(contentSession.c_str(), "\n", 0);
				int length = wxGLOBAL->ArrayLength(filesSession);
				for(int i=0; i < length; i++)
				{
					if(Glib::file_test(filesSession[i], Glib::FILE_TEST_EXISTS))
					{
						std::string fs = filesSession[i];
						LastSessionFiles.push_back(fs);
					}
				}	
				g_strfreev(filesSession);
			}
		}
		else //Create new RecentList file
		{
			if (readOnly == false)
			{
				//File.WriteAllText(Application.StartupPath + "\\Settings\\RecentFiles.txt","");
				Glib::ustring path = wxGLOBAL->getSettingsPath();
				path.append("/LastSessionFiles.txt");
				Glib::file_set_contents(path, "");
			}
		}
	}
	catch (...)
	{
		//System.Diagnostics.Debug.WriteLine("wxSettings.LoadSettings - RecentFiles Error : " + ex.Message);
		wxGLOBAL->DebugPrint("wxSettings.LoadSettings(LastSessionFiles) Error");
	}




	

	//SET ENVIRONMENT VARIABLES
	SetEnvironmentVariables();

	
	//LOAD TEMPLATES
	CreateDefaultTemplates();
	try
	{
		Glib::ustring pathCSoundTemplate = wxGLOBAL->getSettingsPath();
		pathCSoundTemplate.append("/CSoundTemplate.txt");
		
		Glib::ustring pathPythonTemplate = wxGLOBAL->getSettingsPath();
		pathPythonTemplate.append("/PythonTemplate.txt");
		
		Glib::ustring pathLuaTemplate = wxGLOBAL->getSettingsPath();
		pathLuaTemplate.append("/LuaTemplate.txt");

		Glib::ustring pathCabbageTemplate = wxGLOBAL->getSettingsPath();
		pathCabbageTemplate.append("/CabbageTemplate.txt");

		//Csound template
		if (Glib::file_test(pathCSoundTemplate, Glib::FILE_TEST_EXISTS))
		{
			Templates.CSound = Glib::file_get_contents(pathCSoundTemplate);
		}
		else
		{
			if (readOnly == false)
			{
				Glib::file_set_contents(pathCSoundTemplate, Templates.CSound);
			}
		}

		//Python Template
		if (Glib::file_test(pathPythonTemplate, Glib::FILE_TEST_EXISTS))
		{
			Templates.Python = Glib::file_get_contents(pathPythonTemplate);
		}
		else
		{
			//Templates.Python = "";
			if (readOnly == false)
			{
				Glib::file_set_contents(pathPythonTemplate, Templates.Python);
			}
		}


		//Lua Template
		if (Glib::file_test(pathLuaTemplate, Glib::FILE_TEST_EXISTS))
		{
			Templates.Lua = Glib::file_get_contents(pathLuaTemplate);
		}
		else
		{
			//Templates.Lua = "";
			if (readOnly == false)
			{
				Glib::file_set_contents(pathLuaTemplate, Templates.Lua);
			}
		}

		//Cabbage Template
		if (Glib::file_test(pathCabbageTemplate, Glib::FILE_TEST_EXISTS))
		{
			Templates.Cabbage = Glib::file_get_contents(pathCabbageTemplate);
		}
		else
		{
			if (readOnly == false)
			{
				Glib::file_set_contents(pathCabbageTemplate, Templates.Cabbage);
			}
		}
	}
	catch (...)
	{
		//System.Diagnostics.Debug.WriteLine("wxSettings.LoadSettings - Templates Error : " + ex.Message);
		wxGLOBAL->DebugPrint("wxSettings.LoadSettings(Templates) - Error");
	}

	

	return true;

}

//Useful methods for LoadSettings
Glib::ustring wxSettings::GetField(Glib::ustring s)
{
	try
	{
		//return s.Split(d.ToCharArray())[0];
		//Glib::ustring ret = strtok ((char*)s.c_str(), d);
		int pos = s.find("|",0);
		if(pos > 0) return s.substr(0,pos);
		return "";
	}
	catch (...)
	{
		wxGLOBAL->DebugPrint("wxSettings.GetField Error");
		return "";
	}
}
Glib::ustring wxSettings::GetValue(Glib::ustring s)
{
	try
	{
		//return s.Split(d.ToCharArray())[1];
		//Glib::ustring ret = strtok ((char*)s.c_str(), d);
		//ret = strtok(NULL, d);
		int pos = s.find("|",0);
		if(pos > 0) return s.substr(pos+1);
		return "";
	}
	catch (...)
	{
		wxGLOBAL->DebugPrint("wxSettings.GetValue");
		return "";
	}
}
/*
private Point GetPointValue(string s)
{
	try
	{
		s = s.ToLower();
		s = s.Replace("{", "");
		s = s.Replace("x=", "");
		s = s.Replace("y=", "");
		s = s.Replace("}", "");

		Int32 x, y;
		x = Convert.ToInt32(s.Split(",".ToCharArray())[0]);
		y = Convert.ToInt32(s.Split(",".ToCharArray())[1]);
		return new Point(x, y);
	}
	catch (Exception ex)
	{
		System.Diagnostics.Debug.WriteLine(
		                                   "wxSettings.GetPointValue Error : " + ex.Message);
		return new Point(0, 0);
	}


}
*/









///////////////////////////////////////////////////////////////////////////////
// SAVE SETTINGS
///////////////////////////////////////////////////////////////////////////////
bool wxSettings::SaveSettings()
{
	Glib::ustring temp = wxGLOBAL->getSettingsPath();
	temp.append("/WinXoundSettings.txt");
	return SaveSettings(temp);
}

bool wxSettings::SaveSettings(const Glib::ustring filename)
{

	//NEW METHOD
	//List<string> output = new List<string>();
	Glib::ustring output = "";
	output.append("WinXoundSettingsLinux");
	output.append(d);
	output.append("3.3.0");
	output.append("\n");

	//VERSION
	//output.append("VERSION" + d + this.VERSION);

	//GENERAL
	output.append("General.ShowUtilitiesMessage");
	output.append(d);
	output.append(wxGLOBAL->BoolToString(General.ShowUtilitiesMessage));
	output.append("\n");

	output.append("General.ShowImportOrcScoMessage");
	output.append(d);
	output.append(wxGLOBAL->BoolToString(General.ShowImportOrcScoMessage));
	output.append("\n");

	output.append("General.ShowReadOnlyFileMessage");
	output.append(d);
	output.append(wxGLOBAL->BoolToString(General.ShowReadOnlyFileMessage));
	output.append("\n");

	/*output.append("General.BringWinXoundToFrontForCabbage");
	output.append(d);
	output.append(wxGLOBAL->BoolToString(General.BringWinXoundToFrontForCabbage));
	output.append("\n");*/

	//output.append("General.WindowState");
	//output.append(d);
	//output.append(General.WindowState);

	output.append("General.WindowSize");
	output.append(d);
	output.append(wxGLOBAL->PointToString(General.WindowSize));
	output.append("\n");

	output.append("General.WindowPosition");
	output.append(d);
	output.append(wxGLOBAL->PointToString(General.WindowPosition));
	output.append("\n");

	output.append("General.CompilerWindowSize");
	output.append(d);
	output.append(wxGLOBAL->PointToString(General.CompilerWindowSize));
	output.append("\n");

	output.append("General.CompilerWindowPosition");
	output.append(d);
	output.append(wxGLOBAL->PointToString(General.CompilerWindowPosition));
	output.append("\n");

	output.append("General.FirstStart");
	output.append(d);
	output.append(wxGLOBAL->BoolToString(General.FirstStart));
	output.append("\n");

	output.append("General.FindWholeWord");
	output.append(d);
	output.append(wxGLOBAL->BoolToString(General.FindWholeWord));
	output.append("\n");

	output.append("General.FindMatchCase");
	output.append(d);
	output.append(wxGLOBAL->BoolToString(General.FindMatchCase));
	output.append("\n");

	output.append("General.ReplaceFromCaret");
	output.append(d);
	output.append(wxGLOBAL->BoolToString(General.ReplaceFromCaret));
	output.append("\n");

	output.append("General.ShowToolbar");
	output.append(d);
	output.append(wxGLOBAL->BoolToString(General.ShowToolbar));
	output.append("\n");

	output.append("General.StartupAction");
	output.append(d);
	output.append(wxGLOBAL->IntToString(General.StartupAction));
	output.append("\n");

	output.append("General.OrcScoImport");
	output.append(d);
	output.append(wxGLOBAL->IntToString(General.OrcScoImport));
	output.append("\n");

	output.append("General.DefaultWavePlayer");
	output.append(d);
	output.append(wxGLOBAL->IntToString(General.DefaultWavePlayer));
	output.append("\n");

	output.append("General.CSoundDefaultFlags");
	output.append(d);
	output.append(General.CSoundDefaultFlags);
	output.append("\n");

	output.append("General.OpenSoundFileWith");
	output.append(d);
	output.append(wxGLOBAL->IntToString(General.OpenSoundFileWith));
	output.append("\n");

	output.append("General.PythonDefaultFlags");
	output.append(d);
	output.append(General.PythonDefaultFlags);
	output.append("\n");

	output.append("General.CompilerFontName");
	output.append(d);
	output.append(General.CompilerFontName);
	output.append("\n");

	output.append("General.CompilerFontSize");
	output.append(d);
	output.append(wxGLOBAL->IntToString(General.CompilerFontSize));
	output.append("\n");

	output.append("General.LuaDefaultFlags");
	output.append(d);
	output.append(General.LuaDefaultFlags);
	output.append("\n");

	output.append("General.UseWinXoundFlags");
	output.append(d);
	output.append(wxGLOBAL->BoolToString(General.UseWinXoundFlags));
	output.append("\n");

	/*output.append("General.CheckForCabbageUpdates");
	output.append(d);
	output.append(wxGLOBAL->BoolToString(General.CheckForCabbageUpdates));
	output.append("\n");*/



	//CSoundAdditionalFlags:
	gchar** flags = g_strsplit(General.CSoundAdditionalFlags.c_str(), "\n", 0);
	int length = wxGLOBAL->ArrayLength(flags);
	for(int i=0; i < length; i++)
	{
		std::string f = flags[i];
		output.append("General.CSoundAdditionalFlags");
		output.append(d);
		output.append(f);
		output.append("\n");
	}	
	g_strfreev(flags);

	



	//DIRECTORY
	output.append("Directory.WavePlayer");
	output.append(d);
	output.append(Directory.WavePlayer);
	output.append("\n");
	
	output.append("Directory.WaveEditor");
	output.append(d);
	output.append(Directory.WaveEditor);
	output.append("\n");

	output.append("Directory.Calculator");
	output.append(d);
	output.append(Directory.Calculator);
	output.append("\n");
	
	output.append("Directory.WorkingDir");
	output.append(d);
	output.append(Directory.WorkingDir);
	output.append("\n");

	output.append("Directory.LastUsedPath");
	output.append(d);
	output.append(Directory.LastUsedPath);
	output.append("\n");
	
	output.append("Directory.CSoundConsole");
	output.append(d);
	output.append(Directory.CSoundConsole);
	output.append("\n");
	
	output.append("Directory.Winsound");
	output.append(d);
	output.append(Directory.Winsound);
	output.append("\n");
	
	output.append("Directory.CSoundAV");
	output.append(d);
	output.append(Directory.CSoundAV);
	output.append("\n");
	
	output.append("Directory.CSoundAVHelp");
	output.append(d);
	output.append(Directory.CSoundAVHelp);
	output.append("\n");
	
	output.append("Directory.CSoundHelpHTML");
	output.append(d);
	output.append(Directory.CSoundHelpHTML);
	output.append("\n");
	
	output.append("Directory.SFDIR");
	output.append(d);
	output.append(Directory.SFDIR);
	output.append("\n");
	
	output.append("Directory.SSDIR");
	output.append(d);
	output.append(Directory.SSDIR);
	output.append("\n");
	
	output.append("Directory.SADIR");
	output.append(d);
	output.append(Directory.SADIR);
	output.append("\n");
	
	output.append("Directory.MFDIR");
	output.append(d);
	output.append(Directory.MFDIR);
	output.append("\n");
	
	output.append("Directory.INCDIR");
	output.append(d);
	output.append(Directory.INCDIR);
	output.append("\n");

	output.append("Directory.OPCODEDIR");
	output.append(d);
	output.append(Directory.OPCODEDIR);
	output.append("\n");
	
	output.append("Directory.UseSFDIR");
	output.append(d);
	output.append(wxGLOBAL->BoolToString(Directory.UseSFDIR));
	output.append("\n");
	
	output.append("Directory.PythonConsolePath");
	output.append(d);
	output.append(Directory.PythonConsolePath);
	output.append("\n");
	
	output.append("Directory.PythonIdlePath");
	output.append(d);
	output.append(Directory.PythonIdlePath);
	output.append("\n");
	
	output.append("Directory.LuaConsolePath");
	output.append(d);
	output.append(Directory.LuaConsolePath);
	output.append("\n");
	
	output.append("Directory.LuaGuiPath");
	output.append(d);
	output.append(Directory.LuaGuiPath);
	output.append("\n");

	/*output.append("Directory.CabbagePath");
	output.append(d);
	output.append(Directory.CabbagePath);
	output.append("\n");*/
	
	

	//EDITORPROPERTIES
	output.append("EditorProperties.DefaultFontName");
	output.append(d);
	output.append(EditorProperties.DefaultFontName);
	output.append("\n");
	
	output.append("EditorProperties.DefaultFontSize");
	output.append(d);
	output.append(wxGLOBAL->IntToString(EditorProperties.DefaultFontSize));
	output.append("\n");
	
	output.append("EditorProperties.DefaultTabSize");
	output.append(d);
	output.append(wxGLOBAL->IntToString(EditorProperties.DefaultTabSize));
	output.append("\n");
	
	output.append("EditorProperties.ShowVerticalRuler");
	output.append(d);
	output.append(wxGLOBAL->BoolToString(EditorProperties.ShowVerticalRuler));
	output.append("\n");
	
	output.append("EditorProperties.ShowMatchingBracket");
	output.append(d);
	output.append(wxGLOBAL->BoolToString(EditorProperties.ShowMatchingBracket));
	output.append("\n");
	
	output.append("EditorProperties.ShowLineNumbers");
	output.append(d);
	output.append(wxGLOBAL->BoolToString(EditorProperties.ShowLineNumbers));
	output.append("\n");

	output.append("EditorProperties.ShowFoldLine");
	output.append(d);
	output.append(wxGLOBAL->BoolToString(EditorProperties.ShowFoldLine));
	output.append("\n");
	
	output.append("EditorProperties.ShowIntelliTip");
	output.append(d);
	output.append(wxGLOBAL->BoolToString(EditorProperties.ShowIntelliTip));
	output.append("\n");
	
	output.append("EditorProperties.ShowExplorer");
	output.append(d);
	output.append(wxGLOBAL->BoolToString(EditorProperties.ShowExplorer));
	output.append("\n");
	
	output.append("EditorProperties.MarkCaretLine");
	output.append(d);
	output.append(wxGLOBAL->BoolToString(EditorProperties.MarkCaretLine));
	output.append("\n");
	
	output.append("EditorProperties.SaveBookmarks");
	output.append(d);
	output.append(wxGLOBAL->BoolToString(EditorProperties.SaveBookmarks));
	output.append("\n");
	
	output.append("EditorProperties.UseMixedPython");
	output.append(d);
	output.append(wxGLOBAL->BoolToString(EditorProperties.UseMixedPython));
	output.append("\n");
	
	output.append("EditorProperties.ExplorerShowOptions");
	output.append(d);
	output.append(wxGLOBAL->BoolToString(EditorProperties.ExplorerShowOptions));
	output.append("\n");
	
	output.append("EditorProperties.ExplorerShowInstrMacros");
	output.append(d);
	output.append(wxGLOBAL->BoolToString(EditorProperties.ExplorerShowInstrMacros));
	output.append("\n");
	
	output.append("EditorProperties.ExplorerShowInstrOpcodes");
	output.append(d);
	output.append(wxGLOBAL->BoolToString(EditorProperties.ExplorerShowInstrOpcodes));
	output.append("\n");
	
	output.append("EditorProperties.ExplorerShowInstrInstruments");
	output.append(d);
	output.append(wxGLOBAL->BoolToString(EditorProperties.ExplorerShowInstrInstruments));
	output.append("\n");
	
	output.append("EditorProperties.ExplorerShowScoreFunctions");
	output.append(d);
	output.append(wxGLOBAL->BoolToString(EditorProperties.ExplorerShowScoreFunctions));
	output.append("\n");
	
	output.append("EditorProperties.ExplorerShowScoreMacros");
	output.append(d);
	output.append(wxGLOBAL->BoolToString(EditorProperties.ExplorerShowScoreMacros));
	output.append("\n");
	
	output.append("EditorProperties.ExplorerShowScoreSections");
	output.append(d);
	output.append(wxGLOBAL->BoolToString(EditorProperties.ExplorerShowScoreSections));
	output.append("\n");
	
	output.append("EditorProperties.ExplorerFontSize");
	output.append(d);
	output.append(wxGLOBAL->IntToString(EditorProperties.ExplorerFontSize));
	output.append("\n");
	
	
	//CSound Styles
	//foreach (string s in EditorProperties.CSoundStyles)
	for (uint i = 0; i < g_list_length(EditorProperties.CSoundStyles); i++)
	{
		//output.append("EditorProperties.CSoundStyles" + d + s);
		output.append("EditorProperties.CSoundStyles");
		output.append(d);
		output.append((const gchar*)g_list_nth(EditorProperties.CSoundStyles, i)->data);
		output.append("\n");

		//std::cout << "CSound Styles: " << i << " - NUM: " << 
		//	g_list_length(EditorProperties.CSoundStyles) << std::endl;
	}
	
	//Python Styles
	//foreach (string s in EditorProperties.PythonStyles)
	for (uint i = 0; i < g_list_length(EditorProperties.PythonStyles); i++)
	{
		//output.append("EditorProperties.PythonStyles" + d + s);
		output.append("EditorProperties.PythonStyles");
		output.append(d);
		output.append((const gchar*)g_list_nth(EditorProperties.PythonStyles, i)->data);
		output.append("\n");
	}
	//Lua Styles
	//foreach (string s in EditorProperties.LuaStyles)
	for (uint i = 0; i < g_list_length(EditorProperties.LuaStyles); i++)
	{
		//output.append("EditorProperties.LuaStyles" + d + s);
		output.append("EditorProperties.LuaStyles");
		output.append(d);
		output.append((const gchar*)g_list_nth(EditorProperties.LuaStyles, i)->data);
		output.append("\n");
	}

	
	//CODEFORMAT
	output.append("CodeFormat.FormatHeader");
	output.append(d);
	output.append(wxGLOBAL->BoolToString(CodeFormat.FormatHeader));
	output.append("\n");
	
	output.append("CodeFormat.FormatInstruments");
	output.append(d);
	output.append(wxGLOBAL->BoolToString(CodeFormat.FormatInstruments));
	output.append("\n");
	
	output.append("CodeFormat.FormatFunctions");
	output.append(d);
	output.append(wxGLOBAL->BoolToString(CodeFormat.FormatFunctions));
	output.append("\n");
	
	output.append("CodeFormat.FormatScoreInstruments");
	output.append(d);
	output.append(wxGLOBAL->BoolToString(CodeFormat.FormatScoreInstruments));
	output.append("\n");
	
	output.append("CodeFormat.FormatTempo");
	output.append(d);
	output.append(wxGLOBAL->BoolToString(CodeFormat.FormatTempo));
	output.append("\n");
	
	output.append("CodeFormat.InstrumentsType");
	output.append(d);
	output.append(wxGLOBAL->IntToString(CodeFormat.InstrumentsType));
	output.append("\n");
	
	output.append("CodeFormat.TabIndent");
	output.append(d);
	output.append(wxGLOBAL->IntToString(CodeFormat.TabIndent));
	output.append("\n");
	

	
	//SAVE SETTINGS
	try
	{
		//File.WriteAllLines(filename, output.ToArray());
		//return true;
		//g_file_set_contents(filename.c_str(),		                    
		//                    output.c_str(), 
		//                    output.length(), 
		//                    NULL);
		//wxGLOBAL->DebugPrint("SAVE SETTINGS", g_get_user_config_dir());
		Glib::file_set_contents(filename, output);
	}
	catch (...)
	{
		wxGLOBAL->DebugPrint("SAVE SETTINGS ERROR");
	}



	
	//SAVE RECENT FILES LIST
	try
	{	
		//wxGLOBAL->DebugPrint("wxSettings.SaveSettings(RecentFiles)",
		//		             "SAVE RECENT LIST");
		
		Glib::ustring path = wxGLOBAL->getSettingsPath();
		path.append("/RecentFiles.txt");

		Glib::ustring text = "";

		for (uint i=0; i < RecentFiles.size(); i++ )
		{
			text.append(RecentFiles[i]);
			text.append("\n");
		}
		
		Glib::file_set_contents(path, text);
		
	}
	catch (...)
	{
		wxGLOBAL->DebugPrint("wxSettings SaveSettings (RecentFiles) - Error");
	}
	


	//SAVE LAST SESSION FILES LIST
	try
	{			
		Glib::ustring pathSession = wxGLOBAL->getSettingsPath();
		pathSession.append("/LastSessionFiles.txt");

		Glib::ustring textSession = "";

		for (uint i=0; i < LastSessionFiles.size(); i++ )
		{
			textSession.append(LastSessionFiles[i]);
			textSession.append("\n");
		}
		
		Glib::file_set_contents(pathSession, textSession);
		
	}
	catch (...)
	{
		wxGLOBAL->DebugPrint("wxSettings SaveSettings (LastSessionFiles) - Error");
	}

	

	//SAVE TEMPLATES
	try
	{
		Glib::ustring pathCSoundTemplate = wxGLOBAL->getSettingsPath();
		pathCSoundTemplate.append("/CSoundTemplate.txt");
		
		Glib::ustring pathPythonTemplate = wxGLOBAL->getSettingsPath();
		pathPythonTemplate.append("/PythonTemplate.txt");
		
		Glib::ustring pathLuaTemplate = wxGLOBAL->getSettingsPath();
		pathLuaTemplate.append("/LuaTemplate.txt");

		Glib::ustring pathCabbageTemplate = wxGLOBAL->getSettingsPath();
		pathCabbageTemplate.append("/CabbageTemplate.txt");

		Glib::file_set_contents(pathCSoundTemplate, Templates.CSound);
		Glib::file_set_contents(pathPythonTemplate, Templates.Python);
		Glib::file_set_contents(pathLuaTemplate, Templates.Lua);
		Glib::file_set_contents(pathCabbageTemplate, Templates.Cabbage);
		
	}
	catch (...)
	{
		//System.Diagnostics.Debug.WriteLine("SaveSettings - Templates Error : " + ex.Message);
		wxGLOBAL->DebugPrint("wxSettings.SaveSettings(Templates) - Error");
	}


	
	return true;

}



///////////////////////////////////////////////////////////////////////////////
// CREATE DEFAULT SETTINGS
///////////////////////////////////////////////////////////////////////////////
void wxSettings::CreateDefaultWinXoundSettings()
{
	//VERSION
	//this.VERSION = "3";

	//GENERAL
	General.ShowUtilitiesMessage = false;
	General.ShowImportOrcScoMessage = false;
	General.ShowReadOnlyFileMessage = true;
	//General.BringWinXoundToFrontForCabbage = true;
	//General.WindowState = FormWindowState.Normal;
	General.WindowSize = Gdk::Point(900, 600);
	General.WindowPosition = Gdk::Point(0, 0);
	General.CompilerWindowSize = Gdk::Point(700, 400);
	General.CompilerWindowPosition = Gdk::Point(0, 0);
	General.FirstStart = true;
	General.FindWholeWord = false;
	General.FindMatchCase = false;
	General.ReplaceFromCaret = false;
	General.ShowToolbar = true;
	General.StartupAction = 0;
	General.OrcScoImport = 0;
	General.DefaultWavePlayer = 0;
	////ToolBar Items (checked in FormMain)
	General.CompilerFontName = "Monospace";
	General.CompilerFontSize = 10;
	General.CSoundDefaultFlags = "-B4096 --displays --asciidisplay";
	General.OpenSoundFileWith = 1;
	General.PythonDefaultFlags = "-u";
	General.LuaDefaultFlags = "";
	General.UseWinXoundFlags = true;
	//General.CheckForCabbageUpdates = false;
	General.CSoundAdditionalFlags = getDefaultAlternativeCSoundFlags();
		
	//DIRECTORY
	Directory.WavePlayer = "";
	Directory.WaveEditor = "";
	Directory.Calculator = "";
	Directory.WorkingDir = "";
	Directory.LastUsedPath = wxGLOBAL->getHomePath(); //"";
	Directory.CSoundConsole = "/usr/bin/csound"; //""
	Directory.Winsound = "/usr/bin/qutecsound";
	Directory.CSoundAV = ""; //NOT NEEDED IN LINUX
	Directory.CSoundAVHelp = ""; //NOT NEEDED IN LINUX
	Directory.CSoundHelpHTML = "/usr/share/doc/csound-doc/html/index.html";
	Directory.SFDIR = "";
	Directory.SSDIR = "";
	Directory.SADIR = "";
	Directory.MFDIR = "";
	Directory.INCDIR = "";
	Directory.OPCODEDIR = "";
	Directory.UseSFDIR = true;
	Directory.PythonConsolePath = "/usr/bin/python";
	Directory.PythonIdlePath = "";
	Directory.LuaConsolePath = "/usr/bin/lua";
	Directory.LuaGuiPath = "";
	Directory.CabbagePath = "";

	//EDITORPROPERTIES
	EditorProperties.DefaultFontName = "Monospace";
	EditorProperties.DefaultFontSize = 10;
	EditorProperties.DefaultTabSize = 8;
	EditorProperties.ShowVerticalRuler = false;
	EditorProperties.ShowMatchingBracket = true;
	EditorProperties.ShowLineNumbers = true;
	EditorProperties.ShowFoldLine = true;
	EditorProperties.ShowIntelliTip = true;
	EditorProperties.ShowExplorer = true;
	EditorProperties.MarkCaretLine = false;
	EditorProperties.SaveBookmarks = true;
	EditorProperties.UseMixedPython = true;
	EditorProperties.ExplorerShowOptions = true;
	EditorProperties.ExplorerShowInstrMacros = true;
	EditorProperties.ExplorerShowInstrOpcodes = true;
	EditorProperties.ExplorerShowInstrInstruments = true;
	EditorProperties.ExplorerShowScoreFunctions = true;
	EditorProperties.ExplorerShowScoreMacros = true;
	EditorProperties.ExplorerShowScoreSections = true;
	EditorProperties.ExplorerFontSize = 0;

	//CODEFORMAT
	CodeFormat.FormatHeader = true;
	CodeFormat.FormatInstruments = true;
	CodeFormat.FormatFunctions = true;
	CodeFormat.FormatScoreInstruments = true;
	CodeFormat.FormatTempo = true;
	CodeFormat.InstrumentsType = 0;
	CodeFormat.TabIndent = 8;

	//EditorProperties Styles
	FreeSyntaxLists();	
	CreateDefaultCSoundStyles();
	CreateDefaultPythonStyles();
	CreateDefaultLuaStyles();

	//TEMPLATES
	CreateDefaultTemplates();



}

void wxSettings::CreateDefaultTemplates()
{
	Templates.CSound =
		"<CsoundSynthesizer>\n\n\n"
		"<CsOptions>\n\n"
		"</CsOptions>\n\n\n"
		"<CsInstruments>\n\n"
		"</CsInstruments>\n\n\n"
		"<CsScore>\n\n"
		"</CsScore>\n\n\n"
		"</CsoundSynthesizer>";

	Templates.Python = "";

	Templates.Lua = "";

	Templates.Cabbage =
		"<Cabbage>\n"
		"</Cabbage>\n\n"
		"<CsoundSynthesizer>\n\n"
		"<CsOptions>\n"
		"-d -n\n"
		"</CsOptions>\n\n"
		"<CsInstruments>\n"
		"sr = 44100\n"
		"ksmps = 32\n"
		"nchnls = 2\n\n"
		"instr 1\n"
		"endin\n"
		"</CsInstruments>\n\n"
		"<CsScore>\n"
		"i1 0 1000\n"
		"</CsScore>\n\n"
		"</CsoundSynthesizer>";
}


//RecentFiles = NULL; //example: list = g_list_append (list, "first");
void wxSettings::CreateDefaultCSoundStyles()
{
	//CSOUND DEFAULT STYLES
	//int StyleNumber, int Fore, int Back, 
	//bool Bold, bool Italic, int Alpha, 
	//bool EolFilled, string FriendlyName

	//g_sprintf 
	//CSOUND:
	//this.EditorProperties.CSoundStyles = new List<string>();
	EditorProperties.CSoundStyles = NULL;

	//DEFAULT STYLE "32"
	EditorProperties.CSoundStyles = g_list_append(EditorProperties.CSoundStyles, 
				   (gpointer)"32,#000000,#FFFFFF,false,false,256,false,DefaultStyle");

	//WHITE SPACE - (SCE_C_DEFAULT 0)
	EditorProperties.CSoundStyles = g_list_append (EditorProperties.CSoundStyles, 
			       (gpointer)"0,#000000,#FFFFFF,false,false,256,false,WhiteSpace");

	//Comment Multi  - (SCE_C_COMMENT 1)   
	EditorProperties.CSoundStyles = g_list_append (EditorProperties.CSoundStyles, 
			       (gpointer)"1,#009600,#FFFFFF,false,false,256,false,CommentMulti");

	//Comment Line ";" - (SCE_C_COMMENTLINE 2)
	EditorProperties.CSoundStyles = g_list_append (EditorProperties.CSoundStyles, 
				   (gpointer)"2,#009600,#FFFFFF,false,false,256,false,CommentLine");

	//Numbers color - (SCE_C_NUMBER 4)
	EditorProperties.CSoundStyles = g_list_append (EditorProperties.CSoundStyles, 
	               (gpointer)"4,#000000,#FFFFFF,false,false,256,false,Numbers");

	//Keyword(0): OPCODES - (SCE_C_WORD 5)
	EditorProperties.CSoundStyles = g_list_append (EditorProperties.CSoundStyles, 
	               (gpointer)"5,#0000FF,#FFFFFF,false,false,256,false,Opcodes");

	//Double quoted string - (SCE_C_STRING 6)
	EditorProperties.CSoundStyles = g_list_append (EditorProperties.CSoundStyles, 
	               (gpointer)"6,#B40000,#FFFFFF,false,false,256,false,DoubleQuotedString");

	//Preprocessor and Macro - (SCE_C_PREPROCESSOR 9)
	EditorProperties.CSoundStyles = g_list_append (EditorProperties.CSoundStyles, 
	               (gpointer)"9,#A032A0,#FFFFFF,false,false,256,false,PreprocessorAndMacro");

	//Operator [=] - (SCE_C_OPERATOR 10)
	EditorProperties.CSoundStyles = g_list_append (EditorProperties.CSoundStyles, 
	               (gpointer)"10,#0000FF,#FFFFFF,false,false,256,false,Operator=");

	//CSD Tags, Keyword(1) - (SCE_C_WORD2 16)
	EditorProperties.CSoundStyles = g_list_append (EditorProperties.CSoundStyles, 
	               (gpointer)"16,#DC006E,#FFFFFF,false,false,256,false,CsdTags");

	//UserOpcodes - (SCE_C_WORD3 20)
	EditorProperties.CSoundStyles = g_list_append (EditorProperties.CSoundStyles, 
	               (gpointer)"20,#A032A0,#FFFFFF,false,false,256,false,UserOpcodes");

	//INSTR-ENDIN color - Keyword(3) - (SCE_WXOUND_WORD4 21)
	EditorProperties.CSoundStyles = g_list_append (EditorProperties.CSoundStyles, 
	               (gpointer)"21,#0000FF,#FFFFFF,false,false,256,false,InstrEndin");

	//DEFAULT STYLE "33" STYLE_NUMBERS_MARGINS
	EditorProperties.CSoundStyles = g_list_append (EditorProperties.CSoundStyles, 
	               (gpointer)"33,#828282,#F4F4F4,false,false,256,false,Margins");

	//Styles 0-255 for Scintilla
	//Style 256 = Text Selection   //Win_Linux:#D2D2D2   OsX:#A0C8FF
	//Style 257 = Bookmarks
	//Style 258 = Vertical Ruler
	//Style 259 = Caret Line Marker
	EditorProperties.CSoundStyles = g_list_append (EditorProperties.CSoundStyles, 
	               (gpointer)"256,#000000,#D2D2D2,false,false,256,false,TextSelection");
	EditorProperties.CSoundStyles = g_list_append (EditorProperties.CSoundStyles, 
	               (gpointer)"257,#0000FF,#0000FF,false,false,40,false,Bookmarks");
	EditorProperties.CSoundStyles = g_list_append (EditorProperties.CSoundStyles, 
	               (gpointer)"258,#C0C0C0,#000000,false,false,256,false,VerticalRuler");
	EditorProperties.CSoundStyles = g_list_append (EditorProperties.CSoundStyles, 
	               (gpointer)"259,#FFFF64,#000000,false,false,256,false,CaretLineMarker");

}


void wxSettings::CreateDefaultPythonStyles()
{
	//DEFAULT PYTHON STYLES
	//int StyleNumber, int Fore, int Back, 
	//bool Bold, bool Italic, int Alpha, 
	//bool EolFilled, string FriendlyName
	//PYTHON:
	EditorProperties.PythonStyles = NULL;

	//DEFAULT STYLE "32"
	EditorProperties.PythonStyles = g_list_append(EditorProperties.PythonStyles, 
		(gpointer)"32,#000000,#FFFFFF,false,false,256,false,DefaultStyle");

	//White Space - (SCE_P_DEFAULT 0)
	EditorProperties.PythonStyles = g_list_append(EditorProperties.PythonStyles, 
		(gpointer)"0,#000000,#FFFFFF,false,false,256,false,WhiteSpace");

	//Comment Line "#"  - (SCE_P_COMMENTLINE 1)
	EditorProperties.PythonStyles = g_list_append(EditorProperties.PythonStyles, 
		(gpointer)"1,#009600,#FFFFFF,false,false,256,false,CommentLine");

	//Numbers - (SCE_P_NUMBER 2)
	EditorProperties.PythonStyles = g_list_append(EditorProperties.PythonStyles, 
		(gpointer)"2,#007F7F,#FFFFFF,false,false,256,false,Numbers");

	//Strings "'" - (SCE_P_STRING 3)
	EditorProperties.PythonStyles = g_list_append(EditorProperties.PythonStyles, 
		(gpointer)"3,#960000,#FFFFFF,false,false,256,false,Strings");

	//Single quoted string - (SCE_P_CHARACTER 4)
	EditorProperties.PythonStyles = g_list_append(EditorProperties.PythonStyles, 
		(gpointer)"4,#960000,#FFFFFF,false,false,256,false,StringsSingle");

	//Keyword(0) - (SCE_P_WORD 5) - 0,0,127 + BOLD
	EditorProperties.PythonStyles = g_list_append(EditorProperties.PythonStyles, 
		(gpointer)"5,#00007F,#FFFFFF,True,false,256,false,Keywords");

	//Triple quoted string - (SCE_P_TRIPLE 6)
	EditorProperties.PythonStyles = g_list_append(EditorProperties.PythonStyles, 
		(gpointer)"6,#960000,#FFFFFF,false,false,256,false,StringsTriple");

	//TripleDouble quoted string - (SCE_P_TRIPLEDOUBLE 7)
	EditorProperties.PythonStyles = g_list_append(EditorProperties.PythonStyles, 
		(gpointer)"7,#960000,#FFFFFF,false,false,256,false,StringsTripleD");

	//Class name definition - (SCE_P_CLASSNAME 8) - 0,0,255 + BOLD
	EditorProperties.PythonStyles = g_list_append(EditorProperties.PythonStyles, 
		(gpointer)"8,#0000FF,#FFFFFF,True,false,256,false,Classes");

	//Function or method name definition - (SCE_P_DEFNAME 9) -  0,127,127 + BOLD
	EditorProperties.PythonStyles = g_list_append(EditorProperties.PythonStyles, 
		(gpointer)"9,#007F7F,#FFFFFF,True,false,256,false,Functions");

	//Operators - (SCE_P_OPERATOR 10) - TextForeColor + BOLD
	EditorProperties.PythonStyles = g_list_append(EditorProperties.PythonStyles, 
		(gpointer)"10,#000000,#FFFFFF,True,false,256,false,Operators");

	//all words - ( SCE_P_IDENTIFIER 11)
	EditorProperties.PythonStyles = g_list_append(EditorProperties.PythonStyles, 
		(gpointer)"11,#000000,#FFFFFF,false,false,256,false,Words");

	//Comment-blocks - ( SCE_P_COMMENTBLOCK 12)
	EditorProperties.PythonStyles = g_list_append(EditorProperties.PythonStyles, 
		(gpointer)"12,#009600,#FFFFFF,false,false,256,false,CommentBlocks");

	//End of line where string is not closed - ( SCE_P_STRINGEOL 13)
	EditorProperties.PythonStyles = g_list_append(EditorProperties.PythonStyles, 
		(gpointer)"13,#000000,#E0C0E0,false,false,256,false,EolStringNotClosed");

	//Highlighted identifiers - ( SCE_P_WORD2 14)
	EditorProperties.PythonStyles = g_list_append(EditorProperties.PythonStyles, 
		(gpointer)"14,#0000FF,#FFFFFF,false,false,256,false,Identifiers");

	//Decorators @dec1 - (SCE_C_WORD2 15)
	EditorProperties.PythonStyles = g_list_append(EditorProperties.PythonStyles, 
		(gpointer)"15,#805000,#FFFFFF,false,false,256,false,Decorators");

	//DEFAULT STYLE "33" STYLE_NUMBERS_MARGINS
	EditorProperties.PythonStyles = g_list_append(EditorProperties.PythonStyles, 
		(gpointer)"33,#828282,#F4F4F4,false,false,256,false,Margins");

	//Styles 0-255 for Scintilla
	//Style 256 = Text Selection
	//Style 257 = Bookmarks
	//Style 258 = Vertical Ruler
	//Style 259 = Caret Line Marker
	//Style 260 = Line Numbers
	EditorProperties.PythonStyles = g_list_append (EditorProperties.PythonStyles, 
		(gpointer)"256,#000000,#D2D2D2,false,false,256,false,TextSelection");
	EditorProperties.PythonStyles = g_list_append (EditorProperties.PythonStyles, 
		(gpointer)"257,#0000FF,#0000FF,false,false,40,false,Bookmarks");
	EditorProperties.PythonStyles = g_list_append (EditorProperties.PythonStyles, 
		(gpointer)"258,#C0C0C0,#000000,false,false,256,false,VerticalRuler");
	EditorProperties.PythonStyles = g_list_append (EditorProperties.PythonStyles, 
		(gpointer)"259,#FFFF64,#000000,false,false,256,false,CaretLineMarker");

}



void wxSettings::CreateDefaultLuaStyles()
{
	//DEFAULT LUA STYLES
	//int StyleNumber, int Fore, int Back, 
	//bool Bold, bool Italic, int Alpha, 
	//bool EolFilled, string FriendlyName
	//LUA:
	EditorProperties.LuaStyles = NULL;

	//DEFAULT STYLE "32"
	EditorProperties.LuaStyles = g_list_append(EditorProperties.LuaStyles, 
		(gpointer)"32,#000000,#FFFFFF,false,false,256,false,DefaultStyle");

	//White Space - (SCE_P_DEFAULT 0)
	EditorProperties.LuaStyles = g_list_append(EditorProperties.LuaStyles, 
		(gpointer)"0,#000000,#FFFFFF,false,false,256,false,WhiteSpace");

	//Comment Block
	EditorProperties.LuaStyles = g_list_append(EditorProperties.LuaStyles, 
		(gpointer)"1,#009600,#FFFFFF,False,False,256,False,CommentBlock");

	//Comment Line
	EditorProperties.LuaStyles = g_list_append(EditorProperties.LuaStyles, 
		(gpointer)"2,#009600,#FFFFFF,False,False,256,False,CommentLine");

	//DocComment
	EditorProperties.LuaStyles = g_list_append(EditorProperties.LuaStyles, 
		(gpointer)"3,#960000,#FFFFFF,False,False,256,False,DocComment");
	
	//Numbers
	EditorProperties.LuaStyles = g_list_append(EditorProperties.LuaStyles, 
		(gpointer)"4,#007F7F,#FFFFFF,False,False,256,False,Numbers");

	//Keyword(5) - 0,0,127 + BOLD
	EditorProperties.LuaStyles = g_list_append(EditorProperties.LuaStyles, 
		(gpointer)"5,#0000E6,#FFFFFF,False,False,256,False,Keywords");

    //Double quoted string //127,0,127
	EditorProperties.LuaStyles = g_list_append(EditorProperties.LuaStyles, 
		(gpointer)"6,#7F007F,#FFFFFF,False,False,256,False,Strings");

    //Single quoted string //127,0,127
	EditorProperties.LuaStyles = g_list_append(EditorProperties.LuaStyles, 
		(gpointer)"7,#7F007F,#FFFFFF,False,False,256,False,StringsSingle");

    //Literal string  //fore: 127,0,127 - back: 200,255,255
	EditorProperties.LuaStyles = g_list_append(EditorProperties.LuaStyles, 
		(gpointer)"8,#0000FF,#FFFFFF,False,False,256,False,LiteralString");

    //Preprocessor -  //127,127,0
	EditorProperties.LuaStyles = g_list_append(EditorProperties.LuaStyles, 
		(gpointer)"9,#7F7F00,#FFFFFF,True,False,256,False,Preprocessor");

    //Operators - TextForeColor
	EditorProperties.LuaStyles = g_list_append(EditorProperties.LuaStyles, 
		(gpointer)"10,#000000,#FFFFFF,False,False,256,False,Operators");

    //all words
	EditorProperties.LuaStyles = g_list_append(EditorProperties.LuaStyles, 
		(gpointer)"11,#000000,#FFFFFF,False,False,256,False,Words");

    //End of line where string is not closed
	EditorProperties.LuaStyles = g_list_append(EditorProperties.LuaStyles, 
		(gpointer)"12,#000000,#E0C0E0,False,False,256,False,EolStringNotClose");

	//Other Keywords
	//style.lua.13=$(style.lua.5),back:#F5FFF5
	//style.lua.14=$(style.lua.5),back:#F5F5FF
	//style.lua.15=$(style.lua.5),back:#FFF5F5
	//style.lua.16=$(style.lua.5),back:#FFF5FF
	//style.lua.17=$(style.lua.5),back:#FFFFF5
	//style.lua.18=$(style.lua.5),back:#FFA0A0
	//style.lua.19=$(style.lua.5),back:#FFF5F5

    //Keywords
	EditorProperties.LuaStyles = g_list_append(EditorProperties.LuaStyles, 
		(gpointer)"13,#000000,#F5FFF5,False,False,256,False,Keywords");

    //Keywords
	EditorProperties.LuaStyles = g_list_append(EditorProperties.LuaStyles, 
		(gpointer)"14,#000000,#F5F5FF,False,False,256,False,Keywords");

    //Keywords
	EditorProperties.LuaStyles = g_list_append(EditorProperties.LuaStyles, 
		(gpointer)"15,#000000,#FFF5F5,False,False,256,False,Keywords");

    //Keywords
	EditorProperties.LuaStyles = g_list_append(EditorProperties.LuaStyles, 
		(gpointer)"16,#000000,#FFF5FF,False,False,256,False,Keywords");

    //Keywords
	EditorProperties.LuaStyles = g_list_append(EditorProperties.LuaStyles, 
		(gpointer)"17,#000000,#FFFFF5,False,False,256,False,Keywords");

    //Keywords
	EditorProperties.LuaStyles = g_list_append(EditorProperties.LuaStyles, 
		(gpointer)"18,#000000,#F5A0A0,False,False,256,False,Keywords");

    //Keywords
	EditorProperties.LuaStyles = g_list_append(EditorProperties.LuaStyles, 
		(gpointer)"19,#000000,#FFF5F5,False,False,256,False,Keywords");


	//////////////////////////////////////////
	//DEFAULT STYLE "33" STYLE_NUMBERS_MARGINS
	EditorProperties.LuaStyles = g_list_append(EditorProperties.LuaStyles, 
		(gpointer)"33,#828282,#F4F4F4,False,False,256,False,Margins");

	//Styles 0-255 for Scintilla
	//Style 256 = Text Selection
	//Style 257 = Bookmarks
	//Style 258 = Vertical Ruler
	//Style 259 = Caret Line Marker
	//Style 260 = Line Numbers
	EditorProperties.LuaStyles = g_list_append (EditorProperties.LuaStyles, 
		(gpointer)"256,#000000,#D2D2D2,false,false,256,false,TextSelection");
	EditorProperties.LuaStyles = g_list_append (EditorProperties.LuaStyles, 
		(gpointer)"257,#0000FF,#0000FF,false,false,40,false,Bookmarks");
	EditorProperties.LuaStyles = g_list_append (EditorProperties.LuaStyles, 
		(gpointer)"258,#C0C0C0,#000000,false,false,256,false,VerticalRuler");
	EditorProperties.LuaStyles = g_list_append (EditorProperties.LuaStyles, 
		(gpointer)"259,#FFFF64,#000000,false,false,256,false,CaretLineMarker");

}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////






///////////////////////////////////////////////////////////////////////////////
// RECENT FILES
///////////////////////////////////////////////////////////////////////////////
void wxSettings::RecentFilesRemoveExcess()
{
	try
	{
		/*
		if (list.Count > MAX_RECENT_FILES)
		{
			Int32 count = list.Count - MAX_RECENT_FILES;
			list.RemoveRange(MAX_RECENT_FILES, count);
		}
		*/
		if(RecentFiles.size() > MAX_RECENT_FILES)
		{
			//gint count = RecentFiles.size() - MAX_RECENT_FILES;
			//RecentFiles.erase(RecentFiles.begin() + MAX_RECENT_FILES,
			//                  myvector.begin() + MAX_RECENT_FILES + count);
			RecentFiles.resize(MAX_RECENT_FILES);
		}
	}
	catch (...)
	{
		wxGLOBAL->DebugPrint("wxSettings - RecentFilesRemoveExcess Error");
	}
}

void wxSettings::RecentFilesInsert(const gchar* mFileName)
{
	try
	{
		if(Glib::file_test(mFileName, Glib::FILE_TEST_EXISTS))
		{
			for (uint i=0; i<RecentFiles.size(); i++ )
			{
				if(RecentFiles[i] == mFileName)
				{
					RecentFiles.erase(RecentFiles.begin() + i);
				}
			}
		}

		Glib::ustring temp = mFileName;
		RecentFiles.insert(RecentFiles.begin(), temp);
		RecentFilesRemoveExcess();
		
	}

	catch (...)
	{
		wxGLOBAL->DebugPrint("wxSettings - RecentFilesInsert Error");
	}
	
}


///////////////////////////////////////////////////////////////////////////////
// LAST SESSION FILES
///////////////////////////////////////////////////////////////////////////////
void wxSettings::LastSessionFilesClear()
{
	LastSessionFiles.clear();
}
void wxSettings::LastSessionFilesInsert(const gchar* mFileName)
{
	try
	{
		if(Glib::file_test(mFileName, Glib::FILE_TEST_EXISTS))
		{
			std::string temp = mFileName;
			LastSessionFiles.insert(LastSessionFiles.begin(), temp);
		}
		
	}

	catch (...)
	{
		wxGLOBAL->DebugPrint("wxSettings - LastSessionFilesInsert Error");
	}
}




///////////////////////////////////////////////////////////////////////////////
// ENVIRONMENT VARIABLES
///////////////////////////////////////////////////////////////////////////////
void wxSettings::SetEnvironmentVariables()
{
	//SET ENVIRONMENT VARIABLES

	if(wxSETTINGS->Directory.SFDIR != "")
		setenv("SFDIR",wxSETTINGS->Directory.SFDIR, 1);
	
	if(wxSETTINGS->Directory.SSDIR != "")
		setenv("SSDIR",wxSETTINGS->Directory.SSDIR, 1);

	if(wxSETTINGS->Directory.SADIR != "")
		setenv("SADIR",wxSETTINGS->Directory.SADIR, 1);

	if(wxSETTINGS->Directory.MFDIR != "")
		setenv("MFDIR",wxSETTINGS->Directory.MFDIR, 1);

	if(wxSETTINGS->Directory.INCDIR != "")
		setenv("INCDIR",wxSETTINGS->Directory.INCDIR, 1);

	if(wxSETTINGS->Directory.OPCODEDIR != "")
	{
		setenv("OPCODEDIR",wxSETTINGS->Directory.OPCODEDIR, 1);
		setenv("OPCODEDIR64",wxSETTINGS->Directory.OPCODEDIR, 1);
	}

}









































////////////////////////////////////////////////////////////////////////////////
// WINDOW SETTINGS
////////////////////////////////////////////////////////////////////////////////

void wxSettings::showWindowAt(gint x, gint y)
{

	//settingsWindow->set_title(Glib::ustring::compose("WinXound Settings (%1)",
	//                                                 wxGLOBAL->getSettingsPath()));
	
	//Set settings on the various widgets
	radiobuttonStartupActionNothing->set_active((General.StartupAction == 0));
	radiobuttonStartupActionCSound->set_active((General.StartupAction == 1));
	radiobuttonStartupActionPython->set_active((General.StartupAction == 2));
	radiobuttonStartupActionLua->set_active((General.StartupAction == 3));
	radiobuttonStartupActionLastSession->set_active((General.StartupAction == 4));

	radiobuttonOSIAskAlways->set_active(General.OrcScoImport == 0);
	radiobuttonOSIConvertToCsd->set_active(General.OrcScoImport == 1);
	radiobuttonOSIOpenSeparately->set_active(General.OrcScoImport == 2);

	radiobuttonWavePlayerInternal->set_active(General.DefaultWavePlayer == 0);
	radiobuttonExternalMediaPlayer->set_active(General.DefaultWavePlayer == 1);

	checkbuttonShowReadOnlyAlert->set_active(General.ShowReadOnlyFileMessage);
	//checkbuttonCabbageFileUpdated->set_active(General.BringWinXoundToFrontForCabbage);
	//checkbuttonCheckForCabbageUpdates->set_active(General.CheckForCabbageUpdates);
	textviewAlternativeCSoundFlags->get_buffer()->set_text(General.CSoundAdditionalFlags);
	
	textviewTemplatesCSound->get_buffer()->set_text(Templates.CSound);
	textviewTemplatesPython->get_buffer()->set_text(Templates.Python);
	textviewTemplatesLua->get_buffer()->set_text(Templates.Lua);
	textviewTemplatesCabbage->get_buffer()->set_text(Templates.Cabbage);
	//buttonDefaultTemplates;
	entryCSoundExecutable->set_text(Directory.CSoundConsole);
	//buttonBrowseCSoundExecutable;
	entryCSoundHelp->set_text(Directory.CSoundHelpHTML);
	//buttonBrowseCSoundHelp;
	entryCSoundExternalGUI->set_text(Directory.Winsound);
	//buttonBrowseCSoundExternalGUI;
	entryWavePlayer->set_text(Directory.WavePlayer);
	//buttonBrowseWavePlayer;
	entryWaveEditor->set_text(Directory.WaveEditor);
	//buttonBrowseWaveEditor;
	entryCalculator->set_text(Directory.Calculator);
	//buttonBrowseCalculator;
	entryWorkingDirectory->set_text(Directory.WorkingDir);
	//buttonBrowseWorkingDirectory;
	entryPythonCompiler->set_text(Directory.PythonConsolePath);
	//buttonBrowsePythonCompiler;
	entryPythonExternalGUI->set_text(Directory.PythonIdlePath);
	//buttonBrowsePythonExternalGUI;
	entryLuaCompiler->set_text(Directory.LuaConsolePath);
	//buttonBrowseLuaCompiler;
	entryLuaExternalGUI->set_text(Directory.LuaGuiPath);
	//buttonBrowseLuaExternalGUI;

	//CabbagePath
	//entryCabbageExe->set_text(Directory.CabbagePath);
	
	//buttonAutoSearchPaths;
	checkbuttonSFDIR->set_active(Directory.UseSFDIR);
	entrySFDIR->set_text(Directory.SFDIR);
	//buttonBrowseSFDIR;
	//buttonClearSFDIR;
	entrySSDIR->set_text(Directory.SSDIR);
	//buttonBrowseSSDIR;
	//buttonClearSSDIR;
	entrySADIR->set_text(Directory.SADIR);
	//buttonBrowseSADIR;
	//buttonClearSADIR;
	entryMFDIR->set_text(Directory.MFDIR);
	//buttonBrowseMFDIR;
	//buttonClearMFDIR;
	entryINCDIR->set_text(Directory.INCDIR);
	//buttonBrowseINCDIR;
	//buttonClearINCDIR;
	entryOPCODEDIR->set_text(Directory.OPCODEDIR);

	
	entryCompilerFontName->set_text(General.CompilerFontName);
	//buttonBrowseFontsCompiler;
	spinbuttonCompilerFontSize->set_value(General.CompilerFontSize);
	//buttonDefaultFontCompiler;
	entryCSoundCompilerFlags->set_text(General.CSoundDefaultFlags);
	//buttonCSoundDefaultFlags;
	radiobuttonOpenSoundfileWithNothing->set_active((General.OpenSoundFileWith == 0));
	radiobuttonOpenSoundfileWithMediaPlayer->set_active((General.OpenSoundFileWith == 1));
	radiobuttonOpenSoundfileWithWaveEditor->set_active((General.OpenSoundFileWith == 2));
	entryPythonCompilerFlags->set_text(General.PythonDefaultFlags);
	//buttonPythonDefaultFlags;
	entryLuaCompilerFlags->set_text(General.LuaDefaultFlags);
	//buttonLuaDefaultFlags;
	entryTextFontName->set_text(EditorProperties.DefaultFontName);
	//buttonBrowseFontsText;
	spinbuttonTextFontSize->set_value(EditorProperties.DefaultFontSize);
	spinbuttonTextTabIndentSize->set_value(EditorProperties.DefaultTabSize);
	//buttonDefaultFontText;
	checkbuttonTextShowMatchingBracket->set_active(EditorProperties.ShowMatchingBracket);
	checkbuttonTextSaveBookmarks->set_active(EditorProperties.SaveBookmarks);
	checkbuttonTextMarkCaretLine->set_active(EditorProperties.MarkCaretLine);
	checkbuttonTextShowVerticalRuler->set_active(EditorProperties.ShowVerticalRuler);
	checkbuttonShowFoldLine->set_active(EditorProperties.ShowFoldLine);
	//buttonDefaultTextProperties;
	//buttonDefaultSyntaxCSound;
	//buttonDefaultSyntaxPython;
	//buttonDefaultSyntaxLua;
	checkbuttonPythonMixed->set_active(EditorProperties.UseMixedPython);
	checkbuttonOptions->set_active(EditorProperties.ExplorerShowOptions);
	checkbuttonInstrMacros->set_active(EditorProperties.ExplorerShowInstrMacros);
	checkbuttonInstrOpcode->set_active(EditorProperties.ExplorerShowInstrOpcodes);
	checkbuttonInstrInstr->set_active(EditorProperties.ExplorerShowInstrInstruments);
	checkbuttonScoreFunctions->set_active(EditorProperties.ExplorerShowScoreFunctions);
	checkbuttonScoreMacros->set_active(EditorProperties.ExplorerShowScoreMacros);
	checkbuttonScoreSections->set_active(EditorProperties.ExplorerShowScoreSections);
	radiobuttonExplorerFontSmall->set_active((EditorProperties.ExplorerFontSize == 0));
	radiobuttonExplorerFontMedium->set_active((EditorProperties.ExplorerFontSize == 1));
	radiobuttonExplorerFontLarge->set_active((EditorProperties.ExplorerFontSize == 2));
	//buttonCreateDesktopShortcut;
	//buttonExportSettings;
	//buttonImportSettings;
	//buttonCancel;
	//buttonSaveSettings;                      

	

	RefreshSyntaxColors("csound");
	RefreshSyntaxColors("python");
	RefreshSyntaxColors("lua");
	



	//Show window
	settingsWindow->show_all();

	
	//TODO: MAYBE!!!
	//Hide the DefaultMediaPlayer frame (for the moment
	//on Linux we use only the external player)
	frameDefaultMediaPlayer->set_visible(FALSE);

	
	settingsWindow->move(x - (settingsWindow->get_width() / 2), 
	 					 y - (settingsWindow->get_height() / 2));
	settingsWindow->present(); //Deiconify if necessary
	settingsWindow->grab_focus();

	//settingsWindow->set_title(wxGLOBAL->getSettingsPath());
}


void wxSettings::SettingsSaveNotification()
{
	m_signal_settings_save.emit();
}

void wxSettings::closeWindow()
{	
	settingsWindow->hide();
}

void wxSettings::associateWidgets(Glib::RefPtr<Gtk::Builder> builder)
{
	//WINDOW WIDGETS
	builder->get_widget("radiobuttonStartupActionNothing",radiobuttonStartupActionNothing);
	builder->get_widget("radiobuttonStartupActionCSound",radiobuttonStartupActionCSound);
	builder->get_widget("radiobuttonStartupActionPython",radiobuttonStartupActionPython);
	builder->get_widget("radiobuttonStartupActionLua",radiobuttonStartupActionLua);
	builder->get_widget("radiobuttonStartupActionLastSession",radiobuttonStartupActionLastSession);

	builder->get_widget("radiobuttonOSIAskAlways",radiobuttonOSIAskAlways);
	builder->get_widget("radiobuttonOSIConvertToCsd",radiobuttonOSIConvertToCsd);
	builder->get_widget("radiobuttonOSIOpenSeparately",radiobuttonOSIOpenSeparately);

	builder->get_widget("radiobuttonWavePlayerInternal",radiobuttonWavePlayerInternal);
	builder->get_widget("radiobuttonExternalMediaPlayer",radiobuttonExternalMediaPlayer);
	
	builder->get_widget("checkbuttonShowReadOnlyAlert",checkbuttonShowReadOnlyAlert);

	//builder->get_widget("checkbuttonCabbageFileUpdated",checkbuttonCabbageFileUpdated);
	//builder->get_widget("checkbuttonCheckForCabbageUpdates",checkbuttonCheckForCabbageUpdates);


	builder->get_widget("textviewTemplatesCSound",textviewTemplatesCSound);
	builder->get_widget("textviewTemplatesPython",textviewTemplatesPython);
	builder->get_widget("textviewTemplatesLua",textviewTemplatesLua);
	builder->get_widget("textviewTemplatesCabbage",textviewTemplatesCabbage);

	builder->get_widget("buttonDefaultTemplates",buttonDefaultTemplates);
	buttonDefaultTemplates->signal_clicked().connect(
		sigc::mem_fun(*this, &wxSettings::on_buttonDefaultTemplates_Clicked));

	builder->get_widget("entryCSoundExecutable",entryCSoundExecutable);
	builder->get_widget("buttonBrowseCSoundExecutable",buttonBrowseCSoundExecutable);
	buttonBrowseCSoundExecutable->signal_clicked().connect(
				sigc::bind<Glib::ustring>(
				sigc::mem_fun(*this, &wxSettings::on_buttonBrowse_Clicked), "CSoundExecutable"));
	
	builder->get_widget("entryCSoundHelp",entryCSoundHelp);
	builder->get_widget("buttonBrowseCSoundHelp",buttonBrowseCSoundHelp);
	buttonBrowseCSoundHelp->signal_clicked().connect(
				sigc::bind<Glib::ustring>(
				sigc::mem_fun(*this, &wxSettings::on_buttonBrowse_Clicked), "CSoundHelp"));
	
	builder->get_widget("entryCSoundExternalGUI",entryCSoundExternalGUI);
	builder->get_widget("buttonBrowseCSoundExternalGUI",buttonBrowseCSoundExternalGUI);
	buttonBrowseCSoundExternalGUI->signal_clicked().connect(
				sigc::bind<Glib::ustring>(
				sigc::mem_fun(*this, &wxSettings::on_buttonBrowse_Clicked), "CSoundExternalGUI"));
	
	builder->get_widget("entryWavePlayer",entryWavePlayer);
	builder->get_widget("buttonBrowseWavePlayer",buttonBrowseWavePlayer);
	buttonBrowseWavePlayer->signal_clicked().connect(
				sigc::bind<Glib::ustring>(
				sigc::mem_fun(*this, &wxSettings::on_buttonBrowse_Clicked), "WavePlayer"));
	
	builder->get_widget("entryWaveEditor",entryWaveEditor);
	builder->get_widget("buttonBrowseWaveEditor",buttonBrowseWaveEditor);
	buttonBrowseWaveEditor->signal_clicked().connect(
				sigc::bind<Glib::ustring>(
				sigc::mem_fun(*this, &wxSettings::on_buttonBrowse_Clicked), "WaveEditor"));
	
	builder->get_widget("entryCalculator",entryCalculator);
	builder->get_widget("buttonBrowseCalculator",buttonBrowseCalculator);
	buttonBrowseCalculator->signal_clicked().connect(
				sigc::bind<Glib::ustring>(
				sigc::mem_fun(*this, &wxSettings::on_buttonBrowse_Clicked), "Calculator"));
	
	builder->get_widget("entryWorkingDirectory",entryWorkingDirectory);
	builder->get_widget("buttonBrowseWorkingDirectory",buttonBrowseWorkingDirectory);
	buttonBrowseWorkingDirectory->signal_clicked().connect(
				sigc::bind<Glib::ustring>(
				sigc::mem_fun(*this, &wxSettings::on_buttonBrowseDirectory_Clicked), "WorkingDirectory"));

	
	builder->get_widget("entryPythonCompiler",entryPythonCompiler);
	builder->get_widget("buttonBrowsePythonCompiler",buttonBrowsePythonCompiler);
	buttonBrowsePythonCompiler->signal_clicked().connect(
				sigc::bind<Glib::ustring>(
				sigc::mem_fun(*this, &wxSettings::on_buttonBrowse_Clicked), "PythonCompiler"));
	
	builder->get_widget("entryPythonExternalGUI",entryPythonExternalGUI);
	builder->get_widget("buttonBrowsePythonExternalGUI",buttonBrowsePythonExternalGUI);
	buttonBrowsePythonExternalGUI->signal_clicked().connect(
				sigc::bind<Glib::ustring>(
				sigc::mem_fun(*this, &wxSettings::on_buttonBrowse_Clicked), "PythonExternalGUI"));
	
	builder->get_widget("entryLuaCompiler",entryLuaCompiler);
	builder->get_widget("buttonBrowseLuaCompiler",buttonBrowseLuaCompiler);
	buttonBrowseLuaCompiler->signal_clicked().connect(
				sigc::bind<Glib::ustring>(
				sigc::mem_fun(*this, &wxSettings::on_buttonBrowse_Clicked), "LuaCompiler"));
	
	builder->get_widget("entryLuaExternalGUI",entryLuaExternalGUI);
	builder->get_widget("buttonBrowseLuaExternalGUI",buttonBrowseLuaExternalGUI);
	buttonBrowseLuaExternalGUI->signal_clicked().connect(
				sigc::bind<Glib::ustring>(
				sigc::mem_fun(*this, &wxSettings::on_buttonBrowse_Clicked), "LuaExternalGUI"));

	/*//CABBAGE
	builder->get_widget("entryCabbageExe",entryCabbageExe);
	builder->get_widget("buttonBrowseCabbageExe",buttonBrowseCabbageExe);
	buttonBrowseCabbageExe->signal_clicked().connect(
				sigc::bind<Glib::ustring>(
				sigc::mem_fun(*this, &wxSettings::on_buttonBrowse_Clicked), "CabbageExe"));
	builder->get_widget("buttonClearCabbageExe",buttonClearCabbageExe);
	buttonClearCabbageExe->signal_clicked().connect(
				sigc::bind<Glib::ustring>(
				sigc::mem_fun(*this, &wxSettings::on_buttonClearEnvironment_Clicked), "CabbageExe"));
	*/
	
	builder->get_widget("buttonAutoSearchPaths",buttonAutoSearchPaths);
	buttonAutoSearchPaths->signal_clicked().connect(
		sigc::mem_fun(*this, &wxSettings::on_buttonAutoSearchPaths_Clicked));
	

	builder->get_widget("checkbuttonSFDIR",checkbuttonSFDIR);
	builder->get_widget("entrySFDIR",entrySFDIR);
	builder->get_widget("buttonBrowseSFDIR",buttonBrowseSFDIR);
	buttonBrowseSFDIR->signal_clicked().connect(
				sigc::bind<Glib::ustring>(
				sigc::mem_fun(*this, &wxSettings::on_buttonBrowseDirectory_Clicked), "SFDIR"));
	builder->get_widget("buttonClearSFDIR",buttonClearSFDIR);
	buttonClearSFDIR->signal_clicked().connect(
				sigc::bind<Glib::ustring>(
				sigc::mem_fun(*this, &wxSettings::on_buttonClearEnvironment_Clicked), "SFDIR"));

	
	builder->get_widget("entrySSDIR",entrySSDIR);
	builder->get_widget("buttonBrowseSSDIR",buttonBrowseSSDIR);
	buttonBrowseSSDIR->signal_clicked().connect(
				sigc::bind<Glib::ustring>(
				sigc::mem_fun(*this, &wxSettings::on_buttonBrowseDirectory_Clicked), "SSDIR"));
	builder->get_widget("buttonClearSSDIR",buttonClearSSDIR);
	buttonClearSSDIR->signal_clicked().connect(
				sigc::bind<Glib::ustring>(
				sigc::mem_fun(*this, &wxSettings::on_buttonClearEnvironment_Clicked), "SSDIR"));

	
	builder->get_widget("entrySADIR",entrySADIR);
	builder->get_widget("buttonBrowseSADIR",buttonBrowseSADIR);
	buttonBrowseSADIR->signal_clicked().connect(
				sigc::bind<Glib::ustring>(
				sigc::mem_fun(*this, &wxSettings::on_buttonBrowseDirectory_Clicked), "SADIR"));
	builder->get_widget("buttonClearSADIR",buttonClearSADIR);
	buttonClearSADIR->signal_clicked().connect(
				sigc::bind<Glib::ustring>(
				sigc::mem_fun(*this, &wxSettings::on_buttonClearEnvironment_Clicked), "SADIR"));

	
	builder->get_widget("entryMFDIR",entryMFDIR);
	builder->get_widget("buttonBrowseMFDIR",buttonBrowseMFDIR);
	buttonBrowseMFDIR->signal_clicked().connect(
				sigc::bind<Glib::ustring>(
				sigc::mem_fun(*this, &wxSettings::on_buttonBrowseDirectory_Clicked), "MFDIR"));
	builder->get_widget("buttonClearMFDIR",buttonClearMFDIR);
	buttonClearMFDIR->signal_clicked().connect(
				sigc::bind<Glib::ustring>(
				sigc::mem_fun(*this, &wxSettings::on_buttonClearEnvironment_Clicked), "MFDIR"));

	
	builder->get_widget("entryINCDIR",entryINCDIR);
	builder->get_widget("buttonBrowseINCDIR",buttonBrowseINCDIR);
	buttonBrowseINCDIR->signal_clicked().connect(
				sigc::bind<Glib::ustring>(
				sigc::mem_fun(*this, &wxSettings::on_buttonBrowseDirectory_Clicked), "INCDIR"));
	builder->get_widget("buttonClearINCDIR",buttonClearINCDIR);
	buttonClearINCDIR->signal_clicked().connect(
				sigc::bind<Glib::ustring>(
				sigc::mem_fun(*this, &wxSettings::on_buttonClearEnvironment_Clicked), "INCDIR"));

	builder->get_widget("entryOPCODEDIR",entryOPCODEDIR);
	builder->get_widget("buttonBrowseOPCODEDIR",buttonBrowseOPCODEDIR);
	buttonBrowseOPCODEDIR->signal_clicked().connect(
				sigc::bind<Glib::ustring>(
				sigc::mem_fun(*this, &wxSettings::on_buttonBrowseDirectory_Clicked), "OPCODEDIR"));
	builder->get_widget("buttonClearOPCODEDIR",buttonClearOPCODEDIR);
	buttonClearOPCODEDIR->signal_clicked().connect(
				sigc::bind<Glib::ustring>(
				sigc::mem_fun(*this, &wxSettings::on_buttonClearEnvironment_Clicked), "OPCODEDIR"));


	
	builder->get_widget("entryCompilerFontName",entryCompilerFontName);
	
	builder->get_widget("buttonBrowseFontsCompiler",buttonBrowseFontsCompiler);
	buttonBrowseFontsCompiler->signal_clicked().connect(
		sigc::mem_fun(*this, &wxSettings::on_buttonBrowseFontsCompiler_Clicked));

	builder->get_widget("spinbuttonCompilerFontSize",spinbuttonCompilerFontSize);
	
	builder->get_widget("buttonDefaultFontCompiler",buttonDefaultFontCompiler);
	buttonDefaultFontCompiler->signal_clicked().connect(
		sigc::mem_fun(*this, &wxSettings::on_buttonDefaultFontCompiler_Clicked));
	
	builder->get_widget("entryCSoundCompilerFlags",entryCSoundCompilerFlags);
	builder->get_widget("buttonCSoundDefaultFlags",buttonCSoundDefaultFlags);
	buttonCSoundDefaultFlags->signal_clicked().connect(
		sigc::mem_fun(*this, &wxSettings::on_buttonCSoundDefaultFlags_Clicked));
	
	builder->get_widget("radiobuttonOpenSoundfileWithNothing",radiobuttonOpenSoundfileWithNothing);
	builder->get_widget("radiobuttonOpenSoundfileWithMediaPlayer",radiobuttonOpenSoundfileWithMediaPlayer);
	builder->get_widget("radiobuttonOpenSoundfileWithWaveEditor",radiobuttonOpenSoundfileWithWaveEditor);

	builder->get_widget("entryPythonCompilerFlags",entryPythonCompilerFlags);
	builder->get_widget("buttonPythonDefaultFlags",buttonPythonDefaultFlags);
	buttonPythonDefaultFlags->signal_clicked().connect(
		sigc::mem_fun(*this, &wxSettings::on_buttonPythonDefaultFlags_Clicked));
	
	builder->get_widget("entryLuaCompilerFlags",entryLuaCompilerFlags);
	builder->get_widget("buttonLuaDefaultFlags",buttonLuaDefaultFlags);
	buttonLuaDefaultFlags->signal_clicked().connect(
		sigc::mem_fun(*this, &wxSettings::on_buttonLuaDefaultFlags_Clicked));
	
	builder->get_widget("entryTextFontName",entryTextFontName);

	builder->get_widget("buttonBrowseFontsText",buttonBrowseFontsText);
	buttonBrowseFontsText->signal_clicked().connect(
		sigc::mem_fun(*this, &wxSettings::on_buttonBrowseFontsText_Clicked));
	
	builder->get_widget("spinbuttonTextFontSize",spinbuttonTextFontSize);
	builder->get_widget("spinbuttonTextTabIndentSize",spinbuttonTextTabIndentSize);
	
	builder->get_widget("buttonDefaultFontText",buttonDefaultFontText);
	buttonDefaultFontText->signal_clicked().connect(
		sigc::mem_fun(*this, &wxSettings::on_buttonDefaultFontText_Clicked));
	
	builder->get_widget("checkbuttonTextShowMatchingBracket",checkbuttonTextShowMatchingBracket);
	builder->get_widget("checkbuttonTextSaveBookmarks",checkbuttonTextSaveBookmarks);
	builder->get_widget("checkbuttonTextMarkCaretLine",checkbuttonTextMarkCaretLine);
	builder->get_widget("checkbuttonTextShowVerticalRuler",checkbuttonTextShowVerticalRuler);
	builder->get_widget("checkbuttonShowFoldLine",checkbuttonShowFoldLine);

	builder->get_widget("buttonDefaultTextProperties",buttonDefaultTextProperties);
	buttonDefaultTextProperties->signal_clicked().connect(
		sigc::mem_fun(*this, &wxSettings::on_buttonDefaultTextProperties_Clicked));


	builder->get_widget("buttonDefaultSyntaxCSound",buttonDefaultSyntaxCSound);
	buttonDefaultSyntaxCSound->signal_clicked().connect(
		sigc::mem_fun(*this, &wxSettings::on_buttonDefaultSyntaxCSound_Clicked));
	
	builder->get_widget("buttonDefaultSyntaxPython",buttonDefaultSyntaxPython);
	buttonDefaultSyntaxPython->signal_clicked().connect(
		sigc::mem_fun(*this, &wxSettings::on_buttonDefaultSyntaxPython_Clicked));
	
	builder->get_widget("buttonDefaultSyntaxLua",buttonDefaultSyntaxLua);
	buttonDefaultSyntaxLua->signal_clicked().connect(
		sigc::mem_fun(*this, &wxSettings::on_buttonDefaultSyntaxLua_Clicked));

	//Python mixed syntax
	builder->get_widget("checkbuttonPythonMixed",checkbuttonPythonMixed);

	//Colors
	builder->get_widget("vboxCSoundColors",vboxCSoundColors);
	builder->get_widget("vboxPythonColors",vboxPythonColors);
	builder->get_widget("vboxLuaColors",vboxLuaColors);
	builder->get_widget("notebookColors",notebookColors);

	
	
	builder->get_widget("checkbuttonOptions",checkbuttonOptions);
	builder->get_widget("checkbuttonInstrMacros",checkbuttonInstrMacros);
	builder->get_widget("checkbuttonInstrOpcode",checkbuttonInstrOpcode);
	builder->get_widget("checkbuttonInstrInstr",checkbuttonInstrInstr);
	builder->get_widget("checkbuttonScoreFunctions",checkbuttonScoreFunctions);
	builder->get_widget("checkbuttonScoreMacros",checkbuttonScoreMacros);
	builder->get_widget("checkbuttonScoreSections",checkbuttonScoreSections);
	builder->get_widget("radiobuttonExplorerFontSmall",radiobuttonExplorerFontSmall);
	builder->get_widget("radiobuttonExplorerFontMedium",radiobuttonExplorerFontMedium);
	builder->get_widget("radiobuttonExplorerFontLarge",radiobuttonExplorerFontLarge);

	builder->get_widget("buttonCreateDesktopShortcut",buttonCreateDesktopShortcut);
	buttonCreateDesktopShortcut->signal_clicked().connect(
		sigc::mem_fun(*this, &wxSettings::on_buttonCreateDesktopShortcut_Clicked));
	
	builder->get_widget("buttonExportSettings",buttonExportSettings);
	buttonExportSettings->signal_clicked().connect(
		sigc::mem_fun(*this, &wxSettings::on_buttonExportSettings_Clicked));
	
	builder->get_widget("buttonImportSettings",buttonImportSettings);
	buttonImportSettings->signal_clicked().connect(
		sigc::mem_fun(*this, &wxSettings::on_buttonImportSettings_Clicked));

	builder->get_widget("buttonCancel",buttonCancel);
	buttonCancel->signal_clicked().connect(
		sigc::mem_fun(*this, &wxSettings::on_buttonCancel_Clicked));
	
	builder->get_widget("buttonSaveSettings",buttonSaveSettings);
	buttonSaveSettings->signal_clicked().connect(
		sigc::mem_fun(*this, &wxSettings::on_buttonSaveSettings_Clicked));

	builder->get_widget("textviewAlternativeCSoundFlags",textviewAlternativeCSoundFlags);
	builder->get_widget("buttonDefaultAlternativeCSoundFlags",buttonDefaultAlternativeCSoundFlags);
	buttonDefaultAlternativeCSoundFlags->signal_clicked().connect(
		sigc::mem_fun(*this, &wxSettings::on_buttonDefaultAlternativeCSoundFlags_Clicked));

	
	builder->get_widget("frameDefaultMediaPlayer",frameDefaultMediaPlayer);
	////frameDefaultMediaPlayer->set_visible(FALSE);
	
}


void wxSettings::on_buttonSaveSettings_Clicked()
{
	//SaveSettings
	ApplySettings();
	settingsWindow->hide();
    SaveSettings();

	//Notify about save clicked
	SettingsSaveNotification();
}

void wxSettings::on_buttonDefaultAlternativeCSoundFlags_Clicked()
{
	textviewAlternativeCSoundFlags->get_buffer()->set_text(getDefaultAlternativeCSoundFlags());
}

Glib::ustring wxSettings::getDefaultAlternativeCSoundFlags()
{
	return 
		"[Realtime output]: -odac\n"
		"[Render to filename]: -W -o\"CompilerOutput.wav\"\n"
		"[Render to file using csd/orc/sco name]: -W -o\"*.wav\"\n"
		"[Render to file asking for its name]: -W -o\"?.wav\"";
}

void wxSettings::on_buttonCancel_Clicked()
{
	closeWindow();
}

void wxSettings::ApplySettings()
{
	//Apply settings of the various widgets
	//radiobuttonStartupActionNothing->set_active((General.StartupAction == 0));
	//radiobuttonStartupActionCSound->set_active((General.StartupAction == 1));
	//radiobuttonStartupActionPython->set_active((General.StartupAction == 2));
	//radiobuttonStartupActionLua->set_active((General.StartupAction == 3));

	//Startup Action
	if (radiobuttonStartupActionLastSession->get_active())
		General.StartupAction = 4;
	else if(radiobuttonStartupActionLua->get_active())
		General.StartupAction = 3;
    else if (radiobuttonStartupActionPython->get_active())
		General.StartupAction = 2;
    else if (radiobuttonStartupActionCSound->get_active())
		General.StartupAction = 1;
    else 
		General.StartupAction = 0;

	//OrcScoImport
	if (radiobuttonOSIOpenSeparately->get_active())
		General.OrcScoImport = 2;
	else if(radiobuttonOSIConvertToCsd->get_active())
		General.OrcScoImport = 1;
    else 
		General.OrcScoImport = 0;

	//Default Media Player
	if(radiobuttonExternalMediaPlayer->get_active())
		General.DefaultWavePlayer = 1;
    else 
		General.DefaultWavePlayer = 0;

	
	General.ShowReadOnlyFileMessage = checkbuttonShowReadOnlyAlert->get_active();
	//General.BringWinXoundToFrontForCabbage = checkbuttonCabbageFileUpdated->get_active();
	//General.CheckForCabbageUpdates = checkbuttonCheckForCabbageUpdates->get_active();                                 
	General.CSoundAdditionalFlags = textviewAlternativeCSoundFlags->get_buffer()->get_text();
	
	Templates.CSound = textviewTemplatesCSound->get_buffer()->get_text();
	Templates.Python = textviewTemplatesPython->get_buffer()->get_text();
	Templates.Lua = textviewTemplatesLua->get_buffer()->get_text();
	Templates.Cabbage = textviewTemplatesCabbage->get_buffer()->get_text();

	Directory.CSoundConsole = entryCSoundExecutable->get_text();
	Directory.CSoundHelpHTML = entryCSoundHelp->get_text();
	Directory.Winsound = entryCSoundExternalGUI->get_text();
	Directory.WavePlayer = entryWavePlayer->get_text();
	Directory.WaveEditor = entryWaveEditor->get_text();
	Directory.Calculator = entryCalculator->get_text();
	Directory.WorkingDir = entryWorkingDirectory->get_text();
	Directory.PythonConsolePath = entryPythonCompiler->get_text();
	Directory.PythonIdlePath = entryPythonExternalGUI->get_text();
	Directory.LuaConsolePath = entryLuaCompiler->get_text();
	Directory.LuaGuiPath = entryLuaExternalGUI->get_text();
	//Directory.CabbagePath = entryCabbageExe->get_text();

	Directory.UseSFDIR = checkbuttonSFDIR->get_active();
	Directory.SFDIR = entrySFDIR->get_text();
	Directory.SSDIR = entrySSDIR->get_text();
	Directory.SADIR = entrySADIR->get_text();
	Directory.MFDIR = entryMFDIR->get_text();
	Directory.INCDIR = entryINCDIR->get_text();
	Directory.OPCODEDIR = entryOPCODEDIR->get_text();
	
	General.CompilerFontName = entryCompilerFontName->get_text();
	General.CompilerFontSize = spinbuttonCompilerFontSize->get_value();
	General.CSoundDefaultFlags = entryCSoundCompilerFlags->get_text();

	
	//radiobuttonOpenSoundfileWithNothing->set_active((General.OpenSoundFileWith == 0));
	//radiobuttonOpenSoundfileWithMediaPlayer->set_active((General.OpenSoundFileWith == 1));
	//radiobuttonOpenSoundfileWithWaveEditor->set_active((General.OpenSoundFileWith == 2));
	if (radiobuttonOpenSoundfileWithWaveEditor->get_active())
		General.OpenSoundFileWith = 2;
	else if (radiobuttonOpenSoundfileWithMediaPlayer->get_active())
		General.OpenSoundFileWith = 1;
	else 
		General.OpenSoundFileWith = 0;



	General.PythonDefaultFlags = entryPythonCompilerFlags->get_text();
	General.LuaDefaultFlags = entryLuaCompilerFlags->get_text();
	EditorProperties.DefaultFontName = entryTextFontName->get_text();

	EditorProperties.DefaultFontSize = spinbuttonTextFontSize->get_value();
	EditorProperties.DefaultTabSize = spinbuttonTextTabIndentSize->get_value();

	EditorProperties.ShowMatchingBracket = checkbuttonTextShowMatchingBracket->get_active();
	EditorProperties.SaveBookmarks = checkbuttonTextSaveBookmarks->get_active();
	EditorProperties.MarkCaretLine = checkbuttonTextMarkCaretLine->get_active();
	EditorProperties.ShowVerticalRuler = checkbuttonTextShowVerticalRuler->get_active();
	EditorProperties.ShowFoldLine = checkbuttonShowFoldLine->get_active();

	EditorProperties.ExplorerShowOptions = checkbuttonOptions->get_active();
	EditorProperties.ExplorerShowInstrMacros = checkbuttonInstrMacros->get_active();
	EditorProperties.ExplorerShowInstrOpcodes = checkbuttonInstrOpcode->get_active();
	EditorProperties.ExplorerShowInstrInstruments = checkbuttonInstrInstr->get_active();
	EditorProperties.ExplorerShowScoreFunctions = checkbuttonScoreFunctions->get_active();
	EditorProperties.ExplorerShowScoreMacros = checkbuttonScoreMacros->get_active();
	EditorProperties.ExplorerShowScoreSections = checkbuttonScoreSections->get_active();

	RetrieveSyntaxPanelItems(&EditorProperties.CSoundStyles);
	RetrieveSyntaxPanelItems(&EditorProperties.PythonStyles);
	RetrieveSyntaxPanelItems(&EditorProperties.LuaStyles);
	EditorProperties.UseMixedPython = checkbuttonPythonMixed->get_active();
	

	
	//radiobuttonExplorerFontSmall->set_active((EditorProperties.ExplorerFontSize == 0));
	//radiobuttonExplorerFontMedium->set_active((EditorProperties.ExplorerFontSize == 1));
	//radiobuttonExplorerFontLarge->set_active((EditorProperties.ExplorerFontSize == 2));
	if (radiobuttonExplorerFontLarge->get_active())
		EditorProperties.ExplorerFontSize = 2;
	else if (radiobuttonExplorerFontMedium->get_active())
		EditorProperties.ExplorerFontSize = 1;
	else 
		EditorProperties.ExplorerFontSize = 0;

}

bool wxSettings::on_key_press_event(GdkEventKey* event)
{
	//wxGLOBAL->DebugPrint("KEY", "PRESSED");
	if (event->keyval == GDK_Escape)
	{
		on_buttonCancel_Clicked();
	}
	return false;
}


void wxSettings::on_buttonBrowseFontsText_Clicked()
{
	Gtk::FontSelectionDialog fontdialog("TextEditor Font");
	fontdialog.set_transient_for(*settingsWindow);
	fontdialog.set_font_name(
		Glib::ustring::compose("%1 %2", 
		                       entryTextFontName->get_text(),
		                       spinbuttonTextFontSize->get_value()));
	fontdialog.set_preview_text("a1 oscili 10000, 440, 1");

	int result = fontdialog.run();

	//Handle the response:
	switch(result)
	{
		case(Gtk::RESPONSE_OK):
		{
			if(fontdialog.get_font_name() == "") return;
			
			Pango::FontDescription* pfd = new Pango::FontDescription(fontdialog.get_font_name());
			std::cerr << pfd->to_string() << std::endl;
			
			entryTextFontName->set_text(pfd->get_family());
			spinbuttonTextFontSize->set_value(pfd->get_size() / float(Pango::SCALE));
			
			delete pfd;
			break;
		}
	}
}

void wxSettings::on_buttonBrowseFontsCompiler_Clicked()
{
	Gtk::FontSelectionDialog fontdialog("Compiler Font");
	fontdialog.set_transient_for(*settingsWindow);
	fontdialog.set_font_name(
		Glib::ustring::compose("%1 %2", 
		                       entryCompilerFontName->get_text(),
		                       spinbuttonCompilerFontSize->get_value()));

	int result = fontdialog.run();

	//Handle the response:
	switch(result)
	{
		case(Gtk::RESPONSE_OK):
		{
			if(fontdialog.get_font_name() == "") return;
			
			Pango::FontDescription* pfd = new Pango::FontDescription(fontdialog.get_font_name());
			
			entryCompilerFontName->set_text(pfd->get_family());
			spinbuttonCompilerFontSize->set_value(pfd->get_size() / float(Pango::SCALE));
			
			delete pfd;
			break;
		}
	}
}


void wxSettings::on_buttonDefaultTemplates_Clicked()
{
	CreateDefaultTemplates();
	textviewTemplatesCSound->get_buffer()->set_text(Templates.CSound);
	textviewTemplatesPython->get_buffer()->set_text(Templates.Python);
	textviewTemplatesLua->get_buffer()->set_text(Templates.Lua);
	textviewTemplatesCabbage->get_buffer()->set_text(Templates.Cabbage);
}


void wxSettings::on_buttonExportSettings_Clicked()
{  
	try
	{
		Gtk::FileChooserDialog dialog("Export WinXound Settings to File ...",
		                              Gtk::FILE_CHOOSER_ACTION_SAVE);
		dialog.set_transient_for(*settingsWindow);
		dialog.set_current_folder(Directory.LastUsedPath);
		dialog.set_current_name("WinXoundSettings.wxs");
		
		//Add response buttons the the dialog:
		dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
		dialog.add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_OK);

		Gtk::FileFilter filter_supported_files;
		filter_supported_files.set_name("WinXound Settings Files");
		filter_supported_files.add_pattern("*.wxs");
		dialog.add_filter(filter_supported_files);

		//If the WorkingDir is not empty and exists add it to the Open Dialog Box:
		if(Glib::file_test(wxSETTINGS->Directory.WorkingDir, 
		                   (Glib::FILE_TEST_EXISTS | Glib::FILE_TEST_IS_DIR)))
		{
			dialog.add_shortcut_folder(wxSETTINGS->Directory.WorkingDir);
		}
		

		int result = dialog.run();

		if (result != Gtk::RESPONSE_OK) return;

		std::string filename = dialog.get_filename();
		Directory.LastUsedPath = dialog.get_current_folder();

		ApplySettings();
		bool ret = SaveSettings(filename);
		if (ret)
		{
			wxGLOBAL->ShowMessageBox(settingsWindow,
			                         "Settings exported successfully",
			                         "WinXound Information",
			                         Gtk::BUTTONS_OK);
		}
		else
		{			wxGLOBAL->ShowMessageBox(settingsWindow,
			                         "An error has occurred during export!",
			                         "WinXound Information",
			                         Gtk::BUTTONS_OK);
		}			                
		
	}	

	catch (...)
	{
		wxGLOBAL->DebugPrint("wxSettings - buttonExportSettings Error");
	}
	
}
	
void wxSettings::on_buttonImportSettings_Clicked()
{
	try
	{	
		Gtk::FileChooserDialog dialog("Import WinXound Settings File",
		                              Gtk::FILE_CHOOSER_ACTION_OPEN);
		dialog.set_transient_for(*settingsWindow);

		//Add response buttons to the dialog:
		dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
		dialog.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);

		dialog.set_select_multiple(TRUE);
		dialog.set_current_folder(wxSETTINGS->Directory.LastUsedPath);

		//Add filters, so that only certain file types can be selected:
		Gtk::FileFilter filter_supported_files;
		filter_supported_files.set_name("WinXound Settings Files");
		filter_supported_files.add_pattern("*.wxs");
		dialog.add_filter(filter_supported_files);

		//If the WorkingDir is not empty and exists add it to the Open Dialog Box:
		if(Glib::file_test(wxSETTINGS->Directory.WorkingDir, 
		                   (Glib::FILE_TEST_EXISTS | Glib::FILE_TEST_IS_DIR)))
		{
			dialog.add_shortcut_folder(wxSETTINGS->Directory.WorkingDir);
		}

		
		int result = dialog.run();

		if(result != Gtk::RESPONSE_OK) return;

		
		Directory.LastUsedPath = dialog.get_current_folder();
		std::string filename = dialog.get_filename();
		Glib::ustring contents = Glib::file_get_contents(filename);

		//Check only for Linux Settings (not Windows)
		if(contents.find("WinXoundSettingsLinux") == Glib::ustring::npos)
		{
			wxGLOBAL->ShowMessageBox(settingsWindow,
			                         "This file does not contain valid settings for the Linux Version of WinXound.\n"
			                         "Probably you are trying to import a Windows version file...",
			                         "WinXound Settings Information",
			                         Gtk::BUTTONS_OK);
			return;
		}


		result = wxGLOBAL->ShowMessageBox(settingsWindow,
		                                  "Your current WinXound settings will be overwritten\n"
		                                  "by this new imported version.\n"
		                                  "Would you like to proceed?",
		                                  "WinXound Settings Information",
		                                  Gtk::BUTTONS_YES_NO);

		if(result != Gtk::RESPONSE_YES) return;

		//Import Settings
		bool result2 = LoadSettings(filename, false);
		if (result2)
		{
			wxGLOBAL->ShowMessageBox(settingsWindow,
			                         "Settings imported successfully!",
			                         "WinXound Information",
			                         Gtk::BUTTONS_OK);
		}
		else
		{
			CreateDefaultWinXoundSettings();
			wxGLOBAL->ShowMessageBox(settingsWindow,
			                         "An error has occurred during import!",
			                         "WinXound Information",
			                         Gtk::BUTTONS_OK);
			return;
		}

		SaveSettings();
		this->closeWindow();
	}

	catch (...)
	{
		wxGLOBAL->DebugPrint("wxSettings - buttonExportSettings Error");
	}
}


//TODO: PROBABLY TO REMOVE IT!!!???
void wxSettings::on_buttonCreateDesktopShortcut_Clicked()
{
	
	Glib::ustring text = 
		Glib::ustring::compose("[Desktop Entry]\n"
		                       "Version=1.0\n"
		                       "Type=Application\n"
		                       "Name=WinXound\n"
		                       "GenericName=An open source Editor for CSound\n"
		                       "Comment=An open source Editor for CSound\n"
		                       "Exec=%1\n" // %F\n"
		                       "Icon=%2/winxound.png\n"
		                       "Path=%3\n"
		                       "Terminal=false\n",
		                       //"Categories=GTK;Development;IDE;\n"
		                       //"MimeType=\n"
		                       //"StartupNotify=true\n",
		                       wxGLOBAL->getExePath(),
		                       wxGLOBAL->getIconsPath(),
		                       Glib::path_get_dirname(wxGLOBAL->getExePath()));
	
				

	Glib::ustring path = Glib::get_user_special_dir(G_USER_DIRECTORY_DESKTOP);
	path.append("/WinXound.desktop");
	//Glib::file_set_contents(path, text);
	Glib::file_set_contents(path, text);

	chmod(path.c_str(),0777);

	//wxGLOBAL->DebugPrint(path.c_str(), text.c_str());
	
}

void wxSettings::on_buttonDefaultTextProperties_Clicked()
{
	checkbuttonTextShowMatchingBracket->set_active(TRUE);
	checkbuttonTextSaveBookmarks->set_active(TRUE);
	checkbuttonTextMarkCaretLine->set_active(FALSE);
	checkbuttonTextShowVerticalRuler->set_active(FALSE);
	checkbuttonShowFoldLine->set_active(TRUE);
}

void wxSettings::on_buttonDefaultFontText_Clicked()
{	
	entryTextFontName->set_text("Monospace");
	spinbuttonTextFontSize->set_value(10);
	spinbuttonTextTabIndentSize->set_value(8);
}

void wxSettings::on_buttonDefaultFontCompiler_Clicked()
{
	entryCompilerFontName->set_text("Monospace");
	spinbuttonCompilerFontSize->set_value(10);
}


void wxSettings::on_buttonBrowse_Clicked(Glib::ustring name)
{
	//wxGLOBAL->DebugPrint("on_buttonBrowse_Clicked", name.c_str());

	//if(name != "WorkingDirectory")
	Gtk::FileChooserDialog dialog(name, Gtk::FILE_CHOOSER_ACTION_OPEN);

	//Gtk::FileChooserDialog dialog("Please choose a folder",
	//      Gtk::FILE_CHOOSER_ACTION_SELECT_FOLDER);
	dialog.set_transient_for(*settingsWindow);

	//Add response buttons the the dialog:
	dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	dialog.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);

	if(name == "CSoundHelp")
		dialog.set_current_folder("/usr/share/doc/csound-doc/html");
	else if(name == "CabbageExe")
		dialog.set_current_folder(Glib::get_home_dir());
	else
		dialog.set_current_folder("/usr/bin");

	//Add filters
	Gtk::FileFilter filter_supported_files;
	Gtk::FileFilter filter_any;
	if(name == "CSoundExecutable")
	{
		filter_supported_files.set_name("CSound Binary");
		filter_supported_files.add_pattern("csound");			

		filter_any.set_name("Any files");
		filter_any.add_pattern("*");

		dialog.add_filter(filter_supported_files);
		dialog.add_filter(filter_any);
	}
	else if(name == "CSoundHelp")
	{
		filter_supported_files.set_name("CSound Help");
		filter_supported_files.add_pattern("index.html");			

		filter_any.set_name("Any files");
		filter_any.add_pattern("*");

		dialog.add_filter(filter_supported_files);
		dialog.add_filter(filter_any);
	}
	else if(name == "CSoundExternalGUI")
	{
		filter_supported_files.set_name("External GUI binary");
		filter_supported_files.add_pattern("qutecsound");			

		filter_any.set_name("Any files");
		filter_any.add_pattern("*");

		dialog.add_filter(filter_supported_files);
		dialog.add_filter(filter_any);
	}
	else if(name == "WavePlayer")
	{		
		filter_any.set_name("Any files");
		filter_any.add_pattern("*");

		dialog.add_filter(filter_any);
	}
	else if(name == "WaveEditor")
	{
		filter_any.set_name("Any files");
		filter_any.add_pattern("*");

		dialog.add_filter(filter_any);
	}
	else if(name == "Calculator")
	{
		filter_any.set_name("Any files");
		filter_any.add_pattern("*");

		dialog.add_filter(filter_any);
	}
	else if(name == "PythonCompiler")
	{
		filter_supported_files.set_name("Python binary");
		filter_supported_files.add_pattern("python");			

		filter_any.set_name("Any files");
		filter_any.add_pattern("*");

		dialog.add_filter(filter_supported_files);
		dialog.add_filter(filter_any);
	}
	else if(name == "PythonExternalGUI")
	{
		filter_any.set_name("Any files");
		filter_any.add_pattern("*");

		dialog.add_filter(filter_any);
	}
	else if(name == "LuaCompiler")
	{
		//filter_supported_files.set_name("Lua binary");
		//filter_supported_files.add_pattern("lua");			

		filter_any.set_name("Any files");
		filter_any.add_pattern("*");

		//dialog.add_filter(filter_supported_files);
		dialog.add_filter(filter_any);
	}
	else if(name == "LuaExternalGUI")
	{
		filter_any.set_name("Any files");
		filter_any.add_pattern("*");

		dialog.add_filter(filter_any);
	}
	else if(name == "CabbageExe")
	{
		filter_any.set_name("Any files");
		filter_any.add_pattern("*");

		dialog.add_filter(filter_any);
	}

	////dialog.add_filter(filter_supported_files);
	////dialog.add_filter(filter_any);

	int result = dialog.run();

	if(result == Gtk::RESPONSE_OK)
	{
		//wxSETTINGS->Directory.LastUsedPath = dialog.get_current_folder();
		std::string filename = dialog.get_filename();

		if(name == "CSoundExecutable")
		{
			entryCSoundExecutable->set_text(filename);
		}
		else if(name == "CSoundHelp")
		{
			entryCSoundHelp->set_text(filename);
		}
		else if(name == "CSoundExternalGUI")
		{
			entryCSoundExternalGUI->set_text(filename);
		}
		else if(name == "WavePlayer")
		{
			entryWavePlayer->set_text(filename);;
		}
		else if(name == "WaveEditor")
		{
			entryWaveEditor->set_text(filename);
		}
		else if(name == "Calculator")
		{
			entryCalculator->set_text(filename);
		}
		else if(name == "PythonCompiler")
		{
			entryPythonCompiler->set_text(filename);
		}
		else if(name == "PythonExternalGUI")
		{
			entryPythonExternalGUI->set_text(filename);
		}
		else if(name == "LuaCompiler")
		{
			entryLuaCompiler->set_text(filename);;
		}
		else if(name == "LuaExternalGUI")
		{
			entryLuaExternalGUI->set_text(filename);
		}
		else if(name == "CabbageExe")
		{
			entryCabbageExe->set_text(filename);
		}
	}
}
	



void wxSettings::on_buttonBrowseDirectory_Clicked(Glib::ustring name)
{
	//wxGLOBAL->DebugPrint("on_buttonBrowseDirectory_Clicked", name.c_str());

	Gtk::FileChooserDialog dialog(name, Gtk::FILE_CHOOSER_ACTION_SELECT_FOLDER);
	dialog.set_transient_for(*settingsWindow);

	//Add response buttons the the dialog:
	dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	dialog.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);

	if(name == "OPCODEDIR")
		dialog.set_current_folder("/usr");
	else if(name == "WorkingDirectory")
		dialog.set_current_folder(Glib::get_home_dir());
	else
		dialog.set_current_folder(wxSETTINGS->Directory.LastUsedPath);
		
	
	int result = dialog.run();

	if(result == Gtk::RESPONSE_OK)
	{
		wxSETTINGS->Directory.LastUsedPath = dialog.get_current_folder();
		std::string dir = dialog.get_current_folder();

		if(name == "WorkingDirectory")
		{
			entryWorkingDirectory->set_text(dir);
		}
		else if(name == "SFDIR")
		{
			entrySFDIR->set_text(dir);
		}
		else if(name == "SSDIR")
		{
			entrySSDIR->set_text(dir);
		}
		else if(name == "SADIR")
		{
			entrySADIR->set_text(dir);
		}
		else if(name == "MFDIR")
		{
			entryMFDIR->set_text(dir);
		}
		else if(name == "INCDIR")
		{
			entryINCDIR->set_text(dir);
		}
		else if(name == "OPCODEDIR")
		{
			entryOPCODEDIR->set_text(dir);
		}
	}
}

void wxSettings::on_buttonClearEnvironment_Clicked(Glib::ustring name)
{
	if(name == "SFDIR")
	{
		entrySFDIR->set_text("");
	}
	else if(name == "SSDIR")
	{
		entrySSDIR->set_text("");
	}
	else if(name == "SADIR")
	{
		entrySADIR->set_text("");
	}
	else if(name == "MFDIR")
	{
		entryMFDIR->set_text("");
	}
	else if(name == "INCDIR")
	{
		entryINCDIR->set_text("");
	}
	else if(name == "OPCODEDIR")
	{
		entryOPCODEDIR->set_text("");
	}
	else if(name == "CabbageExe")
	{
		Glib::ustring cabbageExePath = 
			Glib::ustring::compose("%1/CabbageLinux/%2",wxGLOBAL->getCabbagePath(), CABBAGE_NAME);

		//std::cerr << "CabbageEXE_Path:" << cabbageExePath << std::endl;
		
		if(Glib::file_test(cabbageExePath, Glib::FILE_TEST_EXISTS))
		{
			this->Directory.CabbagePath = cabbageExePath;
			entryCabbageExe->set_text(this->Directory.CabbagePath);
			return;
		}
		else
		{
			//std::cerr << "CabbageEXE_Path Not Found!" << std::endl;
			entryCabbageExe->set_text("");
		}
	}
}

void wxSettings::on_buttonCSoundDefaultFlags_Clicked()
{
	entryCSoundCompilerFlags->set_text("-B4096 --displays --asciidisplay");
}

void wxSettings::on_buttonPythonDefaultFlags_Clicked()
{
	entryPythonCompilerFlags->set_text("-u");
}

void wxSettings::on_buttonLuaDefaultFlags_Clicked()
{
	entryLuaCompilerFlags->set_text("");
}


void wxSettings::FreeSyntaxLists()
{	
	FreeSyntaxLists("all");
}

void wxSettings::FreeSyntaxLists(Glib::ustring language)
{

	if(language == "csound" ||
	   language == "all")
	{
		//CSound Styles
		if(g_list_first(EditorProperties.CSoundStyles) != NULL)
		{
			for (gint i = g_list_length(EditorProperties.CSoundStyles) - 1; 
			     i > -1 ; 
			     i--)
			{		
				gpointer p = g_list_nth_data(EditorProperties.CSoundStyles, i);			
				EditorProperties.CSoundStyles = 
					g_list_remove(EditorProperties.CSoundStyles, p);
				//g_free(p);
			}
		}
		g_list_free(EditorProperties.CSoundStyles);
		EditorProperties.CSoundStyles = NULL;
	}

	
	if(language == "python" ||
	   language == "all")
	{
		//Python Styles
		if(g_list_first(EditorProperties.PythonStyles) != NULL)
		{
			for (gint i = g_list_length(EditorProperties.PythonStyles) - 1; 
			     i > -1 ; 
			     i--)
			{
				gpointer p = g_list_nth_data(EditorProperties.PythonStyles, i);
				EditorProperties.PythonStyles = 
					g_list_remove(EditorProperties.PythonStyles, p);
				//g_free(p);
			}
		}
		g_list_free(EditorProperties.PythonStyles);
		EditorProperties.PythonStyles = NULL;
	}


	if(language == "lua" ||
	   language == "all")
	{
		//Lua Styles
		if(g_list_first(EditorProperties.LuaStyles) != NULL)
		{
			for (gint i = g_list_length(EditorProperties.LuaStyles) - 1; 
			     i > -1 ; 
			     i--)
			{
				gpointer p = g_list_nth_data(EditorProperties.LuaStyles, i);
				EditorProperties.LuaStyles = 
					g_list_remove(EditorProperties.LuaStyles, p);
				//g_free(p);
			}
		}
		g_list_free(EditorProperties.LuaStyles);
		EditorProperties.LuaStyles = NULL;
	}

	
}



void wxSettings::on_buttonAutoSearchPaths_Clicked()
{

	Glib::ustring strCSoundExecutable,
				  strCSoundHelp,
				  strCSoundExternalGUI,
				  strPythonCompiler,
				  strPythonExternalGUI,
				  strLuaCompiler,
				  strLuaExternalGUI,
				  strWavePlayer,
				  strWaveEditor,
				  strCalculator;


	LookForBinaries(strCSoundExecutable,
	                strCSoundHelp,
	                strCSoundExternalGUI,
	                strPythonCompiler,
	                strPythonExternalGUI,
	                strLuaCompiler,
	                strLuaExternalGUI,
	                strWavePlayer,
	                strWaveEditor,
	                strCalculator);	

	//CSOUND EXECUTABLE
	entryCSoundExecutable->set_text(strCSoundExecutable);

	//CSOUND HELP (HTML)
	entryCSoundHelp->set_text(strCSoundHelp);

	//CSOUND GUI (QuteCsound)
	entryCSoundExternalGUI->set_text(strCSoundExternalGUI);

	//PYTHON COMPILER
	entryPythonCompiler->set_text(strPythonCompiler);

	//PYTHON GUI
	entryPythonExternalGUI->set_text(strPythonExternalGUI);

	//LUA COMPILER
	entryLuaCompiler->set_text(strLuaCompiler);

	//LUA GUI
	entryLuaExternalGUI->set_text(strLuaExternalGUI);	

	//WAVE PLAYER
	entryWavePlayer->set_text(strWavePlayer);

	//WAVE EDITOR
	entryWaveEditor->set_text(strWaveEditor);

	//CALCULATOR
	entryCalculator->set_text(strCalculator);


	                         
}



void wxSettings::LookForBinaries(Glib::ustring& entryCSoundExecutable,
                                 Glib::ustring& entryCSoundHelp,
                                 Glib::ustring& entryCSoundExternalGUI,
                                 Glib::ustring& entryPythonCompiler,
                                 Glib::ustring& entryPythonExternalGUI,
                                 Glib::ustring& entryLuaCompiler,
                                 Glib::ustring& entryLuaExternalGUI,
                                 Glib::ustring& entryWavePlayer,
                                 Glib::ustring& entryWaveEditor,
                                 Glib::ustring& entryCalculator)
{
	
	Glib::ustring strNotFound = "... NOT FOUND ...";

	
	//CSOUND EXECUTABLE
	entryCSoundExecutable = strNotFound;
	if(Glib::file_test("/usr/bin/csound", Glib::FILE_TEST_EXISTS))
		entryCSoundExecutable = "/usr/bin/csound";
	else if(Glib::file_test("/usr/local/bin/csound", Glib::FILE_TEST_EXISTS))
		entryCSoundExecutable = "/usr/local/bin/csound";

	
	//CSOUND HELP (HTML)
	entryCSoundHelp = strNotFound;
	if(Glib::file_test("/usr/share/doc/csound-doc/html/index.html", Glib::FILE_TEST_EXISTS))
		entryCSoundHelp = "/usr/share/doc/csound-doc/html/index.html";
	else if(Glib::file_test("/usr/local/share/doc/csound-doc/html/index.html", Glib::FILE_TEST_EXISTS))
		entryCSoundHelp = "/usr/local/share/doc/csound-doc/html/index.html";

	
	//CSOUND GUI (QuteCsound)
	entryCSoundExternalGUI = strNotFound;
	//---/usr/bin
	if(Glib::file_test("/usr/bin/qutecsound", Glib::FILE_TEST_EXISTS))
		entryCSoundExternalGUI = "/usr/bin/qutecsound";
	else if(Glib::file_test("/usr/bin/qutecsoundf", Glib::FILE_TEST_EXISTS))
		entryCSoundExternalGUI = "/usr/bin/qutecsoundf";
	else if(Glib::file_test("/usr/bin/qutecsound-f", Glib::FILE_TEST_EXISTS))
		entryCSoundExternalGUI = "/usr/bin/qutecsound-f";
	else if(Glib::file_test("/usr/bin/qutecsound-d", Glib::FILE_TEST_EXISTS))
		entryCSoundExternalGUI = "/usr/bin/qutecsound-d";
	//---/usr/local/bin
	else if(Glib::file_test("/usr/local/bin/qutecsound", Glib::FILE_TEST_EXISTS))
		entryCSoundExternalGUI = "/usr/local/bin/qutecsound";
	else if(Glib::file_test("/usr/local/bin/qutecsoundf", Glib::FILE_TEST_EXISTS))
		entryCSoundExternalGUI = "/usr/local/bin/qutecsoundf";
	else if(Glib::file_test("/usr/local/bin/qutecsound-f", Glib::FILE_TEST_EXISTS))
		entryCSoundExternalGUI = "/usr/local/bin/qutecsound-f";
	else if(Glib::file_test("/usr/local/bin/qutecsound-d", Glib::FILE_TEST_EXISTS))
		entryCSoundExternalGUI = "/usr/local/bin/qutecsound-d";

	
	//PYTHON COMPILER
	entryPythonCompiler = strNotFound;
	if(Glib::file_test("/usr/bin/python", Glib::FILE_TEST_EXISTS))
		entryPythonCompiler = "/usr/bin/python";
	else if(Glib::file_test("/usr/local/bin/python", Glib::FILE_TEST_EXISTS))
		entryPythonCompiler = "/usr/local/bin/python";

	
	//PYTHON GUI
	entryPythonExternalGUI = strNotFound;
	if(Glib::file_test("/usr/bin/idle", Glib::FILE_TEST_EXISTS))
		entryPythonExternalGUI = "/usr/bin/idle";
	else if(Glib::file_test("/usr/local/bin/idle", Glib::FILE_TEST_EXISTS))
		entryPythonExternalGUI = "/usr/local/bin/idle";

	
	//LUA COMPILER
	entryLuaCompiler = strNotFound;
	if(Glib::file_test("/usr/bin/lua", Glib::FILE_TEST_EXISTS))
		entryLuaCompiler = "/usr/bin/lua";
	else if(Glib::file_test("/usr/local/bin/lua", Glib::FILE_TEST_EXISTS))
		entryLuaCompiler = "/usr/local/bin/lua";
	else
	{
		Glib::Dir dir ("/usr/bin/");
		for(Glib::Dir::iterator p = dir.begin(); p != dir.end(); ++p)
		{
			Glib::ustring temp = *p;
			if(temp.find("lua") != Glib::ustring::npos)
			{
				entryLuaCompiler = "/usr/bin/" + temp;
				break;
			}
		}
	}

	
	//LUA GUI
	entryLuaExternalGUI = strNotFound;

	
	//WAVE PLAYER
	entryWavePlayer = strNotFound;
	if(Glib::file_test("/usr/bin/totem", Glib::FILE_TEST_EXISTS))
		entryWavePlayer = "/usr/bin/totem";
	else if(Glib::file_test("/usr/bin/kplayer", Glib::FILE_TEST_EXISTS)) 
		entryWavePlayer = "/usr/bin/kplayer";
	else if(Glib::file_test("/usr/bin/amarok", Glib::FILE_TEST_EXISTS))
		entryWavePlayer = "/usr/bin/amarok";
	else if(Glib::file_test("/usr/bin/vlc", Glib::FILE_TEST_EXISTS)) 
		entryWavePlayer = "/usr/bin/vlc";

	
	//WAVE EDITOR
	entryWaveEditor = strNotFound;
	if(Glib::file_test("/usr/bin/audacity", Glib::FILE_TEST_EXISTS))
		entryWaveEditor = "/usr/bin/audacity";
	else if(Glib::file_test("/usr/bin/rezound", Glib::FILE_TEST_EXISTS))
		entryWaveEditor = "/usr/bin/rezound";
	else if(Glib::file_test("/usr/bin/jokosher", Glib::FILE_TEST_EXISTS))
		entryWaveEditor = "/usr/bin/jokosher";
	else if(Glib::file_test("/usr/bin/gnusound", Glib::FILE_TEST_EXISTS))
		entryWaveEditor = "/usr/bin/gnusound";


	//CALCULATOR
	entryCalculator = strNotFound;
	if(Glib::file_test("/usr/bin/gcalctool", Glib::FILE_TEST_EXISTS))
		entryCalculator = "/usr/bin/gcalctool";
	else if(Glib::file_test("/usr/bin/kcalc", Glib::FILE_TEST_EXISTS))
		entryCalculator = "/usr/bin/kcalc";


	Glib::ustring message = 
		"CSound Compiler: " + entryCSoundExecutable + "\n" +
		"CSound Html Help: " + entryCSoundHelp + "\n" +
		"CSound External GUI: " + entryCSoundExternalGUI + "\n" +
		"Python Compiler: " + entryPythonCompiler + "\n" +
		"Python External GUI: " + entryPythonExternalGUI + "\n" +
		"Lua Compiler: " + entryLuaCompiler + "\n" +
		"Lua External GUI: " + entryLuaExternalGUI + "\n" +
		"Wave Player: " + entryWavePlayer + "\n" +
		"Wave Editor: " + entryWaveEditor + "\n" +
		"Calculator: " + entryCalculator + "\n";

	wxGLOBAL->ShowMessageBox(message.c_str(),
	                         "WinXound Autosearch Paths result",
	                         Gtk::BUTTONS_OK);
	
}










void wxSettings::CreateSyntaxPanelItems(GList* styleList)
{

	//Create the Tree model:
	//if(synRefTreeModel != NULL)
	//	synRefTreeModel.clear();	
	Glib::RefPtr<Gtk::TreeStore> synRefTreeModel;
	synRefTreeModel = Gtk::TreeStore::create(synColumns);

	Glib::ustring language;
	if(styleList == EditorProperties.LuaStyles)
		language = "lua";
	else if(styleList == EditorProperties.PythonStyles)
		language = "python";
	else if(styleList == EditorProperties.CSoundStyles)
		language = "csound";
	else 
		return;
		

	Glib::ustring color = "";
	Glib::ustring name = "";
	//Glib::ustring foreColor = "";
	//Glib::ustring backColor = "";
	bool bold = false;
	bool italic = false;
	gint alpha = 256;
	
	
	for (uint i = 0; i < g_list_length(styleList); i++)
	{
		//output.append((const gchar*)g_list_nth(EditorProperties.CSoundStyles, i)->data);
		gint stylenumber = 
			StyleGetStyleNumber((const gchar*)g_list_nth(styleList, i)->data);	

		name = StyleGetFriendlyName(language, stylenumber);
		//foreColor = StyleGetForeColor(language, stylenumber);
		//backColor = StyleGetBackColor(language, stylenumber);
		bold = StyleGetBold(language, stylenumber);
		italic = StyleGetItalic(language, stylenumber);
		alpha = StyleGetAlpha(language, stylenumber);
		
		/*
		//FORE COLOR
		Gtk::Button* foreButton = new Gtk::Button();
		color.parse(StyleGetForeColor("csound", stylenumber));
		foreButton->modify_bg(Gtk::STATE_NORMAL, color);
		if(name == "Bookmarks")
			foreButton->set_sensitive(TRUE);
		else
			foreButton->set_sensitive(FALSE);

			
		//BACK COLOR
		Gtk::Button* backButton = new Gtk::Button();
		color.parse(StyleGetBackColor("csound", stylenumber));
		backButton->modify_bg(Gtk::STATE_NORMAL, color);
		if(name == "VerticalRuler" ||
		   name == "CaretLineMarker")
			backButton->set_sensitive(TRUE);
		else
			backButton->set_sensitive(FALSE);


		//BOLD
		Gtk::CheckButton* boldButton = new Gtk::CheckButton();
		boldButton->set_active(StyleGetBold("csound", stylenumber));		
		//ITALIC
		Gtk::CheckButton* italicButton = new Gtk::CheckButton();
		italicButton->set_active(StyleGetItalic("csound", stylenumber));

		if(name == "Margins" ||
		   name == "TextSelection" ||
		   name == "Bookmarks" || 
		   name == "VerticalRuler" ||
		   name == "CaretLineMarker")
		{
			boldButton->set_sensitive(FALSE);
			italicButton->set_sensitive(FALSE);
		}
		else
		{
			boldButton->set_sensitive(TRUE);
			italicButton->set_sensitive(TRUE);
		}

		
		//ALPHA
		Gtk::SpinButton* alphaButton = new Gtk::SpinButton();
		alphaButton->set_value(StyleGetAlpha("csound", stylenumber));	
		if(name == "Bookmarks")
			alphaButton->set_sensitive(TRUE);
		else
			alphaButton->set_sensitive(FALSE);
		
		*/



		
		Glib::RefPtr<Gdk::Pixbuf>  pixbufFORE;
		pixbufFORE = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB,
		                             FALSE, 8,
		                             30,14);
		//gdk_color_parse (StyleGetForeColor(language, stylenumber).c_str(), &color);
		color = StyleGetForeColor(language, stylenumber);
		color.erase(0,1);
		color.insert(0,"0x");
		color.append("FF");
		//int c = std::strtol(color.c_str(), NULL, 16);
		pixbufFORE->fill(wxGLOBAL->StringToHex(color));


		
		Glib::RefPtr<Gdk::Pixbuf>  pixbufBACK;
		pixbufBACK = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB,
		                             false, 8,
		                             30,14);
		//gdk_color_parse (StyleGetBackColor(language, stylenumber).c_str(), &color);
		color = StyleGetBackColor(language, stylenumber);
		color.erase(0,1);
		color.insert(0,"0x");
		color.append("FF");
		//c = std::strtol(color.c_str(), NULL, 16);
		pixbufBACK->fill(wxGLOBAL->StringToHex(color));



		

		Gtk::TreeModel::Row rowSyntax = *(synRefTreeModel->append());
		rowSyntax[synColumns.FriendlyName] = name;
		rowSyntax[synColumns.ForeColor] = pixbufFORE; //foreColor;
		rowSyntax[synColumns.BackColor] = pixbufBACK; //backColor;
		rowSyntax[synColumns.Bold] = bold;
		rowSyntax[synColumns.Italic] = italic;
		rowSyntax[synColumns.Alpha] = alpha;
		rowSyntax[synColumns.StyleNumber] = stylenumber;
		
	}

	/*
		//FORE COLOR
		Gtk::Button* foreButton = new Gtk::Button();
		color.parse(StyleGetForeColor("csound", stylenumber));
		foreButton->modify_bg(Gtk::STATE_NORMAL, color);
	*/

	Gtk::TreeView* tree = NULL;

	if(language == "lua")
		tree = treeLuaColors;
	else if(language == "python")
		tree = treePythonColors;
	else
		tree = treeCSoundColors;
	
	//treeCSoundColors = new Gtk::TreeView();
	tree->set_model(synRefTreeModel);
	//tree->remove_all_columns();
	tree->append_column("Name", synColumns.FriendlyName);
	tree->append_column("Fore", synColumns.ForeColor);
	tree->append_column("Back", synColumns.BackColor);
	tree->append_column_editable("Bold", synColumns.Bold);
	tree->append_column_editable("Italic", synColumns.Italic);
	tree->append_column_editable("Alpha", synColumns.Alpha);
	//tree->expand_all();
	
	

	/*
	Gtk::TreeView* tree = new Gtk::TreeView();
	tree->set_model(synRefTreeModel);
	tree->append_column("Name", synColumns.FriendlyName);
	
	//Create a new coloumn to add correctly the pixbuf images
	Gtk::TreeView::Column* pColumn =
    Gtk::manage( new Gtk::TreeView::Column("Fore") );
	
	pColumn->pack_start(synColumns.foreButton, false);
	
	//Add the TreeView's view columns:
	tree->append_column(*pColumn);

	*/


	//treeCSoundColors->add_events(Gdk::BUTTON_PRESS_MASK);
	tree->signal_button_press_event().connect(
		sigc::mem_fun(*this, &wxSettings::on_syntax_colors_Clicked), false);
	

	Gtk::ScrolledWindow* mScrolledWindow = Gtk::manage(new Gtk::ScrolledWindow());
	mScrolledWindow->add(*tree);
	mScrolledWindow->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

	if(language == "lua")
	{
		vboxLuaColors->pack_start(*mScrolledWindow, TRUE, TRUE, 0);
		vboxLuaColors->reorder_child(*mScrolledWindow,0);
	}
	else if(language == "python")
	{
		vboxPythonColors->pack_start(*mScrolledWindow, TRUE, TRUE, 0);
		vboxPythonColors->reorder_child(*mScrolledWindow,0);
	}
	else
	{
		vboxCSoundColors->pack_start(*mScrolledWindow, TRUE, TRUE, 0);
		vboxCSoundColors->reorder_child(*mScrolledWindow,0);
	}


	try
	{
		//SELECT THE FIRST ROW OF THE TREEVIEW
		Glib::RefPtr<Gtk::TreeSelection> sel = tree->get_selection();
		Gtk::TreeModel::Row row = synRefTreeModel->children()[0]; //The fifth row.
		if (row) sel->select(row);
	}
	catch(...)
	{
		wxGLOBAL->DebugPrint("wxSettings CreateSyntaxPanelItems - SelectFirstRow Error");
	}
	
}

bool wxSettings::on_syntax_colors_Clicked(GdkEventButton* event)
{
	Glib::ustring name = "";
	Glib::ustring foreColor = "";
	Glib::ustring backColor = "";

	Glib::ustring language = "";
	Gtk::TreeView* tree = NULL;

	if(notebookColors->get_current_page() == 2)
	{
		tree = treeLuaColors;
		language = "lua";
	}
	else if(notebookColors->get_current_page() == 1)
	{
		tree = treePythonColors;
		language = "python";
	}
	else
	{
		tree = treeCSoundColors; 
		language = "csound";
	}

	//std::cout << language << std::endl;
	
	Gtk::TreePath path;
	Gtk::TreeViewColumn* col;
	int cell_x, cell_y;
	//tree->get_path_at_pos((int)event->x, (int)event->y, path);
	tree->get_path_at_pos((int)event->x, (int)event->y, 
	                      path, col,
	                      cell_x, cell_y);
	
	if(!path)return false;
	
	Gtk::TreeModel::iterator iter = tree->get_model()->get_iter(path);
	

	//Glib::RefPtr<Gtk::TreeSelection> sel = treeViewStructure->get_selection();
	//Gtk::TreeModel::iterator iter = sel->get_selected();;
	if(iter) //If anything is selected
	{
		//Retrieve row text
		Gtk::TreeModel::Row row = *iter;
		//Do something with the row.
		row.get_value(0, name);

		Glib::RefPtr<Gdk::Pixbuf>  pixbufFORE;
		row.get_value(1, pixbufFORE);

		Glib::RefPtr<Gdk::Pixbuf>  pixbufBACK;
		row.get_value(2, pixbufBACK);

		//std::cout << name << "Title: " << col->get_title() << std::endl;

		std::vector<int> indexes = path.get_indices();
		if(col->get_title() == "Fore") 
			PickColor(indexes[0], pixbufFORE, language);
		else if(col->get_title() == "Back") 
			PickColor(indexes[0], pixbufBACK, language);

	}
	
	return false;
}

void wxSettings::PickColor(gint index, Glib::RefPtr<Gdk::Pixbuf> pixbuf, Glib::ustring language)
{
	//std::cout << "Index: " << index << " - Language: " << language << std::endl;

	if(language == "lua")
		tempList = EditorProperties.LuaStyles;
	else if(language == "python")
		tempList = EditorProperties.PythonStyles;
	else
		tempList = EditorProperties.CSoundStyles;
	
	gint stylenumber = 
			StyleGetStyleNumber((const gchar*)g_list_nth(tempList, index)->data);	

	Gtk::ColorSelectionDialog dialog("Pick color");
    dialog.set_transient_for(*settingsWindow);

    Gtk::ColorSelection* colorsel = dialog.get_colorsel();

	
	//Retrieve old color
	////Glib::ustring oldColor = StyleGetForeColor(language, stylenumber);
	Gdk::Color color;
	guint8* pixelsFORE = pixbuf->get_pixels();
	color.set_rgb(pixelsFORE[0] * 65535 / 255,
	              pixelsFORE[1] * 65535 / 255,
	              pixelsFORE[2] * 65535 / 255);


	//Set current color
	////Gdk::Color color(oldColor);
	////colorsel->set_has_opacity_control(FALSE);
    colorsel->set_previous_color(color);
    colorsel->set_current_color (color);
    colorsel->set_has_palette (true);

    if (dialog.run() == Gtk::RESPONSE_OK)
    {
        //color_ = colorsel->get_current_color();
		//Glib::ustring strColor = colorsel->get_current_color().to_string();
		//std::cout << strColor << std::endl;
		//std::cout << wxGLOBAL->HexToString(colorsel->get_current_color()).uppercase() << std::endl;

		Glib::ustring strColor = wxGLOBAL->HexToString(colorsel->get_current_color()).uppercase();
		strColor.erase(0,1);
		strColor.insert(0,"0x");
		strColor.append("FF");
		pixbuf->fill(wxGLOBAL->StringToHex(strColor));

    }
}

void wxSettings::RefreshSyntaxColors(Glib::ustring language)
{
	Gtk::TreeView* tree = NULL;
	GList* list = NULL;
	
	if(language == "lua")
	{
		tree = treeLuaColors;
		list = EditorProperties.LuaStyles;
	}
	else if(language == "python")
	{
		tree = treePythonColors;
		list = EditorProperties.PythonStyles;
	}
	else if(language == "csound")
	{
		tree = treeCSoundColors;
		list = EditorProperties.CSoundStyles;
	}
	else
		return;



	Glib::ustring color = "";
	gint index = 0;
	Glib::ustring name = "";
	bool bold = false;
	bool italic = false;
	gint alpha = 256;

	//ITERATE THROUGH ALL TREE ELEMENTS
	//for(guint index = 0; index < synRefTreeModel->children().size(); index++)
	for (Gtk::TreeIter iter = tree->get_model()->children().begin(); 
	     iter != tree->get_model()->children().end();
	     ++iter) //iter++)
	{
		if(g_list_first(list) == NULL) return;
		
		gint stylenumber = 
			StyleGetStyleNumber((const gchar*)g_list_nth(list, index)->data);
		index++;

		if(iter)
		{
			//Retrieve row text
			Gtk::TreeModel::Row row = *iter;
			
			//Do something with the row.
			row.get_value(0, name);

			Glib::RefPtr<Gdk::Pixbuf>  pixbufFORE;
			row.get_value(1, pixbufFORE);

			Glib::RefPtr<Gdk::Pixbuf>  pixbufBACK;
			row.get_value(2, pixbufBACK);

			//Set values
			bold = StyleGetBold(language, stylenumber);
			italic = StyleGetItalic(language, stylenumber);
			alpha = StyleGetAlpha(language, stylenumber);
			row.set_value(3, bold);
			row.set_value(4, italic);
			row.set_value(5, alpha);	
			row.set_value(6, stylenumber);	

			color = StyleGetForeColor(language, stylenumber);
			color.erase(0,1);
			color.insert(0,"0x");
			color.append("FF");
			pixbufFORE->fill(wxGLOBAL->StringToHex(color));

			color = StyleGetBackColor(language, stylenumber);
			color.erase(0,1);
			color.insert(0,"0x");
			color.append("FF");
			pixbufBACK->fill(wxGLOBAL->StringToHex(color));

		}
	}	
}


void wxSettings::RetrieveSyntaxPanelItems(GList** styleList)
{
	
	std::cout << "RetrieveSyntaxPanelItems" << std::endl;
	
	Gtk::TreeView* tree = NULL;
	
	if(*styleList == EditorProperties.LuaStyles)
	{
		tree = treeLuaColors;
	}
	else if(*styleList == EditorProperties.PythonStyles)
	{
		tree = treePythonColors;
	}
	else if(*styleList == EditorProperties.CSoundStyles)
	{
		tree = treeCSoundColors;
	}
	else
		return;



	//FREE THE LIST
	if(g_list_first(*styleList) != NULL)
	{
		for (gint i = g_list_length(*styleList) - 1; 
		     i > -1 ; 
		     i--)
		{		
			gpointer p = g_list_nth_data(*styleList, i);			
			*styleList = g_list_remove(*styleList, p);
		}
	}
	//g_list_free(*styleList);


	
	gint index = 0;
	
	Glib::ustring name = "";
	Glib::ustring fore = "";
	Glib::ustring back = "";
	bool bold = false;
	bool italic = false;
	gint alpha = 256;
	gint stylenumber = 0;

	Gdk::Color color;
	Glib::ustring value = "";
	
	//ITERATE THROUGH ALL TREE ELEMENTS (Retrieve values)
	//for(guint index = 0; index < synRefTreeModel->children().size(); index++)
	for (Gtk::TreeIter iter = tree->get_model()->children().begin(); 
	     iter != tree->get_model()->children().end();
	     ++iter) //iter++)
	{
		
	
		if(iter) //If anything is selected
		{
			value = "";
			
			//Retrieve row text
			Gtk::TreeModel::Row row = *iter;
			
			//Do something with the row.
			row.get_value(0, name);

			//FORE COLOR
			Glib::RefPtr<Gdk::Pixbuf>  pixbufFORE;
			row.get_value(1, pixbufFORE);
			//guchar* pixels = pixbufFORE->get_pixels();
			guint8* pixelsFORE = pixbufFORE->get_pixels();
			//int rowstride = pixbufFORE->get_rowstride();
			//int n_channels = pixbufFORE->get_n_channels();
			//p = pixels * rowstride * n_channels;
			//printf("Color is {%d, %d, %d}\n", pixels[0], pixels[1], pixels[2]);
			color.set_rgb(pixelsFORE[0] * 65535 / 255, 
			              pixelsFORE[1] * 65535 / 255, 
			              pixelsFORE[2] * 65535 / 255);
			fore = wxGLOBAL->HexToString(color).uppercase();


			//BACK COLOR
			Glib::RefPtr<Gdk::Pixbuf>  pixbufBACK;
			row.get_value(2, pixbufBACK);
			guint8* pixelsBACK = pixbufBACK->get_pixels();
			color.set_rgb(pixelsBACK[0] * 65535 / 255, 
			              pixelsBACK[1] * 65535 / 255, 
			              pixelsBACK[2] * 65535 / 255);
			back = wxGLOBAL->HexToString(color).uppercase();


			
			//BOLD, ITALIC, ALPHA
			row.get_value(3, bold);
			row.get_value(4, italic);
			row.get_value(5, alpha);

			//StyleNumber
			row.get_value(6, stylenumber);

			//Check Alpha value
			if(alpha < 0 || alpha > 256)
				alpha = 256;
			 
			//Compose value string
			value = 
				Glib::ustring::compose("%1,%2,%3,%4,%5,%6,%7,%8",
				                       stylenumber,
				                       fore, back,
				                       (bold ? "true" : "false"), 
				                       (italic ? "true" : "false"), 
				                       alpha, "false", name);

			//std::cout << value << std::endl;
		
			////Append value to list
			*styleList = g_list_append (*styleList, 
			                            g_strdup(value.c_str()));

		}	
	}
}
                                            
void wxSettings::on_buttonDefaultSyntaxCSound_Clicked()
{
	FreeSyntaxLists("csound");
	CreateDefaultCSoundStyles();
	RefreshSyntaxColors("csound");
}

void wxSettings::on_buttonDefaultSyntaxPython_Clicked()
{
	FreeSyntaxLists("python");
	CreateDefaultPythonStyles();
	RefreshSyntaxColors("python");
}

void wxSettings::on_buttonDefaultSyntaxLua_Clicked()
{
	FreeSyntaxLists("lua");
	CreateDefaultLuaStyles();
	RefreshSyntaxColors("lua");
}

void wxSettings::OpenDirectoriesTab()
{

}

