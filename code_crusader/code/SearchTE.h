/******************************************************************************
 SearchTE.h

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_SearchTE
#define _H_SearchTE

#include <jx-af/jcore/JTextEditor.h>

class SearchDocument;

class SearchTE : public JTextEditor
{
public:

	SearchTE();

	~SearchTE() override;

	void	SearchFiles(const JPtrArray<JString>& fileList,
						const JPtrArray<JString>& nameList,
						const bool onlyListFiles,
						const bool listFilesWithoutMatch,
						SearchDocument* doc);

	bool	ReplaceAllForward();

	bool	TEHasSearchText() const override;

	void	Cancel();

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

	bool	itsCancelledFlag;

private:

	void	SearchFile(const JString& fileName, const JString& printName,
					   const bool onlyListFiles, const bool listFilesWithoutMatch,
					   const JRegex& searchRegex, const bool entireWord,
					   SearchDocument* doc);

	void	QueueErrorMessage(SearchDocument* doc, const JUtf8Byte* key,
							  const JUtf8Byte* map[], const JSize size);
	void	QueueFileNameMessage(SearchDocument* doc, const JString& name);

	bool	IsKnownBinaryFile(const JString& fileName) const;

	// not allowed

	SearchTE(const SearchTE&) = delete;

public:

	// JBroadcaster messages

	static const JUtf8Byte* kIncrementProgress;
	static const JUtf8Byte* kSearchResult;
	static const JUtf8Byte* kAdditionalMatch;
	static const JUtf8Byte* kFileName;
	static const JUtf8Byte* kError;

	class IncrementProgress : public JBroadcaster::Message
	{
	public:

		IncrementProgress()
			:
			JBroadcaster::Message(kIncrementProgress)
			{ };
	};

	class SearchResult : public JBroadcaster::Message
	{
	public:

		SearchResult(JString* n, const JIndex i, JString* t, const JStyledText::TextRange& r)
			:
			JBroadcaster::Message(kSearchResult),
			itsFileName(n),
			itsLineIndex(i),
			itsQuotedText(t),
			itsRange(r)
		{ };

		~SearchResult()
		{
			jdelete itsFileName;
			jdelete itsQuotedText;
		};

		const JString&
		GetFileName()
			const
		{
			return *itsFileName;
		};

		JIndex
		GetLineIndex()
			const
		{
			return itsLineIndex;
		};

		const JString&
		GetQuotedText()
			const
		{
			return *itsQuotedText;
		};

		JStyledText::TextRange
		GetRange()
			const
		{
			return itsRange;
		};

	private:

		JString*						itsFileName;
		const JIndex					itsLineIndex;
		JString*						itsQuotedText;
		const JStyledText::TextRange	itsRange;
	};

	class AdditionalMatch : public JBroadcaster::Message
	{
	public:

		AdditionalMatch(const JStyledText::TextRange& r)
			:
			JBroadcaster::Message(kAdditionalMatch),
			itsRange(r)
		{ };

		JStyledText::TextRange
		GetRange()
			const
		{
			return itsRange;
		};

	private:

		const JStyledText::TextRange	itsRange;
	};

	class FileName : public JBroadcaster::Message
	{
	public:

		FileName(JString* n)
			:
			JBroadcaster::Message(kFileName),
			itsFileName(n)
		{ };

		~FileName()
		{
			jdelete itsFileName;
		};

		const JString&
		GetFileName()
			const
		{
			return *itsFileName;
		};

	private:

		JString*	itsFileName;
	};

	class Error : public JBroadcaster::Message
	{
	public:

		Error(JString* m)
			:
			JBroadcaster::Message(kError),
			itsMessage(m)
		{ };

		~Error()
		{
			jdelete itsMessage;
		};

		const JString&
		GetMessage()
			const
		{
			return *itsMessage;
		};

	private:

		JString*	itsMessage;
	};
};


/******************************************************************************
 Cancel

 ******************************************************************************/

inline void
SearchTE::Cancel()
{
	itsCancelledFlag = true;
}

#endif
