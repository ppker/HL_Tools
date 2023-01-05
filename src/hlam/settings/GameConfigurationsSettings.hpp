#pragma once

#include <memory>
#include <vector>

#include <QObject>
#include <QString>
#include <QUuid>

class QSettings;

class GameConfiguration final
{
public:
	QUuid Id;
	QString Name;
	QString GameExecutable;
	QString BaseGameDirectory;
	QString ModDirectory;
};

class GameConfigurationsSettings final : public QObject
{
	Q_OBJECT

public:
	explicit GameConfigurationsSettings(QSettings* settings)
		: _settings(settings)
	{
	}

	~GameConfigurationsSettings() = default;

	void LoadSettings();
	void SaveSettings();

	std::vector<const GameConfiguration*> GetConfigurations() const;

	const GameConfiguration* GetConfigurationById(const QUuid& id) const;

	void AddConfiguration(std::unique_ptr<GameConfiguration>&& configuration);

	void RemoveConfiguration(const QUuid& id);

	void UpdateConfiguration(const GameConfiguration& configuration);

	void RemoveAll();

	const GameConfiguration* GetDefaultConfiguration() const { return GetConfigurationById(_defaultConfiguration); }

	void SetDefaultConfiguration(const QUuid& id);

signals:
	void ConfigurationAdded(const GameConfiguration* configuration);
	void ConfigurationRemoved(const GameConfiguration* configuration);
	void ConfigurationUpdated(const GameConfiguration* configuration);

	void DefaultConfigurationChanged(const GameConfiguration* current, const GameConfiguration* previous);

private:
	QSettings* const _settings;

	std::vector<std::unique_ptr<GameConfiguration>> _gameConfigurations;

	QUuid _defaultConfiguration;
};
