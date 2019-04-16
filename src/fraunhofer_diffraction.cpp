#include <fraunhofer_diffraction.h>

#include <image_byte_buffer.h>
#include <aperture.h>
#include <complex_map_2d.h>
#include <cft_estimator_2d.h>
#include <spectrum.h>
#include <color.h>
#include <cmf_table.h>

// Temp
#include <texture_node.h>
#include <intersection_point.h>

manta::FraunhoferDiffraction::FraunhoferDiffraction() {
	m_physicalSensorWidth = (math::real)1.0;
}

manta::FraunhoferDiffraction::~FraunhoferDiffraction() {

}

void manta::FraunhoferDiffraction::generate(const Aperture *aperture, const TextureNode *dirtMap, int outputResolution, math::real physicalSensorWidth, CmfTable *colorTable, Spectrum *sourceSpectrum, const Settings *settingsIn) {
	Settings settings;
	if (settingsIn == nullptr) defaultSettings(&settings);
	else settings = *settingsIn;
	
	int MAX_SAMPLES = settings.maxSamples;
	int minWavelength = settings.minWaveLength;
	int maxWavelength = settings.maxWaveLength;
	int wavelengthStep = settings.wavelengthStep;

	math::real apertureRadius = aperture->getRadius();
	math::real apertureRadius_inv = 1 / apertureRadius;
	math::real sensorWidth = physicalSensorWidth;
	math::real sensorElementWidth = physicalSensorWidth / outputResolution;

	m_physicalSensorWidth = physicalSensorWidth;
	m_sensorElementWidth = sensorElementWidth;
	m_colorTable = colorTable;
	m_sourceSpectrum = sourceSpectrum;

	math::real_d minFrequencyStep = sensorElementWidth / (maxWavelength * 1e-6);
	math::real_d maxFrequencyStep = sensorElementWidth / (minWavelength * 1e-6);

	int estimatorSamples = 0;
	math::real_d sampleWindow = 0.0;
	math::real_d maxFreq = (sensorWidth / 2) / (minWavelength * 1e-6);

	sampleWindow = CftEstimator2D::getMinPhysicalDim(minFrequencyStep / settings.frequencyMultiplier, apertureRadius * 2);
	estimatorSamples = CftEstimator2D::getMinSamples(maxFreq, sampleWindow, MAX_SAMPLES);

	math::real dx = (math::real)(sampleWindow / estimatorSamples);
	math::real dy = (math::real)(sampleWindow / estimatorSamples);
	math::real cx = (math::real)(sampleWindow / 2);
	math::real cy = (math::real)(sampleWindow / 2);

	// Generate aperture function
	ComplexMap2D apertureFunction;
	apertureFunction.initialize(estimatorSamples, estimatorSamples);

	IntersectionPoint a;
	for (int i = 0; i < estimatorSamples; i++) {
		math::real x = i * dx - cx;
		math::real u = (x * apertureRadius_inv) + (math::real)0.5;

		for (int j = 0; j < estimatorSamples; j++) {
			math::real y = j * dy - cy;
			math::real v = (y * apertureRadius_inv) + (math::real)0.5;

			a.m_textureCoodinates = math::loadVector(u, v);


			math::real dirt = (math::real)1.0;
			if (dirtMap != nullptr) dirt = math::getScalar(dirtMap->sample(&a));

			if (aperture->filter(x, y)) {
				apertureFunction.set(math::Complex((math::real_d)1.0 * dirt, (math::real_d)0.0), i, j);
			}
			else {
				apertureFunction.set(math::Complex((math::real_d)0.0, (math::real_d)0.0), i, j);
			}
		}
	}

	CftEstimator2D estimator;
	estimator.initialize(&apertureFunction, sampleWindow, sampleWindow);
	apertureFunction.destroy();

	math::real_d sdx = sensorElementWidth;
	math::real_d sdy = sensorElementWidth;
	math::real_d scx = sensorWidth / 2;
	math::real_d scy = sensorWidth / 2;
	math::real_d maxFreqX = estimator.getHorizontalFreqRange();
	math::real_d maxFreqY = estimator.getVerticalFreqRange();

	VectorMap2D temp;
	temp.initialize(outputResolution, outputResolution);
	generateMap(&estimator, minWavelength, maxWavelength, wavelengthStep, settings.textureSamples, &temp);

	temp.roll(&m_diffractionPattern);
	temp.destroy();
	estimator.destroy();

	normalize(settings.deltaWeight);
}

void manta::FraunhoferDiffraction::destroy() {
	m_diffractionPattern.destroy();
}

manta::math::Vector manta::FraunhoferDiffraction::samplePattern(math::real dx, math::real dy) const {
	math::real u, v;

	u = dx / (m_physicalSensorWidth);
	v = dy / (m_physicalSensorWidth);

	if (u < 0) u += (math::real)1.0;
	if (v < 0) v += (math::real)1.0;

	return m_diffractionPattern.sample(u, v);
}

void manta::FraunhoferDiffraction::normalize(math::real_d deltaWeight) {
	int res = m_diffractionPattern.getWidth();

	// For an accurate flux measurement of the halo, the center point must be 0
	m_diffractionPattern.set(math::constants::Zero, 0, 0);

	math::real_d totalFlux = 0;
	for (int i = 0; i < res; i++) {
		for (int j = 0; j < res; j++) {
			totalFlux += math::getY(m_diffractionPattern.get(i, j));
		}
	}

	// Normalize
	math::Vector norm = math::div(math::loadScalar((math::real)(1 - deltaWeight)), math::loadScalar((math::real)totalFlux));
	for (int i = 0; i < res; i++) {
		for (int j = 0; j < res; j++) {
			math::Vector xyzColor = m_diffractionPattern.get(i, j);
			xyzColor = math::mul(xyzColor, norm);

			ColorXyz xyzColorT = ColorXyz(math::getX(xyzColor), math::getY(xyzColor), math::getZ(xyzColor));
			math::Vector rgbColor = m_colorTable->xyzToRgb(xyzColorT);

			m_diffractionPattern.set(rgbColor, i, j);
		}
	}

	m_diffractionPattern.set(math::loadScalar((math::real)deltaWeight), 0, 0);
}

void manta::FraunhoferDiffraction::defaultSettings(Settings *settings) {
	settings->maxSamples = 4096;
	settings->textureSamples = 10;
	settings->maxWaveLength = 780;
	settings->minWaveLength = 380;
	settings->wavelengthStep = 5;
	settings->frequencyMultiplier = 3.0;
	settings->deltaWeight = (math::real_d)0.93;
}

#include <iostream>

void manta::FraunhoferDiffraction::generateMap(const CftEstimator2D *estimator, int startWavelength, int endWavelength, int wavelengthStep, int textureSamples, VectorMap2D *target) const {
	int res = target->getWidth();

	math::real_d sdx = m_sensorElementWidth;
	math::real_d sdy = m_sensorElementWidth;
	math::real_d scx = m_physicalSensorWidth / 2;
	math::real_d scy = m_physicalSensorWidth / 2;
	math::real_d maxFreqX = estimator->getHorizontalFreqRange();
	math::real_d maxFreqY = estimator->getVerticalFreqRange();

	int steps = (endWavelength - startWavelength) / wavelengthStep;
	Spectrum spectrum;
	spectrum.initialize(steps + 1, (math::real)startWavelength, (math::real)endWavelength, nullptr);

	for (int i = 0; i < res; i++) {
		math::real_d x = i * sdx - scx;

		for (int j = 0; j < res; j++) {
			math::real_d y = j * sdy - scy;
			spectrum.clear();

			for (int wavelength = startWavelength; wavelength <= endWavelength; wavelength += wavelengthStep) {
				math::real_d freqSpace = (sdx * 1E6 / wavelength) * 0.9;

				for (int s = 0; s < textureSamples; s++) {
					math::real randX = math::uniformRandom() - (math::real)0.5;
					math::real randY = math::uniformRandom() - (math::real)0.5;

					math::real_d freq_x = ((x * 1E6) / wavelength);
					math::real_d freq_y = ((y * 1E6) / wavelength);

					freq_x += randX * freqSpace;
					freq_y += randY * freqSpace;

					if (abs(freq_x) > maxFreqX) continue;
					if (abs(freq_y) > maxFreqY) continue;

					math::Complex v = estimator->sample(freq_x, freq_y, (sdx / wavelength) * 1E6);
					v = v * v.conjugate();

					math::real src = m_sourceSpectrum->getValueContinuous((math::real)wavelength);
					int wave_i = (int)((wavelength - startWavelength) / (math::real_d)wavelengthStep + 0.5);
					math::real prev = spectrum.getValueDiscrete(wave_i);
					spectrum.set(wave_i, prev + src * (math::real)v.r / (wavelength * wavelength));
				}
			}

			ColorXyz xyzColor = m_colorTable->spectralToXyz(&spectrum);
			target->set(
				math::loadVector(
					(math::real)xyzColor.x,
					(math::real)xyzColor.y,
					(math::real)xyzColor.z), i, j);
		}
	}

	spectrum.destroy();
}
