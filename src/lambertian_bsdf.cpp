#include <lambertian_bsdf.h>

#include <vector_material_node.h>

manta::LambertianBSDF::LambertianBSDF() {

}

manta::LambertianBSDF::~LambertianBSDF() {

}

manta::math::Vector manta::LambertianBSDF::sampleF(const IntersectionPoint *surfaceInteraction, const math::Vector &i, math::Vector *o, math::real *pdf, StackAllocator *stackAllocator) const {	
	// Uniformly sample a hemisphere
	math::real r1 = math::uniformRandom(math::constants::TWO_PI);
	math::real r2 = math::uniformRandom();
	math::real r2s = (math::real)sqrt(1 - r2 * r2);

	math::Vector direction = math::loadVector(
		cos(r1) * r2s,
		sin(r1) * r2s,
		r2);

	*o = direction;
	*pdf = (math::real)1.0 / math::constants::TWO_PI;

	return math::loadScalar((math::real)1.0 / math::constants::PI);
}

manta::math::Vector manta::LambertianBSDF::generateMicrosurfaceNormal(const BSDFInput &bsdfInput) const {
	math::real r1 = math::uniformRandom(math::constants::TWO_PI);
	math::real r2 = math::uniformRandom();
	math::real r2s = (math::real)sqrt(r2);

	math::Vector direction = math::add(
		math::mul(bsdfInput.u, math::loadScalar(cos(r1) * r2s)),
		math::mul(bsdfInput.v, math::loadScalar(sin(r1) * r2s)));
	direction = math::add(
		direction,
		math::mul(bsdfInput.normal, math::loadScalar(sqrt(1 - r2))));
	
	math::Vector microfacetNormal = math::inverseReflect(direction, bsdfInput.incident);

	return microfacetNormal;
}

manta::math::real manta::LambertianBSDF::generateWeight(const BSDFInput &bsdfInput, const math::Vector &m, const math::Vector &o) const {
	return math::real(1.0);
}

manta::math::real manta::LambertianBSDF::bidirectionalShadowMasking(const BSDFInput &bsdfInput, const math::Vector &o, const math::Vector &m) const {
	return (math::real)1.0;
}

manta::math::real manta::LambertianBSDF::g1(const BSDFInput &bsdfInput, const math::Vector &v, const math::Vector &m) const {
	return (math::real)0.0 /* Not supported */;
}
