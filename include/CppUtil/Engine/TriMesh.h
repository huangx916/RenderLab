#ifndef _ENGINE_PRIMITIVE_TRIMESH_H_
#define _ENGINE_PRIMITIVE_TRIMESH_H_

#include <CppUtil/Engine/Primitive.h>
#include <CppUtil/Engine/Triangle.h>

#include <CppUtil/Basic/UGM/Normal.h>
#include <CppUtil/Basic/UGM/Point2.h>

namespace CppUtil {
	namespace Engine {
		class TriMesh final : public Primitive {
		public:
			enum ENUM_TYPE
			{
				INVALID,
				CODE, // default
				CUBE,
				SPHERE,
				PLANE,
				DISK,
			};
		public:
			TriMesh(const std::vector<uint> & indice,
				const std::vector<Point3> & positions,
				const std::vector<Normalf> & normals,
				const std::vector<Point2> & texcoords,
				const std::vector<Normalf> & tangents = std::vector<Normalf>(),
				ENUM_TYPE type = ENUM_TYPE::CODE);

			TriMesh(uint triNum, uint vertexNum,
				const uint * indice,
				const float * positions,
				const float * normals,
				const float * texcoords,
				const float * tangents = nullptr,
				ENUM_TYPE type = ENUM_TYPE::CODE);

		public:
			static const Basic::Ptr<TriMesh> New(const std::vector<uint> & indice,
				const std::vector<Point3> & positions,
				const std::vector<Normalf> & normals,
				const std::vector<Point2> & texcoords,
				const std::vector<Normalf> & tangents = std::vector<Normalf>(),
				ENUM_TYPE type = ENUM_TYPE::CODE) {
				return Basic::New<TriMesh>(indice, positions, normals, texcoords, tangents, type);
			}

			static const Basic::Ptr<TriMesh> New(uint triNum, uint vertexNum,
				const uint * indice,
				const float * positions,
				const float * normals,
				const float * texcoords,
				const float * tangents = nullptr,
				ENUM_TYPE type = ENUM_TYPE::CODE) {
				return Basic::New<TriMesh>(triNum, vertexNum, indice, positions, normals, texcoords, tangents, type);
			}

		private:
			virtual ~TriMesh() = default;

		private:
			virtual void Init_AfterGenPtr() override;

		public:
			ENUM_TYPE GetType() const { return type; }

			const std::vector<Point3> & GetPositions() const { return positions; }
			const std::vector<Normalf> & GetNormals() const { return normals; }
			const std::vector<Point2> & GetTexcoords() const { return texcoords; }
			const std::vector<Normalf> & GetTangents() const { return tangents; }
			const std::vector<uint> & GetIndice() const { return indice; }
			const std::vector<Basic::Ptr<Triangle>> & GetTriangles() const { return triangles; }

		public:
			virtual const BBoxf GetBBox() const override {
				return box;
			}

		public:
			static const Basic::Ptr<TriMesh> GenCube();
			static const Basic::Ptr<TriMesh> GenSphere();
			static const Basic::Ptr<TriMesh> GenPlane();
			static const Basic::Ptr<TriMesh> GenDisk();

		private:
			void GenTangents();

		private:
			ENUM_TYPE type;

			std::vector<uint> indice;
			std::vector<Point3> positions;
			std::vector<Normalf> normals;
			std::vector<Point2> texcoords;
			std::vector<Normalf> tangents;

			std::vector<Basic::Ptr<Triangle>> triangles;

			BBoxf box;
		};
	}
}

#endif//!_ENGINE_PRIMITIVE_TRIMESH_H_
