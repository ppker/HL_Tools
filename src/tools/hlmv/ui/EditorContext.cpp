#include <algorithm>
#include <cassert>
#include <iterator>
#include <stdexcept>

#include <QMessageBox>

#include "filesystem/CFileSystem.h"
#include "filesystem/IFileSystem.h"

#include "soundsystem/CSoundSystem.h"
#include "soundsystem/ISoundSystem.h"

#include "ui/EditorContext.hpp"

#include "ui/assets/Assets.hpp"

#include "ui/options/OptionsPageRegistry.hpp"
#include "ui/settings/GameConfiguration.hpp"
#include "ui/settings/GameEnvironment.hpp"

namespace ui
{
LoadedAsset::LoadedAsset(std::unique_ptr<assets::Asset>&& asset, QWidget* editWidget)
	: _asset(std::move(asset))
	, _editWidget(editWidget)
{
}

LoadedAsset::~LoadedAsset() = default;

EditorContext::EditorContext(
	QSettings* settings,
	std::unique_ptr<options::OptionsPageRegistry>&& optionsPageRegistry,
	std::unique_ptr<assets::IAssetProviderRegistry>&& assetProviderRegistry, QObject* parent)
	: QObject(parent)
	, _settings(settings)
	, _timer(new QTimer(this))
	, _optionsPageRegistry(std::move(optionsPageRegistry))
	, _fileSystem(std::make_unique<filesystem::CFileSystem>())
	, _soundSystem(std::make_unique<soundsystem::CSoundSystem>())
	, _assetProviderRegistry(std::move(assetProviderRegistry))
{
	_settings->setParent(this);

	//TODO: set up filesystem based on game configuration
	if (!_fileSystem->Initialize())
	{
		QMessageBox::critical(nullptr, "Fatal Error", "Failed to initialize file system");
		//TODO: ensure error handling cleans up properly
		throw std::runtime_error("Failed to initialize file system");
	}

	if (!_soundSystem->Initialize(_fileSystem.get()))
	{
		QMessageBox::critical(nullptr, "Fatal Error", "Failed to initialize sound system");
		throw std::runtime_error("Failed to initialize sound system");
	}

	connect(_timer, &QTimer::timeout, this, &EditorContext::OnTimerTick);
}

EditorContext::~EditorContext()
{
	_soundSystem->Shutdown();
	_fileSystem->Shutdown();
}

std::vector<settings::GameEnvironment*> EditorContext::GetGameEnvironments() const
{
	std::vector<settings::GameEnvironment*> environments;

	environments.reserve(_gameEnvironments.size());

	std::transform(_gameEnvironments.begin(), _gameEnvironments.end(), std::back_inserter(environments), [](const auto& environment)
		{
			return environment.get();
		});

	return environments;
}

settings::GameEnvironment* EditorContext::GetGameEnvironmentById(const QUuid& id) const
{
	if (auto it = std::find_if(_gameEnvironments.begin(), _gameEnvironments.end(), [&](const auto& environment)
		{
			return environment->GetId() == id;
		}
	); it != _gameEnvironments.end())
	{
		return it->get();
	}

	return nullptr;
}

void EditorContext::AddGameEnvironment(std::unique_ptr<settings::GameEnvironment>&& gameEnvironment)
{
	assert(gameEnvironment);

	auto& ref = _gameEnvironments.emplace_back(std::move(gameEnvironment));

	emit GameEnvironmentAdded(ref.get());
}

void EditorContext::RemoveGameEnvironment(const QUuid& id)
{
	if (auto it = std::find_if(_gameEnvironments.begin(), _gameEnvironments.end(), [&](const auto& environment)
		{
			return environment->GetId() == id;
		}
	); it != _gameEnvironments.end())
	{
		const std::unique_ptr<settings::GameEnvironment> gameEnvironment{std::move(*it)};

		_gameEnvironments.erase(it);

		emit GameEnvironmentRemoved(gameEnvironment.get());
	}
}

void EditorContext::OnTimerTick()
{
	//TODO: update frequency should be controllable
	emit Tick();
}
}
