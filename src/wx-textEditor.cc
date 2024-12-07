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
#include "wx-textEditor.h"

//#define SCI_WIDTH_MARGINS 50
#define	SCI_MEASURE_STRING "0000000"

#define SSM(sci, m, w, l) scintilla_send_message(sci, m, w, l)

#define SCI_NAMESPACE




wxTextEditor::wxTextEditor()  	
{
	this->CreateNewTextEditor();
	std::cerr << "wxTextEditor Created" << std::endl;
}
wxTextEditor::~wxTextEditor(void)
{	
	this->ClearAllText();
	this->emptyUndoBuffer();
	//this->setSavePoint();	
	
	remove();

	
	//delete textView1_Widget;
	//textView1_Widget = NULL;
	//delete textView2_Widget;
	//textView1_Widget = NULL;

	
	g_object_unref(G_OBJECT(textView2));
	g_object_unref(G_OBJECT(textView1));

	//gtk_widget_unref(GTK_WIDGET(textView1));
	//gtk_widget_unref(GTK_WIDGET(textView2));
	//g_object_unref(textView2);
	//g_object_unref(textView1);

	textView1 = NULL;
	textView2 = NULL;

	std::cout << "wxTextEditor Released" << std::endl;

}



//TextEditorView initWithFrame:
void wxTextEditor::CreateNewTextEditor(void)
{
	SCI_WIDTH_MARGINS_1 = 50;
	SCI_WIDTH_MARGINS_2 = 50;
	
	//GtkWidget* cEditor = scintilla_new();
	//sci = SCINTILLA(cEditor);
	//scintilla_set_id(sci, 0);
	//editor = Glib::wrap(cEditor);
	
	textView1 = SCINTILLA(scintilla_new());
	textView2 = SCINTILLA(scintilla_new());
	//g_object_ref_sink(textView1);
	//g_object_ref_sink(textView2);
	

	//gtk_widget_show(GTK_WIDGET(textView1));
	//gtk_widget_show(GTK_WIDGET(textView2));

   	scintilla_set_id(textView1, 0);
	scintilla_set_id(textView2, 1);

   	gtk_widget_set_usize(GTK_WIDGET(textView1), 500, 100);
	gtk_widget_set_usize(GTK_WIDGET(textView2), 500, 100);
	
	//Set Scintilla DocPointer for secondary view equal to first view
	SSM(textView2, SCI_SETDOCPOINTER, 0, SSM(textView1, SCI_GETDOCPOINTER, 0, 0));
	
	//SET CODEPAGE TO UTF8
	SSM(textView1, SCI_SETCODEPAGE, SC_CP_UTF8, 0);
	SSM(textView2, SCI_SETCODEPAGE, SC_CP_UTF8, 0);
	
	_OldFocusedEditor = NULL;
	_ShowMatchingBracket = false;
	
	//Add scintilla views to our HPaned class
	//add1(*Glib::wrap(GTK_WIDGET(textView1)));
	//add2(*Glib::wrap(GTK_WIDGET(textView2)));
	//pack1 (*Glib::wrap(GTK_WIDGET(textView1)), TRUE, FALSE);
	//pack2 (*Glib::wrap(GTK_WIDGET(textView2)), TRUE, FALSE);
	//this->get_child1()->set_size_request(-1,10);

	//gtk_container_add(GTK_CONTAINER(this), GTK_WIDGET(textView1));
	textView1_Widget = Glib::wrap(GTK_WIDGET(textView1));
	textView2_Widget = Glib::wrap(GTK_WIDGET(textView2));
	add(*textView1_Widget);

	//Show widgets
	show();

	
	//Various Scintilla Settings:
	//[self setTextEditorFont: [NSFont fontWithName:@"Andale Mono" size: 12]];
	SSM(textView1, SCI_STYLESETFONT, 32, (sptr_t)"!Monospace");
	SSM(textView2, SCI_STYLESETFONT, 32, (sptr_t)"!Monospace");
	SSM(textView1, SCI_STYLESETSIZE, 32, 10);
	SSM(textView2, SCI_STYLESETSIZE, 32, 10);
	SCI_WIDTH_MARGINS_1 = measureTextWidth("999999", 1);
	SCI_WIDTH_MARGINS_2 = measureTextWidth("999999", 2);
	SCI_INDENT_MARGINS_1 = getIndentMarginWidth(1);
	SCI_INDENT_MARGINS_2 = getIndentMarginWidth(2);

	
	//Assign margin size for Line Numbers and other margins
	//TextView1:
	SSM(textView1, SCI_MARKERDEFINE, 0, SC_MARK_BACKGROUND); //SC_MARK_BACKGROUND //SC_MARK_ARROW
	SSM(textView1, SCI_SETMARGINTYPEN, 0, SC_MARGIN_NUMBER); //SC_MARGIN_NUMBER
	SSM(textView1, SCI_SETMARGINTYPEN, 1, 0);
	SSM(textView1, SCI_SETMARGINTYPEN, 2, SC_MARGIN_SYMBOL);
	SSM(textView1, SCI_SETMARGINWIDTHN, 0, SCI_WIDTH_MARGINS_1);
	SSM(textView1, SCI_SETMARGINWIDTHN, 1, 0); //10
	SSM(textView1, SCI_SETMARGINWIDTHN, 2, SCI_INDENT_MARGINS_1);
	//Folding stuffs:
	SSM(textView1, SCI_SETMARGINMASKN, 2, SC_MASK_FOLDERS);
	SSM(textView1, SCI_SETMARGINSENSITIVEN, 2, TRUE);
	SSM(textView1, SCI_SETPROPERTY, (uptr_t)"fold", (sptr_t)"1");
	SSM(textView1, SCI_SETPROPERTY, (uptr_t)"fold.at.else", (sptr_t)"1");
	SSM(textView1, SCI_MARKERDEFINE, SC_MARKNUM_FOLDER, SC_MARK_BOXPLUS);
	SSM(textView1, SCI_MARKERDEFINE, SC_MARKNUM_FOLDEROPEN, SC_MARK_BOXMINUS);
	SSM(textView1, SCI_MARKERDEFINE, SC_MARKNUM_FOLDEREND, SC_MARK_EMPTY);
	SSM(textView1, SCI_MARKERDEFINE, SC_MARKNUM_FOLDERMIDTAIL, SC_MARK_EMPTY);
	SSM(textView1, SCI_MARKERDEFINE, SC_MARKNUM_FOLDEROPENMID, SC_MARK_EMPTY);
	SSM(textView1, SCI_MARKERDEFINE, SC_MARKNUM_FOLDERSUB, SC_MARK_VLINE);
	SSM(textView1, SCI_MARKERDEFINE, SC_MARKNUM_FOLDERTAIL, SC_MARK_LCORNER);

	//TextView2:
	SSM(textView2, SCI_MARKERDEFINE, 0, SC_MARK_BACKGROUND); //SC_MARK_BACKGROUND //SC_MARK_ARROW
	SSM(textView2, SCI_SETMARGINTYPEN, 0, SC_MARGIN_NUMBER); //SC_MARGIN_NUMBER
	SSM(textView2, SCI_SETMARGINTYPEN, 1, 0);
	SSM(textView2, SCI_SETMARGINTYPEN, 2, SC_MARGIN_SYMBOL);
	SSM(textView2, SCI_SETMARGINWIDTHN, 0, SCI_WIDTH_MARGINS_2);
	SSM(textView2, SCI_SETMARGINWIDTHN, 1, 0);
	SSM(textView2, SCI_SETMARGINWIDTHN, 2, SCI_INDENT_MARGINS_2);
	//Folding stuffs:
	SSM(textView2, SCI_SETMARGINMASKN, 2, SC_MASK_FOLDERS);
	SSM(textView2, SCI_SETMARGINSENSITIVEN, 2, TRUE);
	SSM(textView2, SCI_SETPROPERTY, (uptr_t)"fold", (sptr_t)"1");
	SSM(textView2, SCI_SETPROPERTY, (uptr_t)"fold.at.else", (sptr_t)"1");
	SSM(textView2, SCI_MARKERDEFINE, SC_MARKNUM_FOLDER, SC_MARK_BOXPLUS);
	SSM(textView2, SCI_MARKERDEFINE, SC_MARKNUM_FOLDEROPEN, SC_MARK_BOXMINUS);
	SSM(textView2, SCI_MARKERDEFINE, SC_MARKNUM_FOLDEREND, SC_MARK_EMPTY);
	SSM(textView2, SCI_MARKERDEFINE, SC_MARKNUM_FOLDERMIDTAIL, SC_MARK_EMPTY);
	SSM(textView2, SCI_MARKERDEFINE, SC_MARKNUM_FOLDEROPENMID, SC_MARK_EMPTY);
	SSM(textView2, SCI_MARKERDEFINE, SC_MARKNUM_FOLDERSUB, SC_MARK_VLINE);
	SSM(textView2, SCI_MARKERDEFINE, SC_MARKNUM_FOLDERTAIL, SC_MARK_LCORNER);



	//[self setShowLineNumbers: true];

	//SCI_SetAdditionalCaretsVisible = 0 (false)
	SSM(textView1, SCI_SETVIRTUALSPACEOPTIONS, 1, 0);
	SSM(textView2, SCI_SETVIRTUALSPACEOPTIONS, 1, 0);
	SSM(textView1, SCI_SETADDITIONALCARETSVISIBLE, 0, 0);
	SSM(textView2, SCI_SETADDITIONALCARETSVISIBLE, 0, 0);


	//SET AUTOCOMPLETION STUFFS
	AutocSetCancelAtStart(FALSE);
	AutocSetDropRestOfWord(TRUE);
	AutocSetAutoHide(FALSE);
	AutocSetMaxHeight(6);  //Default: 5
	


	////SSM(textView1, SCI_SETMOUSEDOWNCAPTURES, 1, 0);


	
	/*
	[[NSNotificationCenter defaultCenter] addObserver:self 
											 selector:@selector(SCI_ZOOM_Notification:) 
												 name:@"SCIZoomChanged" 
											   object:textView1];
	[[NSNotificationCenter defaultCenter] addObserver:self 
											 selector:@selector(SCI_ZOOM_Notification:) 
												 name:@"SCIZoomChanged" 
											   object:textView2];
	*/

	
	//Disable popup menu
	SSM(textView1, SCI_USEPOPUP, 0, 0);
	SSM(textView2, SCI_USEPOPUP, 0, 0);



	//MOUSE CAPTURE
	//SSM(textView1, SCI_SETMOUSEDOWNCAPTURES, 1, 0);
	//SSM(textView2, SCI_SETMOUSEDOWNCAPTURES, 1, 0);
	

	//[self RemoveSplit];
	//[self setFocusOnPrimaryView];
	_OldFocusedEditor = textView1;
	
	
	//HIDE THE SECONDARY VIEW
	gtk_widget_hide(GTK_WIDGET(textView2));
	
	return;

}




////////////////////////////////////////////////////////////////////////////////
//METHODS
////////////////////////////////////////////////////////////////////////////////

gint wxTextEditor::getIndentMarginWidth(int view)
{
	int w = measureTextWidth("9", view) + 3;
	return w;
}

Glib::ustring wxTextEditor::getFontName(int styleNumber)
{
	//const char* result = new char[256];	
	//gchar* result = (gchar*)g_malloc(256);
	gchar* result = new gchar[256];
		
	SSM(textView1, SCI_STYLEGETFONT, styleNumber, (sptr_t)result);

	Glib::ustring ret = result;
	delete[] result;
	
	return ret; 

	//!!!REMEMBER TO FREE RESULT: 
	//g_malloc -> g_free(result);
	//new -> delete[] result
}


//SCI_TEXTWIDTH(int styleNumber, const char *text)
gint wxTextEditor::measureTextWidth(const gchar* text, gint view)
{	
	if(view == 2)
		return SSM(textView2, SCI_TEXTWIDTH, 32, (sptr_t) text);
	else
		return SSM(textView1, SCI_TEXTWIDTH, 32, (sptr_t) text);
}



void wxTextEditor::setSearchFlags(gint searchflags)
{
	//[textView1 setGeneralProperty:SCI_SETSEARCHFLAGS parameter:searchflags value:0];
	//[textView2 setGeneralProperty:SCI_SETSEARCHFLAGS parameter:searchflags value:0];
	SSM(textView1, SCI_SETSEARCHFLAGS, searchflags, 0);
	SSM(textView2, SCI_SETSEARCHFLAGS, searchflags, 0);
}

void  wxTextEditor::setTargetStart(gint mStart)
{
	//[textView1 setGeneralProperty:SCI_SETTARGETSTART parameter:mStart value:0];
	//[textView2 setGeneralProperty:SCI_SETTARGETSTART parameter:mStart value:0];
	SSM(textView1, SCI_SETTARGETSTART, mStart, 0);
	SSM(textView2, SCI_SETTARGETSTART, mStart, 0);
}


void wxTextEditor::setTargetEnd(gint mEnd)
{
	//[textView1 setGeneralProperty:SCI_SETTARGETEND parameter:mEnd value:0];
	//[textView2 setGeneralProperty:SCI_SETTARGETEND parameter:mEnd value:0];
	SSM(textView1, SCI_SETTARGETEND, mEnd, 0);
	SSM(textView2, SCI_SETTARGETEND, mEnd, 0);
}

gint wxTextEditor::searchInTarget(const gchar* text)
{
	//const char* rawValue = [text UTF8String];
	//return [textView1 backend]->WndProc(SCI_SEARCHINTARGET, [text length], (sptr_t)rawValue);

	return SSM(textView1, SCI_SEARCHINTARGET, strlen(text), (sptr_t)text);
}


//SCI_SETSCROLLWIDTH(int pixelWidth)
void wxTextEditor::setScrollWidth(gint pixelWidth)
{
	//[textView1 setGeneralProperty: SCI_SETSCROLLWIDTH parameter: pixelWidth value: 0];
	//[textView2 setGeneralProperty: SCI_SETSCROLLWIDTH parameter: pixelWidth value: 0];
	SSM(textView1, SCI_SETSCROLLWIDTH, pixelWidth, 0);
	SSM(textView2, SCI_SETSCROLLWIDTH, pixelWidth, 0);

}
//SCI_GETSCROLLWIDTH
gint wxTextEditor::getScrollWidth()
{
	//return [textView1 getGeneralProperty:SCI_GETSCROLLWIDTH parameter:0];
	return SSM(textView1, SCI_GETSCROLLWIDTH, 0, 0);
}




//SCI_SETSCROLLWIDTHTRACKING(bool tracking)
//SCI_GETSCROLLWIDTHTRACKING
void wxTextEditor::setScrollWidthTracking(bool tracking)
{
	//[textView1 setGeneralProperty: SCI_SETSCROLLWIDTHTRACKING parameter: tracking value: 0];
	//[textView2 setGeneralProperty: SCI_SETSCROLLWIDTHTRACKING parameter: tracking value: 0];
	SSM(textView1, SCI_SETSCROLLWIDTHTRACKING, tracking, 0);
	SSM(textView2, SCI_SETSCROLLWIDTHTRACKING, tracking, 0);
}


/*
- (void) setContextMenu:(NSMenu*) menu
{
	[[textView1 content] setMenu:menu];
	[[textView2 content] setMenu:menu];
}
*/

gint wxTextEditor::getPositionFromPoint(gint x, gint y)
{
	return SSM(getFocusedEditor(), SCI_POSITIONFROMPOINT, x, y);
}

gint wxTextEditor::getPositionFromPoint(gint x, gint y, gint view)
{
	if(view == 1)
	{
		return SSM(textView1, SCI_POSITIONFROMPOINT, x, y);
	}
	else
	{
		return SSM(textView2, SCI_POSITIONFROMPOINT, x, y);
	}
}

void wxTextEditor::setCaretFore(const gchar* htmlColor)
{
	SSM(textView1, SCI_SETCARETFORE, getColorFromString(htmlColor), 0);
	SSM(textView2, SCI_SETCARETFORE, getColorFromString(htmlColor), 0);
}
void wxTextEditor::setCaretLineBack(const gchar* htmlColor)
{
	SSM(textView1, SCI_SETCARETLINEBACK, getColorFromString(htmlColor), 0);
	SSM(textView2, SCI_SETCARETLINEBACK, getColorFromString(htmlColor), 0);
}

void wxTextEditor::setEdgeColor(const gchar* htmlColor)
{
	SSM(textView1, SCI_SETEDGECOLOUR, getColorFromString(htmlColor), 0);
	SSM(textView2, SCI_SETEDGECOLOUR, getColorFromString(htmlColor), 0);
}

void wxTextEditor::setSelFore(bool useSel, const gchar* htmlColor)
{
	//[textView1 setColorProperty: SCI_SETSELFORE parameter: useSel fromHTML: htmlColor];
	//[textView2 setColorProperty: SCI_SETSELFORE parameter: useSel fromHTML: htmlColor];
	SSM(textView1, SCI_SETSELFORE, useSel, getColorFromString(htmlColor));
	SSM(textView2, SCI_SETSELFORE, useSel, getColorFromString(htmlColor));
}

void wxTextEditor::setSelBack(bool useSel, const gchar* htmlColor)
{
	//[textView1 setColorProperty: SCI_SETSELBACK parameter: useSel fromHTML: htmlColor];
	//[textView2 setColorProperty: SCI_SETSELBACK parameter: useSel fromHTML: htmlColor];
	SSM(textView1, SCI_SETSELBACK, useSel, getColorFromString(htmlColor));
	SSM(textView2, SCI_SETSELBACK, useSel, getColorFromString(htmlColor));
}

void wxTextEditor::StyleSetFore(gint index, const gchar* htmlColor)
{
	//[textView1 setColorProperty: SCI_STYLESETFORE parameter: index fromHTML: htmlColor];
	//[textView2 setColorProperty: SCI_STYLESETFORE parameter: index fromHTML: htmlColor];
	//gint color = getColorFromString(htmlColor);
	SSM(textView1, SCI_STYLESETFORE, index, getColorFromString(htmlColor));
	SSM(textView2, SCI_STYLESETFORE, index, getColorFromString(htmlColor));
}

void wxTextEditor::StyleSetBack(gint index, const gchar* htmlColor)
{
	//[textView1 setColorProperty: SCI_STYLESETBACK parameter: index fromHTML: htmlColor];
	//[textView2 setColorProperty: SCI_STYLESETBACK parameter: index fromHTML: htmlColor];
	SSM(textView1, SCI_STYLESETBACK, index, getColorFromString(htmlColor));
	SSM(textView2, SCI_STYLESETBACK, index, getColorFromString(htmlColor));
}

void wxTextEditor::StyleSetBold(gint index, bool bold)
{
	//[textView1 setGeneralProperty: SCI_STYLESETBOLD parameter: index value: bold];
	//[textView1 setGeneralProperty: SCI_STYLESETBOLD parameter: index value: bold];
	SSM(textView1, SCI_STYLESETBOLD, index, (bold ? 1 : 0));
	SSM(textView2, SCI_STYLESETBOLD, index, (bold ? 1 : 0));
}

void wxTextEditor::StyleSetItalic(gint index, bool italic)
{
	//[textView1 setGeneralProperty: SCI_STYLESETITALIC parameter: index value: italic];
	//[textView1 setGeneralProperty: SCI_STYLESETITALIC parameter: index value: italic];
	SSM(textView1, SCI_STYLESETITALIC, index, (italic ? 1 : 0));
	SSM(textView2, SCI_STYLESETITALIC, index, (italic ? 1 : 0));
}




//gchar* wxTextEditor::getTextEditorFont()	//OK
Glib::ustring wxTextEditor::getTextEditorFont()
{	
	gint	font_size;
	//gchar*  font_name;
	//gchar*  result;
	Glib::ustring font_name;
	Glib::ustring result;

	font_name = this->getFontName(STYLE_DEFAULT);
	font_size = SSM(textView1, SCI_STYLEGETSIZE, STYLE_DEFAULT, 0);

	PangoFontDescription *pfd = pango_font_description_new();
	{
		pango_font_description_set_size(pfd, font_size);
		pango_font_description_set_family(pfd, font_name.c_str());
		result = pango_font_description_to_string(pfd);
	}
	pango_font_description_free(pfd);
	
	//g_free(font_name);
	//delete[] font_name;

	return result;
}

void wxTextEditor::setTextEditorFont(const gchar* fontname, gint fontsize)	//OK
{
	std::cerr << "wxTextEditor setTextEditorFont" << std::endl;

	Glib::ustring fontN = fontname;
	fontN.insert(0, "!");
	
	for (int i = 0; i < 34; i++)
	{
		//[textView1 setStringProperty:SCI_STYLESETFONT parameter:i value:[newfont displayName]];
		//[textView2 setStringProperty:SCI_STYLESETFONT parameter:i value:[newfont displayName]];
		//[textView1 setGeneralProperty:SCI_STYLESETSIZE parameter:i value:[newfont pointSize]];
		//[textView2 setGeneralProperty:SCI_STYLESETSIZE parameter:i value:[newfont pointSize]];
		SSM(textView1, SCI_STYLESETFONT, i, (sptr_t)fontN.c_str());
		SSM(textView2, SCI_STYLESETFONT, i, (sptr_t)fontN.c_str());
		SSM(textView1, SCI_STYLESETSIZE, i, fontsize);
		SSM(textView2, SCI_STYLESETSIZE, i, fontsize);
	}
	
	
	//MEASURE TEXT WIDTH AND SET MARGINS (FOR LINE NUMBERS AND INDENT MARGIN)
	SCI_WIDTH_MARGINS_1 = measureTextWidth(SCI_MEASURE_STRING, 1);
	SCI_WIDTH_MARGINS_2 = measureTextWidth(SCI_MEASURE_STRING, 2);
	SCI_INDENT_MARGINS_1 = getIndentMarginWidth(1);
	SCI_INDENT_MARGINS_2 = getIndentMarginWidth(2);
	
	if(this->getShowLineNumbers())
	{
		//[textView1 setGeneralProperty:SCI_SETMARGINWIDTHN parameter:0 value:SCI_WIDTH_MARGINS];
		//[textView2 setGeneralProperty:SCI_SETMARGINWIDTHN parameter:0 value:SCI_WIDTH_MARGINS];
		SSM(textView1, SCI_SETMARGINWIDTHN, 0, SCI_WIDTH_MARGINS_1);
		SSM(textView2, SCI_SETMARGINWIDTHN, 0, SCI_WIDTH_MARGINS_2);
	}

	if(SSM(textView1, SCI_GETMARGINWIDTHN, 2, 0) > 0)
		SSM(textView1, SCI_SETMARGINWIDTHN, 2, SCI_INDENT_MARGINS_1);

	if(SSM(textView2, SCI_GETMARGINWIDTHN, 2, 0) > 0)
		SSM(textView2, SCI_SETMARGINWIDTHN, 2, SCI_INDENT_MARGINS_2);

}

void wxTextEditor::refreshMargins()
{
	refreshMargins(1);
	refreshMargins(2);
}

void wxTextEditor::refreshMargins(gint view)
{
	//Line numbers margin
	if(this->getShowLineNumbers())
	{
		gint marginWidth = measureTextWidth(SCI_MEASURE_STRING, view);
		
		if(view == 2)
			SSM(textView2, SCI_SETMARGINWIDTHN, 0, marginWidth);
		else
			SSM(textView1, SCI_SETMARGINWIDTHN, 0, marginWidth);	
	}


	//Folding margin
	gint indentWidth = getIndentMarginWidth(view);
	if(view == 2)
	{
		if(SSM(textView2, SCI_GETMARGINWIDTHN, 2, 0) > 0)
			SSM(textView2, SCI_SETMARGINWIDTHN, 2, indentWidth);
	}
	else
	{
		if(SSM(textView1, SCI_GETMARGINWIDTHN, 2, 0) > 0)
			SSM(textView1, SCI_SETMARGINWIDTHN, 2, indentWidth);
	}

	
}


bool wxTextEditor::getShowSpaces() 
{
	//return (BOOL)[textView1 getGeneralProperty:SCI_GETVIEWWS parameter:0];
	return SSM(textView1, SCI_GETVIEWWS, 0, 0);
}

void wxTextEditor::setShowSpaces(bool val)
{
	if (val == true)
	{
		//[textView1 setGeneralProperty:SCI_SETVIEWWS parameter:SCWS_VISIBLEALWAYS value:0];
		//[textView2 setGeneralProperty:SCI_SETVIEWWS parameter:SCWS_VISIBLEALWAYS value:0];
		SSM(textView1, SCI_SETVIEWWS, SCWS_VISIBLEALWAYS, 0);
		SSM(textView2, SCI_SETVIEWWS, SCWS_VISIBLEALWAYS, 0);
	}
	else
	{
		//[textView1 setGeneralProperty:SCI_SETVIEWWS parameter:0 value:0];
		//[textView2 setGeneralProperty:SCI_SETVIEWWS parameter:0 value:0];
		SSM(textView1, SCI_SETVIEWWS, 0, 0);
		SSM(textView2, SCI_SETVIEWWS, 0, 0);
	}
}

bool wxTextEditor::getShowEOLMarker() //OK
{
	//return [textView1 getGeneralProperty:SCI_GETVIEWEOL parameter:0];
	return SSM(textView1, SCI_GETVIEWEOL, 0, 0);
}

void wxTextEditor::setShowEOLMarker(bool val) //OK
{
	//[textView1 setGeneralProperty:SCI_SETVIEWEOL parameter:(val ? 1 : 0) value:0];
	//[textView2 setGeneralProperty:SCI_SETVIEWEOL parameter:(val ? 1 : 0) value:0];
	SSM(textView1, SCI_SETVIEWEOL, (val ? 1 : 0), 0);
	SSM(textView2, SCI_SETVIEWEOL, (val ? 1 : 0), 0);
}

bool wxTextEditor::getEndAtLastLine() //OK
{
	//return [textView1 getGeneralProperty:SCI_GETENDATLASTLINE parameter:0];
	return SSM(textView1, SCI_GETENDATLASTLINE, 0, 0);
}

void wxTextEditor::setEndAtLastLine(bool val) //OK
{
	//[textView1 setGeneralProperty:SCI_SETENDATLASTLINE parameter:(val ? 1 : 0) value:0];
	//[textView2 setGeneralProperty:SCI_SETENDATLASTLINE parameter:(val ? 1 : 0) value:0];
	SSM(textView1, SCI_SETENDATLASTLINE, (val ? 1 : 0), 0);
	SSM(textView2, SCI_SETENDATLASTLINE, (val ? 1 : 0), 0);
}


bool wxTextEditor::getShowMatchingBracket()
{
	return _ShowMatchingBracket;
}



void wxTextEditor::setShowMatchingBracket(bool val)
{
	_ShowMatchingBracket = val;
	//[self BraceHiglight:-1 pos2:-1];
}


bool wxTextEditor::getCanUndo()	//OK
{
	//return [textView1 getGeneralProperty:SCI_CANUNDO parameter:0];
	return SSM(textView1, SCI_CANUNDO, 0, 0);
}

bool wxTextEditor::getCanRedo()	//OK
{
	//return [textView1 getGeneralProperty:SCI_CANREDO parameter:0];
	return SSM(textView1, SCI_CANREDO, 0, 0);
}


gint wxTextEditor::getTabIndent()	//OK
{
	//return [textView1 getGeneralProperty:SCI_GETTABWIDTH parameter:0];
	return SSM(textView1, SCI_GETTABWIDTH, 0, 0);
}

void wxTextEditor::setTabIndent(gint val)	//OK
{
	//[textView1 setGeneralProperty:SCI_SETTABWIDTH parameter:val value:0];
	//[textView2 setGeneralProperty:SCI_SETTABWIDTH parameter:val value:0];
	SSM(textView1, SCI_SETTABWIDTH, val, 0);
	SSM(textView2, SCI_SETTABWIDTH, val, 0);
}


bool wxTextEditor::getShowLineNumbers()	//OK
{
	//return ([textView1 getGeneralProperty:SCI_GETMARGINWIDTHN parameter:0] > 0);
	return (SSM(textView1, SCI_GETMARGINWIDTHN, 0, 0) > 0);
}

void wxTextEditor::setShowLineNumbers(bool val)	//OK
{
	////SCI_WIDTH_MARGINS = [textView1 measureTextWidth:SCI_MEASURE_STRING font:[self getTextEditorFont]];
	//MEASURE TEXT WIDTH AND SET MARGINS (FOR LINE NUMBERS)
	//NSFont* font = [self getTextEditorFont];
	//NSFont* tempFont = [NSFont fontWithName:[font familyName] size:[font pointSize] + [self getZoom]];
	//SCI_WIDTH_MARGINS = [self measureTextWidth:SCI_MEASURE_STRING font:tempFont];

	SCI_WIDTH_MARGINS_1 = measureTextWidth(SCI_MEASURE_STRING, 1);
	SCI_WIDTH_MARGINS_2 = measureTextWidth(SCI_MEASURE_STRING, 2);
	
	if(val == true)
	{
		//[textView1 setGeneralProperty:SCI_SETMARGINWIDTHN parameter:0 value:SCI_WIDTH_MARGINS];
		//[textView2 setGeneralProperty:SCI_SETMARGINWIDTHN parameter:0 value:SCI_WIDTH_MARGINS];
		SSM(textView1, SCI_SETMARGINWIDTHN, 0, SCI_WIDTH_MARGINS_1);
		SSM(textView2, SCI_SETMARGINWIDTHN, 0, SCI_WIDTH_MARGINS_2);
	}
	else 
	{
		//[textView1 setGeneralProperty:SCI_SETMARGINWIDTHN parameter:0 value:0];
		//[textView2 setGeneralProperty:SCI_SETMARGINWIDTHN parameter:0 value:0];
		SSM(textView1, SCI_SETMARGINWIDTHN, 0, 0);
		SSM(textView2, SCI_SETMARGINWIDTHN, 0, 0);
	}
}


bool wxTextEditor::getMarkCaretLine()	//OK
{
	//return [textView1 getGeneralProperty:SCI_GETCARETLINEVISIBLE parameter:0];
	return SSM(textView1, SCI_GETCARETLINEVISIBLE, 0, 0);
}

void wxTextEditor::setMarkCaretLine(bool val)	//OK
{
	//[textView1 setGeneralProperty:SCI_SETCARETLINEVISIBLE parameter:(val ? 1 : 0) value:0];
	//[textView2 setGeneralProperty:SCI_SETCARETLINEVISIBLE parameter:(val ? 1 : 0) value:0];
	SSM(textView1, SCI_SETCARETLINEVISIBLE, (val ? 1 : 0), 0);
	SSM(textView2, SCI_SETCARETLINEVISIBLE, (val ? 1 : 0), 0);
}


bool wxTextEditor::getShowVerticalRuler()	//OK
{
	//return [textView1 getGeneralProperty:SCI_GETEDGEMODE parameter:0];
	return SSM(textView1, SCI_GETEDGEMODE, 0, 0);
}

void wxTextEditor::setShowVerticalRuler(bool val)	//OK
{
	if (val == true)
	{
		//[textView1 setGeneralProperty:SCI_SETEDGECOLUMN parameter:80 value:0];
		//[textView2 setGeneralProperty:SCI_SETEDGECOLUMN parameter:80 value:0];
		//[textView1 setGeneralProperty:SCI_SETEDGEMODE parameter:1 value:0];
		//[textView2 setGeneralProperty:SCI_SETEDGEMODE parameter:1 value:0];
		SSM(textView1, SCI_SETEDGECOLUMN, 80, 0);
		SSM(textView2, SCI_SETEDGECOLUMN, 80, 0);
		SSM(textView1, SCI_SETEDGEMODE, 1, 0);
		SSM(textView2, SCI_SETEDGEMODE, 1, 0);
	}
	else
	{
		//[textView1 setGeneralProperty:SCI_SETEDGEMODE parameter:0 value:0];
		//[textView2 setGeneralProperty:SCI_SETEDGEMODE parameter:0 value:0];
		SSM(textView1, SCI_SETEDGEMODE, 0, 0);
		SSM(textView2, SCI_SETEDGEMODE, 0, 0);
	}
}


bool wxTextEditor::getReadOnly()	//OK
{
	//return [textView1 getGeneralProperty:SCI_GETREADONLY parameter:0];
	return SSM(textView1, SCI_GETREADONLY, 0, 0);
}

void wxTextEditor::setReadOnly(bool val)	//OK
{
	////[textView1 setEditable:val];
	////[textView2 setEditable:val];
	//[textView1 setGeneralProperty:SCI_SETREADONLY parameter:val ? 1 : 0 value:0];
	//[textView2 setGeneralProperty:SCI_SETREADONLY parameter:val ? 1 : 0 value:0];
	SSM(textView1, SCI_SETREADONLY, (val ? 1 : 0), 0);
	SSM(textView2, SCI_SETREADONLY, (val ? 1 : 0), 0);
}


ScintillaObject* wxTextEditor::getPrimaryView()
{
	return textView1;
}
ScintillaObject* wxTextEditor::getSecondaryView()
{
	return textView2;
}

Gtk::Widget*  wxTextEditor::get_textView1_Widget()
{
	return textView1_Widget;
}
Gtk::Widget*  wxTextEditor::get_textView2_Widget()
{
	return textView2_Widget;
}


void wxTextEditor::SetUndoCollection(bool value)
{
	SSM(textView1, SCI_SETUNDOCOLLECTION, (value ? 1 : 0), 0);
	SSM(textView2, SCI_SETUNDOCOLLECTION, (value ? 1 : 0), 0);
}



//This returns length-1 characters of text from the start of the document 
//plus one terminating 0 character. To collect all the text in a document, 
//use SCI_GETLENGTH  to get the number of characters in the document (nLen), 
//allocate a character buffer of length nLen+1 bytes, then call 
//SCI_GETTEXT(nLen+1, char *text). If the text argument is 0 then the length 
//that should be allocated to store the entire document is returned.
//gchar* wxTextEditor::getText()	
Glib::ustring wxTextEditor::getText()
{
	gint len = SSM(textView1, SCI_GETLENGTH, 0, 0);
	gchar* text = new gchar[len + 1];
	SSM(textView1, SCI_GETTEXT, len + 1, (sptr_t) text);

	Glib::ustring ret = text;
	delete[] text;
	
	return ret;
}

void wxTextEditor::setText(const gchar* text)	//OK
{
	if(text != NULL )
	{
		SSM(textView1, SCI_SETTEXT, 0, (sptr_t) text);
	}
}



//TODO: for color printing feature
//-(PrintDocument) getPrintDocument;




/*
	void gtk_widget_reparent (GtkWidget *widget, 
	                          GtkWidget *new_parent)
							  
	******

	g_object_ref(widget);
    gtk_container_remove(GTK_CONTAINER(old_parent), widget);
    gtk_container_add(GTK_CONTAINER(new_parent), widget);
    g_object_unref(widget);
*/	


//gtk_widget_hide(GTK_WIDGET(textView2));
bool wxTextEditor::getIsSplitted() //OK
{
	//return ([[self subviews] count] > 1);
	return gtk_widget_get_visible(GTK_WIDGET(textView2));
}


void wxTextEditor::Split()
{
	if(this->getIsSplitted())
		this->RemoveSplit();
	

	//Add reference to textView widgets before remove them
	g_object_ref(GTK_OBJECT(textView1));
	g_object_ref(GTK_OBJECT(textView2));

	remove(); //remove current View
	Gtk::VPaned* VPane = Gtk::manage(new Gtk::VPaned());
	add(*VPane);
	
	//VPane->add1(*textView1_Widget);
	//VPane->add2(*textView2_Widget);
	VPane->pack1(*textView1_Widget, TRUE, TRUE);
	VPane->pack2(*textView2_Widget, TRUE, TRUE);
	VPane->show();
	
	show_all_children(); 
	
	this->setFocusOnPrimaryView();

}

//add(*Glib::wrap(GTK_WIDGET(textView1)));
void wxTextEditor::RemoveSplit()	//OK
{	
	if(this->getIsSplitted() == false) return;

	g_object_ref(GTK_OBJECT(textView1));
	g_object_ref(GTK_OBJECT(textView2));
	
	gtk_widget_hide(GTK_WIDGET(textView2));

	//GtkWidget* parent = gtk_widget_get_parent(GTK_WIDGET(this));
	Gtk::VPaned* pane = (Gtk::VPaned*)gtk_widget_get_parent(GTK_WIDGET(textView1));
	gtk_container_remove(GTK_CONTAINER(pane), GTK_WIDGET(textView1));
	gtk_container_remove(GTK_CONTAINER(pane), GTK_WIDGET(textView2));

	remove(); //remove current View

	//add(*Glib::wrap(GTK_WIDGET(textView1)));
	add(*textView1_Widget);
	
	show_all_children(); 
	
	this->setFocusOnPrimaryView();
}


void wxTextEditor::SplitVertical()
{
	if(this->getIsSplitted())
		this->RemoveSplit();

	//Add reference to textView widgets before remove them
	g_object_ref(GTK_OBJECT(textView1));
	g_object_ref(GTK_OBJECT(textView2));

	remove(); //remove current View
	Gtk::HPaned* HPane = Gtk::manage(new Gtk::HPaned());
	add(*HPane);
	
	//HPane->add1(*Glib::wrap(GTK_WIDGET(textView1)));
	//HPane->add2(*Glib::wrap(GTK_WIDGET(textView2)));
	HPane->pack1(*textView1_Widget, TRUE, TRUE);
	HPane->pack2(*textView2_Widget, TRUE, TRUE);
	HPane->show();

	show_all_children(); 
	
	this->setFocusOnPrimaryView();
}		




gboolean wxTextEditor::LoadFile(const gchar* filename)
{

	//TODO: UTF8 !!!???
	Glib::ustring text;

	try
	{
		text = Glib::file_get_contents(filename);
	}
	catch(...)//TODO: FILE ERROR !!!
	{
		std::cout << "wxTextEditor::LoadFile Error" << std::endl;
		return false;
	}
	
	this->setText(text.c_str());

	text = "";

	return true;
}

gboolean wxTextEditor::SaveFile(const gchar* filename)
{
	//TODO: UTF8 ???

	Glib::file_set_contents(filename, this->getText());

	/*
	try
	{
		Glib::file_set_contents(filename, this->getText());
	}
	catch(...) //TODO: FILE ERROR !!!
	{
		std::cout << "wxTextEditor::SaveFile Error" << std::endl;
		return false;
	}
	*/

	return true;
}



void wxTextEditor::setHighlight(const gchar* languageName)		//OK
{
	//[textView1 setStringProperty:SCI_SETLEXERLANGUAGE parameter:0 value:languageName];	
	//[textView2 setStringProperty:SCI_SETLEXERLANGUAGE parameter:0 value:languageName];	
	SSM(textView1, SCI_SETLEXERLANGUAGE, 0, (sptr_t)languageName);
	SSM(textView2, SCI_SETLEXERLANGUAGE, 0, (sptr_t)languageName);

	/*
	//SSM(textView1, SCI_SETLEXER, SCLEX_WINXOUND, 0);
   	SSM(textView1, SCI_STYLESETFORE, SCE_C_COMMENT, getColorFromString("#008000")); //0xFF80FF); //0x008000
   	SSM(textView1, SCI_STYLESETFORE, SCE_C_COMMENTLINE, getColorFromString("#008000")); //getIntColorFromHex(
   	SSM(textView1, SCI_STYLESETFORE, SCE_C_NUMBER, 0x808000);
   	SSM(textView1, SCI_STYLESETFORE, SCE_C_WORD, 0x800000);
   	SSM(textView1, SCI_STYLESETFORE, SCE_C_STRING, 0x800080);
   	SSM(textView1, SCI_STYLESETBOLD, SCE_C_OPERATOR, 1);
	*/
	
	
	//std::cout << SSM(textView1, SCI_GETLEXER, 0, 0);
}


void wxTextEditor::setKeyWords(gint keyWordSet, const gchar* keyWordList)
{
	//const char* keyWordSetCHAR = [keyWordList UTF8String];
	//[textView1 setReferenceProperty: SCI_SETKEYWORDS parameter: keyWordSet value: keyWordSetCHAR];
	//[textView2 setReferenceProperty: SCI_SETKEYWORDS parameter: keyWordSet value: keyWordSetCHAR];
	SSM(textView1, SCI_SETKEYWORDS, keyWordSet, (sptr_t)keyWordList);
	SSM(textView2, SCI_SETKEYWORDS, keyWordSet, (sptr_t)keyWordList);
}


void wxTextEditor::setWordChars(const gchar* wordChars)		//OK
{
	//[textView1 setStringProperty:SCI_SETWORDCHARS parameter:0 value:wordChars];
	//[textView2 setStringProperty:SCI_SETWORDCHARS parameter:0 value:wordChars];
	SSM(textView1, SCI_SETWORDCHARS, 0, (sptr_t)wordChars);
	SSM(textView2, SCI_SETWORDCHARS, 0, (sptr_t)wordChars);
}


void wxTextEditor::setCodePage(gint codepage)			//OK
{
	//[textView1 setGeneralProperty:SCI_SETCODEPAGE parameter:codepage value:0];
	//[textView2 setGeneralProperty:SCI_SETCODEPAGE parameter:codepage value:0];
	SSM(textView1, SCI_SETCODEPAGE, codepage, 0);
	SSM(textView2, SCI_SETCODEPAGE, codepage, 0);
}


//const gchar* wxTextEditor::getTextOfLine(gint lineNumber)		//OK 
Glib::ustring wxTextEditor::getTextOfLine(gint lineNumber)	
{
	//NSString* result = nil;
	//NSInteger start = [self getPositionFromLineNumber:lineNumber];
	//NSInteger end = [self getLineEndPosition:lineNumber];
	//NSRange range = NSMakeRange(start, end - start);
	////result = [[self getText] substringWithRange:range];
	//result = [self getTextRange:range];
	//return result;

	
	////If you want the length of the line not including any end of line characters, 
	////use SCI_GETLINEENDPOSITION(line) - SCI_POSITIONFROMLINE(line).
	////gint len = this->getLineLength(); //This returns the length of the line, including any line end characters
	//gint len = this->getLineEndPosition(lineNumber) - this->getPositionFromLineNumber(lineNumber);

	
	gint len = SSM(textView1, SCI_GETLINE, lineNumber, 0);
	
	if(len > 0)
	{
		gchar* linebuf = new gchar[len+1];
		SSM(textView1, SCI_GETLINE, lineNumber, (sptr_t) linebuf);

		Glib::ustring str = linebuf;
		delete[] linebuf;
		
		gint found = str.find("\n");
		//if (found != std::string::npos)
		if (found > -1)
			str.erase(found, str.length() - found);

		found = str.find("\r");
		if (found > -1)
			str.erase(found, str.length() - found);

		return str.c_str();

	}

	return "";
}

Glib::ustring wxTextEditor::getTextOfLineWithEol(gint lineNumber)		
{
	gint len = SSM(textView1, SCI_GETLINE, lineNumber, 0);

	if(len > 0)
	{
		gchar* linebuf = new gchar[len+1];
		SSM(textView1, SCI_GETLINE, lineNumber, (sptr_t) linebuf);

		Glib::ustring str = linebuf;
		delete[] linebuf;

		return str;
		
		//ret = new gchar[len+1];
		//SSM(textView1, SCI_GETLINE, lineNumber, (sptr_t) ret);
	}

	return "";

}





//gchar* wxTextEditor::getTextRange(gint start, gint end)
Glib::ustring wxTextEditor::getTextRange(gint start, gint end)
{
	gint len = (end - start) + 1; //Zero terminated!!!
	gchar* text = new gchar[len + 1];
	
	struct Sci_TextRange tr;
	tr.chrg.cpMin = start;
	tr.chrg.cpMax = end;
	tr.lpstrText = text;
	SSM(textView1, SCI_GETTEXTRANGE, 0, (long) &tr);

	Glib::ustring ret = text; //tr.lpstrText
	delete[] text;
	
	return ret;
}


gint wxTextEditor::getCurrentLineNumber()		//OK
{
	//[[self getFocusedEditor] getGeneralProperty:SCI_GETCURRENTPOS parameter:0];
	gint curPosition = SSM(this->getFocusedEditor(), SCI_GETCURRENTPOS, 0, 0);
	//return [[self getFocusedEditor] getGeneralProperty:SCI_LINEFROMPOSITION parameter:curPosition]; 
	return SSM(this->getFocusedEditor(), SCI_LINEFROMPOSITION, curPosition, 0);
}


gint wxTextEditor::getLineLength(gint lineNumber)
{
	//return [textView1 getGeneralProperty:SCI_LINELENGTH parameter:lineNumber];
	return SSM(textView1, SCI_LINELENGTH, lineNumber, 0);
}


void  wxTextEditor::ClearAllText()
{
	SSM(textView1, SCI_CLEARALL, 0, 0);
}


gint wxTextEditor::getCaretPosition()
{
	//return [[self getFocusedEditor] getGeneralProperty:SCI_GETCURRENTPOS parameter:0];
	return SSM(this->getFocusedEditor(), SCI_GETCURRENTPOS, 0, 0);
}

void wxTextEditor::setCaretPosition(gint position)
{
	//[[self getFocusedEditor] setGeneralProperty:SCI_GOTOPOS parameter:position value:0];
	SSM(this->getFocusedEditor(), SCI_GOTOPOS, position, 0);
}

/*
- (void) updateCaret
{
	[[self getFocusedEditor] backend]->UpdateCaret();
}
*/

void wxTextEditor::GoToLine(gint lineNumber)	//OK
{
	SSM(this->getFocusedEditor(), SCI_ENSUREVISIBLE, lineNumber, 0);
	SSM(this->getFocusedEditor(), SCI_GOTOLINE, lineNumber, 0);
}


//- (void) InsertText: (NSInteger) position 
//			   text: (NSString*) text
void wxTextEditor::InsertText(gint position, const gchar* text)
{
	//CONVERT EOLS in order to match default Eol
	//text = [textView1 backend]->ConvertEOL(text);
	//[textView1 setStringProperty:SCI_INSERTTEXT parameter:position value:text];
	SSM(textView1, SCI_INSERTTEXT, position, (sptr_t)text);
}



//Add text at current caret position
void wxTextEditor::AddText(const gchar* text)
{
	//NSInteger length = [text length];
	//[textView1 setStringProperty:SCI_ADDTEXT parameter:length value:text];
	if (text != NULL)
	{
		SSM(textView1, SCI_ADDTEXT, strlen(text), (sptr_t)text);
	}
}


//Add text at the end of document
void wxTextEditor::AppendText(const gchar* text)
{
	//NSInteger length = [text length];
	//[textView1 setStringProperty:SCI_APPENDTEXT parameter:length value:text];
	if (text != NULL)
	{
		SSM(textView1, SCI_APPENDTEXT, strlen(text), (sptr_t)text);
	}
}



Glib::ustring wxTextEditor::getCharAt(gint position)	//OK
{
	try 
	{
		//unichar KeyDec = [textView1 getGeneralProperty:SCI_GETCHARAT parameter:position];
		//KeyDec > 32 ??
		//if (KeyDec > 0 & KeyDec < 128)
		//{
		//	return [NSString stringWithCharacters:&KeyDec length:1];
		//}

		gunichar c = SSM(textView1, SCI_GETCHARAT, position, 0);
		Glib::ustring temp;
		temp = c;
		
		return temp; //(gchar*) SSM(textView1, SCI_GETCHARAT, position, 0);
	}
	catch(...) 
	{
		std::cout << "wxTextEditor::getCharAt Error" << std::endl;
		//NSLog(@"TextEditorView -> getCharAt Error: %@ - %@", [e name], [e reason]);
	}

	return "";

	//return (gchar) SSM(textView1, SCI_GETCHARAT, position, 0);
}



gint wxTextEditor::getTextLength()
{
	//return [textView1 getGeneralProperty:SCI_GETTEXTLENGTH parameter:0];
	//if(textView1 == NULL) return 0;	
	return SSM(textView1, SCI_GETTEXTLENGTH, 0, 0);
}


gint wxTextEditor::getLinesCount()
{
	//return [textView1 getGeneralProperty:SCI_GETLINECOUNT parameter:0];
	return SSM(textView1, SCI_GETLINECOUNT, 0, 0);
}


void wxTextEditor::setFirstVisibleLine(gint lineNumber)
{
	/*
	if([[textView2 window] firstResponder] == [textView2 content])
	{
		[self setFirstVisibleLineAtView:lineNumber view:2];
	}
	else [self setFirstVisibleLineAtView:lineNumber view:1];
	*/

	if(SSM(textView2, SCI_GETFOCUS, 0, 0) > 0)
		this->setFirstVisibleLineAtView(lineNumber, 2);
	else
		this->setFirstVisibleLineAtView(lineNumber, 1);
}

void wxTextEditor::setFirstVisibleLineAtView(gint lineNumber, gint view)
{		
	gint firstVisibleLine = 0;
	if (view == 2)
	{
		//firstVisibleLine = SSM(textView2, SCI_GETFIRSTVISIBLELINE, 0, 0);
		//SSM(textView2, SCI_GOTOLINE, lineNumber, 0);
		//SSM(textView2, SCI_LINESCROLL, 0, (lineNumber - firstVisibleLine));
		//this->setFocusOnSecondaryView();
		SSM(textView2, SCI_SETFIRSTVISIBLELINE, lineNumber, 0);
		SSM(textView2, SCI_GOTOLINE, lineNumber, 0);
		SSM(textView2, SCI_SETFOCUS, lineNumber, 0);
	}
	else
	{
		//firstVisibleLine = SSM(textView1, SCI_GETFIRSTVISIBLELINE, 0, 0);
		//SSM(textView1, SCI_GOTOLINE, lineNumber, 0);
		//SSM(textView1, SCI_LINESCROLL, 0, (lineNumber - firstVisibleLine));
		//this->setFocusOnPrimaryView();
		SSM(textView1, SCI_SETFIRSTVISIBLELINE, lineNumber, 0);
		SSM(textView1, SCI_GOTOLINE, lineNumber, 0);
		SSM(textView1, SCI_SETFOCUS, lineNumber, 0);
	}
}



gint wxTextEditor::getFirstVisibleLine()
{
	//return [[self getFocusedEditor] getGeneralProperty:SCI_GETFIRSTVISIBLELINE parameter:0];
	return SSM(this->getFocusedEditor(), SCI_GETFIRSTVISIBLELINE, 0, 0);
}

gint wxTextEditor::getFirstVisibleLineAtView(gint view)
{
	if (view == 2)
	{
		//return [textView2 getGeneralProperty:SCI_GETFIRSTVISIBLELINE parameter:0];
		return SSM(textView2, SCI_GETFIRSTVISIBLELINE, 0, 0);
	}
	else
	{
		return SSM(textView1, SCI_GETFIRSTVISIBLELINE, 0, 0);
	}
}


gint wxTextEditor::getLinesOnScreen()
{
	//return [[self getFocusedEditor] getGeneralProperty:SCI_LINESONSCREEN parameter:0];
	return SSM(textView2, SCI_LINESONSCREEN, 0, 0);
}


bool wxTextEditor::getCanPaste()
{
	//return [[self getFocusedEditor] backend]->CanPaste();
	return SSM(this->getFocusedEditor(), SCI_CANPASTE, 0, 0);
}



void wxTextEditor::PerformUndo()
{
	//[[self getFocusedEditor] setGeneralProperty:SCI_UNDO parameter:0 value:0];
	SSM(this->getFocusedEditor(), SCI_UNDO, 0, 0);
}


void wxTextEditor::PerformRedo()
{
	//[[self getFocusedEditor] setGeneralProperty:SCI_REDO parameter:0 value:0];
	SSM(this->getFocusedEditor(), SCI_REDO, 0, 0);
}


void wxTextEditor::PerformCopy()
{
	//[[self getFocusedEditor] backend]->Copy();
	SSM(this->getFocusedEditor(), SCI_COPY, 0, 0);
}

void wxTextEditor::PerformCut()
{
	//[[self getFocusedEditor] setGeneralProperty:SCI_CUT parameter:0 value:0];
	//[[self getFocusedEditor] backend]->Cut();
	SSM(this->getFocusedEditor(), SCI_CUT, 0, 0);
}


void wxTextEditor::PerformPaste()
{
	//[[self getFocusedEditor] backend]->Paste();
	SSM(this->getFocusedEditor(), SCI_PASTE, 0, 0);
}


void wxTextEditor::PerformDelete()
{
	//[[self getFocusedEditor] setGeneralProperty:SCI_CLEAR parameter:0 value:0];
	SSM(this->getFocusedEditor(), SCI_CLEAR, 0, 0);
}


void wxTextEditor::PerformSelectAll()
{
	//[[self getFocusedEditor] setGeneralProperty:SCI_SELECTALL parameter:0 value:0];
	SSM(this->getFocusedEditor(), SCI_SELECTALL, 0, 0);
}




bool wxTextEditor::isTextChanged()
{
	//return ([textView1 getGeneralProperty:SCI_GETMODIFY parameter:0] != 0);
	return SSM(textView1, SCI_GETMODIFY, 0, 0);
}


void wxTextEditor::setSavePoint()
{
	//[textView1 setGeneralProperty:SCI_SETSAVEPOINT parameter:0 value:0];
	SSM(textView1, SCI_SETSAVEPOINT, 0, 0);
}


void wxTextEditor::emptyUndoBuffer()
{
	//[textView1 setGeneralProperty:SCI_EMPTYUNDOBUFFER parameter:0 value:0];
	SSM(textView1, SCI_EMPTYUNDOBUFFER, 0, 0);
}


gint wxTextEditor::getZoom()
{
	//return [[self getFocusedEditor] getGeneralProperty:SCI_GETZOOM parameter:0];
	return SSM(this->getFocusedEditor(), SCI_GETZOOM, 0, 0);
}

gint wxTextEditor::getZoomForView1()
{
	//return [textView1 getGeneralProperty:SCI_GETZOOM parameter:0];
	return SSM(textView1, SCI_GETZOOM, 0, 0);
}

gint wxTextEditor::getZoomForView2()
{
	//return [textView2 getGeneralProperty:SCI_GETZOOM parameter:0];
	return SSM(textView2, SCI_GETZOOM, 0, 0);
}

void wxTextEditor::setZoom(gint zoom)
{
	//[[self getFocusedEditor] setGeneralProperty:SCI_SETZOOM parameter:zoom value:0];
	SSM(this->getFocusedEditor(), SCI_SETZOOM, zoom, 0);
}
void wxTextEditor::setZoomForView1(gint zoom)
{
	//[textView1 setGeneralProperty:SCI_SETZOOM parameter:zoom value:0];
	SSM(textView1, SCI_SETZOOM, zoom, 0);
}
void wxTextEditor::setZoomForView2(gint zoom)
{
	//[textView2 setGeneralProperty:SCI_SETZOOM parameter:zoom value:0];
	SSM(textView2, SCI_SETZOOM, zoom, 0);
}




/*
- (NSInteger) FindText: (NSString*) text 
		MatchWholeWord: (BOOL) MatchWholeWord 
			 MatchCase: (BOOL) MatchCase 
			IsBackward: (BOOL) IsBackward 
			SelectText: (BOOL) SelectText 
		   ShowMessage: (BOOL) ShowMessage 
			   SkipRem: (BOOL) SkipRem
*/
gint wxTextEditor::FindText(const gchar* text, 
                            bool MatchWholeWord,
                            bool MatchCase,
                            bool IsBackward,
                            bool SelectText,
                            bool ShowMessage,
                            bool SkipRem)  
{
	return FindText(text,
	                MatchWholeWord,
	                MatchCase,
	                IsBackward,
	                SelectText,
	                ShowMessage,
	                SkipRem,
	                -1, 
	                -1,
	                false);
}


/*
- (NSInteger) FindText: (NSString*) text 
		MatchWholeWord: (BOOL) MatchWholeWord 
			 MatchCase: (BOOL) MatchCase 
			IsBackward: (BOOL) IsBackward 
			SelectText: (BOOL) SelectText 
		   ShowMessage: (BOOL) ShowMessage 
			   SkipRem: (BOOL) SkipRem
				 start: (NSInteger) Start
				   end: (NSInteger) End
*/
gint wxTextEditor::FindText(const gchar* text, 
                            bool MatchWholeWord,
                            bool MatchCase,
                            bool IsBackward,
                            bool SelectText,
                            bool ShowMessage,
                            bool SkipRem,
                            gint Start,
                            gint End)
{
	return FindText(text,
	                MatchWholeWord,
	                MatchCase,
	                IsBackward,
	                SelectText,
	                ShowMessage,
	                SkipRem,
	                Start, 
	                End,
	                false);
}


/*
- (NSInteger) FindTextEx: (NSString*) text 
		  MatchWholeWord: (BOOL) MatchWholeWord 
			   MatchCase: (BOOL) MatchCase 
			  IsBackward: (BOOL) IsBackward 
			  SelectText: (BOOL) SelectText 
		     ShowMessage: (BOOL) ShowMessage 
			     SkipRem: (BOOL) SkipRem
				   start: (NSInteger) start
					 end: (NSInteger) end
				useRegEx: (BOOL) useRegEx;
*/
gint wxTextEditor::FindText(const gchar* text, 
                            bool MatchWholeWord,
                            bool MatchCase,
                            bool IsBackward,
                            bool SelectText,
                            bool ShowMessage,
                            bool SkipRem,
                            gint start,
                            gint end,
                            bool useRegEx)          
{
	if (getTextLength() > 0)
	{
		//NSString* StringToFind = text;
		Glib::ustring StringToFind = text;
		
		gint mStart = getCaretPosition(); //GetFocusedEditor.GetCurrentPos();
		gint mEnd = getTextLength(); //GetFocusedEditor.GetTextLength();
		gint mSearchFlags = 0;
		gint mFindPos = -1;
		
		
		if (start > -1) mStart = start;
		if (end > -1) mEnd = end;
		
		if (MatchWholeWord)
		{
			mSearchFlags |= SCFIND_WHOLEWORD;
		}
		
		if (MatchCase)
		{
			mSearchFlags |= SCFIND_MATCHCASE;
		}
		
		if(useRegEx)	//ADD REGULAR EXPRESSION SEARCH
		{
			mSearchFlags |= SCFIND_REGEXP;
		}
		
		if (IsBackward)
		{
			//Search backward
			mStart = getCaretPosition() - 1; //GetFocusedEditor.GetCurrentPos() - 1;
			mEnd = 0;
		}
			
		
		//Search routine
		try
		{
			//[self setSearchFlags:mSearchFlags];
			setSearchFlags(mSearchFlags);
			
			do
			{
				//GetFocusedEditor.SetTargetStart(mStart);
				//GetFocusedEditor.SetTargetEnd(mEnd);
				setTargetStart(mStart);
				setTargetEnd(mEnd);
				

				//mFindPos = [[self getFocusedEditor] searchInTarget:StringToFind]; //GetFocusedEditor.SearchInTarget(StringToFind);
				//mFindPos = [self searchInTarget:StringToFind];
				mFindPos = searchInTarget(StringToFind.c_str());
				if (mFindPos > -1)
				{
					mStart = mFindPos + StringToFind.length();//1;
					//if (!(this.IsRemAt(mFindPos) && SkipRem == true))
					//if (![self IsRemAt:mFindPos] && SkipRem == true)
					{
						if (SelectText == true)
						{
							gint lineNumber = getLineNumberFromPosition(mFindPos);
							SSM(getFocusedEditor(), SCI_ENSUREVISIBLE, lineNumber, 0);
							//[self setSelectionStart:mFindPos];
							setSelectionStart(mFindPos);
							//[self setSelectionEnd:mFindPos + [StringToFind length]];
							setSelectionEnd(mFindPos + StringToFind.length());
							//[self ScrollCaret];
							ScrollCaret();
							//[self setFocus];
							setFocus();
							return mFindPos;
						}
						else if (SelectText == false)
						{
							return mFindPos;
						}
					}
				}
				else
				{
					if (ShowMessage == true)
					{
						/*NSRunAlertPanel(@"WinXound Find and Replace", 
						                 @"Text not Found", 
						                 @"OK", 
						                 nil, nil);
										 */
						Gtk::MessageDialog dialog("WinXound Find and Replace",
						                          false /* use_markup */, 
						                          Gtk::MESSAGE_INFO,
						                          Gtk::BUTTONS_OK);
						dialog.set_secondary_text("Text not Found!");
					}
					return -1;
				}
			}
			while (true);
			
		}
		
		catch (...)
		{
			return -1;
		}
		
	}
	return -1;
}



/*
- (NSMutableArray*) SearchText: (NSString*) text 
				MatchWholeWord: (BOOL) MatchWholeWord 
					 MatchCase: (BOOL) MatchCase 
					IsBackward: (BOOL) IsBackward 
					   SkipRem: (BOOL) SkipRem
{
	return [self SearchTextEx:text 
			 MatchWholeWord:MatchWholeWord
				  MatchCase:MatchCase
				 IsBackward:IsBackward
					SkipRem:SkipRem	
					  start:-1];
}


- (NSMutableArray*) SearchTextEx: (NSString*) text 
				  MatchWholeWord: (BOOL) MatchWholeWord 
					   MatchCase: (BOOL) MatchCase 
					  IsBackward: (BOOL) IsBackward 
						 SkipRem: (BOOL) SkipRem
						   start: (NSInteger) start
{
	if ([self getTextLength] > 0)
	{
		
		//ArrayList mMatches = new ArrayList();
		NSMutableArray* mMatches = [[NSMutableArray alloc] init];
		
		NSString* StringToFind = text;
		NSInteger mStart = 0;
		if (start > 0) mStart = start;
		NSInteger mFindPos = -1;
		NSInteger mTextLength = [self getTextLength];
		
		do
		{
//			mFindPos = this.FindText(StringToFind, MatchWholeWord, MatchCase,
//									 IsBackward, false, false, SkipRem,
//									 mStart, this.GetTextLength());
			[self FindTextEx:StringToFind 
				MatchWholeWord:MatchWholeWord 
				   MatchCase:MatchCase 
				  IsBackward:IsBackward 
				  SelectText:false 
				 ShowMessage:false 
					 SkipRem:SkipRem	
					   start:mStart
						 end:mTextLength
					useRegEx:false];
			
			if (mFindPos > -1)
			{
				mStart = mFindPos + 1;
				//mMatches.Add(mFindPos);
				//[NSNumber numberWithInt: myInt]
				[mMatches addObject: [NSNumber numberWithInt:mFindPos]];
			}
			else
			{
				break;
			}
		}
		while (true);
		
		return mMatches;
		
	}
	
	else return nil;
}
*/


//SCI_REPLACETARGET(int length, const char *text)
//If length is -1, text is a zero terminated string, 
//otherwise length sets the number of character to replace the target with
void wxTextEditor::ReplaceTarget(gint offset, gint length, const gchar* replaceString)
{
	//[self setTargetStart:offset]; //GetFocusedEditor.SetTargetStart(offset);
	//[self setTargetEnd:(offset + length)]; //GetFocusedEditor.SetTargetEnd(offset + length);
	//[[self getFocusedEditor] setStringProperty:SCI_REPLACETARGET parameter:-1 value:ReplaceString];

	this->setTargetStart(offset);
	this->setTargetEnd(offset + length);
	SSM(this->getFocusedEditor(), SCI_REPLACETARGET, -1, (sptr_t)replaceString);
}


void wxTextEditor::ReplaceText(const gchar* replaceString)
{
	/*
	if ([[self getSelectedText] length] > 0 &&
		[self getSelectedText] != nil)
	{
		[self setSelectedText:replaceString];
		//[self setFocus];
	}
	*/

	if(this->getSelectedText().size() > 0)
	{
		this->setSelectedText(replaceString);
	}
}


gint wxTextEditor::ReplaceAllText(const gchar* StringToFind,
                                  const gchar* ReplaceString,
                                  bool MatchWholeWord,
                                  bool MatchCase,
                                  bool FromCaretPosition, 
                                  bool FCPUp)
{
	//if ([[self getSelectedText] length] < 1) return;

	gint mStart = 0;
	gint mEnd = this->getTextLength();
	gint mFindPos = -1;
	gint mSearchFlags = 0;
	gint mTotalOcc = 0;
	
	if (FromCaretPosition)
	{
		mStart = this->getCaretPosition();
		if (FCPUp)
		{
			mStart = this->getCaretPosition() - 1; //GetFocusedEditor.GetCurrentPos() - 1;
			mEnd = 0;
		}
	}
		
	if (MatchWholeWord)
	{
		mSearchFlags |= SCFIND_WHOLEWORD;
	}
	if (MatchCase)
	{
		mSearchFlags |= SCFIND_MATCHCASE;
	}
	//GetFocusedEditor.SetSearchFlags(mSearchFlags);
	//[self setSearchFlags:mSearchFlags];	
	this->setSearchFlags(mSearchFlags);

	do
	{
		//GetFocusedEditor.SetTargetStart(mStart);
		setTargetStart(mStart);
		//GetFocusedEditor.SetTargetEnd(mEnd);
		setTargetEnd(mEnd);
		
		
		//mFindPos = GetFocusedEditor.SearchInTarget(StringToFind);
		mFindPos = searchInTarget(StringToFind);
		if (mFindPos > -1)
		{
			mStart = mFindPos + strlen(ReplaceString); //ReplaceString.Length;
			//GetFocusedEditor.SetSelectionStart(mFindPos);
			setSelectionStart(mFindPos);
			//GetFocusedEditor.SetSelectionEnd(mFindPos + StringToFind.Length);
			setSelectionEnd(mFindPos + strlen(StringToFind));
			//GetFocusedEditor.ReplaceSel(ReplaceString);
			setSelectedText(ReplaceString);
			mTotalOcc++;
		}
		else
		{
			if (mStart == 0)
			{
				return 0;
			}
			else
			{
				return mTotalOcc;
			}
			
			break;
		}
	}
	while (true);
	
	
	return 0;

}


gint wxTextEditor::getLineNumberFromPosition(gint position)
{
	//NSLog(@"getLineNumberFromPosition: %d",[[self getFocusedEditor] getGeneralProperty:SCI_LINEFROMPOSITION parameter:position]);
	//return [textView1 getGeneralProperty:SCI_LINEFROMPOSITION parameter:position];
	return SSM(textView1, SCI_LINEFROMPOSITION, position, 0);
}




gint wxTextEditor::getPositionFromLineNumber(gint linenumber)
{
	//return [textView1 getGeneralProperty:SCI_POSITIONFROMLINE	parameter:linenumber];
	return SSM(textView1, SCI_POSITIONFROMLINE, linenumber, 0);
}



Glib::ustring wxTextEditor::getCurrentWord()
{
	//return [self getWordAt:[self getCaretPosition]];
	//tEditor.textEditor.GetWordAt(tEditor.textEditor.GetCaretPosition());
	return getWordAt(getCaretPosition());
}

Glib::ustring wxTextEditor::getCurrentWordForAutoc()
{
	gint position = getCaretPosition();
	Glib::ustring WordAt = "";
	Glib::ustring tempWord = "";
	
	Glib::ustring c = "";
	gint StartPos = 0;
	gint EndPos = 0;
	
	try
	{
		for (StartPos = position; StartPos > 0; StartPos--)
		{
			c = getCharAt(StartPos - 1);
			if (isValidCharForAutoc(c.c_str()))
			{
				tempWord.append(c);
			}
			else break;
		}

		WordAt.append(g_strreverse((gchar*)tempWord.c_str()));

	}
	catch (...)
	{
		std::cout << "wxTextEditor::getCurrentWordForAutoc Error" << std::endl;
	}

	return WordAt;

		
}


gint wxTextEditor::getWordStart(gint position)
{
	//return [textView1 backend]->WndProc(SCI_WORDSTARTPOSITION, position, true);	
	return SSM(textView1, SCI_WORDSTARTPOSITION, position, true);
}

gint wxTextEditor::getWordEnd(gint position)
{
	//return [textView1 backend]->WndProc(SCI_WORDENDPOSITION, position, true);	
	return SSM(textView1, SCI_WORDENDPOSITION, position, true);
}



Gdk::Point wxTextEditor::getQuotesPosition(gint position)
{

	gint mStart = 0;
	gint mEnd = 0;
	gint mCaretPosition = position; 
	gint mCurrentLineNumber = getLineNumberFromPosition(position);

	for (gint c = mCaretPosition;
	     c >= getPositionFromLineNumber(mCurrentLineNumber);
	     c--)
	{
		if (getCharAt(c) == "\"" ||
		    getCharAt(c) == "'")
		{
			mStart = c + 1;
			break;
		}
	}

	for (gint c = mCaretPosition;
	     c <= getLineEndPosition(mCurrentLineNumber);
	     c++)
	{
		if (getCharAt(c) == "\"" ||
		    getCharAt(c) == "'")
		{
			mEnd = c;
			break;
		}
	}

	return Gdk::Point(mStart, mEnd);
}

Glib::ustring wxTextEditor::getTextInQuotes(gint position)
{
	Glib::ustring mString = "";

	Gdk::Point p = getQuotesPosition(position);

	//if ((mEnd - mStart) > 0 && mStart >= 0)
	if ((p.get_y() - p.get_x()) > 0 && p.get_x() >= 0)
	{
		//mString = this.GetText().Substring(mStart, mEnd - mStart);
		//mString = this.GetText().Substring(p.X, (p.Y - p.X));
		mString = getTextRange(p.get_x(), p.get_y()); //start, end
		return mString;
	}

	return "";
}




Glib::ustring wxTextEditor::getWordAt(gint position)
{
	Glib::ustring WordAt = "";
	Glib::ustring tempWord = "";
	
	Glib::ustring c = "";
	gint StartPos = 0;
	gint EndPos = 0;
	
	try
	{
		for (StartPos = position; StartPos > 0; StartPos--)
		{
			c = getCharAt(StartPos - 1);
			if (isValidChar(c.c_str()))
			{
				//[tempWord appendString:c];
				tempWord.append(c);
			}
			else break;
		}

		WordAt.append(g_strreverse((gchar*)tempWord.c_str()));
		
		//for (EndPos = GetFocusedEditor.GetCurrentPos();
		//     EndPos < GetFocusedEditor.GetTextLength(); EndPos++)
		for (EndPos = position;
			 EndPos < getTextLength(); EndPos++)
		{
			// Find the end of the word. 
			c = getCharAt(EndPos); //GetFocusedEditor.GetCharAt(EndPos);
			if (isValidChar(c.c_str()))
			{
				//[WordAt appendString:c];
				WordAt.append(c);
			}
			else break;
		}
		
	}
	catch (...)
	{
		//NSLog(@"TextEditorView -> getWordAt Error: %@ - %@", [e name], [e reason]);
		std::cout << "wxTextEditor::getWordAt Error" << std::endl;
	}
	
	return WordAt;
}



bool wxTextEditor::isValidChar(const gchar* c)
{
	//	return (char.IsLetterOrDigit(c) || char.IsNumber(c) || c == '_' ||
	//			c == '$');
	/*
	NSCharacterSet* cSet = [NSCharacterSet alphanumericCharacterSet];
	return ([c rangeOfCharacterFromSet:cSet].length > 0 ||
			[c isEqualToString: @"_"] || 
			[c isEqualToString: @"$"]);
	*/

	return (std::isalnum(*c) ||
	        g_str_equal(c, "_") ||
	        g_str_equal(c, "$"));
}

bool wxTextEditor::isValidCharForAutoc(const gchar* c)
{
	return (std::isalnum(*c) ||
	        g_str_equal(c, "_") ||
	        g_str_equal(c, "$") ||
	        g_str_equal(c, "<") ||
	        g_str_equal(c, ">") ||
	        g_str_equal(c, "/"));
}



void wxTextEditor::setSelection(gint start, gint end)
{
	//[self setSelectionStart:start];
	//[self setSelectionEnd:end];
	this->setSelectionStart(start);
	this->setSelectionEnd(end);
}

void wxTextEditor::setSelectionStart(gint start)
{
	//[[self getFocusedEditor] setGeneralProperty:SCI_SETSELECTIONSTART parameter:start value:0];
	SSM(this->getFocusedEditor(), SCI_SETSELECTIONSTART, start, 0);
}

void wxTextEditor::setSelectionEnd(gint end)
{
	//[[self getFocusedEditor] setGeneralProperty:SCI_SETSELECTIONEND parameter:end value:0];
	SSM(this->getFocusedEditor(), SCI_SETSELECTIONEND, end, 0);
}

gint wxTextEditor::getSelectionStart()
{
	//return [[self getFocusedEditor] getGeneralProperty:SCI_GETSELECTIONSTART parameter:0];
	return SSM(this->getFocusedEditor(), SCI_GETSELECTIONSTART, 0, 0);
}

gint wxTextEditor::getSelectionEnd()
{
	//return [[self getFocusedEditor] getGeneralProperty:SCI_GETSELECTIONEND parameter:0];
	return SSM(this->getFocusedEditor(), SCI_GETSELECTIONEND, 0, 0);
}


//SCI_GETSELTEXT(<unused>, char *text)
//This copies the currently selected text and a terminating 0 byte to the text buffer. 
//The buffer size should be determined by calling with a NULL pointer for the text 
//argument SCI_GETSELTEXT(0,0). This allows for rectangular and discontiguous 
//selections as well as simple selections
//gchar* wxTextEditor::getSelectedText()
Glib::ustring wxTextEditor::getSelectedText()
{
	//return [[self getFocusedEditor] selectedString];
	//return SSM(this->getFocusedEditor(), SCI_GETSELTEXT, 0, 0);

	gint len = SSM(this->getFocusedEditor(), SCI_GETSELTEXT, 0, 0);
	gchar *selection = new gchar[len + 1];
	SSM(this->getFocusedEditor(), SCI_GETSELTEXT, 0, (sptr_t) selection);

	Glib::ustring ret = selection;
	delete[] selection;

	return ret;
}




void wxTextEditor::setSelectedText(const gchar* text)
{
	//[[self getFocusedEditor] setStringProperty:SCI_REPLACESEL parameter:0 value:text];
	SSM(this->getFocusedEditor(), SCI_REPLACESEL, 0, (sptr_t)text);
}



//SCI_STYLERESETDEFAULT
void wxTextEditor::styleResetToDefault()
{
	//[textView1 setGeneralProperty:SCI_STYLERESETDEFAULT parameter:0 value:0];
	//[textView2 setGeneralProperty:SCI_STYLERESETDEFAULT parameter:0 value:0];
	SSM(textView1, SCI_STYLERESETDEFAULT, 0,0);
	SSM(textView2, SCI_STYLERESETDEFAULT, 0,0);
}



void wxTextEditor::styleClearAll()
{
	//[textView1 setGeneralProperty:SCI_STYLECLEARALL parameter:0 value:0];
	//[textView2 setGeneralProperty:SCI_STYLECLEARALL parameter:0 value:0];
	SSM(textView1, SCI_STYLECLEARALL, 0,0);
	SSM(textView2, SCI_STYLECLEARALL, 0,0);
}


gint wxTextEditor::GetStyleAt(gint position)
{
	//return [[self getFocusedEditor] getGeneralProperty:SCI_GETSTYLEAT parameter:position];
	return SSM(this->getFocusedEditor(), SCI_GETSTYLEAT, position, 0);
}


bool wxTextEditor::isRemAt(gint position)
{
	//TODO:
	return false;
}

gint wxTextEditor::getLineEndPosition(gint linenumber)
{
	//return [[self getFocusedEditor] getGeneralProperty:SCI_GETLINEENDPOSITION parameter:linenumber];
	return SSM(this->getFocusedEditor(), SCI_GETLINEENDPOSITION, linenumber, 0);
}

/*
//- (void) Comment
//{
//}
//
//- (void) UnComment
//{
//}
*/

void wxTextEditor::SelectLine(gint linenumber)
{
	//[self SelectLine: linenumber SetAsFirstVisibleLine: false];
	this->SelectLine(linenumber, false);
}

void wxTextEditor::SelectLine(gint linenumber, bool SetAsFirstVisibleLine)
{
	if (SetAsFirstVisibleLine == true)
	{
		this->setFirstVisibleLine(linenumber);
	}
	else
	{
		SSM(getFocusedEditor(), SCI_ENSUREVISIBLE, linenumber, 0);
		this->ScrollCaret();
	}

	gint mPos = this->getPositionFromLineNumber(linenumber);
	gint mEndPos = this->getLineEndPosition(linenumber);

	//Clear any previous selection
	SSM(this->getFocusedEditor(), SCI_CLEARSELECTIONS, 0, 0);
	
	this->setSelectionStart(mPos);
	this->setSelectionEnd(mEndPos);
	this->setFocus();
}
	
	 
void wxTextEditor::ScrollCaret()
{
	//[[self getFocusedEditor] setGeneralProperty:SCI_SCROLLCARET parameter:0 value:0];
	SSM(this->getFocusedEditor(), SCI_SCROLLCARET, 0, 0);
}



/*
- (NSString*) convertEolOfText:(NSString*)text
{
	return [textView1 backend]->ConvertEOL(text);
}
*/

void wxTextEditor::setEolMode(gint eolMode)
{
	//[textView1 setGeneralProperty:SCI_SETEOLMODE parameter:eolMode value:0];
	//[textView2 setGeneralProperty:SCI_SETEOLMODE parameter:eolMode value:0];
	SSM(textView1, SCI_SETEOLMODE, eolMode, 0);
	SSM(textView2, SCI_SETEOLMODE, eolMode, 0);
}
	 
void wxTextEditor::ConvertEOL(gint eolMode)
{
	//SSM(textView1, SCI_CONVERTEOLS, eolMode, 0);
	//SSM(textView2, SCI_CONVERTEOLS, eolMode, 0);

	std::cerr << "ConvertEOL" << std::endl;
	
	//NEW VERSION
	Glib::ustring newEol = "";
	switch (eolMode)
	{
		case SC_EOL_CRLF:
			newEol = "\r\n";
			break;

		case SC_EOL_CR:
			newEol = "\r";
			break;

		case SC_EOL_LF:
			newEol = "\n";
			break;

		default:
			return;
	}

	//1. convert all eols in "\n"
	//string temp = this.GetText().Replace("\r\n", "\n").Replace("\r", "\n");
	Glib::RefPtr<Glib::Regex> my_regex = Glib::Regex::create("\r\n");
	Glib::ustring s = my_regex->replace(getText(), 
	                                    0, 
	                                    "\n", 
	                                    Glib::REGEX_MATCH_NOTBOL);

	my_regex = Glib::Regex::create("\r");
	Glib::ustring t = my_regex->replace(s, 
	                                    0, 
	                                    "\n", 
	                                    Glib::REGEX_MATCH_NOTBOL);

	
	
	//2. convert "\n" to desired format
	BeginUndoAction();
	this->setText("");
	//Add this action to SCI_MOD_CONTAINER (for Undo/Redo notification)
	AddUndoAction(1, 0);

	if (newEol != "\n")
	{
		//ActiveEditor()->textEditor->setText(t.c_str());
		//this.SetText(temp.Replace("\n", newEol));
		my_regex = Glib::Regex::create("\n");
		Glib::ustring temp = my_regex->replace(s, 
		                                       0, 
		                                       newEol, 
		                                       Glib::REGEX_MATCH_NOTBOL);
		setText(temp.c_str());
		
	}
	else
		setText(t.c_str());

	AddUndoAction(2, 0);
	EndUndoAction();


	
	//3. set Scintilla EOL Mode
	setEolMode(eolMode);


	
	//4. free resources ???
	//temp = null;
	//GC.Collect();

}

//const gchar* wxTextEditor::ConvertEolOfString(const gchar* text)
Glib::ustring wxTextEditor::ConvertEolOfString(Glib::ustring text)
{
	Glib::ustring newEol = "\r\n";
	gint CurrentEolMode = getEOLMode();
	//Glib::ustring baseText = text;

	switch (CurrentEolMode)
	{
		case SC_EOL_CRLF:
			newEol = "\r\n";
			break;

		case SC_EOL_CR:
			newEol = "\r";
			break;

		case SC_EOL_LF:
			newEol = "\n";
			break;
	}

	
	//1. convert all eols in "\n"
	//string temp = this.GetText().Replace("\r\n", "\n").Replace("\r", "\n");
	Glib::RefPtr<Glib::Regex> my_regex = Glib::Regex::create("\r\n");
	Glib::ustring s1 = my_regex->replace(text, 
	                                     0, 
	                                     "\n", 
	                                     Glib::REGEX_MATCH_NOTBOL);

	my_regex = Glib::Regex::create("\r");
	Glib::ustring s2 = my_regex->replace(s1, 
	                                     0, 
	                                     "\n", 
	                                     Glib::REGEX_MATCH_NOTBOL);
	
	
	//2. convert "\n" to desired format
	if (newEol != "\n")
	{
		my_regex = Glib::Regex::create("\n");
		Glib::ustring ret = my_regex->replace(s2, 
		                                       0, 
		                                       newEol, 
		                                       Glib::REGEX_MATCH_NOTBOL);
		return ret;
	}
	else
		return s2;

}

void wxTextEditor::BeginUndoAction()
{
	SSM(textView1, SCI_BEGINUNDOACTION, 0, 0);
}

void wxTextEditor::AddUndoAction(gint token, gint flags)
{
	SSM(textView1, SCI_ADDUNDOACTION, token, flags);
}

void wxTextEditor::EndUndoAction()
{
	SSM(textView1, SCI_ENDUNDOACTION, 0, 0);
}


gint wxTextEditor::getEOLMode()
{
	//return [[self getFocusedEditor] getGeneralProperty:SCI_GETEOLMODE parameter:0];
	return SSM(getFocusedEditor(), SCI_GETEOLMODE, 0, 0);
}

bool wxTextEditor::GetViewEOL()
{
	//return [[self getFocusedEditor] getGeneralProperty:SCI_GETVIEWEOL parameter:0];
	return SSM(getFocusedEditor(), SCI_GETVIEWEOL, 0, 0);
}

//SCI_SETPASTECONVERTENDINGS(bool convert)
void wxTextEditor::SetPasteConvertEndings(bool convert)
{
	//[textView1 setGeneralProperty:SCI_SETPASTECONVERTENDINGS parameter:convert ? 1 : 0 value:0];
	//[textView2 setGeneralProperty:SCI_SETPASTECONVERTENDINGS parameter:convert ? 1 : 0 value:0];
	SSM(textView1, SCI_SETPASTECONVERTENDINGS, (convert ? 1 : 0), 0);
	SSM(textView2, SCI_SETPASTECONVERTENDINGS, (convert ? 1 : 0), 0);
}


gint wxTextEditor::GetEolModeReal()
{
	//string t = this.GetText();
	std::string t = this->getText();
	
	//if (t.Contains("\r\n"))
	if(t.find("\r\n") != std::string::npos)
	{
		return SC_EOL_CRLF;
	}
	else if(t.find("\r") != std::string::npos)
	{
		return SC_EOL_CR;
	}
	else if(t.find("\n") != std::string::npos)
	{
		return SC_EOL_LF;
	}
	else
	{
		return SC_EOL_LF;
	}

}

Glib::ustring wxTextEditor::GetEolModeReport()
{
	//Check Line Endings
	//SET EOL MODE: SC_EOL_CRLF (0), SC_EOL_CR (1), or SC_EOL_LF (2)

	//string s = this.GetText().Replace("\r\n", "");
	Glib::RefPtr<Glib::Regex> my_regex = Glib::Regex::create("\r\n");
	Glib::ustring s = my_regex->replace(getText(), 
	                                    0, 
	                                    "", 
	                                    Glib::REGEX_MATCH_NOTBOL);

	gint crlfOccurrences = (this->getTextLength() - s.size()) / 2;

	//Int32 crOccurrences = s.Length - s.Replace("\r", "").Length;
	my_regex = Glib::Regex::create("\r");
	gint crOccurrences = s.size() -
							my_regex->replace(s.c_str(), 
											  0, 
											  "", 
											  Glib::REGEX_MATCH_NOTBOL).size();

	//Int32 lfOccurrences = s.Length - s.Replace("\n", "").Length;
	my_regex = Glib::Regex::create("\n");
	gint lfOccurrences = s.size() -
							my_regex->replace(s.c_str(), 
											  0, 
											  "", 
											  Glib::REGEX_MATCH_NOTBOL).size();

	//string report =
	//	(crlfOccurrences > 0 ? "CRLF(" + crlfOccurrences + ") " : "") +
	//	(crOccurrences > 0 ? "CR(" + crOccurrences + ") " : "") +
	//	(lfOccurrences > 0 ? "LF(" + lfOccurrences + ") " : "");
	Glib::ustring report = "";
	if(crlfOccurrences > 0)
		report.append(Glib::ustring::compose("CRLF(%1) ", crlfOccurrences));
	if(crOccurrences > 0)
		report.append(Glib::ustring::compose("CR(%1) ", crOccurrences));
	if(lfOccurrences > 0)
		report.append(Glib::ustring::compose("LF(%1) ", lfOccurrences));
	
	if ((crlfOccurrences > 0 && crOccurrences > 0) ||
	    (crlfOccurrences > 0 && lfOccurrences > 0) ||
	    (crOccurrences > 0 && lfOccurrences > 0))
		report.append(" (mixed eols)");

	return report;//.c_str();
}









/*OSX
- (void) SCI_MOD_CONTAINER_Notification: (NSNotification*) notification
{

	//After Undo/Redo we must check for real line endings to synchronize
	//Scintilla Eol Mode
	
	NSInteger eolModeReal = [self GetEolModeReal];
	NSInteger eolModeScintilla = [self GetEOLMode];
	
	if (eolModeReal != eolModeScintilla)
	{
		[self setEolMode:eolModeReal];
	}  
	
	NSLog(@"TextEditorView: SCI_MOD_CONTAINER - Real:%d Scintilla:%d", eolModeReal, eolModeScintilla);
	
}

*/






gint wxTextEditor::BraceMatch(gint pos, gint maxReStyle)
{
	//return [textView1 backend]->WndProc(SCI_WORDENDPOSITION, position, true);		
	//return [[self getFocusedEditor] backend]->WndProc(SCI_BRACEMATCH, pos, maxReStyle);
	return SSM(getFocusedEditor(), SCI_BRACEMATCH, pos, maxReStyle);
}

void wxTextEditor::BraceHiglight(gint pos1, gint pos2)
{
	//[[self getFocusedEditor] setGeneralProperty:SCI_BRACEHIGHLIGHT parameter:pos1 value:pos2];
	SSM(getFocusedEditor(), SCI_BRACEHIGHLIGHT, pos1, pos2);
}



void wxTextEditor::setAllowCaretBeyondEOL(bool value)
{
	SSM(textView1, SCI_SETENDATLASTLINE, (value ? 0 : 1), 0);
	SSM(textView2, SCI_SETENDATLASTLINE, (value ? 0 : 1), 0);
}

void wxTextEditor::RefreshSyntax()
{
	SSM(textView1, SCI_COLOURISE, 0, -1);
	SSM(textView2, SCI_COLOURISE, 0, -1);
}

void wxTextEditor::Colourise(gint view, int startPos, int endPos)
{
	if(view == 2)
		SSM(textView2, SCI_COLOURISE, startPos, endPos);
	else
		SSM(textView1, SCI_COLOURISE, startPos, endPos);
}

void wxTextEditor::Colourise(int startPos, int endPos)
{
	SSM(getFocusedEditor(), SCI_COLOURISE, startPos, endPos);
}

//------------------------------------------------------------------------------------------------
// FOCUS IMPLEMENTATION
//------------------------------------------------------------------------------------------------
ScintillaObject* wxTextEditor::getFocusedEditor()
{
	//if(SSM(textView2, SCI_GETFOCUS, 0, 0) > 0)
	if(textView2_Widget->is_focus())
	{
		return textView2;
	}
	//else if(SSM(textView1, SCI_GETFOCUS, 0, 0) > 0)
	else if(textView1_Widget->is_focus())
	{
		return textView1;
	}
	else
	{
		if(_OldFocusedEditor != NULL)
			return _OldFocusedEditor;
	}
	
	return textView1;
}


void wxTextEditor::setFocus()
{
	//[[self window] makeFirstResponder: [[self getFocusedEditor] content]];
	gtk_widget_grab_focus(GTK_WIDGET(this->getFocusedEditor()));
}

bool wxTextEditor::getFocus()
{
	return (SSM(textView1, SCI_GETFOCUS, 0, 0) ||
	        SSM(textView2, SCI_GETFOCUS, 0, 0));
}

void wxTextEditor::removeFocus()
{
	_OldFocusedEditor = this->getFocusedEditor();
}




void wxTextEditor::setFocusOnPrimaryView()	//OK
{
	//gtk_widget_grab_focus(GTK_WIDGET(textView1));
	textView1_Widget->grab_focus();
	//SSM(textView1, SCI_GRABFOCUS, 0, 0);
	//SSM(textView1, SCI_SETFOCUS, 0, 0);
	
	//_OldFocusedEditor = [textView1 content];
}

void wxTextEditor::setFocusOnSecondaryView()	//OK
{
	//gtk_widget_grab_focus(GTK_WIDGET(textView2));
	textView2_Widget->grab_focus();
	//SSM(textView2, SCI_GRABFOCUS, 0, 0);
	//SSM(textView2, SCI_SETFOCUS, 0, 0);
	
	//_OldFocusedEditor = [textView2 content];
}




//gchar* gdk_color_to_string (const GdkColor *color);
Glib::ustring wxTextEditor::getStringFromColor(GdkColor* color)
{
	//Return a newly allocated string !!!
	//return gdk_color_to_string (color);
	gchar* c = gdk_color_to_string (color);

	Glib::ustring ret = c;
	g_free(c);
	
	return ret;
}

gint wxTextEditor::getColorFromString(const gchar* stringColor)
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



//------------------------------------------------------------------------------------------------
// BOOKMARKS
//------------------------------------------------------------------------------------------------
//gtk.gdk.Color.to_string
//gboolean gdk_color_parse (const gchar *spec, GdkColor *color);
//string = 0x008000 (#008000)
void wxTextEditor::MarkerSetFore(gint number, const gchar* htmlColor)
{
	//[textView1 setColorProperty: SCI_MARKERSETFORE parameter: number fromHTML: htmlColor];
	//[textView2 setColorProperty: SCI_MARKERSETFORE parameter: number fromHTML: htmlColor];
	SSM(textView1, SCI_MARKERSETFORE, number, getColorFromString(htmlColor));
	SSM(textView2, SCI_MARKERSETFORE, number, getColorFromString(htmlColor));
}

void wxTextEditor::MarkerSetBack(gint number, const gchar* htmlColor)
{
	//[textView1 setColorProperty: SCI_MARKERSETBACK parameter: number fromHTML: htmlColor];
	//[textView2 setColorProperty: SCI_MARKERSETBACK parameter: number fromHTML: htmlColor];
	SSM(textView1, SCI_MARKERSETBACK, number, getColorFromString(htmlColor));
	SSM(textView2, SCI_MARKERSETBACK, number, getColorFromString(htmlColor));
}

void wxTextEditor::MarkerSetAlpha(gint number, gint intColor) // intColor value = 0 - 256 
{
	//[textView1 setGeneralProperty: SCI_MARKERSETALPHA parameter: number value: intColor];
	//[textView2 setGeneralProperty: SCI_MARKERSETALPHA parameter: number value: intColor];
	SSM(textView1, SCI_MARKERSETALPHA, number, intColor);
	SSM(textView2, SCI_MARKERSETALPHA, number, intColor);
}

gint wxTextEditor::MarkerGet(gint linenumber)
{
	//return [textView1 getGeneralProperty:SCI_MARKERGET parameter:linenumber];
	return SSM(textView1, SCI_MARKERGET, linenumber, 0);
}

void wxTextEditor::MarkerDelete(gint linenumber)
{
	//[textView1 setGeneralProperty:SCI_MARKERDELETE parameter:linenumber value:0];
	//[textView2 setGeneralProperty:SCI_MARKERDELETE parameter:linenumber value:0];
	SSM(textView1, SCI_MARKERDELETE, linenumber, 0);
	SSM(textView2, SCI_MARKERDELETE, linenumber, 0);
}

void wxTextEditor::MarkerAdd(gint linenumber)
{
	//[textView1 setGeneralProperty:SCI_MARKERADD parameter:linenumber value:0];
	//[textView2 setGeneralProperty:SCI_MARKERADD parameter:linenumber value:0];
	SSM(textView1, SCI_MARKERADD, linenumber, 0);
	SSM(textView2, SCI_MARKERADD, linenumber, 0);
}

void wxTextEditor::MarkerDeleteAll(gint markerNumber)
{
	//[textView1 setGeneralProperty:SCI_MARKERDELETEALL parameter:markerNumber value:0];
	//[textView2 setGeneralProperty:SCI_MARKERDELETEALL parameter:markerNumber value:0];
	SSM(textView1, SCI_MARKERDELETEALL, markerNumber, 0);
	SSM(textView2, SCI_MARKERDELETEALL, markerNumber, 0);
}

gint wxTextEditor::MarkerNext(gint lineStart, gint markerMask)
{
	//return [textView1 getGeneralProperty:SCI_MARKERNEXT parameter:lineStart extra:markerMask];
	return SSM(textView1, SCI_MARKERNEXT, lineStart, markerMask);
}

gint wxTextEditor::MarkerPrevious(gint lineStart, gint markerMask)
{
	//return [textView1 getGeneralProperty:SCI_MARKERPREVIOUS parameter:lineStart extra:markerMask];
	return SSM(textView1, SCI_MARKERPREVIOUS, lineStart, markerMask);
}


void wxTextEditor::InsertRemoveBookmark()
{
	/*
	NSInteger mLine = [self getCurrentLineNumber];
	if([self MarkerGet:mLine] > 0)
	{
		[self MarkerDelete:mLine];
		
	}
	else //Bookmark doesn't exist so we add it
	{
		[self MarkerAdd:mLine];		
	}
	*/
	gint mLine = this->getCurrentLineNumber();
	if(this->MarkerGet(mLine) > 0)
	{
		this->MarkerDelete(mLine);
	}
	else
	{
		this->MarkerAdd(mLine);
	}
}

bool wxTextEditor::hasBookmarks()
{
	//NSInteger mLine = [self MarkerNext:0 markerMask:1];
	//return (mLine > -1);
	gint mLine = this->MarkerNext(0, 1);
	return (mLine > -1);
}

void wxTextEditor::InsertBookmarkAt(gint linePosition)
{
	//[self MarkerAdd:linePosition];
	this->MarkerAdd(linePosition);
}

void wxTextEditor::RemoveAllBookmarks()
{
	//[self MarkerDeleteAll:0];
	this->MarkerDeleteAll(0);
}

void wxTextEditor::GoToNextBookmark()
{
	//NSInteger mLine = [self MarkerNext:[self getCurrentLineNumber]+1 markerMask:1];
	//if (mLine > -1)
	//	[self GoToLine:mLine];

	gint mLine = this->MarkerNext(this->getCurrentLineNumber() + 1, 1);
	if(mLine > -1)
		this->GoToLine(mLine);
}

void wxTextEditor::GoToPreviousBookmark()
{
	//NSInteger mLine = [self MarkerPrevious:[self getCurrentLineNumber]-1 markerMask:1];
	//if (mLine > -1)
	//	[self GoToLine:mLine];

	gint mLine = this->MarkerPrevious(this->getCurrentLineNumber() - 1, 1);
	if(mLine > -1)
		this->GoToLine(mLine);
}


////////////////////////////////////////////////////////////////////////////////
// AUTOCOMPLETION
////////////////////////////////////////////////////////////////////////////////
//SCI_AUTOCSHOW(int lenEntered, const char *list)
void wxTextEditor::AutocShow(int lenEntered, const gchar *list)
{
	SSM(getFocusedEditor(), SCI_AUTOCSHOW, lenEntered, (sptr_t)list);
}

//SCI_AUTOCCANCEL
void wxTextEditor::AutocCancel()
{
	SSM(getFocusedEditor(), SCI_AUTOCCANCEL, 0, 0);
}

//SCI_AUTOCACTIVE
bool wxTextEditor::AutocActive()
{
	return SSM(getFocusedEditor(), SCI_AUTOCACTIVE, 0, 0);
}

//SCI_AUTOCPOSSTART
gint wxTextEditor::AutocPosStart()
{
	return SSM(getFocusedEditor(), SCI_AUTOCPOSSTART, 0, 0);
}

//SCI_AUTOCCOMPLETE
void wxTextEditor::AutocComplete()
{
	SSM(getFocusedEditor(), SCI_AUTOCCOMPLETE, 0, 0);
}

//SCI_AUTOCSTOPS(<unused>, const char *chars)
void wxTextEditor::AutocStops(const gchar *chars)
{
	SSM(getFocusedEditor(), SCI_AUTOCSTOPS, 0, (sptr_t)chars);
}

/*
//SCI_AUTOCSETSEPARATOR(char separator)
void wxTextEditor::AutocSetSeparator(const gchar *separator)
{
	SSM(getFocusedEditor(), SCI_AUTOCSETSEPARATOR, (sptr_t)separator, 0);
}
//SCI_AUTOCGETSEPARATOR
const gchar* wxTextEditor::AutocGetSeparator()
{
	return SSM(getFocusedEditor(), SCI_AUTOCGETSEPARATOR, 0, 0);
}
*/

//SCI_AUTOCSELECT(<unused>, const char *select)
void wxTextEditor::AutocSelect(const gchar *select)
{
	SSM(getFocusedEditor(), SCI_AUTOCSELECT, 0, (sptr_t)select);
}

//SCI_AUTOCGETCURRENT
gint wxTextEditor::AutocGetCurrent()
{
	return SSM(getFocusedEditor(), SCI_AUTOCGETCURRENT, 0, 0);
}

//SCI_AUTOCGETCURRENTTEXT(<unused>, char *text)
//gchar* wxTextEditor::AutocGetCurrentText()
Glib::ustring wxTextEditor::AutocGetCurrentText()
{
	gchar* result = new gchar[256];
	SSM(getFocusedEditor(), SCI_AUTOCGETCURRENTTEXT, 0, (sptr_t)result);

	Glib::ustring ret = result;
	delete[] result;
	
	return ret;
}

//SCI_AUTOCSETCANCELATSTART(bool cancel)
void wxTextEditor::AutocSetCancelAtStart(bool cancel)
{
	SSM(textView1, SCI_AUTOCSETCANCELATSTART, (cancel ? 1 : 0), 0);
	SSM(textView2, SCI_AUTOCSETCANCELATSTART, (cancel ? 1 : 0), 0);
}

//SCI_AUTOCGETCANCELATSTART
bool wxTextEditor::AutocGetCancelAtStart()
{
	return SSM(getFocusedEditor(), SCI_AUTOCGETCANCELATSTART, 0, 0);
}

//SCI_AUTOCSETFILLUPS(<unused>, const char *chars)
void wxTextEditor::AutocSetFillups(const gchar *chars)
{
	SSM(textView1, SCI_AUTOCSETFILLUPS, 0, (sptr_t)chars);
	SSM(textView2, SCI_AUTOCSETFILLUPS, 0, (sptr_t)chars);
}

//SCI_AUTOCSETCHOOSESINGLE(bool chooseSingle)
void wxTextEditor::AutocSetChooseSingle(bool chooseSingle)
{
	SSM(textView1, SCI_AUTOCSETCHOOSESINGLE, (chooseSingle ? 1 : 0), 0);
	SSM(textView2, SCI_AUTOCSETCHOOSESINGLE, (chooseSingle ? 1 : 0), 0);
}

//SCI_AUTOCGETCHOOSESINGLE
bool wxTextEditor::AutocGetChooseSingle()
{
	return SSM(getFocusedEditor(), SCI_AUTOCGETCHOOSESINGLE, 0, 0);
}

//SCI_AUTOCSETIGNORECASE(bool ignoreCase)
void wxTextEditor::AutocSetIgnoreCase(bool ignoreCase)
{
	SSM(textView1, SCI_AUTOCSETIGNORECASE, (ignoreCase ? 1 : 0), 0);
	SSM(textView2, SCI_AUTOCSETIGNORECASE, (ignoreCase ? 1 : 0), 0);
}

//SCI_AUTOCGETIGNORECASE
bool wxTextEditor::AutocGetIgnoreCase()
{
	return SSM(getFocusedEditor(), SCI_AUTOCGETIGNORECASE, 0, 0);
}

//SCI_AUTOCSETAUTOHIDE(bool autoHide)
void wxTextEditor::AutocSetAutoHide(bool autoHide)
{
	SSM(textView1, SCI_AUTOCSETAUTOHIDE, (autoHide ? 1 : 0), 0);
	SSM(textView2, SCI_AUTOCSETAUTOHIDE, (autoHide ? 1 : 0), 0);
}

//SCI_AUTOCGETAUTOHIDE
bool wxTextEditor::AutocGetAutoHide()
{
	return SSM(getFocusedEditor(), SCI_AUTOCGETAUTOHIDE, 0, 0);
}

//SCI_AUTOCSETDROPRESTOFWORD(bool dropRestOfWord)
void wxTextEditor::AutocSetDropRestOfWord(bool dropRestOfWord)
{
	SSM(textView1, SCI_AUTOCSETDROPRESTOFWORD, (dropRestOfWord ? 1 : 0), 0);
	SSM(textView2, SCI_AUTOCSETDROPRESTOFWORD, (dropRestOfWord ? 1 : 0), 0);
}

//SCI_AUTOCGETDROPRESTOFWORD
bool wxTextEditor::AutocGetDropRestOfWord()
{
	return SSM(getFocusedEditor(), SCI_AUTOCGETDROPRESTOFWORD, 0, 0);
}

//SCI_REGISTERIMAGE
//SCI_CLEARREGISTEREDIMAGES
//SCI_AUTOCSETTYPESEPARATOR(char separatorCharacter)
//SCI_AUTOCGETTYPESEPARATOR


//SCI_AUTOCSETMAXHEIGHT(int rowCount)
void wxTextEditor::AutocSetMaxHeight(gint rowCount)
{
	SSM(textView1, SCI_AUTOCSETMAXHEIGHT, rowCount, 0);
	SSM(textView2, SCI_AUTOCSETMAXHEIGHT, rowCount, 0);
}

//SCI_AUTOCGETMAXHEIGHT
gint wxTextEditor::AutocGetMaxHeight()
{
	return SSM(getFocusedEditor(), SCI_AUTOCGETMAXHEIGHT, 0, 0);
}

//SCI_AUTOCSETMAXWIDTH(int characterCount)
void wxTextEditor::AutocSetMaxWidth(gint characterCount)
{
	SSM(textView1, SCI_AUTOCSETMAXWIDTH, characterCount, 0);
	SSM(textView2, SCI_AUTOCSETMAXWIDTH, characterCount, 0);
}

//SCI_AUTOCGETMAXWIDTH
gint wxTextEditor::AutocGetMaxWidth()
{
	return SSM(getFocusedEditor(), SCI_AUTOCGETMAXWIDTH, 0, 0);
}







Glib::ustring wxTextEditor::newLine()
{
	////[textView1 setGeneralProperty:SCI_NEWLINE parameter:0 value:0];
	
	gint eolMode = getEOLMode();
	Glib::ustring newEol = "";
	
	switch (eolMode)
	{
		case SC_EOL_CRLF:
			newEol = "\r\n";
			break;
			
		case SC_EOL_CR:
			newEol = "\r";
			break;
			
		case SC_EOL_LF:
			newEol = "\n";
			break;
			
		default:
			newEol = "\n";
			break;
	}
	
	return newEol;
	
}

//FOLDING STUFFS
void wxTextEditor::ToggleFold(gint line)
{
	SSM(getFocusedEditor(), SCI_TOGGLEFOLD, line, 0);
}

void wxTextEditor::ShowFoldLine(bool value)
{
	if(value)
	{
		SSM(textView1, SCI_SETFOLDFLAGS, 16, 0);
		SSM(textView2, SCI_SETFOLDFLAGS, 16, 0);
	}
	else
	{
		SSM(textView1, SCI_SETFOLDFLAGS, 0, 0);
		SSM(textView2, SCI_SETFOLDFLAGS, 0, 0);
	}
}

void wxTextEditor::SetFoldMarginColour(bool useSettings, const gchar* htmlColor)
{
	SSM(textView1, SCI_SETFOLDMARGINCOLOUR, useSettings, getColorFromString(htmlColor));
}

void wxTextEditor::SetFoldMarginHiColour(bool useSettings, const gchar* htmlColor)
{
	SSM(textView1, SCI_SETFOLDMARGINHICOLOUR, useSettings, getColorFromString(htmlColor));
}

gint wxTextEditor::getFoldLevel(gint line)
{
	return SSM(getFocusedEditor(), SCI_GETFOLDLEVEL, line, 0);
}

gint wxTextEditor::getFoldParent(gint line)
{
	return SSM(getFocusedEditor(), SCI_GETFOLDPARENT, line, 0);
}

bool wxTextEditor::getFoldExpanded(gint line)
{
	return SSM(getFocusedEditor(), SCI_GETFOLDEXPANDED, line, 0);
}

