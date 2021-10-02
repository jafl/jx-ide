/******************************************************************************
 TextDisplayBase.h

	Interface for the TextDisplayBase class

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_TextDisplayBase
#define _H_TextDisplayBase

#include <jx-af/jx/JXTEBase.h>

class JXStyledText;

class TextDisplayBase : public JXTEBase
{
public:

	TextDisplayBase(const Type type, const bool breakCROnly,
					  JXMenuBar* menuBar,
					  JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	TextDisplayBase(JXStyledText* text,
					  const Type type, const bool breakCROnly,
					  JXMenuBar* menuBar,
					  JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	virtual ~TextDisplayBase();

	static void	AdjustFont(JXTEBase* te);

private:

	void	TextDisplayBaseX(JXMenuBar* menuBar, JXScrollbarSet* scrollbarSet);
};

#endif
