/******************************************************************************
 CStyler.h

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_CStyler
#define _H_CStyler

#include "StylerBase.h"
#include "CScanner.h"

class CStyler : public StylerBase, public TextScanner::C::Scanner
{
public:

	static StylerBase*	Instance();
	static void			Shutdown();

	~CStyler();

protected:

	CStyler();

	void	Scan(const JStyledText::TextIndex& startIndex,
				 std::istream& input, const TokenExtra& initData) override;
	void	PreexpandCheckRange(const JString& text,
								const JRunArray<JFont>& styles,
								const JCharacterRange& modifiedRange,
								const bool deletion,
								JStyledText::TextRange* checkRange) override;

	void	UpgradeTypeList(const JFileVersion vers,
							JArray<JFontStyle>* typeStyles) override;

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	static CStyler*	itsSelf;

private:

	bool	SlurpPPComment(JStyledText::TextRange* totalRange);
};

#endif
