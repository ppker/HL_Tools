#include <algorithm>
#include <cassert>

#include "settings/ApplicationSettings.hpp"
#include "settings/RecentFilesSettings.hpp"

#include "application/AssetManager.hpp"
#include "ui/options/OptionsPageGeneral.hpp"

const QString OptionsPageGeneralCategory{QStringLiteral("A.General")};
const QString OptionsPageGeneralId{QStringLiteral("A.General")};

OptionsPageGeneral::OptionsPageGeneral(const std::shared_ptr<ApplicationSettings>& applicationSettings)
	: _applicationSettings(applicationSettings)
{
	assert(_applicationSettings);

	SetCategory(QString{OptionsPageGeneralCategory});
	SetCategoryTitle("General");
	SetId(QString{OptionsPageGeneralId});
	SetPageTitle("General");
	SetWidgetFactory([this](AssetManager* application)
		{
			return new OptionsPageGeneralWidget(application, _applicationSettings.get());
		});
}

OptionsPageGeneral::~OptionsPageGeneral() = default;

OptionsPageGeneralWidget::OptionsPageGeneralWidget(
	AssetManager* application, ApplicationSettings* applicationSettings)
	: _application(application)
	, _applicationSettings(applicationSettings)
{
	_ui.setupUi(this);

	_ui.TickRate->setRange(ApplicationSettings::MinimumTickRate, ApplicationSettings::MaximumTickRate);

	_ui.MouseSensitivitySlider->setRange(ApplicationSettings::MinimumMouseSensitivity, ApplicationSettings::MaximumMouseSensitivity);
	_ui.MouseSensitivitySpinner->setRange(ApplicationSettings::MinimumMouseSensitivity, ApplicationSettings::MaximumMouseSensitivity);

	_ui.MouseWheelSpeedSlider->setRange(ApplicationSettings::MinimumMouseWheelSpeed, ApplicationSettings::MaximumMouseWheelSpeed);
	_ui.MouseWheelSpeedSpinner->setRange(ApplicationSettings::MinimumMouseWheelSpeed, ApplicationSettings::MaximumMouseWheelSpeed);

	_ui.PauseAnimationsOnTimelineClick->setChecked(_applicationSettings->PauseAnimationsOnTimelineClick);
	_ui.AllowTabCloseWithMiddleClick->setChecked(_applicationSettings->ShouldAllowTabCloseWithMiddleClick());
	_ui.OneAssetAtATime->setChecked(_applicationSettings->OneAssetAtATime);
	_ui.MaxRecentFiles->setValue(_applicationSettings->GetRecentFiles()->GetMaxRecentFiles());
	_ui.TickRate->setValue(_applicationSettings->GetTickRate());
	_ui.InvertMouseX->setChecked(_applicationSettings->ShouldInvertMouseX());
	_ui.InvertMouseY->setChecked(_applicationSettings->ShouldInvertMouseY());
	_ui.MouseSensitivitySlider->setValue(_applicationSettings->GetMouseSensitivity());
	_ui.MouseSensitivitySpinner->setValue(_applicationSettings->GetMouseSensitivity());
	_ui.MouseWheelSpeedSlider->setValue(_applicationSettings->GetMouseWheelSpeed());
	_ui.MouseWheelSpeedSpinner->setValue(_applicationSettings->GetMouseWheelSpeed());
	_ui.EnableAudioPlayback->setChecked(_applicationSettings->ShouldEnableAudioPlayback());
	_ui.MuteAudioWhenNotActive->setChecked(_applicationSettings->MuteAudioWhenNotActive);

	connect(_ui.MouseSensitivitySlider, &QSlider::valueChanged, _ui.MouseSensitivitySpinner, &QSpinBox::setValue);
	connect(_ui.MouseSensitivitySpinner, qOverload<int>(&QSpinBox::valueChanged), _ui.MouseSensitivitySlider, &QSlider::setValue);

	connect(_ui.MouseWheelSpeedSlider, &QSlider::valueChanged, _ui.MouseWheelSpeedSpinner, &QSpinBox::setValue);
	connect(_ui.MouseWheelSpeedSpinner, qOverload<int>(&QSpinBox::valueChanged), _ui.MouseWheelSpeedSlider, &QSlider::setValue);
}

OptionsPageGeneralWidget::~OptionsPageGeneralWidget() = default;

void OptionsPageGeneralWidget::ApplyChanges()
{
	_applicationSettings->PauseAnimationsOnTimelineClick = _ui.PauseAnimationsOnTimelineClick->isChecked();
	_applicationSettings->SetAllowTabCloseWithMiddleClick(_ui.AllowTabCloseWithMiddleClick->isChecked());
	_applicationSettings->OneAssetAtATime = _ui.OneAssetAtATime->isChecked();
	_applicationSettings->GetRecentFiles()->SetMaxRecentFiles(_ui.MaxRecentFiles->value());
	_applicationSettings->SetTickRate(_ui.TickRate->value());
	_applicationSettings->SetInvertMouseX(_ui.InvertMouseX->isChecked());
	_applicationSettings->SetInvertMouseY(_ui.InvertMouseY->isChecked());
	_applicationSettings->SetMouseSensitivity(_ui.MouseSensitivitySlider->value());
	_applicationSettings->SetMouseWheelSpeed(_ui.MouseWheelSpeedSlider->value());
	_applicationSettings->SetEnableAudioPlayback(_ui.EnableAudioPlayback->isChecked());
	_applicationSettings->MuteAudioWhenNotActive = _ui.MuteAudioWhenNotActive->isChecked();
}
