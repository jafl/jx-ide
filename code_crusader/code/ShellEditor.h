/******************************************************************************
 ShellEditor.h

	Copyright © 2009 by John Lindal.

 ******************************************************************************/

#ifndef _H_ShellEditor
#define _H_ShellEditor

#include "TextEditor.h"

class ShellDocument;

class ShellEditor : public TextEditor
{
public:

	ShellEditor(TextDocument* document, const JString& fileName,
				 JXMenuBar* menuBar, TELineIndexInput* lineInput,
				 TEColIndexInput* colInput,
				 JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);

	virtual ~ShellEditor();

	void	InsertText(const JString& text);

	void	HandleKeyPress(const JUtf8Character& c,
								   const int keySym, const JXKeyModifiers& modifiers) override;

private:

	ShellDocument*		itsShellDoc;
	JStyledText::TextIndex	itsInsertIndex;
	JFont					itsInsertFont;

private:

	// not allowed

	ShellEditor(const ShellEditor& source);
	const ShellEditor& operator=(const ShellEditor& source);
};

#endif
