/******************************************************************************
 SearchFontManager.h

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_SearchFontManager
#define _H_SearchFontManager

#include <jx-af/jcore/JFontManager.h>

class SearchFontManager : public JFontManager
{
public:

	SearchFontManager();

	virtual ~SearchFontManager();

	void		GetFontNames(JPtrArray<JString>* fontNames) override;
	void		GetMonospaceFontNames(JPtrArray<JString>* fontNames) override;
	bool	GetFontSizes(const JString& name, JSize* minSize,
									 JSize* maxSize, JArray<JSize>* sizeList) override;

protected:

	JSize	GetLineHeight(const JFontID fontID, const JSize size,
								  const JFontStyle& style,
								  JCoordinate* ascent, JCoordinate* descent) override;

	JSize	GetCharWidth(const JFontID fontID, const JUtf8Character& c) override;
	JSize	GetStringWidth(const JFontID fontID, const JString& str) override;

	bool	IsExact(const JFontID id) override;
	bool	HasGlyphForCharacter(const JFontID id, const JUtf8Character& c) override;
	bool	GetSubstituteFontName(const JFont& f, const JUtf8Character& c, JString* name) override;

private:

	// not allowed

	SearchFontManager(const SearchFontManager& source);
	const SearchFontManager& operator=(const SearchFontManager& source);
};

#endif
