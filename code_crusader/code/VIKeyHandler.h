/******************************************************************************
 VIKeyHandler.h

	Copyright © 2010 by John Lindal.

 ******************************************************************************/

#ifndef _H_VIKeyHandler
#define _H_VIKeyHandler

#include <jx-af/jx/JXVIKeyHandler.h>

class TextEditor;

class VIKeyHandler : public JXVIKeyHandler
{
public:

	VIKeyHandler(TextEditor* te);

	~VIKeyHandler();

	bool	HandleKeyPress(const JUtf8Character& key, const bool selectText,
						   const JTextEditor::CaretMotion motion,
						   const bool deleteToTabStop) override;

private:

	TextEditor*	itsCBTE;
};

#endif
