#include <QFileDialog>
#include <QFileInfo>
#include <QMenu>

#include "formats/studiomodel/StudioModelIO.hpp"
#include "formats/studiomodel/StudioModelUtils.hpp"

#include "qt/QtUtilities.hpp"

#include "ui/assets/studiomodel/StudioModelAsset.hpp"
#include "ui/assets/studiomodel/StudioModelAssetProvider.hpp"
#include "ui/assets/studiomodel/compiler/StudioModelCompilerFrontEnd.hpp"
#include "ui/assets/studiomodel/compiler/StudioModelDecompilerFrontEnd.hpp"

namespace studiomodel
{
Q_LOGGING_CATEGORY(HLAMStudioModel, "hlam.studiomodel")

StudioModelAssetProvider::~StudioModelAssetProvider() = default;

QMenu* StudioModelAssetProvider::CreateToolMenu(EditorContext* editorContext)
{
	auto menu = new QMenu("StudioModel");

	menu->addAction("Compile Model...", [=]
		{
			StudioModelCompilerFrontEnd compiler{editorContext, GetStudioModelSettings()};
	compiler.exec();
		});

	menu->addAction("Decompile Model...", [=]
		{
			StudioModelDecompilerFrontEnd decompiler{editorContext, GetStudioModelSettings()};
	decompiler.exec();
		});

	menu->addAction("Edit QC File...", []
		{
			const QString fileName{QFileDialog::getOpenFileName(nullptr, "Select QC File", {}, "QC files (*.qc);;All Files (*.*)")};

	if (!fileName.isEmpty())
	{
		qt::LaunchDefaultProgram(fileName);
	}
		});

	return menu;
}

bool StudioModelAssetProvider::CanLoad(const QString& fileName, FILE* file) const
{
	return studiomdl::IsStudioModel(file);
}

std::unique_ptr<Asset> StudioModelAssetProvider::Load(EditorContext* editorContext, const QString& fileName, FILE* file) const
{
	qCDebug(HLAMStudioModel) << "Trying to load model" << fileName;

	const auto filePath = std::filesystem::u8path(fileName.toStdString());
	auto studioModel = studiomdl::LoadStudioModel(filePath, file);

	auto editableStudioModel = studiomdl::ConvertToEditable(*studioModel);

	QString updatedFileName = fileName;

	QFileInfo fileInfo{fileName};

	//Automatically change the name of dol files to mdl to reflect the conversion
	if (fileInfo.suffix() == StudioModelPS2Extension)
	{
		updatedFileName = fileInfo.path() + '/' + fileInfo.completeBaseName() + '.' + StudioModelExtension;
	}

	qCDebug(HLAMStudioModel) << "Loaded model" << fileName << "as" << updatedFileName;

	return std::make_unique<StudioModelAsset>(std::move(updatedFileName), editorContext, this,
		std::make_unique<studiomdl::EditableStudioModel>(std::move(editableStudioModel)));
}
}