/******************************************************************************
 SourceText.h

	Interface for the SourceText class

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_SourceText
#define _H_SourceText

#include "TextDisplayBase.h"
#include "TextFileType.h"
#include <jx-af/jx/JXStyledText.h>
#include <jx-af/jcore/JSTStyler.h>

class SourceDirector;
class CommandDirector;
class StylerBase;

class SourceText : public TextDisplayBase
{
public:

	SourceText(SourceDirector* srcDir, CommandDirector* cmdDir,
					JXMenuBar* menuBar,
					JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	virtual ~SourceText();

	void	SetFileType(const TextFileType type);

	virtual void	HandleKeyPress(const JUtf8Character& c, const int keySym,
								   const JXKeyModifiers& modifiers) override;

protected:

	virtual void	BoundsResized(const JCoordinate dw, const JCoordinate dh) override;
	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	virtual void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
								  const JXButtonStates& buttonStates,
								  const JXKeyModifiers& modifiers) override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

protected:

	class StyledText : public JXStyledText
	{
		public:

		StyledText(SourceText* owner, JFontManager* fontManager)
			:
			JXStyledText(false, false, fontManager),
			itsOwner(owner),
			itsTokenStartList(JSTStyler::NewTokenStartList())
		{ };

		virtual ~StyledText();

		protected:

		virtual void	AdjustStylesBeforeBroadcast(const JString& text,
													JRunArray<JFont>* styles,
													TextRange* recalcRange,
													TextRange* redrawRange,
													const bool deletion);

		private:

		SourceText*						itsOwner;
		JArray<JSTStyler::TokenData>*	itsTokenStartList;	// nullptr if styling is turned off
	};

	friend class StyledText;

private:

	SourceDirector*	itsSrcDir;
	CommandDirector*	itsCmdDir;
	StylerBase*		itsStyler;
	JIndex				itsFirstSearchMenuItem;	// index of first item added to Search menu
	JSize				itsLastClickCount;

private:

	void	UpdateCustomSearchMenuItems();
	bool	HandleCustomSearchMenuItems(const JIndex index);
};

#endif
