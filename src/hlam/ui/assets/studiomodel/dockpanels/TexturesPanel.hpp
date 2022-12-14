#pragma once

#include "ui_TexturesPanel.h"

#include "ui/DockableWidget.hpp"

namespace studiomdl
{
class EditableStudioModel;
}

namespace ui
{
class StateSnapshot;

namespace assets::studiomodel
{
class ModelChangeEvent;
class StudioModelAsset;

class TexturesPanel final : public DockableWidget
{
	Q_OBJECT

public:
	TexturesPanel(StudioModelAsset* asset);
	~TexturesPanel();

	void OnVisibilityChanged(bool visible) override;

private:
	void InitializeUI();

	void ImportTextureFrom(const QString& fileName, studiomdl::EditableStudioModel& model, int textureIndex);
	void UpdateColormapValue();
	void UpdateUVMapTexture();

private slots:
	void OnModelChanged(const ModelChangeEvent& event);

	void OnSaveSnapshot(StateSnapshot* snapshot);
	void OnLoadSnapshot(StateSnapshot* snapshot);

	void OnScaleChanged(float adjust);

	void OnTextureChanged(int index);

	void OnTextureViewScaleSliderChanged(int value);
	void OnTextureViewScaleSpinnerChanged(double value);

	void OnUVLineWidthSliderChanged(int value);
	void OnUVLineWidthSpinnerChanged(double value);

	void OnTextureNameChanged();
	void OnTextureNameRejected();

	void OnChromeChanged();
	void OnAdditiveChanged();
	void OnTransparentChanged();
	void OnFlatShadeChanged();
	void OnFullbrightChanged();
	void OnMipmapsChanged();

	void OnOverlayUVMapChanged();

	void OnImportTexture();
	void OnExportTexture();
	void OnExportUVMap();

	void OnImportAllTextures();
	void OnExportAllTextures();

	void OnTopColorSliderChanged();
	void OnBottomColorSliderChanged();
	void OnTopColorSpinnerChanged();
	void OnBottomColorSpinnerChanged();

	void OnTextureFiltersChanged();
	void OnPowerOf2TexturesChanged();

private:
	Ui_TexturesPanel _ui;
	StudioModelAsset* const _asset;
};
}
}
