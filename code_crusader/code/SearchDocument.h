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

	virtual void	OpenPrevListItem() override;
	virtual void	OpenNextListItem() override;

	virtual void	ConvertSelectionToFullPath(JString* fileName) const override;

protected:

	SearchDocument(const bool isReplace, const bool onlyListFiles,
					 const JSize fileCount,
					 JProcess* p, const int fd,
					 const JString& windowTitle);

	virtual void		PlaceCmdLineWidgets() override;
	virtual void		AppendText(const JString& text) override;
	virtual bool	ProcessFinished(const JProcess::Finished& info) override;
	virtual bool	NeedsFormattedData() const override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	const bool			itsIsReplaceFlag;
	const bool			itsOnlyListFilesFlag;
	bool				itsFoundFlag;			// true => something matched
	JStyledText::TextIndex	itsPrevQuoteIndex;		// start of previous quote from file

	JXTextMenu*				itsMatchMenu;
	JXProgressIndicator*	itsIndicator;	// nullptr after ProcessFinished()
	SearchTE*				itsReplaceTE;	// nullptr unless replacing

private:

	void	ReplaceAll(const JString& fileName);

	void	UpdateMatchMenu();
	void	HandleMatchMenu(const JIndex index);

	void		ShowFirstMatch();
	bool	ShowPrevMatch();
	bool	ShowNextMatch();

	JFontStyle	GetFileNameStyle() const;
	JFontStyle	GetMatchStyle() const;
	JFontStyle	GetErrorStyle() const;

	// not allowed

	SearchDocument(const SearchDocument& source);
	const SearchDocument& operator=(const SearchDocument& source);
};

#endif