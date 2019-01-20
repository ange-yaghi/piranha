#include <lens_camera_ray_emitter.h>

#include <light_ray.h>
#include <lens.h>
#include <sampler_2d.h>
#include <stack_allocator.h>

manta::LensCameraRayEmitter::LensCameraRayEmitter() {

}

manta::LensCameraRayEmitter::~LensCameraRayEmitter() {

}

void manta::LensCameraRayEmitter::generateRays() {
	int totalRayCount = m_sampler->getTotalSampleCount(m_sampleCount);
	
	// Create all rays
	initializeRays(totalRayCount);
	LightRay *rays = getRays();

	math::Vector *sampleOrigins = (math::Vector *)getStackAllocator()->allocate(sizeof(math::Vector) * totalRayCount, 16);
	m_sampler->generateSamples(totalRayCount, sampleOrigins);

	LensScanHint hint;
	m_lens->lensScan(m_position, &hint, 4, m_sampler->getBoundaryWidth());

	for (int i = 0; i < totalRayCount; i++) {
		math::Vector position = math::add(m_position, sampleOrigins[i]);

		bool result = m_lens->generateOutgoingRay(position, &hint, &rays[i]);
		rays[i].setIntensity(math::constants::Zero);
		rays[i].setWeight((math::real)1.0);
	}

	getStackAllocator()->free((void *)sampleOrigins);
}
