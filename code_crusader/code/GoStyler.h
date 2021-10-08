/******************************************************************************
 GoStyler.h

	Copyright © 2021 by John Lindal.

 ******************************************************************************/

#ifndef _H_GoStyler
#define _H_GoStyler

#include "StylerBase.h"
#include "GoScanner.h"

class GoStyler : public StylerBase, public TextScanner::Go::Scanner
{
public:

	static StylerBase*	Instance();
	static void			Shutdown();

	~GoStyler() override;

protected:

	GoStyler();

	void	Scan(const JStyledText::TextIndex& startIndex,
				 std::istream& input, const TokenExtra& initData) override;
	void	UpgradeTypeList(const JFileVersion vers,
							JArray<JFontStyle>* typeStyles) override;

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	static GoStyler*	itsSelf;
};

#endif
