/******************************************************************************
 TextEditor.h

	Copyright © 1996-2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_TextEditor
#define _H_TextEditor

#include <jx-af/jx/JXTEBase.h>
#include <jx-af/jx/JXStyledText.h>
#include <jx-af/jcore/JSTStyler.h>
#include "TextFileType.h"

class JXMenu;
class JXStringCompletionMenu;
class TextDocument;
class TELineIndexInput;
class TEColIndexInput;
class FunctionMenu;
class TEScriptMenu;

class TextEditor : public JXTEBase
{
public:

	TextEditor(TextDocument* document, const JString& fileName,
				JXMenuBar* menuBar, TELineIndexInput* lineInput,
				TEColIndexInput* colInput, const bool pasteStyledText,
				JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	~TextEditor() override;

	TextDocument*	GetDocument();

	void	OpenSelection();
	void	ScrollForDefinition(const Language lang);

	void	HandleKeyPress(const JUtf8Character& c,
								   const int keySym, const JXKeyModifiers& modifiers) override;
	void	HandleShortcut(const int key, const JXKeyModifiers& modifiers) override;

	JSize	GetTabCharCount() const;
	void	SetTabCharCount(const JSize charCount);

	void	SetFont(const JString& name, const JSize size,
					const JSize tabCharCount);
	void	SetFont(const JString& name, const JSize size,
					const JSize tabCharCount, const bool breakCROnly);
	void	SetWritable(const bool writable);

	JCoordinate	CalcTabWidth(const JFont& font, const JSize tabCharCount) const;

	bool	WillBalanceWhileTyping() const;
	void	ShouldBalanceWhileTyping(const bool balance);

	bool	WillScrollToBalance() const;
	void	ShouldScrollToBalance(const bool scroll);

	bool	WillBeepWhenTypeUnbalanced() const;
	void	ShouldBeepWhenTypeUnbalanced(const bool beep);

	bool	TabIsSmart() const;
	void	TabShouldBeSmart(const bool smart);

	bool	OverrideAllowsDragAndDrop() const;
	void	OverrideShouldAllowDragAndDrop(const bool allow);

	bool	GetRightMarginWidth(JSize* width) const;
	void	SetRightMarginWidth(const bool show, const JSize width);
	void	SetRightMarginColor(const JColorID color);

	const JString&	GetScriptPath() const;
	void			SetScriptPath(const JString& path);

	void	RecalcStyles();

	// preferences

	void	ReadSetup(std::istream& input);
	void	WriteSetup(std::ostream& output) const;

	// called by TextDocument

	void	FileTypeChanged(const TextFileType type);
	void	UpdateWritable(const JString& name);

	// called by PSPrinter

	void	SetFontBeforePrintPS(const JSize fontSize);
	void	ResetFontAfterPrintPS();

	// called by CB*KeyHandler

	TELineIndexInput*	GetLineInput();

protected:

	void	Draw(JXWindowPainter& p, const JRect& rect) override;

	void	AdjustCursor(const JPoint& pt, const JXKeyModifiers& modifiers) override;
	void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
								  const JXButtonStates& buttonStates,
								  const JXKeyModifiers& modifiers) override;

	JCoordinate	GetPrintHeaderHeight(JPagePrinter& p) const override;
	void		DrawPrintHeader(JPagePrinter& p, const JCoordinate footerHeight) override;

	void	Receive(JBroadcaster* sender, const Message& message) override;

protected:

	class StyledText : public JStyledText
	{
	public:

		StyledText(TextDocument* doc, const bool pasteStyledText);

		~StyledText() override;

	protected:

		void	AdjustStylesBeforeBroadcast(
							const JString& text, JRunArray<JFont>* styles,
							JStyledText::TextRange* recalcRange,
							JStyledText::TextRange* redrawRange,
							const bool deletion) override;

	private:

		TextDocument*					itsDoc;
		JArray<JSTStyler::TokenData>*	itsTokenStartList;	// nullptr if styling is turned off

	private:

		StyledText(const StyledText&) = delete;
		StyledText& operator=(const StyledText&) = delete;
	};

private:

	TextDocument*			itsDoc;					// this owns us
	JSize					itsTabCharCount;
	JSize					itsLastClickCount;
	JXKeyModifiers			itsLastModifiers;

	JIndex					itsExecScriptCmdIndex;	// index of "Run script" on Edit menu
	JIndex					itsSearchMenuItemOffset;// offset of first item added to Search menu
	FunctionMenu*			itsFnMenu;				// nullptr if not source
	JXStringCompletionMenu*	itsCompletionMenu;		// nullptr if doesn't have completer
	JXTextMenu*				itsContextMenu;			// nullptr until first used
	JString					itsScriptPath;			// can be empty

	// owned by their enclosures

	TELineIndexInput*	itsLineInput;
	TEColIndexInput*	itsColInput;

	// balance while typing

	bool	itsBalanceWhileTypingFlag;
	bool	itsScrollToBalanceFlag;
	bool	itsBeepWhenTypeUnbalancedFlag;

	// right margin

	bool	itsDrawRightMarginFlag;
	JSize		itsRightMarginWidth;
	JColorID	itsRightMarginColor;		// saved by PrefsManager

	// other options

	bool	itsAllowDNDFlag;			// buffered since Meta turns it on
	bool	itsSmartTabFlag;
	bool	itsSavedBreakCROnlyFlag;	// used during printing

private:

	void	UpdateTabHandling();

	void		UpdateCustomEditMenuItems();
	bool	HandleCustomEditMenuItems(const JIndex index);

	void		UpdateCustomSearchMenuItems();
	bool	HandleCustomSearchMenuItems(const JIndex index);

	void	PlaceBookmark();

	void	FindSelectedSymbol(const JXMouseButton button, const bool useContext);
	void	DisplayManPage();

	JIndex	GetLineIndex(const JStyledText::TextIndex& startIndex) const;

	void	PrivateSetTabCharCount(const JSize charCount);

	bool	IsNonstdError(JString* fileName,
							  JStyledText::TextRange* fileNameRange,
							  JIndex* lineIndex) const;

	void	ShowBalancingOpenGroup();

	void			CreateContextMenu();
	TEScriptMenu*	CreateScriptMenu(JXMenu* parent, const JIndex index);

	void	UpdateContextMenu();
	void	HandleContextMenu(const JIndex index);

	void	SelectWordIfNoSelection();
};


/******************************************************************************
 GetDocument

 ******************************************************************************/

inline TextDocument*
TextEditor::GetDocument()
{
	return itsDoc;
}

/******************************************************************************
 Tab width (# of characters)

 ******************************************************************************/

inline void
TextEditor::PrivateSetTabCharCount
	(
	const JSize charCount
	)
{
	itsTabCharCount = charCount;
	GetText()->SetCRMTabCharCount(itsTabCharCount);
}

inline JSize
TextEditor::GetTabCharCount()
	const
{
	return itsTabCharCount;
}

/******************************************************************************
 CalcTabWidth

 ******************************************************************************/

inline JCoordinate
TextEditor::CalcTabWidth
	(
	const JFont&	font,
	const JSize		tabCharCount
	)
	const
{
	return tabCharCount * font.GetCharWidth(GetFontManager(), JUtf8Character(' '));
}

/******************************************************************************
 SetFont

 ******************************************************************************/

inline void
TextEditor::SetFont
	(
	const JString&	name,
	const JSize		size,
	const JSize		tabCharCount
	)
{
	SetFont(name, size, tabCharCount, WillBreakCROnly());
}

/******************************************************************************
 Balance while typing

 ******************************************************************************/

inline bool
TextEditor::WillBalanceWhileTyping()
	const
{
	return itsBalanceWhileTypingFlag;
}

inline void
TextEditor::ShouldBalanceWhileTyping
	(
	const bool balance
	)
{
	itsBalanceWhileTypingFlag = balance;
}

inline bool
TextEditor::WillScrollToBalance()
	const
{
	return itsScrollToBalanceFlag;
}

inline void
TextEditor::ShouldScrollToBalance
	(
	const bool scroll
	)
{
	itsScrollToBalanceFlag = scroll;
}

inline bool
TextEditor::WillBeepWhenTypeUnbalanced()
	const
{
	return itsBeepWhenTypeUnbalancedFlag;
}

inline void
TextEditor::ShouldBeepWhenTypeUnbalanced
	(
	const bool beep
	)
{
	itsBeepWhenTypeUnbalancedFlag = beep;
}

/******************************************************************************
 Smart tab

 ******************************************************************************/

inline bool
TextEditor::TabIsSmart()
	const
{
	return itsSmartTabFlag;
}

inline void
TextEditor::TabShouldBeSmart
	(
	const bool smart
	)
{
	itsSmartTabFlag = smart;
}

/******************************************************************************
 Allow DND

	We buffer the value since Meta toggles it.

 ******************************************************************************/

inline bool
TextEditor::OverrideAllowsDragAndDrop()
	const
{
	return itsAllowDNDFlag;
}

inline void
TextEditor::OverrideShouldAllowDragAndDrop
	(
	const bool allow
	)
{
	itsAllowDNDFlag = allow;
	ShouldAllowDragAndDrop(allow);
}

/******************************************************************************
 Right margin width

 ******************************************************************************/

inline bool
TextEditor::GetRightMarginWidth
	(
	JSize* width
	)
	const
{
	*width = itsRightMarginWidth;
	return itsDrawRightMarginFlag;
}

inline void
TextEditor::SetRightMarginWidth
	(
	const bool	show,
	const JSize		width
	)
{
	itsRightMarginWidth = width;

	if (show != itsDrawRightMarginFlag)
		{
		itsDrawRightMarginFlag = show;
		Refresh();
		}
}

inline void
TextEditor::SetRightMarginColor
	(
	const JColorID color
	)
{
	if (color != itsRightMarginColor)
		{
		itsRightMarginColor = color;
		SetWhitespaceColor(color);
		if (itsDrawRightMarginFlag)
			{
			Refresh();
			}
		}
}

/******************************************************************************
 Script path

 ******************************************************************************/

inline const JString&
TextEditor::GetScriptPath()
	const
{
	return itsScriptPath;
}

inline void
TextEditor::SetScriptPath
	(
	const JString& path
	)
{
	itsScriptPath = path;
}

/******************************************************************************
 GetLineInput

 ******************************************************************************/

inline TELineIndexInput*
TextEditor::GetLineInput()
{
	return itsLineInput;
}

#endif
