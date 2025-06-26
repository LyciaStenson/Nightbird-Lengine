#include <Core/RenderTarget.h>

#include <Core/Scene.h>
#include <Vulkan/DescriptorSetLayoutManager.h>
#include <Vulkan/GlobalDescriptorSetManager.h>

using namespace Nightbird;

RenderTarget::RenderTarget(Renderer* renderer)
	: renderer(renderer)
{

}