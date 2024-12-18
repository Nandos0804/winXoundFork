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

#include "../scintilla/Scintilla.h"
#include "../scintilla/SciLexer.h"
#include "../scintilla/ScintillaWidget.h"

//class ScintillaObject;

#ifndef _WX_TEXTEDITOR_H_
#define _WX_TEXTEDITOR_H_



class wxTextEditor : public Gtk::Frame //public Gtk::VPaned
{
public:
	wxTextEditor();
	virtual ~wxTextEditor();

	void CreateNewTextEditor(void);
	//gchar* getFontName(int styleNumber);
	Glib::ustring getFontName(int styleNumber);
	gint measureTextWidth(const gchar *text, gint view);
	void setSearchFlags(gint searchflags);
	void setTargetStart(gint mStart);
	void setTargetEnd(gint mEnd);
	gint searchInTarget(const gchar *text);
	void setScrollWidth(gint pixelWidth);
	gint getScrollWidth();
	void setScrollWidthTracking(bool tracking);
	bool getShowSpaces();
	void setShowSpaces(bool val);
	bool getShowEOLMarker();
	void setShowEOLMarker(bool val);
	bool getEndAtLastLine();
	void setEndAtLastLine(bool val);
	bool getShowMatchingBracket();
	void setShowMatchingBracket(bool val);
	bool getCanUndo();
	bool getCanRedo();
	gint getTabIndent();
	void setTabIndent(gint val);
	bool getShowLineNumbers();
	void setShowLineNumbers(bool val);
	bool getMarkCaretLine();
	void setMarkCaretLine(bool val);
	bool getShowVerticalRuler();
	void setShowVerticalRuler(bool val);
	bool getReadOnly();
	void setReadOnly(bool val);
	ScintillaObject* getPrimaryView();
	ScintillaObject* getSecondaryView();
	//gchar* getText();
	Glib::ustring getText();
	void setText(const gchar* text);
	gboolean LoadFile(const gchar* filename);
	gboolean SaveFile(const gchar* filename);
	gint getTextLength();
	void setHighlight(const gchar* languageName);
	void setKeyWords(gint keyWordSet, const gchar* keyWordList);
	void setWordChars(const gchar* wordChars);
	void setCodePage(gint codepage);
	gint getLineLength(gint lineNumber);
	gint getLineEndPosition(gint linenumber);
	gint getPositionFromLineNumber(gint linenumber);
	//const gchar* getTextOfLine(gint lineNumber);
	Glib::ustring getTextOfLine(gint lineNumber);
	Glib::ustring getTextOfLineWithEol(gint lineNumber);
	ScintillaObject* getFocusedEditor();
	gint getCurrentLineNumber();
	void ClearAllText();
	gint getCaretPosition();
	void setCaretPosition(gint position);
	void GoToLine(gint lineNumber);
	void InsertText(gint position, const gchar* text);
	void AddText(const gchar* text);
	void AppendText(const gchar* text);
	Glib::ustring getCharAt(gint position);
	gint getLinesCount();
	gint getFirstVisibleLine();
	gint getFirstVisibleLineAtView(gint view);
	gint getLinesOnScreen();
	bool getCanPaste();
	void PerformUndo();
	void PerformRedo();
	void PerformCopy();
	void PerformCut();
	void PerformPaste();
	void PerformDelete();
	void PerformSelectAll();
	bool isTextChanged();
	void setSavePoint();
	void emptyUndoBuffer();
	gint getZoom();
	gint getZoomForView1();
	gint getZoomForView2();
	void setZoom(gint zoom);
	void setZoomForView1(gint zoom);
	void setZoomForView2(gint zoom);
	void ReplaceTarget(gint offset, gint length, const gchar* replaceString);
	void ReplaceText(const gchar* replaceString);
	//gchar* getSelectedText();
	Glib::ustring getSelectedText();
	void setSelectedText(const gchar* text);
	void setSelection(gint start, gint end);
	void setSelectionStart(gint start);
	void setSelectionEnd(gint end);
	gint getSelectionStart();
	gint getSelectionEnd();
	void styleResetToDefault();
	void styleClearAll();
	void SelectLine(gint linenumber);
	void SelectLine(gint linenumber, bool SetAsFirstVisibleLine);
	void ScrollCaret();
	gint GetStyleAt(gint position);
	void setFirstVisibleLine(gint lineNumber);
	void setFirstVisibleLineAtView(gint lineNumber, gint view);
	void setFocusOnPrimaryView();
	void setFocusOnSecondaryView();
	void setFocus();
	bool getFocus();
	void removeFocus();
	void MarkerSetFore(gint number, const gchar* htmlColor);
	void MarkerSetBack(gint number, const gchar* htmlColor);
	void MarkerSetAlpha(gint number, gint intColor);
	gint MarkerGet(gint linenumber);
	void MarkerDelete(gint linenumber);
	void MarkerAdd(gint linenumber);
	void MarkerDeleteAll(gint markerNumber);
	gint MarkerNext(gint lineStart, gint markerMask);
	gint MarkerPrevious(gint lineStart, gint markerMask);
	Glib::ustring getStringFromColor(GdkColor* color);
	gint getColorFromString(const gchar* stringColor);
	void InsertRemoveBookmark();
	bool hasBookmarks();
	void InsertBookmarkAt(gint linePosition);
	void RemoveAllBookmarks();
	void GoToNextBookmark();
	void GoToPreviousBookmark();
	void setCaretFore(const gchar* htmlColor);
	void setCaretLineBack(const gchar* htmlColor);
	void setEdgeColor(const gchar* htmlColor);
	void setSelFore(bool useSel, const gchar* htmlColor);
	void setSelBack(bool useSel, const gchar* htmlColor);
	void StyleSetFore(gint index, const gchar* htmlColor);
	void StyleSetBack(gint index, const gchar* htmlColor);
	void StyleSetBold(gint index, bool bold);
	void StyleSetItalic(gint index, bool italic);
	//gchar* getTextEditorFont();
	Glib::ustring getTextEditorFont();
	void setTextEditorFont(const gchar* fontname, gint fontsize);
	bool getIsSplitted();
	void RemoveSplit();
	void Split();
	void SplitVertical();
	void setAllowCaretBeyondEOL(bool value);
	void RefreshSyntax();
	Glib::ustring getWordAt(gint position);
	bool isValidChar(const gchar* c);
	bool isValidCharForAutoc(const gchar* c);
	gint FindText(const gchar* text, 
	              bool MatchWholeWord,
	              bool MatchCase,
	              bool IsBackward,
	              bool SelectText,
	              bool ShowMessage,
	              bool SkipRem);
	gint FindText(const gchar* text, 
	              bool MatchWholeWord,
	              bool MatchCase,
	              bool IsBackward,
	              bool SelectText,
	              bool ShowMessage,
	              bool SkipRem,
	              gint Start,
	              gint End);
	gint FindText(const gchar* text, 
	              bool MatchWholeWord,
	              bool MatchCase,
	              bool IsBackward,
	              bool SelectText,
	              bool ShowMessage,
	              bool SkipRem,
	              gint start,
	              gint end,
	              bool useRegEx);
	gint getLineNumberFromPosition(gint position);
	bool isRemAt(gint position);
	void setEolMode(gint eolMode);
	void ConvertEOL(gint eolMode);
	gint getEOLMode();
	bool GetViewEOL();
	void SetPasteConvertEndings(bool convert);
	gint GetEolModeReal();
	Glib::ustring GetEolModeReport();
	
	//const gchar* ConvertEolOfString(const gchar* text);
	Glib::ustring ConvertEolOfString(Glib::ustring baseText);
	
	void BeginUndoAction();
	void AddUndoAction(gint token, gint flags);
	void EndUndoAction();
	void refreshMargins();
	void refreshMargins(gint view);
	//gchar* getTextRange(gint start, gint end);
	Glib::ustring getTextRange(gint start, gint end);
	gint ReplaceAllText(const gchar* StringToFind,
	                    const gchar* ReplaceString,
	                    bool MatchWholeWord,
	                    bool MatchCase,
	                    bool FromCaretPosition, 
	                    bool FCPUp);

	void AutocShow(int lenEntered, const gchar *list);
	void AutocCancel();
	bool AutocActive();
	gint AutocPosStart();
	void AutocComplete();
	void AutocStops(const gchar *chars);
	void AutocSelect(const gchar *select);
	gint AutocGetCurrent();
	//gchar* AutocGetCurrentText();
	Glib::ustring AutocGetCurrentText();
	void AutocSetCancelAtStart(bool cancel);
	bool AutocGetCancelAtStart();
	void AutocSetFillups(const gchar *chars);
	void AutocSetChooseSingle(bool chooseSingle);
	bool AutocGetChooseSingle();
	void AutocSetIgnoreCase(bool ignoreCase);
	bool AutocGetIgnoreCase();
	void AutocSetAutoHide(bool autoHide);
	bool AutocGetAutoHide();
	void AutocSetDropRestOfWord(bool dropRestOfWord);
	bool AutocGetDropRestOfWord();
	void AutocSetMaxHeight(gint rowCount);
	gint AutocGetMaxHeight();
	void AutocSetMaxWidth(gint characterCount);
	gint AutocGetMaxWidth();
	gint getPositionFromPoint(gint x, gint y);
	gint getPositionFromPoint(gint x, gint y, gint view);

	Glib::ustring getCurrentWord();
	Glib::ustring getCurrentWordForAutoc();

	Gtk::Widget*  get_textView1_Widget();
	Gtk::Widget*  get_textView2_Widget();

	gint BraceMatch(gint pos, gint maxReStyle);
	void BraceHiglight(gint pos1, gint pos2);

	Gdk::Point getQuotesPosition(gint position);
	Glib::ustring getTextInQuotes(gint position);

	Glib::ustring newLine();

	gint getWordStart(gint position);
	gint getWordEnd(gint position);

	void SetUndoCollection(bool value);

	void Colourise(gint view, int startPos, int endPos);
	void Colourise(int startPos, int endPos);

	gint getIndentMarginWidth(int view);
	void ToggleFold(gint line);
	void ShowFoldLine(bool value);
	void SetFoldMarginColour(bool useSettings, const gchar* htmlColor);
	void SetFoldMarginHiColour(bool useSettings, const gchar* htmlColor);
	gint getFoldLevel(gint line);
	gint getFoldParent(gint line);
	bool getFoldExpanded(gint line);
	
	

	
protected:

private:
	ScintillaObject*	textView1;
	ScintillaObject*	textView2;
	Gtk::Widget*		textView1_Widget;
	Gtk::Widget*		textView2_Widget;

	ScintillaObject*	_OldFocusedEditor;
	bool				_ShowMatchingBracket;
	int					SCI_WIDTH_MARGINS_1;
	int					SCI_WIDTH_MARGINS_2;
	int					SCI_INDENT_MARGINS_1;
	int					SCI_INDENT_MARGINS_2;

};

#endif // _WX_TEXTEDITOR_H_
