/******************************************************************************
 PropertiesStyler.h

	Copyright © 2015 by John Lindal.

 ******************************************************************************/

#ifndef _H_PropertiesStyler
#define _H_PropertiesStyler

#include "StylerBase.h"
#include "PropertiesScanner.h"

class PropertiesStyler : public StylerBase, public TextScanner::Properties::Scanner
{
public:

	static StylerBase*	Instance();
	static void				Shutdown();

	virtual ~PropertiesStyler();

protected:

	PropertiesStyler();

	virtual void	Scan(const JStyledText::TextIndex& startIndex,
						 std::istream& input, const TokenExtra& initData) override;
	virtual void	UpgradeTypeList(const JFileVersion vers,
									JArray<JFontStyle>* typeStyles) override;

private:

	static PropertiesStyler*	itsSelf;
};

#endif
