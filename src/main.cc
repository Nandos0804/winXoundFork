/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * main.cc
 * Copyright (C) Stefano Bonetti 2010 <stefano_bonetti@tin.i>
 * 
 * WinXound_GTKmm is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * WinXound_GTKmm is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <gtkmm.h>
#include <iostream>

#include "wx-main.h"


/* For testing propose use the local (not installed) ui file */
/* #define UI_FILE PACKAGE_DATA_DIR"/winxound/ui/winxound_gtkmm.ui" */
//#define UI_FILE "src/winxound_gtkmm.ui"
   
int
main (int argc, char *argv[])
{
	/*
	Gtk::Main kit(argc, argv);
	wxMain main;
	//Shows the window and returns when it is closed.
	Gtk::Main::run(main);
	return 0;
	*/

	
	try
	{
		Gtk::Main kit(argc, argv);
		//wxMain main;
		wxMain main(argc, argv);
		kit.run(main.get_window());
		return 0;
	}
	catch (std::exception const& ex)
	{
		std::cerr << "main Error: " << ex.what() << std::endl;
		return 1;
	}

}



































////////////////////////////////////////////////////////////////////////////////
//CALLBACKS FOR MENU
////////////////////////////////////////////////////////////////////////////////
void
on_menuitemExit_activate (GtkMenuItem *self, gpointer user_data)
{
	gtk_main_quit ();
}

void
on_menuitemNew_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemOpen_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemSave_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemSaveAs_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemSaveAll_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemClose_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemCloseAll_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemFileInfo_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemImportOrcSco_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemImportOrc_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemImportSco_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemExportOrcSco_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemExportOrc_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemExportSco_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemPrint_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemPrintPreview_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemSettings_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemRecentFiles_activate (GtkMenuItem *self, gpointer user_data)
{

}




////////////////////////////////////////////////////////////////////////////////
//MENU EDIT CALLBACKS
////////////////////////////////////////////////////////////////////////////////
void
on_menuitemUndo_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemRedo_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemCut_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemCopy_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemPaste_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemDelete_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemSelectAll_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemFindAndReplace_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemFindNext_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemFindPrevious_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemJumpToCaret_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemFindLineNumber_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemCommentLine_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemRemoveCommentLine_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemInsertRemoveBookmark_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemRemoveAllBookmarks_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemGoToNextBookmark_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemGoToPreviousBookmark_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemFormatCode_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemFormatCodeOptions_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemCodeRepositoryShowWindow_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemCodeRepositoryStoreSelectedText_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemListOpcodes_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemCSoundOpcodesRepository_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemLineEndingsCRLF_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemLineEndingsCR_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemLineEndingsLF_activate (GtkMenuItem *self, gpointer user_data)
{

}




////////////////////////////////////////////////////////////////////////////////
//MENU VIEW CALLBACKS
////////////////////////////////////////////////////////////////////////////////
void
on_menuitemLineNumbers_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemExplorer_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemOnlineHelp_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemToolbar_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemShowAllTools_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemHideAllTools_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemFullCode_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemSplitHorizontal_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemSplitHorizontalOrcSco_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemSplitVertical_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemSplitVerticalOrcSco_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemShowCode_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemShowCompiler_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemShowHelp_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemShowHideWhiteSpaces_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemShowHideEOLS_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemNavigateBackward_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemNavigateForward_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemSetCaretOnPrimaryView_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemSetCaretOnSecondaryView_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemScreenPositionUP_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemScreenPositionDOWN_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemScreenPositionLEFT_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemScreenPositionRIGHT_activate (GtkMenuItem *self, gpointer user_data)
{

}






////////////////////////////////////////////////////////////////////////////////
//MENU TOOLS CALLBACKS
////////////////////////////////////////////////////////////////////////////////
void
on_menuitemCompile_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemCompileExternal_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemUseWinXoundFlags_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemRunExternalGUI_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemAnalysis_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemMediaPlayer_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemExternalWaveEditor_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemCalculator_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemTerminal_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemWinXoundTest_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemOrchestraHeader_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemOptionsRealtime_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemOptionsFileOutput_activate (GtkMenuItem *self, gpointer user_data)
{

}







////////////////////////////////////////////////////////////////////////////////
//MENU HELP CALLBACKS
////////////////////////////////////////////////////////////////////////////////
void
on_menuitemWinXoundHelp_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemOpcodeHelp_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemCSoundHelp_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_menuitemCSoundTutorial_activate (GtkMenuItem *self, gpointer user_data)
{

}

void
on_About_activate (GtkMenuItem *self, gpointer user_data)
{

}

