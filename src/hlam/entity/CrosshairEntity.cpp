#include <QOpenGLFunctions_1_1>

#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "entity/CrosshairEntity.hpp"
#include "entity/HLMVStudioModelEntity.hpp"

#include "graphics/GraphicsUtils.hpp"
#include "graphics/SceneContext.hpp"

#include "settings/ApplicationSettings.hpp"
#include "settings/ColorSettings.hpp"

#include "plugins/halflife/studiomodel/StudioModelColors.hpp"

void CrosshairEntity::Draw(graphics::SceneContext& sc, RenderPasses renderPass)
{
	if (ShowCrosshair)
	{
		const int centerX = sc.WindowWidth / 2;
		const int centerY = sc.WindowHeight / 2;

		sc.OpenGLFunctions->glDisable(GL_CULL_FACE);

		sc.OpenGLFunctions->glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		sc.OpenGLFunctions->glDisable(GL_TEXTURE_2D);

		auto colors = GetContext()->AppSettings->GetColorSettings();

		sc.OpenGLFunctions->glColor4fv(glm::value_ptr(colors->GetColor(studiomodel::CrosshairColor)));

		sc.OpenGLFunctions->glPointSize(CROSSHAIR_LINE_WIDTH);
		sc.OpenGLFunctions->glLineWidth(CROSSHAIR_LINE_WIDTH);

		sc.OpenGLFunctions->glBegin(GL_POINTS);

		sc.OpenGLFunctions->glVertex2f(centerX - CROSSHAIR_LINE_WIDTH / 2, centerY + 1);

		sc.OpenGLFunctions->glEnd();

		sc.OpenGLFunctions->glBegin(GL_LINES);

		sc.OpenGLFunctions->glVertex2f(centerX - CROSSHAIR_LINE_START, centerY);
		sc.OpenGLFunctions->glVertex2f(centerX - CROSSHAIR_LINE_END, centerY);

		sc.OpenGLFunctions->glVertex2f(centerX + CROSSHAIR_LINE_START, centerY);
		sc.OpenGLFunctions->glVertex2f(centerX + CROSSHAIR_LINE_END, centerY);

		sc.OpenGLFunctions->glVertex2f(centerX, centerY - CROSSHAIR_LINE_START);
		sc.OpenGLFunctions->glVertex2f(centerX, centerY - CROSSHAIR_LINE_END);

		sc.OpenGLFunctions->glVertex2f(centerX, centerY + CROSSHAIR_LINE_START);
		sc.OpenGLFunctions->glVertex2f(centerX, centerY + CROSSHAIR_LINE_END);

		sc.OpenGLFunctions->glEnd();

		sc.OpenGLFunctions->glPointSize(1);
		sc.OpenGLFunctions->glLineWidth(1);
	}
}
