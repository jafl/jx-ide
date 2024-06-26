/******************************************************************************
 SearchST.h

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_SearchST
#define _H_SearchST

#include <jx-af/jcore/JStyledText.h>

class SearchDocument;

class SearchST : public JStyledText
{
public:

	SearchST();

	~SearchST() override;

	void	SearchFiles(const JPtrArray<JString>& fileList,
						const JPtrArray<JString>& nameList,
						const bool onlyListFiles,
						const bool listFilesWithoutMatch,
						SearchDocument* doc);

	bool	ReplaceAllForward();

	void	Cancel();

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

	SearchST(const SearchST&) = delete;

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

		SearchResult(const SearchResult&) = delete;
		SearchResult& operator=(const SearchResult&) = delete;
		SearchResult(SearchResult &&) = delete;
		SearchResult const & operator=(SearchResult &&) = delete;

		~SearchResult() override
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

		FileName(const FileName&) = delete;
		FileName& operator=(const FileName&) = delete;
		FileName(FileName &&) = delete;
		FileName const & operator=(FileName &&) = delete;

		~FileName() override
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

		Error(const Error&) = delete;
		Error& operator=(const Error&) = delete;
		Error(Error &&) = delete;
		Error const & operator=(Error &&) = delete;

		~Error() override
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
SearchST::Cancel()
{
	itsCancelledFlag = true;
}

#endif
