#include "soundsystem/SoundConstants.hpp"
#include "soundsystem/ISoundSystem.hpp"

#include "entity/HLMVStudioModelEntity.hpp"

#include "plugins/halflife/studiomodel/StudioModelAsset.hpp"

#include "settings/ApplicationSettings.hpp"

#undef PlaySound

void HLMVStudioModelEntity::Spawn()
{
	SetThink(&ThisClass::AnimThink);

	//Always think for smooth animations
	AlwaysThink = true;

	SetSequence(0);
	SetController(0, 0.0f);
	SetController(1, 0.0f);
	SetController(2, 0.0f);
	SetController(3, 0.0f);
	SetMouth(0.0f);

	auto model = GetEditableModel();

	for (int n = 0; n < model->Bodyparts.size(); ++n)
	{
		SetBodygroup(n, 0);
	}

	SetSkin(0);
}

void HLMVStudioModelEntity::HandleAnimEvent(const AnimEvent& event)
{
	switch (event.id)
	{
	case SCRIPT_EVENT_SOUND:			// Play a named wave file
	case SCRIPT_EVENT_SOUND_VOICE:
	case SCRIPT_CLIENT_EVENT_SOUND:
	{
		if (GetContext()->AppSettings->PlaySounds)
		{
			int pitch = PITCH_NORM;

			if (GetContext()->AppSettings->FramerateAffectsPitch)
			{
				pitch = static_cast<int>(pitch * GetFrameRate());
			}

			GetContext()->SoundSystem->PlaySound(event.options, VOLUME_NORM, pitch);
		}

		break;
	}

	default: break;
	}
}

void HLMVStudioModelEntity::AnimThink()
{
	if (GetContext()->Asset->PlaySequence)
	{
		const float flTime = AdvanceFrame(0.0f, 0.1f);

		DispatchAnimEvents();
	}
}
