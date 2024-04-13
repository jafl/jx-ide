/******************************************************************************
 SymbolList.h

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_SymbolList
#define _H_SymbolList

#include <jx-af/jcore/JContainer.h>
#include "CtagsUser.h"
#include "PrefsManager.h"		// for FileTypesChanged
#include <jx-af/jcore/JFAID.h>
#include <jx-af/jcore/JArray.h>

class JProgressDisplay;
class ProjectDocument;
class Class;

class SymbolList : public JContainer, public CtagsUser
{
public:

	static const JSize kBlockSize = 4096;

	struct ContextNamespace
	{
		Language			lang;
		JPtrArray<JString>*	list;

		ContextNamespace()	// keep JArray happy
			:
			lang(kOtherLang),
			list(nullptr)
		{ }

		ContextNamespace(const Language _lang, JPtrArray<JString>* _list)
			:
			lang(_lang),
			list(_list)
		{ }

		void CleanOut()
		{
			// tree is not owned

			jdelete list;
			list = nullptr;
		}
	};

public:

	SymbolList(ProjectDocument* projDoc);

	~SymbolList() override;

	const JString&	GetSymbol(const JIndex symbolIndex,
							  Language* lang, Type* type,
							  bool* fullyQualifiedFileScope = nullptr) const;
	const JString&	GetFile(const JIndex symbolIndex, JIndex* lineIndex) const;
	bool			GetSignature(const JIndex symbolIndex,
								 const JString** signature) const;

	bool	IsUniqueClassName(const JString& name, Language* lang) const;
	bool	FindSymbol(const JString& name, const JFAID_t contextFileID,
					   const JArray<ContextNamespace>& contextNamespaceList,
					   const bool findDeclaration, const bool findDefinition,
					   JArray<JIndex>* matchList) const;
	bool	FindAllSymbols(const Class* theClass, const bool findDeclaration,
						   const bool findDefinition, JArray<JIndex>* matchList) const;
	bool	ClosestMatch(const JString& prefixStr,
						 JArray<JIndex>& visibleList, JIndex* index) const;

	void	ReadSetup(std::istream& projInput, const JFileVersion projVers,
					  std::istream* symInput, const JFileVersion symVers);
	void	WriteSetup(std::ostream& projOutput, std::ostream* symOutput) const;

	// for loading updated symbols

	void	ReadSetup(std::istream& input, const JFileVersion vers);

	// called by SymbolDirector

	void	FileTypesChanged(const PrefsManager::FileTypesChanged& info);
	void	PrepareForUpdate(const bool reparseAll, JProgressDisplay& pg);
	bool	UpdateFinished(const JArray<JFAID_t>& deadFileList, JProgressDisplay& pg);

	// called by FileListTable

	bool	NeedsReparseAll() const;
	void	FileChanged(const JString& fileName,
						const TextFileType fileType, const JFAID_t id);

protected:

	void	InitCtags(std::ostream& output) override;

	void	Receive(JBroadcaster* sender, const Message& message) override;

public:		// ought to be private

	struct SymbolInfo
	{
		JString*	name;		// shared with StringCompleter!
		JString*	signature;	// can be nullptr
		Language	lang;
		Type		type;
		bool		fullyQualifiedFileScope;
		JFAID_t		fileID;
		JIndex		lineIndex;

		SymbolInfo()
			:
			name(nullptr), signature(nullptr), lang(kOtherLang), type(kUnknownST),
			fullyQualifiedFileScope(false),
			fileID(JFAID::kInvalidID), lineIndex(0)
		{ };

		SymbolInfo(JString* n, JString* s, const Language l, const Type t,
				   const bool fqfs, const JFAID_t id, const JIndex line)
			:
			name(n), signature(s), lang(l), type(t),
			fullyQualifiedFileScope(fqfs),
			fileID(id), lineIndex(line)
		{ };

		void Free();
	};

private:

	ProjectDocument*	itsProjDoc;					// not owned
	JArray<SymbolInfo>*	itsSymbolList;
	bool				itsReparseAllFlag;			// true => flush all on next update
	bool				itsChangedDuringParseFlag;
	bool				itsBeganEmptyFlag;			// true => ignore RemoveFile()

private:

	void	RemoveAllSymbols();
	void	RemoveFile(const JIndex id);

	void	ParseFile(const JString& fileName,
					  const TextFileType fileType, const JFAID_t id);
	void	ReadSymbolList(std::istream& input, const Language lang,
						   const JString& fileName, const JFAID_t fileID);

	bool	ConvertToFullNames(JArray<JIndex>* noContextList,
							   JArray<JIndex>* contextList,
							   const JArray<ContextNamespace>& contextNamespaceList) const;
	void	PrepareContextNamespaceList(JPtrArray<JString>* contextNamespace,
										const JUtf8Byte* namespaceOp) const;
	bool	InContext(const JString& fullName,
					  const JPtrArray<JString>& contextNamespace,
					  const JString::Case caseSensitive) const;

	void	Sort(JArray<JIndex>* list) const;

	static std::weak_ordering
	CompareSymbols(const SymbolInfo& s1, const SymbolInfo& s2);

	static std::weak_ordering
	CompareSymbolsAndTypes(const SymbolInfo& s1, const SymbolInfo& s2);

	// not allowed

	SymbolList(const SymbolList&) = delete;
	SymbolList& operator=(const SymbolList&) = delete;

public:

	// JBroadcaster messages

	static const JUtf8Byte* kChanged;
	static const JUtf8Byte* kUpdateFoundChanges;

	class Changed : public JBroadcaster::Message
	{
	public:

		Changed()
			:
			JBroadcaster::Message(kChanged)
		{ };
	};

	class UpdateFoundChanges : public JBroadcaster::Message
	{
	public:

		UpdateFoundChanges()
			:
			JBroadcaster::Message(kUpdateFoundChanges)
		{ };
	};
};


/******************************************************************************
 NeedsReparseAll

 ******************************************************************************/

inline bool
SymbolList::NeedsReparseAll()
	const
{
	return itsReparseAllFlag;
}

/******************************************************************************
 GetSignature

 ******************************************************************************/

inline bool
SymbolList::GetSignature
	(
	const JIndex	symbolIndex,
	const JString**	signature
	)
	const
{
	const SymbolInfo info = itsSymbolList->GetItem(symbolIndex);
	*signature = info.signature;
	return info.signature != nullptr;
}

#endif
