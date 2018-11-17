#include "simple_specular_diffuse_material.h"

#include <light_ray.h>
#include <intersection_point.h>
#include <monte_carlo_specular_diffuse_group.h>

manta::SimpleSpecularDiffuseMaterial::SimpleSpecularDiffuseMaterial() {
	m_maxDiffuseDegree = 4;
	m_maxSpecularDegree = 5;

	m_autoDisableEmitters = true;
	m_enableDiffuse = true;
	m_enableSpecular = true;
}

manta::SimpleSpecularDiffuseMaterial::~SimpleSpecularDiffuseMaterial() {
}

void manta::SimpleSpecularDiffuseMaterial::integrateRay(LightRay * ray, const RayEmitterGroup * _rayEmitter) const {
	math::Vector addedLight = math::constants::Zero;

	if (_rayEmitter != nullptr) {
		MonteCarloSpecularDiffuseGroup *rayEmitter = (MonteCarloSpecularDiffuseGroup *)_rayEmitter;
		SimpleRayEmitter *specular = rayEmitter->m_specularEmitter;
		SimpleMonteCarloEmitter *diffuse = rayEmitter->m_diffuseEmitter;

		if (specular != nullptr) {
			LightRay *reflectedRay = specular->getRay();
			addedLight = math::add(addedLight, math::mul(m_specularColor, reflectedRay->getIntensity()));
		}

		if (diffuse != nullptr) {
			LightRay *reflectedRay = diffuse->getRay();
			addedLight = math::add(addedLight, math::mul(m_diffuseColor, reflectedRay->getIntensity()));
		}
	}

	ray->setIntensity(
		math::add(
			addedLight,
			m_emission
		)
	);
}

void manta::SimpleSpecularDiffuseMaterial::setSpecularColor(const math::Vector &specular) {
	m_specularColor = specular;

	if (m_autoDisableEmitters) {
		double magnitude = math::getScalar(math::magnitudeSquared3(specular));
		if (magnitude < 1e-4) {
			m_enableSpecular = false;
		}
		else {
			m_enableSpecular = true;
		}
	}
	else {
		m_enableSpecular = true;
	}
}

void manta::SimpleSpecularDiffuseMaterial::setDiffuseColor(const math::Vector &diffuse) {
	m_diffuseColor = diffuse;

	if (m_autoDisableEmitters) {
		double magnitude = math::getScalar(math::magnitudeSquared3(diffuse));
		if (magnitude < 1e-4) {
			m_enableDiffuse = false;
		}
		else {
			m_enableDiffuse = true;
		}
	}
	else {
		m_enableDiffuse = true;
	}
}

void manta::SimpleSpecularDiffuseMaterial::setAutoDisableEmitters(bool autoDisableEmitters) {
	m_autoDisableEmitters = autoDisableEmitters;

	if (!m_autoDisableEmitters) {
		m_enableDiffuse = true;
		m_enableSpecular = true;
	}
	else {
		setSpecularColor(m_specularColor);
		setDiffuseColor(m_diffuseColor);
	}
}

void manta::SimpleSpecularDiffuseMaterial::preconfigureEmitterGroup(RayEmitterGroup * _group, int degree) const {
	MonteCarloSpecularDiffuseGroup *group = (MonteCarloSpecularDiffuseGroup *)_group;
	if (degree >= m_maxDiffuseDegree || !m_enableDiffuse) group->setDiffuseEnabled(false);
	else group->setDiffuseEnabled(true);

	if (degree >= m_maxSpecularDegree || !m_enableSpecular) group->setSpecularEnabled(false);
	else group->setSpecularEnabled(true);
}

manta::RayEmitterGroup * manta::SimpleSpecularDiffuseMaterial::generateRayEmittersInternal(const LightRay * ray, const IntersectionPoint * intersectionPoint, int degree, StackAllocator *stackAllocator) const {
	if (degree >= m_maxDiffuseDegree && degree >= m_maxSpecularDegree) {
		return nullptr;
	}

	if (!m_enableDiffuse && !m_enableSpecular) return nullptr;

	// Calculate bias point
	math::Vector biasPoint = math::add(intersectionPoint->m_position, math::mul(intersectionPoint->m_normal, math::loadScalar(0.001f)));
	MonteCarloSpecularDiffuseGroup *newEmitter = createEmitterGroup<MonteCarloSpecularDiffuseGroup>(degree, stackAllocator);

	if (degree < m_maxDiffuseDegree && m_enableDiffuse) {
		newEmitter->m_diffuseEmitter->setNormal(intersectionPoint->m_normal);
		newEmitter->m_diffuseEmitter->setIncident(ray->getDirection());
		newEmitter->m_diffuseEmitter->setPosition(biasPoint);
	}
	
	if (degree < m_maxSpecularDegree && m_enableSpecular) {
		math::Vector perturb = intersectionPoint->m_normal;
		math::Vector n_dot_d = math::dot(intersectionPoint->m_normal, ray->getDirection());
		perturb = math::mul(perturb, math::add(n_dot_d, n_dot_d)); // Multiply by 2
		math::Vector finalDirection = math::sub(ray->getDirection(), perturb);
		finalDirection = math::normalize(finalDirection);

		newEmitter->m_specularEmitter->setDirection(finalDirection);
		newEmitter->m_specularEmitter->setPosition(biasPoint);
	}

	return newEmitter;
}