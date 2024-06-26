/******************************************************************************
 StringCompleter.h

	Interface for StringCompleter class.

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_StringCompleter
#define _H_StringCompleter

#include "TextFileType.h"
#include <jx-af/jcore/JPtrArray-JString.h>
#include <atomic>

class JTextEditor;
class JXIdleTask;
class JXStringCompletionMenu;
class StylerBase;

class StringCompleter : virtual public JBroadcaster
{
public:

	StringCompleter(const Language lang,
					  const JSize keywordCount, const JUtf8Byte** keywordList,
					  const JString::Case caseSensitive);

	~StringCompleter() override;

	void	Reset();
	void	Add(const JString& str);
	void	RemoveAll();

	bool	Complete(JTextEditor* te, JXStringCompletionMenu* menu);

protected:

	void			UpdateWordList();
	virtual void	UpdateWordListExtra();
	void			CopyWordsFromStyler(StylerBase* styler);
	void			CopySymbolsForLanguage(const Language lang);

	virtual bool	IsWordCharacter(const JUtf8Character& c,
									const bool includeNS) const = 0;
	virtual void	MatchCase(const JString& source, JString* target) const;
	void			Receive(JBroadcaster* sender, const Message& message) override;

private:

	const Language		itsLanguage;
	const JSize			itsPredefKeywordCount;
	const JUtf8Byte**	itsPredefKeywordList;
	const JString::Case	itsCaseSensitiveFlag;
	JPtrArray<JString>*	itsStringList;	// contents not owned
	JPtrArray<JString>*	itsOwnedList;
	StylerBase*			itsStyler;		// can be nullptr; not owned; provides extra words
	JXIdleTask*			itsNeedsUpdateTask;
	std::atomic_bool	itsUpdatingFlag;

private:

	void	Add(JString* s);
	bool	Complete(JTextEditor* te, const bool includeNS,
					 JXStringCompletionMenu* menu);
	JSize	Complete(const JString& prefix, JString* maxPrefix,
					 JXStringCompletionMenu* menu) const;

	void	WaitForUpdateThreadFinished();

	// not allowed

	StringCompleter(const StringCompleter&) = delete;
	StringCompleter& operator=(const StringCompleter&) = delete;
};

#endif
