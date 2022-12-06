#include <cassert>
#include <cmath>
#include <utility>

#include <qopenglfunctions_1_1.h>

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

#include "engine/renderer/sprite/SpriteRenderer.hpp"
#include "engine/renderer/studiomodel/StudioModelRenderer.hpp"
#include "engine/shared/renderer/studiomodel/IStudioModelRenderer.hpp"

#include "entity/EntityList.hpp"
#include "entity/HLMVStudioModelEntity.hpp"

#include "graphics/GraphicsUtils.hpp"
#include "graphics/IGraphicsContext.hpp"
#include "graphics/Scene.hpp"
#include "graphics/TextureLoader.hpp"

#include "qt/QtLogSink.hpp"

namespace graphics
{
Scene::Scene(TextureLoader* textureLoader, soundsystem::ISoundSystem* soundSystem, WorldTime* worldTime, ui::settings::StudioModelSettings* settings)
	: _textureLoader(textureLoader)
	, _spriteRenderer(std::make_unique<sprite::SpriteRenderer>(CreateQtLoggerSt(logging::HLAMSpriteRenderer()), worldTime))
	, _studioModelRenderer(std::make_unique<studiomdl::StudioModelRenderer>(CreateQtLoggerSt(logging::HLAMStudioModelRenderer())))
	, _worldTime(worldTime)
	, _entityList(std::make_unique<EntityList>(_worldTime))
	, _entityContext(std::make_unique<EntityContext>(_worldTime,
		_studioModelRenderer.get(), _spriteRenderer.get(),
		_entityList.get(),
		soundSystem,
		settings,
		this))
{
	assert(_textureLoader);

	SetCurrentCamera(nullptr);
}

Scene::~Scene()
{
	_entityList->DestroyAll();
}

void Scene::SetGraphicsContext(std::unique_ptr<IGraphicsContext>&& graphicsContext)
{
	_graphicsContext = std::move(graphicsContext);
}

void Scene::SetOpenGLFunctions(QOpenGLFunctions_1_1* openglFunctions)
{
	_openglFunctions = openglFunctions;
	_textureLoader->SetOpenGLFunctions(openglFunctions);
}

glm::vec3 Scene::GetLightColor() const
{
	return _studioModelRenderer->GetLightColor();
}

void Scene::SetLightColor(const glm::vec3& value)
{
	_studioModelRenderer->SetLightColor(value);
}

glm::vec3 Scene::GetWireframeColor() const
{
	return _studioModelRenderer->GetWireframeColor();
}

void Scene::SetWireframeColor(const glm::vec3& value)
{
	_studioModelRenderer->SetWireframeColor(value);
}

void Scene::AlignOnGround()
{
	auto entity = GetEntity();

	auto model = entity->GetEditableModel();

	//First try finding the idle sequence, since that typically represents a model "at rest"
	//Failing that, use the first sequence
	auto idleFinder = [&]() -> const studiomdl::Sequence*
	{
		if (model->Sequences.empty())
		{
			return nullptr;
		}

		for (const auto& sequence : model->Sequences)
		{
			if (sequence->Label == "idle")
			{
				return sequence.get();
			}
		}

		return model->Sequences[0].get();
	};

	auto sequence = idleFinder();

	entity->SetOrigin({0, 0, sequence ? -sequence->BBMin.z : 0});
}

void Scene::Initialize()
{
	++_initializeCount;

	if (_initializeCount != 1)
	{
		return;
	}

	if (!_studioModelRenderer->Initialize())
	{
		//TODO: handle error
	}
}

void Scene::Shutdown()
{
	--_initializeCount;

	if (_initializeCount != 0)
	{
		return;
	}

	_studioModelRenderer->Shutdown();

	if (BackgroundTexture != 0)
	{
		_openglFunctions->glDeleteTextures(1, &BackgroundTexture);
		BackgroundTexture = 0;
	}

	if (GroundTexture != 0)
	{
		_openglFunctions->glDeleteTextures(1, &GroundTexture);
		GroundTexture = 0;
	}
}

void Scene::Tick()
{
	_entityList->RunFrame();
}

void Scene::Draw()
{
	//TODO: really ugly, needs reworking
	if (nullptr != _entity)
	{
		auto model = _entity->GetEditableModel();
		
		if (model->TexturesNeedCreating)
		{
			model->TexturesNeedCreating = false;
			model->CreateTextures(*_textureLoader);
		}
	}

	_openglFunctions->glClearColor(BackgroundColor.r, BackgroundColor.g, BackgroundColor.b, 1.0f);
	_openglFunctions->glClearStencil(0);
	_openglFunctions->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	_openglFunctions->glViewport(0, 0, _windowWidth, _windowHeight);

	_openglFunctions->glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	_drawnPolygonsCount = 0;

	auto camera = GetCurrentCamera();

	_studioModelRenderer->SetOpenGLFunctions(_openglFunctions);
	_studioModelRenderer->SetViewerOrigin(camera->GetOrigin());
	_studioModelRenderer->SetViewerRight(camera->GetRightVector());

	const unsigned int uiOldPolys = _studioModelRenderer->GetDrawnPolygonsCount();

	DrawRenderables(RenderPass::Background);

	_openglFunctions->glMatrixMode(GL_PROJECTION);
	_openglFunctions->glLoadIdentity();
	_openglFunctions->glLoadMatrixf(glm::value_ptr(camera->GetProjectionMatrix()));

	_openglFunctions->glMatrixMode(GL_MODELVIEW);
	_openglFunctions->glPushMatrix();
	_openglFunctions->glLoadIdentity();
	_openglFunctions->glLoadMatrixf(glm::value_ptr(camera->GetViewMatrix()));

	DrawRenderables(RenderPass::Standard);
	DrawRenderables(RenderPass::Overlay3D);
	DrawRenderables(RenderPass::Overlay2D);

	_drawnPolygonsCount = _studioModelRenderer->GetDrawnPolygonsCount() - uiOldPolys;

	_openglFunctions->glPopMatrix();
}

void Scene::CollectRenderables(RenderPass::RenderPass renderPass, std::vector<BaseEntity*>& renderablesToRender)
{
	renderablesToRender.clear();

	for (auto entity = _entityList->GetFirstEntity(); entity.IsValid(*_entityList); entity = _entityList->GetNextEntity(entity))
	{
		auto ent = entity.Get(*_entityList);

		if (ent->GetRenderPasses() & renderPass)
		{
			renderablesToRender.emplace_back(ent);
		}
	}
}

void Scene::DrawRenderables(RenderPass::RenderPass renderPass)
{
	CollectRenderables(renderPass, _renderablesToRender);

	for (const auto& renderable : _renderablesToRender)
	{
		renderable->Draw(_openglFunctions, renderPass);
	}
}
}
