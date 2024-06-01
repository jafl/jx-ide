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

	~FileNode() override;

	void	OpenFile() const override;
	void	OpenComplementFile() const override;
	void	ViewPlainDiffs(const bool silent) const override;
	void	ViewVCSDiffs(const bool silent) const override;

	void	CollectFilesForParse(const JPtrArray<JString>& allSuffixList,
								 JPtrArray<JString>* fileList) const override;

	void	CreateFilesForTemplate(std::istream& input,
								   const JFileVersion vers) const override;
	void	SaveFilesInTemplate(std::ostream& output) const override;
};

#endif
