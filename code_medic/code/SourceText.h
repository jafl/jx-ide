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

	~SourceText() override;

	void	SetFileType(const TextFileType type);

	void	HandleKeyPress(const JUtf8Character& c, const int keySym,
						   const JXKeyModifiers& modifiers) override;

protected:

	void	BoundsResized(const JCoordinate dw, const JCoordinate dh) override;
	void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
							const JSize clickCount,
							const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers) override;
	void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
						  const JXButtonStates& buttonStates,
						  const JXKeyModifiers& modifiers) override;

	void	Receive(JBroadcaster* sender, const Message& message) override;

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

		~StyledText() override;

	protected:

		void	AdjustStylesBeforeBroadcast(const JString& text,
											JRunArray<JFont>* styles,
											TextRange* recalcRange,
											TextRange* redrawRange,
											const bool deletion) override;

	private:

		SourceText*						itsOwner;
		JArray<JSTStyler::TokenData>*	itsTokenStartList;	// nullptr if styling is turned off

	private:

		StyledText(const StyledText&) = delete;
		StyledText& operator=(const StyledText&) = delete;
	};

	friend class StyledText;

private:

	SourceDirector*		itsSrcDir;
	CommandDirector*	itsCmdDir;
	StylerBase*			itsStyler;
	JIndex				itsFirstSearchMenuItem;	// index of first item added to Search menu
	JSize				itsLastClickCount;

private:

	void	UpdateCustomSearchMenuItems();
	bool	HandleCustomSearchMenuItems(const JIndex index);
};

#endif
