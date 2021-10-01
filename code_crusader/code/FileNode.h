/******************************************************************************
 FileNode.h

	Copyright © 1999 John Lindal.

 ******************************************************************************/

#ifndef _H_FileNode
#define _H_FileNode

#include "FileNodeBase.h"

class FileNode : public FileNodeBase
{
public:

	FileNode(ProjectTree* tree, const JString& fileName);
	FileNode(std::istream& input, const JFileVersion vers,
			   ProjectNode* parent);

	virtual ~FileNode();

	virtual void	OpenFile() const override;
	virtual void	OpenComplementFile() const override;
	virtual void	ViewPlainDiffs(const bool silent) const override;
	virtual void	ViewVCSDiffs(const bool silent) const override;

	virtual bool	ParseFiles(FileListTable* parser,
								   const JPtrArray<JString>& allSuffixList,
								   SymbolList* symbolList,
								   CTree* cTree, DTree* dTree, GoTree* goTree,
								   JavaTree* javaTree, PHPTree* phpTree,
								   JProgressDisplay& pg) const override;

	virtual void	CreateFilesForTemplate(std::istream& input,
										   const JFileVersion vers) const override;
	virtual void	SaveFilesInTemplate(std::ostream& output) const override;

private:

	bool	ParseFile(const JString& fullName, FileListTable* parser,
						  const JPtrArray<JString>& allSuffixList,
						  SymbolList* symbolList,
						  CTree* cTree, DTree* dTree, GoTree* goTree,
						  JavaTree* javaTree, PHPTree* phpTree,
						  JProgressDisplay& pg) const;
};

#endif
