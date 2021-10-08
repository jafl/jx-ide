/******************************************************************************
 PrefsManager.h

	Copyright © 1997-2001 John Lindal.

 ******************************************************************************/

#ifndef _H_PrefsManager
#define _H_PrefsManager

#include <jx-af/jx/JXPrefsManager.h>
#include "TextFileType.h"
#include "Emulator.h"
#include <jx-af/jcore/JStyledText.h>		// need definition of CRMRule

class JRegex;
class JPoint;

class JXWindow;
class JXChooseSaveFile;

class TextDocument;
class TextEditor;
class CharActionManager;
class MacroManager;

class EditFileTypesDialog;
class EditMacroDialog;
class EditCRMDialog;

const JUtf8Byte kContentRegexMarker = '^';

// Preferences -- do not change ID's once they are assigned

enum
{
	kHTMLStyleID = 1,
	kMDIServerID,
	kViewManPagePrefID,
	kDocMgrID,
	kTreeWindSizeID,
	kSourceWindSizeID,
	kSearchTextID,
	kMacroSetListID,
	kHeaderWindSizeID,
	kOtherTextWindSizeID,
	kTextDocID,
	kgCSFSetupID,
	kExecOutputWindSizeID,
	kRunCommandDialogID,
	kDockPrefID,
	kPrintPlainTextID,
	kFindFilePrefID,
	kSymbolDirectorID,
	kTCLStyleID,
	kBourneShellStyleID,
	kDefFontID,
	kProgramVersionID,
	kAppID,
	kDocMgrStateID,
	kTreeSetupID,
	kEditSearchPathsDialogID,
	kUnused1ID,				// unused
	kFileListWindSizeID,
	kCRMRuleSetListID,
	kFileTypeListID,
	kMiscWordWrapID,
	kEditMacroDialogID,
	kEditFileTypesDialogID,
	kRunTEScriptDialogID,
	kEditCRMDialogID,
	kTextColorID,
	kStaticGlobalID,
	kEditCPPMacroDialogID,
	kPrintStyledTextID,
	kProjectWindSizeID,
	kJavaStyleID,
	kTEToolBarID,
	kProjectToolBarID,
	kCTreeToolBarID,
	kFnMenuUpdaterID,
	kDiffFileDialogID,
	kSymbolWindSizeID,
	kSymbolToolBarID,
	kJavaTreeToolBarID,
	kNewProjectCSFID,
	kCStyleID,
	kFileListToolBarID,
	kSearchOutputWindSizeID,
	kCShellStyleID,
	kGlobalCommandsID,
	kEditCommandsDialogID,
	kPerlStyleID,
	kExpireTimeStampID,
	kCSharpStyleID,
	kPythonStyleID,
	kEiffelStyleID,
	kJavaScriptStyleID,
	kSymbolTypeListID,
	kVersionCheckID,
	kEmulatorID,
	kRubyStyleID,
	kINIStyleID,
	kPHPTreeToolBarID,
	kPropertiesStyleID,
	kSQLStyleID,
	kGoStyleID,
	kGoTreeToolBarID,
	kDStyleID,
	kDTreeToolBarID,

	// 10000-19999 are reserved for action/macro

	kEmptyMacroID = 10000,			// reserved and empty
	kFirstMacroID = 10001,
	kLastMacroID  = 19999,

	// 20000-29999 are reserved for CRM rule lists

	kEmptyCRMRuleListID = 20000,		// reserved and empty
	kFirstCRMRuleListID = 20001,
	kLastCRMRuleListID  = 29999
};

class PrefsManager : public JXPrefsManager
{
public:

	// remember to increment shared prefs file version

	enum
	{
		kTextColorIndex = 1,
		kBackColorIndex,
		kCaretColorIndex,
		kSelColorIndex,
		kSelLineColorIndex,
		kRightMarginColorIndex,		// = kColorCount

		kColorCount = kRightMarginColorIndex
	};

public:

	PrefsManager(bool* isNew);

	~PrefsManager();

	JString	GetJCCVersionStr() const;

	bool	GetExpirationTimeStamp(time_t* t) const;
	void	SetExpirationTimeStamp(const time_t t);

	void	EditFileTypes();
	void	EditMacros(MacroManager* macroMgr);
	void	EditCRMRuleLists(JStyledText::CRMRuleList* ruleList);

	bool	GetWindowSize(const JPrefID& id, JPoint* desktopLoc,
						  JCoordinate* width, JCoordinate* height) const;
	void	SaveWindowSize(const JPrefID& id, JXWindow* window);

	bool	RestoreProgramState();
	void	SaveProgramState();
	void	ForgetProgramState();

	// text editor

	void	GetDefaultFont(JString* name, JSize* size) const;
	void	SetDefaultFont(const JString& name, const JSize size);

	JColorID	GetColor(const JIndex index) const;
	void		SetColor(const JIndex index, const JColorID color);
	static bool	ColorIndexValid(const JIndex index);

	Emulator	GetEmulator() const;
	void		SetEmulator(const Emulator type);

	// file types

	TextFileType	GetFileType(const JString& fileName) const;
	TextFileType	GetFileType(const TextDocument& doc,
								CharActionManager** actionMgr, MacroManager** macroMgr,
								JStyledText::CRMRuleList** crmRuleList,
								JString* scriptPath, bool* wordWrap) const;

	bool	EditWithOtherProgram(const JString& fileName, JString* cmd) const;

	void	GetFileSuffixes(const TextFileType type,
							JPtrArray<JString>* list) const;
	void	GetAllFileSuffixes(JPtrArray<JString>* list) const;

	JString	GetDefaultComplementSuffix(const JString& name, const TextFileType type,
									   JIndex* index) const;
	void	SetDefaultComplementSuffix(const JIndex index, const JString& name);

	static bool	FileMatchesSuffix(const JString& fileName,
								  const JPtrArray<JString>& suffixList);

	static bool	GetScriptPaths(JString* sysDir, JString* userDir);

protected:

	void	UpgradeData(const bool isNew, const JFileVersion currentVersion) override;
	void	SaveAllBeforeDestruct() override;
	void	Receive(JBroadcaster* sender, const Message& message) override;

public:

	struct FileTypeInfo
	{
		JString*		suffix;
		JRegex*			nameRegex;		// can be nullptr
		JRegex*			contentRegex;	// can be nullptr
		JUtf8ByteRange	literalRange;	// initially nothing
		TextFileType	type;
		JIndex			macroID;		// MacroSetInfo::id
		JIndex			crmID;			// CRMRuleListInfo::id
		bool			isUserScript;	// true if relative to ~/.jxcb/scripts/
		JString*		scriptPath;		// nullptr if none; *relative*
		bool			wordWrap;
		JString*		complSuffix;
		JString*		editCmd;		// nullptr if type != kExternalFT

		FileTypeInfo()
			:
			suffix(nullptr), nameRegex(nullptr), contentRegex(nullptr), type(kUnknownFT),
			macroID(kEmptyMacroID), crmID(kEmptyCRMRuleListID),
			isUserScript(true), scriptPath(nullptr),
			wordWrap(true), complSuffix(nullptr), editCmd(nullptr)
		{ };

		FileTypeInfo(JString* s, JRegex* nr, JRegex* cr, const TextFileType t,
					 const JIndex macro, const JIndex crm,
					 const bool us, JString* sp,
					 const bool wrap, JString* cs, JString* ec)
			:
			suffix(s), nameRegex(nr), contentRegex(cr), type(t),
			macroID(macro), crmID(crm), isUserScript(us), scriptPath(sp),
			wordWrap(wrap), complSuffix(cs), editCmd(ec)
		{ };

		bool	IsPlainSuffix() const;
		void	CreateRegex();
		void	Free();
	};

	static JUtf8ByteRange	GetLiteralPrefixRange(const JString& regexStr);

	static JListT::CompareResult
		CompareFileTypeSpec(const FileTypeInfo& i1, const FileTypeInfo& i2);

	struct MacroSetInfo
	{
		JIndex					id;
		JString*				name;
		CharActionManager*	action;
		MacroManager*			macro;

		MacroSetInfo()
			:
			id(kEmptyMacroID), name(nullptr),
			action(nullptr), macro(nullptr)
		{ };

		MacroSetInfo(const JPrefID& i, JString* n,
					 CharActionManager* a, MacroManager* m)
			:
			id(i.GetID()), name(n), action(a), macro(m)
		{ };

		void	CreateAndRead(std::istream& input, const JFileVersion vers);
		void	Write(std::ostream& output);
		void	Free();
	};

	static bool		FindMacroID(const JArray<MacroSetInfo>& list,
								const JIndex id, JIndex* index);
	static JIndex	FindMacroName(const JUtf8Byte* macroName,
								  JArray<MacroSetInfo>* macroList,
								  const bool create);

	struct CRMRuleListInfo
	{
		JIndex						id;
		JString*					name;
		JStyledText::CRMRuleList*	list;

		CRMRuleListInfo()
			:
			id(kEmptyCRMRuleListID), name(nullptr), list(nullptr)
		{ };

		CRMRuleListInfo(const JPrefID& i, JString* n,
						JStyledText::CRMRuleList* l)
			:
			id(i.GetID()), name(n), list(l)
		{ };

		void	CreateAndRead(std::istream& input, const JFileVersion vers);
		void	Write(std::ostream& output);
		void	Free();
	};

	static bool		FindCRMRuleListID(const JArray<CRMRuleListInfo>& list,
									  const JIndex id, JIndex* index);
	static JIndex	FindCRMRuleListName(const JUtf8Byte* crmName,
										const JArray<CRMRuleListInfo>& crmList);

private:

	JArray<FileTypeInfo>*		itsFileTypeList;
	JArray<MacroSetInfo>*		itsMacroList;
	JArray<CRMRuleListInfo>*	itsCRMList;
	bool						itsExecOutputWordWrapFlag;
	bool						itsUnknownTypeWordWrapFlag;

	JColorID	itsColor [ kColorCount ];

	EditFileTypesDialog*	itsFileTypesDialog;
	EditMacroDialog*		itsMacroDialog;
	EditCRMDialog*		itsCRMDialog;

private:

	void	UpdateFileTypes(const EditFileTypesDialog& dlog);
	void	UpdateMacros(const EditMacroDialog& dlog);
	void	UpdateCRMRuleLists(const EditCRMDialog& dlog);

	void	GetStringList(const JPrefID& id, JPtrArray<JString>* list) const;
	void	SetStringList(const JPrefID& id, const JPtrArray<JString>& list);

	void	ConvertFromSuffixLists();
	void	ConvertHTMLSuffixesToFileTypes(const std::string& data);
	void	AddDefaultCMacros(MacroManager* mgr) const;
	void	AddDefaultEiffelMacros(MacroManager* mgr) const;
	void	AddDefaultFortranMacros(MacroManager* mgr) const;
	void	AddDefaultHTMLMacros(MacroManager* mgr) const;
	void	AddDefaultJavaMacros(MacroManager* mgr) const;
	void	AddDefaultCSharpMacros(MacroManager* mgr) const;
	void	AddDefaultXMLMacros(MacroManager* mgr) const;
	void	AddDefaultXMLActions(CharActionManager* mgr) const;
	void	InitComplementSuffix(const JString& suffix, JString* complSuffix) const;

	void	ReadFileTypeInfo(const JFileVersion vers);
	void	WriteFileTypeInfo();

	void	CreateAndReadData(JArray<MacroSetInfo>** list, const JFileVersion vers);
	void	WriteData(const JArray<MacroSetInfo>& list);

	void	CreateCRMRuleLists();
	void	CreateDCRMRuleList();
	void	CreateAndReadData(JArray<CRMRuleListInfo>** list, const JFileVersion vers);
	void	WriteData(const JArray<CRMRuleListInfo>& list);

	JArray<FileTypeInfo>*		CreateFileTypeList();
	JArray<MacroSetInfo>*		CreateMacroList();
	JArray<CRMRuleListInfo>*	CreateCRMList();

	TextFileType	GetFileType(const JString& fileName, JIndex* index) const;
	bool			CalcFileType(const TextDocument& doc, JIndex* index) const;
	JString			CleanFileName(const JString& name) const;

	void	ReadColors();
	void	WriteColors();

	void	ReadStaticGlobalPrefs(const JFileVersion vers) const;
	void	WriteStaticGlobalPrefs();

	static JListT::CompareResult
		CompareFileTypeSpecAndLength(const FileTypeInfo& i1, const FileTypeInfo& i2);
	static JListT::CompareResult
		CompareMacroNames(const MacroSetInfo& i1, const MacroSetInfo& i2);
	static JListT::CompareResult
		CompareCRMNames(const CRMRuleListInfo& i1, const CRMRuleListInfo& i2);

public:

	// JBroadcaster messages

	static const JUtf8Byte* kFileTypesChanged;
	static const JUtf8Byte* kTextColorChanged;

	class FileTypesChanged : public JBroadcaster::Message
		{
		public:

			FileTypesChanged()
				:
				JBroadcaster::Message(kFileTypesChanged)
			{
				for (JUnsignedOffset i=0; i<kFTCount; i++)
					{
					itsStatus[i] = false;
					}
			};

			bool
			Changed(const TextFileType fileType)
				const
			{
				return itsStatus[ fileType ];
			}

			bool
			Changed(bool (*typeCheckFn)(const TextFileType type))
				const
			{
				for (JUnsignedOffset i=0; i<kFTCount; i++)
					{
					if (itsStatus[i] && typeCheckFn((TextFileType) i))
						{
						return true;
						}
					}
				return false;
			}

			bool
			AnyChanged()
				const
			{
				for (JUnsignedOffset i=0; i<kFTCount; i++)
					{
					if (itsStatus[i])
						{
						return true;
						}
					}
				return false;
			}

			void
			SetChanged(const TextFileType fileType, const bool status)
			{
				itsStatus[ fileType ] = status;
			}

		private:

			bool itsStatus[ kFTCount ];
		};

	class TextColorChanged : public JBroadcaster::Message
		{
		public:

			TextColorChanged()
				:
				JBroadcaster::Message(kTextColorChanged)
				{ };
		};
};


/******************************************************************************
 ColorIndexValid (static)

 ******************************************************************************/

inline bool
PrefsManager::ColorIndexValid
	(
	const JIndex index
	)
{
	return 1 <= index && index <= kColorCount;
}

#endif
