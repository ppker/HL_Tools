#pragma once

#include <QMouseEvent>
#include <QObject>
#include <QString>
#include <QVariant>

#include <glm/vec2.hpp>

#include "graphics/Camera.hpp"

class QWidget;

namespace ui::camera_operators
{
class CameraOperator : public QObject
{
	Q_OBJECT

public:
	CameraOperator() = default;

	virtual ~CameraOperator() = 0 {}

	graphics::Camera* GetCamera() { return &_camera; }

	virtual QString GetName() const = 0;

	virtual QWidget* CreateEditWidget() = 0;

	virtual void MouseEvent(QMouseEvent& event) = 0;

	virtual void CenterView(float height, float distance, float yaw) = 0;

	virtual void SaveView() = 0;

	virtual void RestoreView() = 0;

signals:
	void CameraPropertiesChanged();

protected:
	glm::vec2 _oldCoordinates{0.f};
	Qt::MouseButtons _trackedButtons{};
	graphics::Camera _camera;
};
}

Q_DECLARE_METATYPE(ui::camera_operators::CameraOperator*)