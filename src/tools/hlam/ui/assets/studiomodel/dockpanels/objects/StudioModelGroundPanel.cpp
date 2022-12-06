#include <limits>

#include <QFileDialog>
#include <QMessageBox>

#include "graphics/IGraphicsContext.hpp"

#include "qt/QtUtilities.hpp"

#include "ui/assets/studiomodel/StudioModelAsset.hpp"
#include "ui/assets/studiomodel/dockpanels/objects/StudioModelGroundPanel.hpp"

#include "ui/settings/StudioModelSettings.hpp"

namespace ui::assets::studiomodel
{
StudioModelGroundPanel::StudioModelGroundPanel(StudioModelAsset* asset, QWidget* parent)
	: QWidget(parent)
	, _asset(asset)
{
	_ui.setupUi(this);

	_ui.GroundTextureSize->setValue(_asset->GetProvider()->GetStudioModelSettings()->FloorTextureLength);

	_ui.GroundOrigin->SetRange(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max());
	_ui.GroundOrigin->SetDecimals(6);

	connect(_ui.ShowGround, &QCheckBox::stateChanged, this, &StudioModelGroundPanel::OnShowGroundChanged);
	connect(_ui.MirrorModelOnGround, &QCheckBox::stateChanged, this, &StudioModelGroundPanel::OnMirrorOnGroundChanged);
	connect(_ui.EnableGroundTextureTiling, &QGroupBox::toggled, this, &StudioModelGroundPanel::OnEnableGroundTextureTilingChanged);
	connect(_ui.GroundTextureSize, qOverload<int>(&QSpinBox::valueChanged), this, &StudioModelGroundPanel::OnGroundTextureSizeChanged);

	connect(_ui.GroundTexture, &QLineEdit::textChanged, this, &StudioModelGroundPanel::OnTextureChanged);
	connect(_ui.BrowseGroundTexture, &QPushButton::clicked, this, &StudioModelGroundPanel::OnBrowseTexture);

	connect(_ui.GroundOrigin, &qt::widgets::Vector3Edit::ValueChanged, this, &StudioModelGroundPanel::OnOriginChanged);
}

void StudioModelGroundPanel::OnLayoutDirectionChanged(QBoxLayout::Direction direction)
{
	_ui.MainLayout->setDirection(direction);
}

void StudioModelGroundPanel::OnShowGroundChanged()
{
	_asset->GetProvider()->GetStudioModelSettings()->ShowGround = _ui.ShowGround->isChecked();

	if (!_asset->GetProvider()->GetStudioModelSettings()->ShowGround)
	{
		_ui.MirrorModelOnGround->setChecked(false);
	}
}

void StudioModelGroundPanel::OnMirrorOnGroundChanged()
{
	auto scene = _asset->GetScene();

	_asset->GetProvider()->GetStudioModelSettings()->MirrorOnGround = _ui.MirrorModelOnGround->isChecked();

	if (_asset->GetProvider()->GetStudioModelSettings()->MirrorOnGround)
	{
		_ui.ShowGround->setChecked(true);
	}
}

void StudioModelGroundPanel::OnEnableGroundTextureTilingChanged()
{
	_asset->GetProvider()->GetStudioModelSettings()->EnableFloorTextureTiling = _ui.EnableGroundTextureTiling->isChecked();
}

void StudioModelGroundPanel::OnGroundTextureSizeChanged()
{
	_asset->GetProvider()->GetStudioModelSettings()->FloorTextureLength = _ui.GroundTextureSize->value();
}

void StudioModelGroundPanel::OnTextureChanged()
{
	auto scene = _asset->GetScene();

	auto openglFunctions = scene->GetOpenGLFunctions();

	const QString fileName = _ui.GroundTexture->text();

	bool setTexture = false;

	scene->GetGraphicsContext()->Begin();

	if (!fileName.isEmpty())
	{
		QImage image{fileName};

		if (!image.isNull())
		{
			image.convertTo(QImage::Format::Format_RGBA8888);

			if (scene->GroundTexture == 0)
			{
				openglFunctions->glGenTextures(1, &scene->GroundTexture);
			}

			openglFunctions->glBindTexture(GL_TEXTURE_2D, scene->GroundTexture);

			openglFunctions->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.constBits());
			openglFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			openglFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			setTexture = true;
		}
	}

	if (setTexture)
	{
		_ui.ShowGround->setChecked(true);
	}
	else
	{
		if (scene->GroundTexture != 0)
		{
			openglFunctions->glDeleteTextures(1, &scene->GroundTexture);
			scene->GroundTexture = 0;
		}
	}

	scene->GetGraphicsContext()->End();
}

void StudioModelGroundPanel::OnBrowseTexture()
{
	const QString fileName{QFileDialog::getOpenFileName(nullptr, {}, {}, qt::GetImagesFileFilter())};

	if (!fileName.isEmpty())
	{
		_ui.GroundTexture->setText(fileName);
	}
}

void StudioModelGroundPanel::OnOriginChanged()
{
	_asset->GetProvider()->GetStudioModelSettings()->FloorOrigin = _ui.GroundOrigin->GetValue();
}
}
