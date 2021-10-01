/******************************************************************************
 CharActionManager.h

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_CharActionManager
#define _H_CharActionManager

#include <jx-af/jcore/JStringPtrMap.h>

class TextDocument;

class CharActionManager
{
public:

	CharActionManager();
	CharActionManager(const CharActionManager& source);

	~CharActionManager();

	void	Perform(const JUtf8Character& c, TextDocument* doc);

	void	ReadSetup(std::istream& input);
	void	WriteSetup(std::ostream& output) const;

	// called by CharActionTable

	void	SetAction(const JUtf8Character& c, const JString& script);
	void	ClearAction(const JUtf8Character& c);
	void	ClearAllActions();

	const JStringPtrMap<JString>&	GetActionMap() const;

private:

	JStringPtrMap<JString>*	itsActionMap;

private:

	// not allowed

	CharActionManager& operator=(const CharActionManager&) = delete;
};


/******************************************************************************
 GetActionMap

 ******************************************************************************/

inline const JStringPtrMap<JString>&
CharActionManager::GetActionMap()
	const
{
	return *itsActionMap;
}

#endif
