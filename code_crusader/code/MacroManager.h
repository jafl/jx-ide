/******************************************************************************
 MacroManager.h

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_MacroManager
#define _H_MacroManager

#include <jx-af/jcore/JStyledText.h>

class JString;
class MacroList;
class TextDocument;

class MacroManager
{
public:

	struct MacroInfo
	{
		JString*	macro;
		JString*	script;

		MacroInfo()
			:
			macro(nullptr), script(nullptr)
		{ };

		MacroInfo(JString* m, JString* s)
			:
			macro(m), script(s)
		{ };
	};

public:

	MacroManager();
	MacroManager(const MacroManager& source);

	~MacroManager();

	bool	Perform(const JStyledText::TextIndex& caretIndex, TextDocument* doc);

	void	ReadSetup(std::istream& input);
	void	WriteSetup(std::ostream& output) const;

	// called by CharActionManager and tables

	static void	Perform(const JString& script, TextDocument* doc);
	static void	HighlightErrors(const JString& script, JRunArray<JFont>* styles);

	// called by MacroTable

	void	AddMacro(const JUtf8Byte* macro, const JUtf8Byte* script);
	void	RemoveAllMacros();

	const MacroList&	GetMacroList() const;

private:

	MacroList*	itsMacroList;

private:

	void	MacroManagerX();

	static JListT::CompareResult
		CompareMacros(const MacroInfo& m1, const MacroInfo& m2);

	// not allowed

	MacroManager& operator=(const MacroManager&) = delete;
};


class MacroList : public JArray<MacroManager::MacroInfo>
{
public:

	void	DeleteAll();
};


/******************************************************************************
 RemoveAllMacros

 ******************************************************************************/

inline void
MacroManager::RemoveAllMacros()
{
	itsMacroList->DeleteAll();
}

/******************************************************************************
 GetMacroList

 ******************************************************************************/

inline const MacroList&
MacroManager::GetMacroList()
	const
{
	return *itsMacroList;
}

#endif
