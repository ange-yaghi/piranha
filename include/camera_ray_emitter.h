#ifndef CAMERA_RAY_EMITTER_H
#define CAMERA_RAY_EMITTER_H

#include <ray_emitter.h>

namespace manta {

	class CameraRayEmitter : public RayEmitter {
	public:
		CameraRayEmitter();
		virtual ~CameraRayEmitter();

		virtual void generateRays();

		void setSampleOffsetX(const math::Vector &sampleOffset) { m_sampleOffsetX = sampleOffset; }
		math::Vector getSampleOffsetX() const { return m_sampleOffsetX; }

		void setSampleOffsetY(const math::Vector &sampleOffset) { m_sampleOffsetY = sampleOffset; }
		math::Vector getSampleOffsetY() const { return m_sampleOffsetY; }

		void setStartDirection(const math::Vector &startDirection) { m_startDirection = startDirection; }
		math::Vector getStartDirection() const { return m_startDirection; }

		void setAASamples(int samples) { m_samples = samples; }
		int getAASamples() const { return m_samples; }

		math::Vector getIntensity() const;

	protected:
		math::Vector m_sampleOffsetX;
		math::Vector m_sampleOffsetY;

		math::Vector m_startDirection;

		int m_samples;
	};

} /* namespace manta */

#endif /* CAMERA_RAY_EMITTER_H */