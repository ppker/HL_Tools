#pragma once

#include <cassert>

#include "ui/camera_operators/CameraOperator.hpp"
#include "ui/camera_operators/dockpanels/FreeLookSettingsPanel.hpp"
#include "ui/settings/GeneralSettings.hpp"

namespace ui::camera_operators
{
/**
*	@brief Camera operator that allows the user to move in the 3D world freely using the mouse and keyboard
*/
class FreeLookCameraOperator final : public CameraOperator
{
public:
	static constexpr float DefaultFOV = 65.f;

	FreeLookCameraOperator(settings::GeneralSettings* generalSettings)
		: _generalSettings(generalSettings)
	{
		assert(_generalSettings);

		_camera.SetFieldOfView(DefaultFOV);
	}

	QString GetName() const override
	{
		return "Free Look";
	}

	QWidget* CreateEditWidget() override
	{
		return new FreeLookSettingsPanel(this);
	}

	void MouseEvent(QMouseEvent& event) override
	{
		//TODO: once the coordinate system is fixed this needs to be updated to work properly
		switch (event.type())
		{
		case QEvent::MouseButtonPress:
		{
			_oldCoordinates.x = event.x();
			_oldCoordinates.y = event.y();

			_trackedButtons |= event.button();

			event.accept();
			break;
		}

		case QEvent::MouseButtonRelease:
		{
			_trackedButtons &= ~event.button();
			event.accept();
			break;
		}

		case QEvent::MouseMove:
		{
			if (_trackedButtons & event.buttons())
			{
				if (event.buttons() & Qt::MouseButton::LeftButton)
				{
					//TODO: this should be a vector, not an angle
					glm::vec3 vecViewDir = _camera.GetViewDirection();

					auto horizontalAdjust = static_cast<float>(event.x() - _oldCoordinates.x);
					auto verticalAdjust = static_cast<float>(event.y() - _oldCoordinates.y);

					if (_generalSettings->ShouldInvertMouseX())
					{
						horizontalAdjust = -horizontalAdjust;
					}

					if (_generalSettings->ShouldInvertMouseY())
					{
						verticalAdjust = -verticalAdjust;
					}

					vecViewDir.y += horizontalAdjust;
					vecViewDir.x += verticalAdjust;

					_camera.SetViewDirection(vecViewDir);

					_oldCoordinates.x = event.x();
					_oldCoordinates.y = event.y();
				}
				else if (event.buttons() & Qt::MouseButton::RightButton)
				{
					auto adjust = static_cast<float>(event.y() - _oldCoordinates.y);

					if (_generalSettings->ShouldInvertMouseY())
					{
						adjust = -adjust;
					}

					_camera.GetOrigin().y += adjust;

					_oldCoordinates.x = event.x();
					_oldCoordinates.y = event.y();
				}
			}

			event.accept();
			break;
		}

		default: break;
		}
	}

private:
	const settings::GeneralSettings* const _generalSettings;
};
}