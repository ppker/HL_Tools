#pragma once

#include <memory>
#include <utility>
#include <vector>

#include <QMainWindow>
#include <QObject>
#include <QPointer>
#include <QScopedPointer>
#include <QSettings>
#include <QString>

#include "application/SingleInstance.hpp"

class IAssetManagerPlugin;
class QApplication;

namespace graphics
{
class IGraphicsContext;
}

namespace ui
{
class EditorContext;
class MainWindow;
}

namespace ui::assets
{
class IAssetProviderRegistry;
}

namespace ui::options
{
class OptionsPageRegistry;
}

namespace ui::settings
{
class ColorSettings;
}

/**
*	@brief Handles program startup and shutdown
*/
class ToolApplication final : public QObject
{
	Q_OBJECT

public:
	ToolApplication();
	~ToolApplication();

	int Run(int argc, char* argv[]);

private:
	void ConfigureApplication(const QString& programName);
	
	void ConfigureOpenGL();

	std::tuple<bool, bool, QString> ParseCommandLine(QApplication& application);

	std::unique_ptr<QSettings> CreateSettings(const QString& programName, bool isPortable);

	bool CheckSingleInstance(const QString& programName, const QString& fileName, QSettings& settings);

	std::unique_ptr<ui::EditorContext> CreateEditorContext(
		std::unique_ptr<QSettings>&& settings, std::unique_ptr<graphics::IGraphicsContext>&& graphicsContext);

	bool AddPlugins(
		QSettings* settings,
		ui::settings::ColorSettings* colorSettings,
		ui::assets::IAssetProviderRegistry* assetProviderRegistry,
		ui::options::OptionsPageRegistry* optionsPageRegistry);

	std::unique_ptr<graphics::IGraphicsContext> InitializeOpenGL();

private:
	template<typename TFunction, typename... Args>
	void CallPlugins(TFunction&& function, Args&&... args)
	{
		for (auto& plugin : _plugins)
		{
			(*plugin.*function)(std::forward<Args>(args)...);
		}
	}

private slots:
	void OnExit();

	void OnFileNameReceived(const QString& fileName);

	void OnStylePathChanged(const QString& stylePath);

private:
	QApplication* _application{};

	std::vector<std::unique_ptr<IAssetManagerPlugin>> _plugins;

	std::unique_ptr<ui::EditorContext> _editorContext;
	QPointer<ui::MainWindow> _mainWindow;

	QScopedPointer<SingleInstance> _singleInstance;
};
