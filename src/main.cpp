#include <iostream>

#include <light_ray.h>
#include <sphere_primitive.h>
#include <intersection_point.h>
#include <scene.h>
#include <scene_object.h>
#include <scene_geometry.h>
#include <material.h>
#include <simple_diffuse_material.h>
#include <simple_specular_diffuse_material.h>
#include <ray_tracer.h>
#include <camera_ray_emitter_group.h>
#include <camera_ray_emitter.h>
#include <image_handling.h>
#include <memory_management.h>
#include <mesh.h>
#include <obj_file_loader.h>

#include <manta_math.h>

using namespace manta;

math::Vector getColor(int r, int g, int b) {
	return math::loadVector(r / 255.0f, g / 255.0f, b / 255.0f);
}

int main() {
	ObjFileLoader objFile;
	bool result = objFile.readObjFile("teapot.obj");

	Scene scene;

	SimpleSpecularDiffuseMaterial leftWallMaterial;
	SimpleSpecularDiffuseMaterial rightWallMaterial;
	SimpleSpecularDiffuseMaterial backWallMaterial;
	SimpleSpecularDiffuseMaterial sphere1Material;
	SimpleSpecularDiffuseMaterial sphere2Material;
	SimpleSpecularDiffuseMaterial lightMaterial;
	SimpleSpecularDiffuseMaterial floorMaterial;
	SimpleSpecularDiffuseMaterial mirrorMaterial;
	SimpleSpecularDiffuseMaterial orbMaterial;

	leftWallMaterial.setEmission(math::loadVector(0.0f, 0.0f, 0.0f));
	leftWallMaterial.setDiffuseColor(getColor(0xf7, 0xd0, 0x8a));
	leftWallMaterial.setSpecularColor(getColor(0, 0, 0));

	rightWallMaterial.setEmission(math::loadVector(0.0f, 0.0f, 0.0f));
	rightWallMaterial.setDiffuseColor(getColor(0x3F, 0x51, 0xB5));
	rightWallMaterial.setDiffuseColor(getColor(0x87, 0xB6, 0xA7));
	rightWallMaterial.setDiffuseColor(getColor(0x40, 0xC4, 0xFF));
	rightWallMaterial.setSpecularColor(getColor(0, 0, 0));

	backWallMaterial.setEmission(math::loadVector(0.0f, 0.0f, 0.0f));
	backWallMaterial.setDiffuseColor(getColor(200, 200, 200));
	backWallMaterial.setSpecularColor(getColor(0, 0, 0));

	sphere1Material.setEmission(math::loadVector(1.9f, 1.2f, 1.2f));
	sphere1Material.setDiffuseColor(getColor(228, 0, 25));
	sphere1Material.setSpecularColor(getColor(90, 90, 90));

	//orbMaterial.setEmission(math::loadVector(0.9f, 0.2f, 0.2f));
	//orbMaterial.setDiffuseColor(getColor(228, 0, 25));
	//orbMaterial.setSpecularColor(getColor(200, 200, 200));
	orbMaterial.setEmission(math::loadVector(0.0f, 0.0f, 0.0f));
	orbMaterial.setDiffuseColor(getColor(0, 0, 0));
	orbMaterial.setSpecularColor(getColor(20, 20, 20));

	mirrorMaterial.setEmission(math::loadVector(10.75f, 10.75f, 10.75f));
	//mirrorMaterial.setDiffuseColor(getColor(0xe3, 0xf0, 0x9b));
	//mirrorMaterial.setSpecularColor(getColor(100, 100, 100));
	mirrorMaterial.setDiffuseColor(getColor(0, 0, 0));
	mirrorMaterial.setSpecularColor(getColor(0, 0, 0));

	sphere2Material.setEmission(math::loadVector(0.0f, 0.0f, 0.0f));
	//sphere2Material.setDiffuseColor(getColor(0xe3, 0xf0, 0x9b));
	//sphere2Material.setSpecularColor(getColor(100, 100, 100));
	sphere2Material.setDiffuseColor(getColor(0, 0, 0));
	sphere2Material.setSpecularColor(getColor(0xFF, 0xFF, 0xFF));

	lightMaterial.setEmission(math::loadVector(1.2f, 1.2f, 1.2f));
	lightMaterial.setDiffuseColor(math::loadVector(1.0f, 1.0f, 1.0f));
	lightMaterial.setSpecularColor(getColor(0, 0, 0));

	floorMaterial.setEmission(math::loadVector(0.0f, 0.0f, 0.0f));
	floorMaterial.setDiffuseColor(getColor(200, 200, 200));
	floorMaterial.setSpecularColor(getColor(0, 0, 0));

	const float sphereSize = 10000.0f;
	const float roomSize = 5.0f;

	SpherePrimitive backWall;
	backWall.setRadius(sphereSize);
	backWall.setPosition(math::loadVector(-sphereSize-roomSize, 0.0f, 0.0f));

	SpherePrimitive leftWall;
	leftWall.setRadius(sphereSize);
	leftWall.setPosition(math::loadVector(0.0f, 0.0f, -sphereSize-roomSize));

	SpherePrimitive rightWall;
	rightWall.setRadius(sphereSize);
	rightWall.setPosition(math::loadVector(0.0f, 0.0f, sphereSize+roomSize));

	SpherePrimitive ceiling;
	ceiling.setRadius(sphereSize);
	ceiling.setPosition(math::loadVector(0.0f, sphereSize+roomSize*2, 0.0f));

	SpherePrimitive sphere;
	sphere.setRadius(1.0f);
	sphere.setPosition(math::loadVector(3.0f, 1.0f, 1.0f));

	const math::real G = 1.618;

	SpherePrimitive sphere2;
	math::real s2_r = G * 1.0;
	sphere2.setRadius(s2_r);
	sphere2.setPosition(math::loadVector(0.0f, s2_r, -2.5f));

	SpherePrimitive sphere3;
	math::real s3_r = G * s2_r;
	sphere3.setRadius(s3_r);
	sphere3.setPosition(math::loadVector(-roomSize + s3_r + 0.5, s3_r, roomSize - s3_r - 0.5));

	SpherePrimitive floor;
	floor.setRadius(sphereSize);
	floor.setPosition(math::loadVector(0.0f, -sphereSize, 0.0f));

	SpherePrimitive light;
	light.setRadius(10.0f);
	light.setPosition(math::loadVector(0.0f, roomSize*2 + 9.75f, 0.0f));

	SpherePrimitive mirror;
	math::real mirrorSize = (math::real)500.0;
	mirror.setRadius(mirrorSize);
	mirror.setPosition(math::loadVector(0.0f, -mirrorSize + 0.0025, 0.0f));

	SpherePrimitive orb;
	math::real ringSize = 300;
	orb.setRadius(ringSize);
	orb.setPosition(math::loadVector(0, ringSize - 0.013 + roomSize * 2, 0));

	// Teapot test
	Mesh teapot;
	teapot.initialize(objFile.getFaceCount(), objFile.getVertexCount());

	Face *tFaces = teapot.getFaces();
	SimpleVertex *tVerts = teapot.getVertices();

	for (unsigned int i = 0; i < objFile.getFaceCount(); i++) {
		ObjFace *face = objFile.getFace(i);
		tFaces[i].u = face->v1 - 1;
		tFaces[i].v = face->v3 - 1;
		tFaces[i].w = face->v2 - 1;
	}

	for (unsigned int i = 0; i < objFile.getVertexCount(); i++) {
		math::Vector3 *v = objFile.getVertex(i);
		tVerts[i].location = math::loadVector(*v);
	}
	teapot.precomputeValues();
	teapot.setFastIntersectEnabled(true);
	teapot.setFastIntersectRadius((math::real)4.0);

	// Outdoor light
	Mesh mesh;
	mesh.initialize(2, 4);

	Face *faces = mesh.getFaces();
	SimpleVertex *vertices = mesh.getVertices();

	faces[0].u = 2;
	faces[0].v = 1;
	faces[0].w = 0;

	faces[1].u = 0;
	faces[1].v = 3;
	faces[1].w = 2;

	vertices[0].location = math::loadVector(-roomSize + 4, 3.0, -roomSize - 70);
	vertices[1].location = math::loadVector(-roomSize + 4 + 25, 3.0, -roomSize - 70);
	vertices[2].location = math::loadVector(-roomSize + 4 + 25, roomSize * 2, -roomSize);
	vertices[3].location = math::loadVector(-roomSize + 4, roomSize * 2, -roomSize);

	mesh.precomputeValues();
	// END PLANE

	// BACK WALL MESH
	Mesh backWallMesh;
	backWallMesh.initialize(2, 4);

	faces = backWallMesh.getFaces();
	vertices = backWallMesh.getVertices();

	faces[0].u = 0;
	faces[0].v = 1;
	faces[0].w = 2;

	faces[1].u = 2;
	faces[1].v = 3;
	faces[1].w = 0;

	vertices[0].location = math::loadVector(-roomSize, -100.0, -roomSize);
	vertices[1].location = math::loadVector(roomSize*100, -100.0, -roomSize);
	vertices[2].location = math::loadVector(roomSize*100, 3.0, -roomSize);
	vertices[3].location = math::loadVector(-roomSize, 3.0, -roomSize);

	backWallMesh.precomputeValues();
	// END PLANE

	// BACK WALL TOP MESH
	Mesh backWallTopMesh;
	backWallTopMesh.initialize(2, 4);

	faces = backWallTopMesh.getFaces();
	vertices = backWallTopMesh.getVertices();

	faces[0].u = 0;
	faces[0].v = 1;
	faces[0].w = 2;

	faces[1].u = 0;
	faces[1].v = 2;
	faces[1].w = 3;

	vertices[0].location = math::loadVector(-roomSize, roomSize * 2 - 3.0, -roomSize);
	vertices[1].location = math::loadVector(roomSize*100, roomSize * 2 - 3.0, -roomSize);
	vertices[2].location = math::loadVector(roomSize*100, 100, -roomSize);
	vertices[3].location = math::loadVector(-roomSize, 100, -roomSize);

	backWallTopMesh.precomputeValues();
	// END PLANE

	constexpr math::real windowFromBackWall = 4.0;
	constexpr math::real windowWidth = 4.0;
	constexpr math::real windowBarWidth = 0.35;

	// RIGHT WALL BACK MESH
	Mesh rightWallBack;
	rightWallBack.initialize(2, 4);

	faces = rightWallBack.getFaces();
	vertices = rightWallBack.getVertices();

	faces[0].u = 0;
	faces[0].v = 1;
	faces[0].w = 2;

	faces[1].u = 0;
	faces[1].v = 2;
	faces[1].w = 3;

	vertices[0].location = math::loadVector(-roomSize-10, 0.0, -roomSize);
	vertices[1].location = math::loadVector(-roomSize + windowFromBackWall, 0.0, -roomSize);
	vertices[2].location = math::loadVector(-roomSize + windowFromBackWall, roomSize * 2, -roomSize);
	vertices[3].location = math::loadVector(-roomSize-10, roomSize * 2, -roomSize);

	rightWallBack.precomputeValues();
	// END PLANE

	// RIGHT WALL FRONT MESH
	Mesh rightWallFront;
	rightWallFront.initialize(2, 4);

	faces = rightWallFront.getFaces();
	vertices = rightWallFront.getVertices();

	faces[0].u = 0;
	faces[0].v = 1;
	faces[0].w = 2;

	faces[1].u = 0;
	faces[1].v = 2;
	faces[1].w = 3;

	vertices[0].location = math::loadVector(-roomSize + windowWidth + windowFromBackWall, -100.0, -roomSize);
	vertices[1].location = math::loadVector(-roomSize + 200, -100.0, -roomSize);
	vertices[2].location = math::loadVector(-roomSize + 200, roomSize * 2, -roomSize);
	vertices[3].location = math::loadVector(-roomSize + windowWidth + windowFromBackWall, 100, -roomSize);

	rightWallFront.precomputeValues();
	// END PLANE

	// VERTICAL BAR
	Mesh windowBarOne;
	windowBarOne.initialize(2, 4);

	faces = windowBarOne.getFaces();
	vertices = windowBarOne.getVertices();

	faces[0].u = 0;
	faces[0].v = 1;
	faces[0].w = 2;

	faces[1].u = 0;
	faces[1].v = 2;
	faces[1].w = 3;

	vertices[0].location = math::loadVector(-roomSize + windowWidth/2 + windowFromBackWall - windowBarWidth, 0.0, -roomSize);
	vertices[1].location = math::loadVector(-roomSize + windowWidth / 2 + windowFromBackWall + windowBarWidth, 0.0, -roomSize);
	vertices[2].location = math::loadVector(-roomSize + windowWidth / 2 + windowFromBackWall + windowBarWidth, roomSize * 2, -roomSize);
	vertices[3].location = math::loadVector(-roomSize + windowWidth / 2 + windowFromBackWall - windowBarWidth, roomSize * 2, -roomSize);

	windowBarOne.precomputeValues();
	// END PLANE

	// HORIZONTAL BAR
	Mesh windowBarTwo;
	windowBarTwo.initialize(2, 4);

	faces = windowBarTwo.getFaces();
	vertices = windowBarTwo.getVertices();

	faces[0].u = 0;
	faces[0].v = 1;
	faces[0].w = 2;

	faces[1].u = 0;
	faces[1].v = 2;
	faces[1].w = 3;

	vertices[0].location = math::loadVector(-roomSize + windowFromBackWall - windowBarWidth, roomSize - windowBarWidth, -roomSize);
	vertices[1].location = math::loadVector(-roomSize + windowWidth + windowFromBackWall, roomSize - windowBarWidth, -roomSize);
	vertices[2].location = math::loadVector(-roomSize + windowWidth + windowFromBackWall, roomSize + windowBarWidth, -roomSize);
	vertices[3].location = math::loadVector(-roomSize + windowFromBackWall - windowBarWidth, roomSize + windowBarWidth, -roomSize);

	windowBarTwo.precomputeValues();
	// END PLANE

	//SceneObject *mirrorObject = scene.createSceneObject();
	//mirrorObject->setMaterial(&mirrorMaterial);
	//mirrorObject->setGeometry(&mirror);

	//SceneObject *orbObject = scene.createSceneObject();
	//orbObject->setMaterial(&floorMaterial);
	//orbObject->setGeometry(&orb);

	SceneObject *planeObject = scene.createSceneObject();
	planeObject->setMaterial(&mirrorMaterial);
	planeObject->setGeometry(&mesh);

	SceneObject *backWallObject = scene.createSceneObject();
	backWallObject->setMaterial(&backWallMaterial);
	backWallObject->setGeometry(&backWall);

	SceneObject *rightWallBottom = scene.createSceneObject();
	rightWallBottom->setMaterial(&leftWallMaterial);
	rightWallBottom->setGeometry(&backWallMesh);

	SceneObject *rightWallTop = scene.createSceneObject();
	rightWallTop->setMaterial(&leftWallMaterial);
	rightWallTop->setGeometry(&backWallTopMesh);

	SceneObject *rightWallBackObject= scene.createSceneObject();
	rightWallBackObject->setMaterial(&leftWallMaterial);
	rightWallBackObject->setGeometry(&rightWallBack);

	SceneObject *rightWallObjectFront = scene.createSceneObject();
	rightWallObjectFront->setMaterial(&leftWallMaterial);
	rightWallObjectFront->setGeometry(&rightWallFront);

	SceneObject *windowBarOneObject = scene.createSceneObject();
	windowBarOneObject->setMaterial(&leftWallMaterial);
	windowBarOneObject->setGeometry(&windowBarOne);

	SceneObject *windowBarTwoObject = scene.createSceneObject();
	windowBarTwoObject->setMaterial(&leftWallMaterial);
	windowBarTwoObject->setGeometry(&windowBarTwo);

	SceneObject *rightWallObject = scene.createSceneObject();
	rightWallObject->setMaterial(&rightWallMaterial);
	rightWallObject->setGeometry(&rightWall);

	//SceneObject *leftWallObject = scene.createSceneObject();
	//leftWallObject->setMaterial(&leftWallMaterial);
	//leftWallObject->setGeometry(&leftWall);

	//SceneObject *lightObject = scene.createSceneObject();
	//lightObject->setMaterial(&lightMaterial);
	//lightObject->setGeometry(&light);

	//SceneObject *sphereObject = scene.createSceneObject();
	//sphereObject->setMaterial(&leftWallMaterial);
	//sphereObject->setGeometry(&sphere);

	//SceneObject *sphere2Object = scene.createSceneObject();
	//sphere2Object->setMaterial(&rightWallMaterial);
	//sphere2Object->setGeometry(&sphere2);

	//SceneObject *sphere3Object = scene.createSceneObject();
	//sphere3Object->setMaterial(&sphere2Material);
	//sphere3Object->setGeometry(&sphere3);

	SceneObject *teapotObject = scene.createSceneObject();
	teapotObject->setMaterial(&sphere2Material);
	teapotObject->setGeometry(&teapot);

	SceneObject *floorObject = scene.createSceneObject();
	floorObject->setMaterial(&floorMaterial);
	floorObject->setGeometry(&floor);

	SceneObject *ceilingObject = scene.createSceneObject();
	ceilingObject->setMaterial(&floorMaterial);
	ceilingObject->setGeometry(&ceiling);

	int width = 1024;
	int height = 768;

	//width = 300;
	//height = 300;

	CameraRayEmitterGroup camera;
	camera.setSamplingWidth(2);
	camera.setDirection(math::loadVector(-1.0f, 0.0f, 0.0f));
	camera.setPosition(math::loadVector(15.0f, roomSize - 1.0, 0.0f));
	camera.setUp(math::loadVector(0.0f, 1.0f, 0.0f));
	camera.setPlaneDistance(1.0f);
	camera.setPlaneHeight(1.0f);
	camera.setResolutionX(width);
	camera.setResolutionY(height);
	camera.setSamplesPerPixel(1);


	//CameraRayEmitter cameraEmitter;
	//cameraEmitter.setDegree(0);
	//cameraEmitter.setSamplesPerRay(1000);

	//cameraEmitter.generateRays();

	//StackAllocator *mainAllocator = new StackAllocator;
	//mainAllocator->initialize(500 * MB);

	RayTracer rayTracer;
	//rayTracer.setThreadCount(12);
	//rayTracer.initializeAllocators(500 * MB, 50 * MB);
	//rayTracer.initializeAllocators(500 * MB, 50 * MB);
	rayTracer.initialize(500 * MB, 50 * MB, 12, 10000, true);
	rayTracer.traceAll(&scene, &camera);

	math::Vector *pixels = (math::Vector *)_aligned_malloc(sizeof(math::Vector) * width * height, 16);

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			math::Vector v = ((CameraRayEmitter *)(camera.getEmitters()[i * width + j]))->getIntensity();
			math::real r = math::getX(v);
			math::real g = math::getY(v);
			math::real b = math::getZ(v);

			if (r > 0.0 || g > 0.0 || b > 0.0) {
				//std::cout << "+";
			} 
			else {
				//std::cout << " ";
			}

			pixels[i * width + j] = v;
		}
		//std::cout << std::endl;
	}

	// Destroy all emitters
	for (int i = camera.getEmitterCount() - 1; i >= 0; i--) {
		((CameraRayEmitter *)(camera.getEmitters()[i]))->destroyRays();
	}

	SaveImageData(pixels, width, height, "test.bmp");
	camera.destroyEmitters();

	rayTracer.destroy();

	int a = 0;
}