#include <CppUtil/Engine/RayIntersector.h>

#include <CppUtil/Engine/SObj.h>
#include <CppUtil/Engine/Ray.h>

#include <CppUtil/Engine/BVHAccel.h>

#include <CppUtil/Engine/CmptGeometry.h>
#include <CppUtil/Engine/CmptTransform.h>

#include <CppUtil/Engine/Sphere.h>
#include <CppUtil/Engine/Plane.h>
#include <CppUtil/Engine/Triangle.h>
#include <CppUtil/Engine/TriMesh.h>
#include <CppUtil/Engine/Disk.h>
#include <CppUtil/Engine/Capsule.h>

#include <CppUtil/Basic/Math.h>
#include <CppUtil/Basic/UGM/Transform.h>

#include <stack>

using namespace CppUtil;
using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace std;

RayIntersector::RayIntersector() {
	RegMemberFunc<BVHAccel>(&RayIntersector::Visit);
	RegMemberFunc<SObj>(&RayIntersector::Visit);
	RegMemberFunc<Sphere>(&RayIntersector::Visit);
	RegMemberFunc<Plane>(&RayIntersector::Visit);
	RegMemberFunc<Triangle>(&RayIntersector::Visit);
	RegMemberFunc<TriMesh>(&RayIntersector::Visit);
	RegMemberFunc<Disk>(&RayIntersector::Visit);
	RegMemberFunc<Capsule>(&RayIntersector::Visit);
}

void RayIntersector::Init(ERay * ray) {
	this->ray = ray;

	rst.closestSObj = nullptr;
	rst.isIntersect = false;
}

bool RayIntersector::Intersect(const BBoxf & bbox, const Val3f & invDir) const {
	const auto & origin = ray->o;

	float tMin = ray->tMin;
	float tMax = ray->tMax;

	for (int i = 0; i < 3; i++) {
		float invD = invDir[i];
		float t0 = (bbox.minP[i] - origin[i]) * invD;
		float t1 = (bbox.maxP[i] - origin[i]) * invD;
		if (invD < 0.0f)
			swap(t0, t1);

		tMin = max(t0, tMin);
		tMax = min(t1, tMax);
		if (tMax < tMin)
			return false;
	}

	return true;
}

void RayIntersector::Visit(Ptr<BVHAccel> bvhAccel) {
	const auto visitor = This();

	const auto origin = ray->o;
	const auto dir = ray->d;
	const auto invDir = ray->InvDir();
	const bool dirIsNeg[3] = { invDir.x < 0,invDir.y < 0,invDir.z < 0 };

	stack<int> nodeIdxStack;
	nodeIdxStack.push(0);
	while (!nodeIdxStack.empty()) {
		const auto nodeIdx = nodeIdxStack.top();
		nodeIdxStack.pop();
		const auto & node = bvhAccel->GetBVHNode(nodeIdx);

		if (!Intersect(node.GetBox(), invDir))
			continue;
		
		if (node.IsLeaf()) {
			for (auto shapeIdx : node.ShapesIdx()) {
				const auto shape = bvhAccel->GetShape(shapeIdx);

				bvhAccel->GetShapeW2LMat(shape).ApplyTo(*ray);
				shape->Accept(visitor);
				ray->o = origin;
				ray->d = dir;

				if (rst.isIntersect) {
					rst.closestSObj = bvhAccel->GetSObj(shape);
					rst.isIntersect = false;
				}
			}
		}
		else {
			const auto firstChildIdx = BVHAccel::LinearBVHNode::FirstChildIdx(nodeIdx);
			const auto secondChildIdx = node.GetSecondChildIdx();
			if (dirIsNeg[node.GetAxis()]) {
				nodeIdxStack.push(firstChildIdx);
				nodeIdxStack.push(secondChildIdx);
			}
			else {
				nodeIdxStack.push(secondChildIdx);
				nodeIdxStack.push(firstChildIdx);
			}
		}
	}

	if (rst.closestSObj) {
		const Ptr<Shape> shape = rst.closestSObj->GetComponent<CmptGeometry>()->primitive;
		const auto l2w = bvhAccel->GetShapeW2LMat(shape).Inverse();
		rst.n = l2w(rst.n).Normalize();
		rst.tangent = l2w(rst.tangent).Normalize();
	}
}

void RayIntersector::Visit(Ptr<SObj> sobj) {
	auto geometry = sobj->GetComponent<CmptGeometry>();
	auto children = sobj->GetChildren();

	if ((geometry == nullptr || !geometry->primitive) && children.size() == 0)
		return;

	auto origSObj = rst.closestSObj;
	auto cmptTransform = sobj->GetComponent<CmptTransform>();
	if (cmptTransform)
		cmptTransform->GetTransform().Inverse().ApplyTo(*ray);

	if (geometry && geometry->primitive) {
		geometry->primitive->Accept(This());
		if (rst.isIntersect)
			rst.closestSObj = sobj;
	}

	for (auto child : children)
		child->Accept(This());

	if (cmptTransform) {
		cmptTransform->GetTransform().ApplyTo(*ray);
		if (rst.closestSObj != origSObj) {
			cmptTransform->GetTransform().ApplyTo(rst.n).NormalizeSelf();
			cmptTransform->GetTransform().ApplyTo(rst.tangent).NormalizeSelf();
		}
	}
}

void RayIntersector::Visit(Ptr<Sphere> sphere) {
	const auto & dir = ray->d;
	const auto & origin = ray->o;

	const Vec3 oc = origin;
	const float a = dir.Dot(dir);
	const float b = oc.Dot(dir);
	const float c = oc.Dot(oc) - 1;
	const float discriminant = b * b - a * c;

	if (discriminant < 0) {
		rst.isIntersect = false;
		return;
	}

	const float tMin = ray->tMin;
	const float tMax = ray->tMax;
	const float sqrt_discriminant = sqrt(discriminant);
	const float inv_a = 1.0f / a;

	float t = - (b + sqrt_discriminant) * inv_a;
	if (t > tMax || t < tMin) {
		t = (-b + sqrt_discriminant) * inv_a;
		if (t > tMax || t < tMin) {
			rst.isIntersect = false;
			return;
		}
	}

	rst.isIntersect = true;
	ray->tMax = t;
	rst.n = ray->At(t);
	rst.texcoord = Sphere::TexcoordOf(rst.n);
	rst.tangent = Sphere::TangentOf(rst.n);
}

void RayIntersector::Visit(Ptr<Plane> plane) {
	const float t = -ray->o.y / ray->d.y;
	if (t<ray->tMin || t > ray->tMax) {
		rst.isIntersect = false;
		return;
	}

	const auto pos = ray->At(t);
	if (pos.x<-0.5 || pos.x>0.5 || pos.z<-0.5 || pos.z>0.5) {
		rst.isIntersect = false;
		return;
	}

	rst.isIntersect = true;
	ray->tMax = t;
	//rst.n = Normalf(0, -Math::sgn(ray->d.y), 0);
	rst.n = Normalf(0, 1, 0);
	rst.texcoord = Point2(pos.x + 0.5f, pos.z + 0.5f);
	rst.tangent = Normalf(1, 0, 0);
}

void RayIntersector::Visit(Ptr<Triangle> triangle) {
	const auto mesh = triangle->GetMesh();
	const int idx1 = triangle->idx[0];
	const int idx2 = triangle->idx[1];
	const int idx3 = triangle->idx[2];

	const auto & positions = mesh->GetPositions();
	const auto & p1 = positions[idx1];
	const auto & p2 = positions[idx2];
	const auto & p3 = positions[idx3];

	const auto & dir = ray->d;

	const auto e1 = p2 - p1;
	const auto e2 = p3 - p1;

	const auto e1_x_d = e1.Cross(dir);
	const float denominator = e1_x_d.Dot(e2);

	if (denominator == 0) {
		rst.isIntersect = false;
		return;
	}

	const float inv_denominator = 1.0f / denominator;

	const auto s = ray->o - p1;

	const auto e2_x_s = e2.Cross(s);
	const float r1 = e2_x_s.Dot(dir);
	const float u = r1 * inv_denominator;
	if (u < 0 || u > 1) {
		rst.isIntersect = false;
		return;
	}

	const float r2 = e1_x_d.Dot(s);
	const float v = r2 * inv_denominator;
	if (v < 0 || v > 1) {
		rst.isIntersect = false;
		return;
	}

	const float u_plus_v = u + v;
	if (u_plus_v > 1) {
		rst.isIntersect = false;
		return;
	}

	const float r3 = e2_x_s.Dot(e1);
	const float t = r3 * inv_denominator;

	if (t < ray->tMin || t > ray->tMax) {
		rst.isIntersect = false;
		return;
	}

	rst.isIntersect = true;

	ray->tMax = t;

	const float w = 1 - u_plus_v;

	// normal
	const auto & normals = mesh->GetNormals();
	const auto & n1 = normals[idx1];
	const auto & n2 = normals[idx2];
	const auto & n3 = normals[idx3];

	rst.n = (w * n1 + u * n2 + v * n3).Normalize();

	// texcoord
	const auto & texcoords = mesh->GetTexcoords();
	const auto & tc1 = texcoords[idx1];
	const auto & tc2 = texcoords[idx2];
	const auto & tc3 = texcoords[idx3];

	rst.texcoord.x = w * tc1.x + u * tc2.x + v * tc3.x;
	rst.texcoord.y = w * tc1.y + u * tc2.y + v * tc3.y;

	// tangent
	const auto & tangents = mesh->GetTangents();
	const auto & tg1 = tangents[idx1];
	const auto & tg2 = tangents[idx2];
	const auto & tg3 = tangents[idx3];

	rst.tangent = (w * tg1 + u * tg2 + v * tg3).Normalize();
}

void RayIntersector::Visit(Ptr<TriMesh> mesh) {
	const auto visitor = This();
	for (auto triangle : mesh->GetTriangles()) {
		triangle->Accept(visitor);
		if (rst.isIntersect) {
			return;
		}
	}
}

void RayIntersector::Visit(Ptr<Disk> disk) {
	const float t = -ray->o.y / ray->d.y;
	if (t < ray->tMin || t > ray->tMax) {
		rst.isIntersect = false;
		return;
	}

	const auto pos = ray->At(t);
	if (Vec3f(pos).Norm2() >= 1.f) {
		rst.isIntersect = false;
		return;
	}

	rst.isIntersect = true;
	ray->tMax = t;
	//rst.n = Normalf(0, -Math::sgn(ray->d.y), 0);
	rst.n = Normalf(0, 1, 0);
	rst.texcoord = Point2((1+pos.x)/2, (1+pos.z)/2);
	rst.tangent = Normalf(1, 0, 0);
}

void RayIntersector::Visit(Ptr<Capsule> capsule) {
	const auto & o = ray->o;
	const auto & d = ray->d;

	float halfH = capsule->height / 2;

	do{ // Բ��
		float a = d.x * d.x + d.z * d.z;
		float b = d.x * o.x + d.z * o.z;
		float c = o.x * o.x + o.z * o.z - 1;

		float discriminant = b * b - a * c;
		if (discriminant <= 0) {
			rst.isIntersect = false;
			return;
		}

		float sqrtDiscriminant = sqrt(discriminant);
		float t = -(b + sqrtDiscriminant) / a;
		if (t < ray->tMin || t > ray->tMax) {
			t = (sqrtDiscriminant - b) / a;
			if (t < ray->tMin || t > ray->tMax)
				break;
		}

		auto pos = ray->At(t);
		if (pos.y <= -halfH || pos.y >= halfH)
			break;

		rst.isIntersect = true;
		ray->tMax = t;
		rst.n = Normalf(pos.x, 0, pos.z);
		rst.texcoord = Sphere::TexcoordOf(Vec3f(pos));
		rst.tangent = Sphere::TangentOf(Vec3f(pos));
		return;
	} while (false);

	float a = d.Dot(d);

	do{// �ϰ���
		Point3 center(0, halfH, 0);
		auto oc = o - center;
		float b = d.Dot(oc);
		float c = oc.Norm2() - 1;

		float discriminant = b * b - a * c;
		if (discriminant <= 0)
			break;

		float sqrtDiscriminant = sqrt(discriminant);
		float t = -(b + sqrtDiscriminant) / a;
		auto pos = ray->At(t);
		if (t < ray->tMin || t > ray->tMax || pos.y <= halfH) {
			t = (sqrtDiscriminant - b) / a;
			pos = ray->At(t);
			if (t < ray->tMin || t > ray->tMax || pos.y <= halfH)
				break;
		}
		
		rst.isIntersect = true;
		ray->tMax = t;
		rst.n = pos - center;
		rst.texcoord = Sphere::TexcoordOf(Vec3f(pos));
		rst.tangent = Sphere::TangentOf(Vec3f(pos));
		return;
	} while (false);

	do {// �°���
		Point3 center(0, -halfH, 0);
		auto oc = o - center;
		float b = d.Dot(oc);
		float c = oc.Norm2() - 1;

		float discriminant = b * b - a * c;
		if (discriminant <= 0)
			break;

		float sqrtDiscriminant = sqrt(discriminant);
		float t = -(b + sqrtDiscriminant) / a;
		auto pos = ray->At(t);
		if (t < ray->tMin || t > ray->tMax || pos.y >= -halfH) {
			t = (sqrtDiscriminant - b) / a;
			pos = ray->At(t);
			if (t < ray->tMin || t > ray->tMax || pos.y >= -halfH)
				break;
		}

		rst.isIntersect = true;
		ray->tMax = t;
		rst.n = pos - center;
		rst.texcoord = Sphere::TexcoordOf(Vec3f(pos));
		rst.tangent = Sphere::TangentOf(Vec3f(pos));
		return;
	} while (false);

	rst.isIntersect = false;
	return;
}
