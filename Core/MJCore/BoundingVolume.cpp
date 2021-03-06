#include "BoundingVolume.h"
#include "Renderer.h"
#include "Object.h"
#include "Shader.h"
#include "Logger.h"
#include "RayCast.h"
#include "GraphicsDevice.h"

BoundingVolume::BoundingVolume(Object* pObject)
	: Component(pObject), pGameObject(nullptr)
{
	
}


bool BoundingVolume::Init(Renderer& renderer) {
	pGameObject = dynamic_cast<GameObject*>(pOwner);
	if (!pGameObject) {
		LogError(L"Target game object is null.\n");
		return false;
	}

	DefaultShader = std::make_shared<ColorShader>(pGameObject);
	if (!DefaultShader)
		return false;

	if (!DefaultShader->Initialize(renderer)) {
		LogError(L"Could not initialize the Default Shader\n");
		return false;
	}
}

void BoundingVolume::Render(Renderer& renderer, const Matrix<float, 4, 4>& viewMatrix, const Matrix<float, 4, 4>& projectionMatrix) {

}


BoundingBox::BoundingBox(Object* pOwner)
	:BoundingVolume(pOwner), pMesh(nullptr)
{

}

void BoundingBox::SetCenter(const Vec3f& _center) {
	center = _center;
}

void BoundingBox::SetExtent(const Vec3f& _extent) {
	extent = _extent;
}

const Vec3f& BoundingBox::GetCenter() const {
	return center;
}

const Vec3f& BoundingBox::GetExtent() const {
	return extent;
}

bool BoundingBox::Init(Renderer& renderer) {
	BoundingVolume::Init(renderer);

	MeshBuilder meshBuilder;
	meshBuilder.AddCube(center, Vec3f(1.0f, 1.0f, 1.0f), RGBA::GREEN);

	if (!pMesh)
		pMesh = std::make_shared<Mesh>();

	if (!pMesh)
		return false;

	meshBuilder.CopyToMesh(renderer, *pMesh, &OpenGL::BindBuffer, &Vertex::Copy, sizeof(Vertex));
}

bool BoundingBox::IsIn(const Vec3f& pos) {

	return true;
}

bool BoundingBox::IsIn(const Ray& ray) {
	return IntersectAABB(ray);
}

bool BoundingBox::IntersectAABB(const Ray& ray) {
	Vec3f min, max;
	GetMinMaxRange(min, max);

	float txMin = (min.x - ray.GetPosition().x) / ray.GetDirection().x;
	float txMax = (max.x - ray.GetPosition().x) / ray.GetDirection().x;

	if (txMax < txMin)
	{
		float temp = 0.0f;
		temp = txMin;
		txMin = txMax;
		txMax = temp;
	}

	float tyMin = (min.y - ray.GetPosition().y) / ray.GetDirection().y;
	float tyMax = (max.y - ray.GetPosition().y) / ray.GetDirection().y;

	if (tyMax < tyMin)
	{
		float temp = 0.0f;
		temp = tyMin;
		tyMin = tyMax;
		tyMax = temp;
	}

	if ((tyMax < txMin) || (txMax < tyMin))
		return false;

	if (txMin < tyMin)
		txMin = tyMin;

	float tzMin = (min.z - ray.GetPosition().z) / ray.GetDirection().z;
	float tzMax = (max.z - ray.GetPosition().z) / ray.GetDirection().z;

	if (tzMax < tzMin) {
		float temp = 0.0f;
		temp = tzMin;
		tzMin = tzMax;
		tzMax = temp;
	}

	if ((tzMax < txMin) || (txMax < tzMin))
		return false;

	if (txMin < tzMin)
		txMin = tzMin;

	if (tzMax < txMax)
		txMax = tzMax;

	return true;
}

Volumef BoundingBox::GetVolume() {
	Volumef volume;
	GetMinMaxRange(volume);
	return volume;
}

void BoundingBox::GetMinMaxRange(Vec3f& min, Vec3f& max) {
	min.x = center.x - fabs(extent.x);		//right
	min.y = center.y - fabs(extent.y);		//down
	min.z = center.z - fabs(extent.z);		//back

	max.x = center.x + fabs(extent.x);		//left
	max.y = center.y + fabs(extent.y);		//up
	max.z = center.z + fabs(extent.z);		//front
}

void BoundingBox::GetMinMaxRange(Volumef& volume) {
	GetMinMaxRange(volume.min, volume.max);
}

void BoundingBox::Render(Renderer& renderer, const Matrix<float, 4, 4>& viewMatrix, const Matrix<float, 4, 4>& projectionMatrix) {
	Renderer::ePrimitive oldDrawPrimitive = renderer.GetDrawMode();
	renderer.SetDrawMode(Renderer::ePrimitive::LINES);
	
	ShaderParameter shaderParam;
	Matrix<float, 4, 4> worldViewMatrix = Matrix<float, 4, 4>::Identity();

	Matrix<float, 4, 4> worldMatrix = pGameObject->GetWorldMatrix();
	MakeWorldViewMatrix(worldMatrix, viewMatrix, worldViewMatrix);
	shaderParam.worldViewMatrix = worldViewMatrix;
	shaderParam.projectionMatrix = projectionMatrix;
	
	DefaultShader->Render(renderer, shaderParam);
	pMesh->Render(renderer);

	renderer.SetDrawMode(oldDrawPrimitive);
}

BoundingSphere::BoundingSphere(Object* pOwner) 
	: BoundingVolume(pOwner)
{

}

bool BoundingSphere::Init(Renderer& renderer) {

	return true;
}

bool BoundingSphere::IsIn(const Ray& ray) {

	return true;
}
bool BoundingSphere::IsIn(const Vec3f& pos) {

	return true;
}

void BoundingSphere::Render(Renderer& renderer, const Matrix<float, 4, 4>& viewMatrix, const Matrix<float, 4, 4>& projectionMatrix) {
	renderer.SetDrawMode(Renderer::ePrimitive::LINES);

}

void BoundingSphere::SetCenter(const Vec3f& _center) {
	center = _center;
}

void BoundingSphere::SetRadius(float _radius) {
	radius = _radius;
}

void BoundingSphere::GetMinMaxRange(Vec3f& min, Vec3f& max) {
	min.x = center.x - radius;		//right
	min.y = center.y - radius;		//down
	min.z = center.z - radius;		//back

	max.x = center.x + radius;		//left
	max.y = center.y + radius;		//up
	max.z = center.z + radius;		//front
}

void BoundingSphere::GetMinMaxRange(Volumef& volume) {
	GetMinMaxRange(volume.min, volume.max);
}

Volumef BoundingSphere::GetVolume() {
	Volumef volume;
	GetMinMaxRange(volume);
	return volume;
}