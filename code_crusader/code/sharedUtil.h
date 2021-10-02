/******************************************************************************
 sharedUtil.h

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_sharedUtil
#define _H_sharedUtil

#include "TextFileType.h"
#include <jx-af/jcore/JPtrArray-JString.h>
#include <jx-af/jcore/jColor.h>

class JString;
class JTextEditor;
class JXTEBase;
class JXInputField;
class JXKeyModifiers;
class FnMenuUpdater;

void	ParseEditorOptions(const JString& fullName, const JString& text,
						   bool* setTabWidth, JSize* tabWidth,
						   bool* setTabMode, bool* tabInsertsSpaces,
						   bool* setAutoIndent, bool* autoIndent);
void	ScrollForDefinition(JXTEBase* te, const Language lang);
void	SelectLines(JTextEditor* te, const JIndexRange& lineRange);
void	BalanceFromSelection(JXTEBase* te, const Language lang);
bool	IsCharacterInWord(const JUtf8Character& c);

#if defined CODE_CRUSADER

	void	WriteSharedPrefs(const bool replace);

#elif defined CODE_MEDIC

	bool	ReadSharedPrefs(JString* fontName, JSize* fontSize,
							JSize* tabCharCount,
							bool* sortFnNames, bool* includeNS,
							bool* packFnNames,
							bool* openComplFileOnTop,
							const JSize colorCount, JRGB colorList[],
							JPtrArray<JString>* cSourceSuffixList,
							JPtrArray<JString>* cHeaderSuffixList,
							JPtrArray<JString>* fortranSuffixList,
							JPtrArray<JString>* javaSuffixList,
							JPtrArray<JString>* phpSuffixList,
							JPtrArray<JString>* dSuffixList,
							JPtrArray<JString>* goSuffixList);

#endif

/******************************************************************************
 Functions for balancing

 ******************************************************************************/

bool	BalanceForward(const Language lang, JStringIterator* iter, JUtf8Character* c);
bool	BalanceBackward(const Language lang, JStringIterator* iter, JUtf8Character* c);

inline bool
IsOpenGroup
	(
	const Language		lang,
	const JUtf8Character&	c
	)
{
	return c == '(' || c == '{' || c == '[';
}

inline bool
IsCloseGroup
	(
	const Language		lang,
	const JUtf8Character&	c
	)
{
	return c == ')' || c == '}' || c == ']';
}

inline bool
IsMatchingPair
	(
	const Language		lang,
	const JUtf8Character&	openChar,
	const JUtf8Character&	closeChar
	)
{
	return ((openChar == '(' && closeChar == ')') ||
			(openChar == '{' && closeChar == '}') ||
			(openChar == '[' && closeChar == ']') ||
			((lang == kHTMLLang || lang == kXMLLang) &&
			 openChar == '<' && closeChar == '>'));
}

#endif
