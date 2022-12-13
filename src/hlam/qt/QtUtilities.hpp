#pragma once

#include <QBoxLayout>
#include <QString>
#include <QWidget>

class QAbstractItemModel;

namespace qt
{
bool LaunchDefaultProgram(const QString& fileName);

QString GetImagesFileFilter();

QString GetSeparatedImagesFileFilter();

/**
*	@brief Gets an empty item model that should be used for widgets that don't allow setting a null model.
*/
QAbstractItemModel* GetEmptyModel();

inline QBoxLayout::Direction GetDirectionForDockArea(Qt::DockWidgetArea area)
{
	switch (area)
	{
	case Qt::DockWidgetArea::TopDockWidgetArea:
	case Qt::DockWidgetArea::BottomDockWidgetArea:
		return QBoxLayout::Direction::LeftToRight;

	default:
		return QBoxLayout::Direction::TopToBottom;
	}
}

inline Qt::Orientation GetOrientationForDockArea(Qt::DockWidgetArea area)
{
	switch (area)
	{
	case Qt::DockWidgetArea::TopDockWidgetArea:
	case Qt::DockWidgetArea::BottomDockWidgetArea:
		return Qt::Orientation::Horizontal;

	default:
		return Qt::Orientation::Vertical;
	}
}

inline void TrySetBoxLayoutDirection(QWidget* widget, QBoxLayout::Direction direction)
{
	if (!widget)
	{
		return;
	}

	if (auto layout = qobject_cast<QBoxLayout*>(widget->layout()); layout)
	{
		layout->setDirection(direction);
	}
}
}