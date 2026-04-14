#include "Core/Reflect.h"

#include "Core/SceneObject.h"
#include "Core/SpatialObject.h"
#include "Core/MeshInstance.h"
#include "Core/Camera.h"
#include "Core/DirectionalLight.h"
#include "Core/PointLight.h"
#include "Core/AudioSource.h"

#include "Core/Mesh.h"

#include "Core/Transform.h"

#include "Core/Math/Vector3.h"
#include "Core/Math/Quat.h"

#include <uuid.h>

NB_REGISTRATION

	NB_CLASS(Nightbird::Core::SceneObject)
		NB_CONSTRUCTOR()
		NB_PROPERTY(m_Name)
	NB_CLASS_END()

	NB_CLASS(Nightbird::Core::SpatialObject)
		NB_CONSTRUCTOR()
		NB_PROPERTY(m_Transform)
	NB_CLASS_END()

	NB_CLASS(Nightbird::Core::MeshInstance)
		NB_CONSTRUCTOR()
		NB_PROPERTY(m_MeshUUID)
	NB_CLASS_END()

	NB_CLASS(Nightbird::Core::Camera)
		NB_CONSTRUCTOR()
		NB_PROPERTY(m_Fov)
	NB_CLASS_END()

	NB_CLASS(Nightbird::Core::DirectionalLight)
		NB_CONSTRUCTOR()
		NB_PROPERTY(m_Color)
		NB_PROPERTY(m_Intensity)
	NB_CLASS_END()

	NB_CLASS(Nightbird::Core::PointLight)
		NB_CONSTRUCTOR()
		NB_PROPERTY(m_Color)
		NB_PROPERTY(m_Intensity)
		NB_PROPERTY(m_Radius)
	NB_CLASS_END()

	NB_CLASS(Nightbird::Core::AudioSource)
		NB_CONSTRUCTOR()
		NB_PROPERTY(m_AudioUUID)
	NB_CLASS_END()

	NB_CLASS_EDITOR_ONLY(Nightbird::Core::Mesh)
		NB_CONSTRUCTOR()
	NB_CLASS_EDITOR_ONLY_END()

	NB_CLASS(Nightbird::Core::Transform)
		NB_CONSTRUCTOR()
		NB_PROPERTY(position)
		NB_PROPERTY(rotation)
		NB_PROPERTY(scale)
	NB_CLASS_END()

	NB_CLASS(Nightbird::Core::Vector3)
		NB_CONSTRUCTOR()
		NB_PROPERTY(x)
		NB_PROPERTY(y)
		NB_PROPERTY(z)
	NB_CLASS_END()

	NB_CLASS(Nightbird::Core::Quat)
		NB_CONSTRUCTOR()
		NB_PROPERTY(x)
		NB_PROPERTY(y)
		NB_PROPERTY(z)
		NB_PROPERTY(w)
	NB_CLASS_END()

	NB_CLASS_EDITOR_ONLY(uuids::uuid)
		NB_CONSTRUCTOR()
	NB_CLASS_EDITOR_ONLY_END()

NB_REGISTRATION_END
