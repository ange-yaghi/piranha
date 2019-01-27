#include <light_ray.h>

manta::LightRay::LightRay() {
	m_intensity = math::constants::Zero;
	m_weight = (math::real)1.0;
}

manta::LightRay::~LightRay() {

}

void manta::LightRay::calculateTransformations() {
	m_kz = math::maxDimension3(math::abs(m_direction));
	m_kx = (m_kz + 1) % 3;
	m_ky = (m_kx + 1) % 3;
	m_permutedDirection = math::permute(m_direction, m_kx, m_ky, m_kz);

	math::real dirZ = math::getZ(m_permutedDirection);
	m_shear.x = -math::getX(m_permutedDirection) / dirZ;
	m_shear.y = -math::getY(m_permutedDirection) / dirZ;
	m_shear.z = (math::real)1.0 / dirZ;
}
