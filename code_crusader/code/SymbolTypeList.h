/******************************************************************************
 SymbolTypeList.h

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_SymbolTypeList
#define _H_SymbolTypeList

#include "SymbolList.h"
#include <jx-af/jcore/JPrefObject.h>

class JXDisplay;
class JXImage;

class SymbolTypeList : public JContainer, public JPrefObject
{
public:

	SymbolTypeList(JXDisplay* display);

	~SymbolTypeList() override;

//	bool		IsVisible(const SymbolList::Type type) const;
	Language	GetLanguage(const SymbolList::Type type) const;
	JFontStyle	GetStyle(const SymbolList::Type type,
						 const JXImage** image) const;

	static void	SkipSetup(std::istream& input, const JFileVersion vers);

protected:

	void	ReadPrefs(std::istream& input) override;
	void	WritePrefs(std::ostream& output) const override;

private:

	struct SymbolTypeInfo
	{
		SymbolList::Type	type;
		Language			lang;
		bool				visible;	// not used
		JFontStyle			style;
		JXImage*			icon;		// can be nullptr; not owned

		SymbolTypeInfo()
			:
			type(SymbolList::kUnknownST), lang(kOtherLang),
			visible(true), icon(nullptr)
		{ };

		SymbolTypeInfo(const SymbolList::Type t, const Language l,
					   const JFontStyle& s, JXImage* i)
			:
			type(t), lang(l), visible(true), style(s), icon(i)
		{ };
	};

private:

	JArray<SymbolTypeInfo>*	itsSymbolTypeList;

	JXImage*	itsCClassIcon;
	JXImage*	itsCStructIcon;
	JXImage*	itsCEnumIcon;
	JXImage*	itsCUnionIcon;
	JXImage*	itsCMacroIcon;
	JXImage*	itsCTypedefIcon;
	JXImage*	itsCNamespaceIcon;
	JXImage*	itsCSharpClassIcon;
	JXImage*	itsCSharpEventIcon;
	JXImage*	itsCSharpInterfaceIcon;
	JXImage*	itsCSharpNamespaceIcon;
	JXImage*	itsCSharpStructIcon;
	JXImage*	itsEiffelClassIcon;
	JXImage*	itsFortranCommonBlockIcon;
	JXImage*	itsJavaClassIcon;
	JXImage*	itsJavaInterfaceIcon;
	JXImage*	itsJavaPackageIcon;
	JXImage*	itsJavaScriptClassIcon;
	JXImage*	itsAssemblyLabelIcon;
	JXImage*	itsPythonClassIcon;
	JXImage*	itsPHPClassIcon;
	JXImage*	itsPHPInterfaceIcon;
	JXImage*	itsBisonNontermDefIcon;
	JXImage*	itsBisonNontermDeclIcon;
	JXImage*	itsBisonTermDeclIcon;
	JXImage*	itsPrototypeIcon;
	JXImage*	itsFunctionIcon;
	JXImage*	itsVariableIcon;
	JXImage*	itsMemberIcon;

private:

	void	CreateSymTypeList(JXDisplay* display);
	void	LoadIcons(JXDisplay* display);

	JIndex	FindType(const SymbolList::Type type) const;

	// not allowed

	SymbolTypeList(const SymbolTypeList&) = delete;
	SymbolTypeList& operator=(const SymbolTypeList&) = delete;

public:

	// JBroadcaster messages

	static const JUtf8Byte* kVisibilityChanged;
	static const JUtf8Byte* kStyleChanged;

	class VisibilityChanged : public JBroadcaster::Message
		{
		public:

			VisibilityChanged()
				:
				JBroadcaster::Message(kVisibilityChanged)
				{ };
		};

	class StyleChanged : public JBroadcaster::Message
		{
		public:

			StyleChanged()
				:
				JBroadcaster::Message(kStyleChanged)
				{ };
		};
};


/******************************************************************************
 IsVisible

 ******************************************************************************/
/*
inline bool
SymbolTypeList::IsVisible
	(
	const SymbolList::Type type
	)
	const
{
	const JIndex i = FindType(type);
	return itsSymbolTypeList->GetItem(i).visible;
}
*/
/******************************************************************************
 GetLanguage

 ******************************************************************************/

inline Language
SymbolTypeList::GetLanguage
	(
	const SymbolList::Type type
	)
	const
{
	const JIndex i = FindType(type);
	return (itsSymbolTypeList->GetItem(i)).lang;
}

/******************************************************************************
 GetStyle

	image can return nullptr.

 ******************************************************************************/

inline JFontStyle
SymbolTypeList::GetStyle
	(
	const SymbolList::Type	type,
	const JXImage**			icon
	)
	const
{
	const JIndex i            = FindType(type);
	const SymbolTypeInfo info = itsSymbolTypeList->GetItem(i);

	*icon = info.icon;
	return info.style;
}

#endif
