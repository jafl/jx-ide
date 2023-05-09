/******************************************************************************
 TextDocument.h

	Interface for the TextDocument class

	Copyright © 1996-98 by John Lindal.

 ******************************************************************************/

#ifndef _H_TextDocument
#define _H_TextDocument

#include <jx-af/jx/JXFileDocument.h>
#include <jx-af/jcore/JPrefObject.h>
#include "TextFileType.h"
#include <jx-af/jx/JXWidget.h>			// for H/VSizing
#include <jx-af/jcore/JStyledText.h>	// for PlainTextFormat, CRMRuleList

class JSTStyler;
class TextEditor;
class FileDragSource;
class FileNameDisplay;
class StylerBase;
class StringCompleter;
class CharActionManager;
class MacroManager;
class CommandMenu;

class JXWidget;
class JXTextButton;
class JXTextCheckbox;
class JXMenuBar;
class JXTextMenu;
class JXToolBar;

class TextDocument;
class TELineIndexInput;
class TEColIndexInput;
class JXScrollbarSet;

using TextEditorCtorFn =
	TextEditor* (*)(TextDocument* document, const JString& fileName,
					JXMenuBar* menuBar,
					TELineIndexInput* lineInput, TEColIndexInput* colInput,
					JXScrollbarSet* scrollbarSet);

class TextDocument : public JXFileDocument, public JPrefObject
{
public:

	TextDocument(const TextFileType type = kUnknownFT,
				 const JUtf8Byte* helpSectionName = "EditorHelp",
				 const bool setWMClass = true,
				 TextEditorCtorFn teCtorFn = ConstructTextEditor);
	TextDocument(const JString& fileName,
				 const TextFileType type = kUnknownFT,
				 const bool tmpl = false);
	TextDocument(std::istream& input, const JFileVersion vers, bool* keep);

	~TextDocument() override;

	void	Activate() override;
	bool	GetMenuIcon(const JXImage** icon) const override;

	void	GoToLine(const JIndex lineIndex) const;
	void	SelectLines(const JIndexRange& range) const;
	void	EditPrefs();
	void	HandleActionButton();

	void	WriteForProject(std::ostream& output) const;

	bool	GetWindowSize(JPoint* desktopLoc,
						  JCoordinate* width, JCoordinate* height) const;
	void	SaveWindowSize() const;

	bool	WillOpenComplFileOnTop() const;
	void	ShouldOpenComplFileOnTop(const bool doIt);

	TextEditor*	GetTextEditor() const;
	JXMenuBar*	GetMenuBar() const;
	JXToolBar*	GetToolBar() const;
	JRect		GetFileDisplayInfo(JXWidget::HSizingOption* hSizing,
								   JXWidget::VSizingOption* vSizing) const;
	void		SetFileDisplayVisible(const bool show);

	TextFileType	GetFileType() const;
	bool			GetStyler(StylerBase** styler) const;
	bool			GetStringCompleter(StringCompleter** completer) const;
	bool			GetCharActionManager(CharActionManager** mgr) const;
	bool			GetMacroManager(MacroManager** mgr) const;

	void	OverrideTabWidth(const JSize tabWidth);
	void	OverrideAutoIndent(const bool autoIndent);
	void	OverrideTabInsertsSpaces(const bool insertSpaces);
	void	OverrideShowWhitespace(const bool showWhitespace);
	void	OverrideBreakCROnly(const bool breakCROnly);
	void	OverrideReadOnly(const bool readOnly);

	static void	ReadStaticGlobalPrefs(std::istream& input, const JFileVersion vers);
	static void	WriteStaticGlobalPrefs(std::ostream& output);

	// called by DocumentManager

	void	UpdateFileType(const bool init = false);
	void	StylerChanged(JSTStyler* styler);

	void	RefreshVCSStatus();

	static bool	OpenAsBinaryFile(const JString& fileName);

	// called by TextEditor

	virtual void	ConvertSelectionToFullPath(JString* fileName) const;

protected:

	JXTextMenu*		InsertTextMenu(const JString& title);
	CommandMenu*	GetCommandMenu();

	void	ReadFile(const JString& fileName, const bool firstTime);
	void	WriteTextFile(std::ostream& output, const bool safetySave) const override;
	void	DiscardChanges() override;

	void	HandleFileModifiedByOthers(const bool modTimeChanged,
									   const bool permsChanged) override;

	void	ReadPrefs(std::istream& input) override;
	void	WritePrefs(std::ostream& output) const override;

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	enum
	{
		kAutoIndentIndex = 0,
		kTabInsertsSpacesIndex,
		kShowWhitespaceIndex,
		kWordWrapIndex,
		kReadOnlyIndex,
		kTabWidthIndex,

		kSettingCount
	};

private:

	TextEditor*		itsTextEditor;				// owned by its enclosure
	bool			itsOpenOverComplementFlag;	// true if open window over complement file
	JXTextMenu*		itsFileMenu;
	JXTextMenu*		itsFileFormatMenu;
	JXTextMenu*		itsDiffMenu;
	CommandMenu*	itsCmdMenu;
	JXTextMenu*		itsWindowMenu;
	JXTextMenu*		itsPrefsMenu;
	JXTextMenu*		itsPrefsStylesMenu;

	TextFileType				itsFileType;
	CharActionManager*			itsActionMgr;		// not owned; can be nullptr
	MacroManager*				itsMacroMgr;		// not owned; can be nullptr
	JStyledText::CRMRuleList*	itsCRMRuleList;		// not owned; can be nullptr

	const JUtf8Byte*	itsHelpSectionName;
	bool				itsUpdateFileTypeFlag;		// false while constructing
	bool				itsOverrideFlag[ kSettingCount ];

	JStyledText::PlainTextFormat	itsFileFormat;

// begin JXLayout

	FileDragSource*  itsFileDragSource;
	JXMenuBar*       itsMenuBar;
	JXTextButton*    itsActionButton;
	JXToolBar*       itsToolBar;
	FileNameDisplay* itsFileDisplay;
	JXTextMenu*      itsSettingsMenu;

// end JXLayout

private:

	void	TextDocumentX1(const TextFileType type);
	void	TextDocumentX2(const bool setWindowSize);
	void	BuildWindow(const bool setWMClass,
						TextEditorCtorFn teCtorFn);
	void	DisplayFileName(const JString& name);

	void	UpdateFileMenu();
	void	HandleFileMenu(const JIndex item);

	bool	OpenSomething();

	void	UpdateFileFormatMenu();
	void	HandleFileFormatMenu(const JIndex item);

	void	UpdateDiffMenu();
	void	HandleDiffMenu(const JIndex item);

	void	HandlePrefsMenu(const JIndex index);

	void	HandlePrefsStylesMenu(const JIndex index);

	void	UpdateSettingsMenu();
	void	HandleSettingsMenu(const JIndex index);

	bool	ReadFromProject(std::istream& input, const JFileVersion vers);

	void	UpdateReadOnlyDisplay();

	static TextEditor*	ConstructTextEditor(TextDocument* document,
											const JString& fileName,
											JXMenuBar* menuBar,
											TELineIndexInput* lineInput,
											TEColIndexInput* colInput,
											JXScrollbarSet* scrollbarSet);

public:

	// JBroadcaster messages

	static const JUtf8Byte* kSaved;

	class Saved : public JBroadcaster::Message
		{
		public:

			Saved()
				:
				JBroadcaster::Message(kSaved)
				{ };
		};
};


/******************************************************************************
 Placement of complement file

 ******************************************************************************/

inline bool
TextDocument::WillOpenComplFileOnTop()
	const
{
	return itsOpenOverComplementFlag;
}

inline void
TextDocument::ShouldOpenComplFileOnTop
	(
	const bool doIt
	)
{
	itsOpenOverComplementFlag = doIt;
}

/******************************************************************************
 GetTextEditor

 ******************************************************************************/

inline TextEditor*
TextDocument::GetTextEditor()
	const
{
	return itsTextEditor;
}

/******************************************************************************
 GetMenuBar

 ******************************************************************************/

inline JXMenuBar*
TextDocument::GetMenuBar()
	const
{
	return itsMenuBar;
}

/******************************************************************************
 File type

 ******************************************************************************/

inline TextFileType
TextDocument::GetFileType()
	const
{
	return itsFileType;
}

/******************************************************************************
 GetStringCompleter

	Returns the StringCompleter to be used for this document, if any.

 ******************************************************************************/

inline bool
TextDocument::GetStringCompleter
	(
	StringCompleter** completer
	)
	const
{
	return GetCompleter(itsFileType, completer);
}

/******************************************************************************
 GetCharActionManager

	Returns the CharActionManager to be used for this document, if any.

 ******************************************************************************/

inline bool
TextDocument::GetCharActionManager
	(
	CharActionManager** mgr
	)
	const
{
	*mgr = itsActionMgr;
	return itsActionMgr != nullptr;
}

/******************************************************************************
 GetMacroManager

	Returns the MacroManager to be used for this document, if any.

 ******************************************************************************/

inline bool
TextDocument::GetMacroManager
	(
	MacroManager** mgr
	)
	const
{
	*mgr = itsMacroMgr;
	return itsMacroMgr != nullptr;
}

/******************************************************************************
 GetToolBar

 ******************************************************************************/

inline JXToolBar*
TextDocument::GetToolBar()
	const
{
	return itsToolBar;
}

/******************************************************************************
 GetCommandMenu (protected)

 ******************************************************************************/

inline CommandMenu*
TextDocument::GetCommandMenu()
{
	return itsCmdMenu;
}

#endif
