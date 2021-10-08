/******************************************************************************
 CPreprocessor.h

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_CPreprocessor
#define _H_CPreprocessor

#include <jx-af/jcore/JUtf8Character.h>
#include <jx-af/jcore/JArray.h>

class JString;
class PPMacroList;

class CPreprocessor
{
public:

	struct MacroInfo
	{
		JString*	name;
		JString*	value;

		MacroInfo()
			:
			name(nullptr), value(nullptr)
		{ };

		MacroInfo(JString* n, JString* v)
			:
			name(n), value(v)
		{ };
	};

public:

	CPreprocessor();

	~CPreprocessor();

	bool	Preprocess(JString* text) const;
	void	PrintMacrosForCTags(std::ostream& output) const;

	void	ReadSetup(std::istream& input, const JFileVersion vers);
	void	WriteSetup(std::ostream& output) const;

	// called by MacroTable

	void	DefineMacro(const JString& name, const JString& value);
	void	UndefineAllMacros();

	const PPMacroList&	GetMacroList() const;

private:

	PPMacroList*	itsMacroList;

private:

	void	ReadMacro(std::istream& input, const JFileVersion vers);

	static JListT::CompareResult
		CompareMacros(const MacroInfo& m1, const MacroInfo& m2);

	// not allowed

	CPreprocessor(const CPreprocessor&) = delete;
	CPreprocessor& operator=(const CPreprocessor&) = delete;
};


class PPMacroList : public JArray<CPreprocessor::MacroInfo>
{
public:

	void	DeleteAll();
};


/******************************************************************************
 UndefineAllMacros

 ******************************************************************************/

inline void
CPreprocessor::UndefineAllMacros()
{
	itsMacroList->DeleteAll();
}

/******************************************************************************
 GetMacroList

 ******************************************************************************/

inline const PPMacroList&
CPreprocessor::GetMacroList()
	const
{
	return *itsMacroList;
}

#endif
