/******************************************************************************
 SearchDocument.h

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_SearchDocument
#define _H_SearchDocument

#include "ExecOutputDocument.h"

class JXTextMenu;
class JXProgressIndicator;
class SearchTE;

class SearchDocument : public ExecOutputDocument
{
public:

	static JError	Create(const JPtrArray<JString>& fileList,
						   const JPtrArray<JString>& nameList,
						   const JRegex& searchRegex,
						   const bool onlyListFiles,
						   const bool listFilesWithoutMatch);

	static JError	Create(const JPtrArray<JString>& fileList,
						   const JPtrArray<JString>& nameList,
						   const JRegex& searchRegex,
						   const JString& replaceStr);

	virtual ~SearchDocument();

	void	OpenPrevListItem() override;
	void	OpenNextListItem() override;

	void	ConvertSelectionToFullPath(JString* fileName) const override;

protected:

	SearchDocument(const bool isReplace, const bool onlyListFiles,
					 const JSize fileCount,
					 JProcess* p, const int fd,
					 const JString& windowTitle);

	void	PlaceCmdLineWidgets() override;
	void	AppendText(const JString& text) override;
	bool	ProcessFinished(const JProcess::Finished& info) override;
	bool	NeedsFormattedData() const override;

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	const bool				itsIsReplaceFlag;
	const bool				itsOnlyListFilesFlag;
	bool					itsFoundFlag;			// true => something matched
	JStyledText::TextIndex	itsPrevQuoteIndex;		// start of previous quote from file

	JXTextMenu*				itsMatchMenu;
	JXProgressIndicator*	itsIndicator;	// nullptr after ProcessFinished()
	SearchTE*				itsReplaceTE;	// nullptr unless replacing

private:

	void	ReplaceAll(const JString& fileName);

	void	UpdateMatchMenu();
	void	HandleMatchMenu(const JIndex index);

	void	ShowFirstMatch();
	bool	ShowPrevMatch();
	bool	ShowNextMatch();

	JFontStyle	GetFileNameStyle() const;
	JFontStyle	GetMatchStyle() const;
	JFontStyle	GetErrorStyle() const;
};

#endif
