/******************************************************************************
 SearchTE.h

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_SearchTE
#define _H_SearchTE

#include <jx-af/jcore/JTextEditor.h>
#include "SearchDocument.h"	// need definition of RecordLink

class SearchTE : public JTextEditor
{
public:

	static const JUtf8Byte kNewMatchLine;
	static const JUtf8Byte kRepeatMatchLine;
	static const JUtf8Byte kIncrementProgress;
	static const JUtf8Byte kError;

public:

	SearchTE();

	virtual ~SearchTE();

	void	SearchFiles(const JPtrArray<JString>& fileList,
						const JPtrArray<JString>& nameList,
						const bool onlyListFiles,
						const bool listFilesWithoutMatch,
						std::ostream& output);

	bool	ReplaceAllForward();

	static void	SetProtocol(SearchDocument::RecordLink* link);

	bool	TEHasSearchText() const override;

protected:

	void	TERefresh() override;
	void	TERefreshRect(const JRect& rect) override;
	void	TERedraw() override;
	void	TESetGUIBounds(const JCoordinate w, const JCoordinate h,
								   const JCoordinate changeY) override;
	bool	TEWidthIsBeyondDisplayCapacity(const JSize width) const override;
	bool	TEScrollToRect(const JRect& rect,
								   const bool centerInDisplay) override;
	bool	TEScrollForDrag(const JPoint& pt) override;
	bool	TEScrollForDND(const JPoint& pt) override;
	void	TESetVertScrollStep(const JCoordinate vStep) override;
	void	TEUpdateClipboard(const JString& text, const JRunArray<JFont>& style) const override;
	bool	TEGetClipboard(JString* text, JRunArray<JFont>* style) const override;
	bool	TEBeginDND() override;
	void	TEPasteDropData() override;
	void	TECaretShouldBlink(const bool blink) override;

private:

	void	SearchFile(const JString& fileName, const JString& printName,
					   const bool onlyListFiles, const bool listFilesWithoutMatch,
					   std::ostream& output,
					   const JRegex& searchRegex, const bool	entireWord);

	bool	IsKnownBinaryFile(const JString& fileName) const;

	// not allowed

	SearchTE(const SearchTE&) = delete;
};

#endif
