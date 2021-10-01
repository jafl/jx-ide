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

	virtual bool	TEHasSearchText() const override;

protected:

	virtual void	TERefresh() override;
	virtual void	TERefreshRect(const JRect& rect) override;
	virtual void	TERedraw() override;
	virtual void	TESetGUIBounds(const JCoordinate w, const JCoordinate h,
								   const JCoordinate changeY) override;
	virtual bool	TEWidthIsBeyondDisplayCapacity(const JSize width) const override;
	virtual bool	TEScrollToRect(const JRect& rect,
								   const bool centerInDisplay) override;
	virtual bool	TEScrollForDrag(const JPoint& pt) override;
	virtual bool	TEScrollForDND(const JPoint& pt) override;
	virtual void	TESetVertScrollStep(const JCoordinate vStep) override;
	virtual void	TEUpdateClipboard(const JString& text, const JRunArray<JFont>& style) const override;
	virtual bool	TEGetClipboard(JString* text, JRunArray<JFont>* style) const override;
	virtual bool	TEBeginDND() override;
	virtual void	TEPasteDropData() override;
	virtual void	TECaretShouldBlink(const bool blink) override;

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
