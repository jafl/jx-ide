/******************************************************************************
 SearchDocument.h

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_SearchDocument
#define _H_SearchDocument

#include "CommandOutputDocument.h"
#include "SearchST.h"		// need messages
#include <boost/fiber/buffered_channel.hpp>

class JXTextMenu;
class JXProgressIndicator;

class SearchDocument : public CommandOutputDocument
{
public:

	static void	Create(JPtrArray<JString>* fileList,
					   JPtrArray<JString>* nameList,
					   const bool onlyListFiles,
					   const bool listFilesWithoutMatch);

	static void	Create(JPtrArray<JString>* fileList,
					   JPtrArray<JString>* nameList,
					   const JString& replaceStr);

	~SearchDocument() override;

	bool	CommandRunning() const override;

	void	OpenPrevListItem() override;
	void	OpenNextListItem() override;

	void	ConvertSelectionToFullPath(JString* fileName) const override;

	// for SearchST

	void	SetSearchST(SearchST* st);
	void	QueueMessage(JBroadcaster::Message* message);
	void	SearchFinished();

protected:

	SearchDocument(const bool isReplace, const bool onlyListFiles,
				   const JSize fileCount, const JString& windowTitle);

	void	PlaceCustomWidgets() override;
	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	const bool				itsIsReplaceFlag;
	const bool				itsOnlyListFilesFlag;
	bool					itsFoundFlag;			// true => something matched
	JStyledText::TextIndex	itsPrevQuoteIndex;		// start of previous quote from file

	JXTextMenu*				itsMatchMenu;
	JXProgressIndicator*	itsIndicator;	// nullptr after ProcessFinished()
	SearchST*				itsReplaceST;	// nullptr unless replacing

	boost::fibers::buffered_channel<JBroadcaster::Message*>*	itsChannel;

	SearchST*		itsSearchST;
	JXTextButton*	itsStopButton;

private:

	void	RecvFromChannel();
	void	AppendSearchResult(const SearchST::SearchResult& msg);
	void	MarkAdditionalMatch(const JStyledText::TextRange& range);
	void	AppendFileName(const JString& text);
	void	AppendError(const JString& text);
	void	AppendText(const JString& text);

	void	ReplaceAll(const JString& fileName);

	void	UpdateButtons();

	void	UpdateMatchMenu();
	void	HandleMatchMenu(const JIndex index);

	void	ShowFirstMatch();
	bool	ShowPrevMatch();
	bool	ShowNextMatch();

	JFontStyle	GetFileNameStyle() const;
	JFontStyle	GetMatchStyle() const;
	JFontStyle	GetErrorStyle() const;
};


/******************************************************************************
 CommandRunning (virtual)

 ******************************************************************************/

inline bool
SearchDocument::CommandRunning()
	const
{
	return itsChannel != nullptr;
}

/******************************************************************************
 SetSearchST

 ******************************************************************************/

inline void
SearchDocument::SetSearchST
	(
	SearchST* st
	)
{
	itsSearchST = st;
}

#endif
