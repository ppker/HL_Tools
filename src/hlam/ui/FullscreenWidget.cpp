#include <QKeyEvent>
#include <QtPlatformHeaders/QWindowsWindowFunctions>

#include "ui/EditorContext.hpp"
#include "ui/FullscreenWidget.hpp"

FullscreenWidget::FullscreenWidget()
{
	//This is not a primary window so don't keep the app running if we're still alive
	setAttribute(Qt::WidgetAttribute::WA_QuitOnClose, false);

	//This has to be a native window for there to be a window handle
	setAttribute(Qt::WidgetAttribute::WA_NativeWindow, true);

	//Without this going fullscreen will cause black flickering
	QWindowsWindowFunctions::setHasBorderInFullScreen(this->windowHandle(), true);
}

FullscreenWidget::~FullscreenWidget() = default;

void FullscreenWidget::SetWidget(QWidget* widget)
{
	CleanupOldWidget();
	setCentralWidget(widget);
	//Filter key events on the scene widget so we can capture exit even if it has focus
	widget->installEventFilter(this);
}

void FullscreenWidget::ExitFullscreen()
{
	this->hide();
	CleanupOldWidget();
	emit ExitedFullscreen();
}

bool FullscreenWidget::ProcessKeyEvent(QKeyEvent* event)
{
	switch (event->key())
	{
	case ExitFullscreenKey:
	{
		ExitFullscreen();
		return true;
	}

	case ToggleFullscreenKey:
	{
		if (this->isFullScreen())
		{
			this->showMaximized();
		}
		else
		{
			this->showFullScreen();
		}

		return true;
	}

	default: return false;
	}
}

void FullscreenWidget::CleanupOldWidget()
{
	// The fullscreen widget doesn't own the central widget so it has to be removed on close.
	auto widget = this->takeCentralWidget();

	if (widget)
	{
		widget->removeEventFilter(this);
		widget->setParent(nullptr);
	}
}

bool FullscreenWidget::eventFilter(QObject* object, QEvent* event)
{
	if (event->type() == QEvent::Type::KeyPress)
	{
		const auto keyEvent = static_cast<QKeyEvent*>(event);

		return ProcessKeyEvent(keyEvent);
	}

	return false;
}

void FullscreenWidget::keyPressEvent(QKeyEvent* event)
{
	if (ProcessKeyEvent(event))
	{
		return;
	}

	QMainWindow::keyPressEvent(event);
}

void FullscreenWidget::closeEvent(QCloseEvent* event)
{
	ExitFullscreen();
	QMainWindow::closeEvent(event);
}
