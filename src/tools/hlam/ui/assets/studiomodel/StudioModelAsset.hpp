#pragma once

#include <cassert>
#include <memory>
#include <stack>
#include <vector>

#include <QObject>

#include "engine/shared/studiomodel/StudioModel.hpp"

#include "graphics/Scene.hpp"

#include "ui/IInputSink.hpp"
#include "ui/assets/Assets.hpp"

namespace graphics
{
class TextureLoader;
}

namespace ui
{
namespace camera_operators
{
class CameraOperator;
}

namespace settings
{
class StudioModelSettings;
}

namespace assets::studiomodel
{
class ModelChangeEvent;
class StudioModelAsset;
class StudioModelEditWidget;

class StudioModelAssetProvider final : public AssetProvider
{
public:
	StudioModelAssetProvider(const std::shared_ptr<settings::StudioModelSettings>& studioModelSettings)
		: _studioModelSettings(studioModelSettings)
	{
	}

	~StudioModelAssetProvider();

	entt::id_type GetAssetType() const override { return entt::type_index<StudioModelAsset>::value(); }

	QString GetProviderName() const override;

	QStringList GetFileTypes() const override;

	QString GetPreferredFileType() const override;

	QMenu* CreateToolMenu(EditorContext* editorContext) override;

	bool CanLoad(const QString& fileName) const override;

	std::unique_ptr<Asset> Load(EditorContext* editorContext, const QString& fileName) const override;

	settings::StudioModelSettings* GetStudioModelSettings() const { return _studioModelSettings.get(); }

private:
	const std::shared_ptr<settings::StudioModelSettings> _studioModelSettings;
};

class StudioModelAsset final : public Asset, public IInputSink
{
	Q_OBJECT

public:
	StudioModelAsset(QString&& fileName,
		EditorContext* editorContext, const StudioModelAssetProvider* provider, std::unique_ptr<studiomdl::StudioModel>&& studioModel);

	~StudioModelAsset();
	StudioModelAsset(const StudioModelAsset&) = delete;
	StudioModelAsset& operator=(const StudioModelAsset&) = delete;

	entt::id_type GetAssetType() const override { return entt::type_index<StudioModelAsset>::value(); }

	const StudioModelAssetProvider* GetProvider() const override { return _provider; }

	void PopulateAssetMenu(QMenu* menu) override;

	QWidget* GetEditWidget() override;

	void SetupFullscreenWidget(FullscreenWidget* fullscreenWidget) override;

	void Save() override;

	void OnMouseEvent(QMouseEvent* event) override;

	EditorContext* GetEditorContext() { return _editorContext; }

	studiomdl::StudioModel* GetStudioModel() { return _studioModel.get(); }

	graphics::TextureLoader* GetTextureLoader() { return _textureLoader.get(); }

	graphics::Scene* GetScene() { return _scene.get(); }

	IInputSink* GetInputSink() const { return _inputSinks.top(); }

	void PushInputSink(IInputSink* inputSink)
	{
		assert(inputSink);

		_inputSinks.push(inputSink);
	}

	void PopInputSink()
	{
		_inputSinks.pop();
	}

	int GetCameraOperatorCount() const { return _cameraOperators.size(); }

	camera_operators::CameraOperator* GetCameraOperator(int index) const;

	void AddCameraOperator(std::unique_ptr<camera_operators::CameraOperator>&& cameraOperator);

	camera_operators::CameraOperator* GetCurrentCameraOperator() const { return _cameraOperator; }

	void SetCurrentCameraOperator(camera_operators::CameraOperator* cameraOperator);

	void AddUndoCommand(QUndoCommand* command)
	{
		GetUndoStack()->push(command);
	}

	void EmitModelChanged(const ModelChangeEvent& event)
	{
		emit ModelChanged(event);
	}

private:
	void ChangeCamera(bool next);

signals:
	void Tick();

	void Draw();

	void ModelChanged(const ModelChangeEvent& event);

	void CameraChanged(camera_operators::CameraOperator* cameraOperator);

private slots:
	void OnTick();

	void OnSceneWidgetMouseEvent(QMouseEvent* event);

	void UpdateColors();

	void OnFloorLengthChanged(int length);

	void OnPreviousCamera();
	void OnNextCamera();

	void OnCenterView();
	void OnSaveView();
	void OnRestoreView();

	void OnLoadGroundTexture();
	void OnUnloadGroundTexture();

	void OnLoadBackgroundTexture();
	void OnUnloadBackgroundTexture();

	void OnDumpModelInfo();

	void OnTakeScreenshot();

private:
	EditorContext* const _editorContext;
	const StudioModelAssetProvider* const _provider;
	const std::unique_ptr<studiomdl::StudioModel> _studioModel;
	const std::unique_ptr<graphics::TextureLoader> _textureLoader;
	const std::unique_ptr<graphics::Scene> _scene;

	std::stack<IInputSink*> _inputSinks;

	std::vector<std::unique_ptr<camera_operators::CameraOperator>> _cameraOperators;

	camera_operators::CameraOperator* _cameraOperator{};

	StudioModelEditWidget* _editWidget{};
};
}
}