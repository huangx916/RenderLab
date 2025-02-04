#ifndef _CPPUTIL_ENGINE_SCENE_SL_COMMON_H_
#define _CPPUTIL_ENGINE_SCENE_SL_COMMON_H_

#include <CppUtil/Engine/SObj.h>

#include <CppUtil/Engine/AllComponents.h>

#include <CppUtil/Engine/Sphere.h>
#include <CppUtil/Engine/Plane.h>
#include <CppUtil/Engine/TriMesh.h>
#include <CppUtil/Engine/Capsule.h>
#include <CppUtil/Engine/Disk.h>

#include <CppUtil/Engine/AreaLight.h>
#include <CppUtil/Engine/PointLight.h>
#include <CppUtil/Engine/DirectionalLight.h>
#include <CppUtil/Engine/SpotLight.h>
#include <CppUtil/Engine/InfiniteAreaLight.h>
#include <CppUtil/Engine/SphereLight.h>
#include <CppUtil/Engine/DiskLight.h>
#include <CppUtil/Engine/CapsuleLight.h>

#include <CppUtil/Engine/AllBSDFs.h>
#include <CppUtil/Engine/Gooch.h>

#include <CppUtil/Basic/Image.h>

#include <functional>

namespace str {
	// ------------- SObj

	namespace SObj {
		const char * const type = "SObj";

		const char * const name = "name";
		const char * const components = "components";
		const char * const children = "children";
	}

	// component

	// ------------- Camera

	namespace CmptCamera {
		const char * const type = "CmptCamera";

		const char * const fov = "fov";
		const char * const ar = "ar";
		const char * const nearPlane = "nearPlane";
		const char * const farPlane = "farPlane";
	}

	// ------------- Geometry

	namespace CmptGeometry {
		const char * const type = "CmptGeometry";
		const char * const primitive = "primitive";
	}

	namespace Sphere {
		const char * const type = "Sphere";
	}

	namespace Plane {
		const char * const type = "Plane";
	}

	namespace TriMesh {
		const char * const type = "TriMesh";

		namespace ENUM_TYPE {
			const char * const INVALID = "invalid";
			const char * const CODE = "code";
			const char * const CUBE = "cube";
			const char * const SPHERE = "sphere";
			const char * const PLANE = "plane";
			const char * const FILE = "file";
			const char * const DISK = "disk";
		}
	}

	namespace Capsule {
		const char * const type = "Capsule";

		const char * const height = "height";
	}

	namespace Disk {
		const char * const type = "Disk";
	}

	// ------------- Light

	namespace CmptLight {
		const char * const type = "CmptLight";
		
		const char * const light = "light";
	}

	namespace AreaLight {
		const char * const type = "Area_Light";

		const char * const color = "color";
		const char * const intensity = "intensity";
		const char * const width = "width";
		const char * const height = "height";
	}

	namespace PointLight {
		const char * const type = "Point_Light";

		const char * const color = "color";
		const char * const intensity = "intensity";
		const char * const radius = "radius";
	}

	namespace DirectionalLight {
		const char * const type = "Directional_Light";

		const char * const color = "color";
		const char * const intensity = "intensity";
	}

	namespace SpotLight {
		const char * const type = "Spot_Light";

		const char * const color = "color";
		const char * const intensity = "intensity";
		const char * const radius = "radius";
		const char * const angle = "angle";
		const char * const fullRatio = "fullRatio";
	}

	namespace InfiniteAreaLight {
		const char * const type = "Infinite_Area_Light";
		
		const char * const colorFactor = "colorFactor";
		const char * const intensity = "intensity";
		const char * const img = "img";
	}

	namespace SphereLight {
		const char * const type = "Sphere_Light";

		const char * const color = "color";
		const char * const intensity = "intensity";
		const char * const radius = "radius";
	}

	namespace DiskLight {
		const char * const type = "Disk_Light";

		const char * const color = "color";
		const char * const intensity = "intensity";
		const char * const radius = "radius";
	}

	namespace CapsuleLight {
		const char * const type = "Capsule_Light";

		const char * const color = "color";
		const char * const intensity = "intensity";
		const char * const radius = "radius";
		const char * const height = "height";
	}

	// ------------- Material

	namespace CmptMaterial {
		const char * const type = "CmptMaterial";

		const char * const material = "material";
	}

	// ------------- Transform

	namespace CmptTransform {
		const char * const type = "CmptTransform";

		const char * const Position = "Position";
		const char * const Rotation = "Rotation";
		const char * const Scale = "Scale";
	}

	namespace BSDF_CookTorrance {
		const char * const type = "BSDF_CookTorrance";

		const char * const ior = "ior";
		const char * const roughness = "roughness";
		const char * const refletance = "refletance";
		const char * const albedo = "albedo";
	}

	namespace BSDF_Diffuse {
		const char * const type = "BSDF_Diffuse";

		const char * const colorFactor = "colorFactor";
		const char * const albedoTexture = "albedoTexture";
	}

	namespace BSDF_Emission {
		const char * const type = "BSDF_Emission";

		const char * const color = "color";
		const char * const intensity = "intensity";
	}

	namespace BSDF_Glass {
		const char * const type = "BSDF_Glass";

		const char * const ior = "ior";
		const char * const transmittance = "transmittance";
		const char * const reflectance = "reflectance";
	}

	namespace Image {
		const char * const path = "path";
	}

	namespace BSDF_MetalWorkflow {
		const char * const type = "BSDF_MetalWorkflow";

		const char * const colorFactor = "colorFactor";
		const char * const albedoTexture = "albedoTexture";
		const char * const metallicFactor = "metallicFactor";
		const char * const metallicTexture = "metallicTexture";
		const char * const roughnessFactor = "roughnessFactor";
		const char * const roughnessTexture = "roughnessTexture";
		const char * const aoTexture = "aoTexture";
		const char * const normalTexture = "normalTexture";
	}

	namespace BSDF_FrostedGlass {
		const char * const type = "BSDF_FrostedGlass";

		const char * const IOR = "IOR";
		const char * const colorFactor = "colorFactor";
		const char * const colorTexture = "colorTexture";
		const char * const roughnessFactor = "roughnessFactor";
		const char * const roughnessTexture = "roughnessTexture";
		const char * const aoTexture = "aoTexture";
		const char * const normalTexture = "normalTexture";
	}

	namespace BSDF_Mirror {
		const char * const type = "BSDF_Mirror";

		const char * const reflectance = "reflectance";
	}

	namespace BSDF_Frostbite {
		const char * const type = "BSDF_Frostbite";

		const char * const colorFactor = "colorFactor";
		const char * const albedoTexture = "albedoTexture";
		const char * const metallicFactor = "metallicFactor";
		const char * const metallicTexture = "metallicTexture";
		const char * const roughnessFactor = "roughnessFactor";
		const char * const roughnessTexture = "roughnessTexture";
		const char * const aoTexture = "aoTexture";
		const char * const normalTexture = "normalTexture";
	}

	namespace Gooch {
		const char * const type = "Gooch";

		const char * const colorFactor = "colorFactor";
		const char * const colorTexture = "colorTexture";
	}
}

#endif//!_CPPUTIL_ENGINE_SCENE_SL_COMMON_H_
