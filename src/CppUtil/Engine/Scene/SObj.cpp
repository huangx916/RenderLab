#include <CppUtil/Engine/SObj.h>

#include "SObjSaver.h"
#include "SObjLoader.h"
#include "AssimpLoader.h"

#include <CppUtil/Engine/Component.h>
#include <CppUtil/Engine/CmptTransform.h>

#include <CppUtil/Basic/Visitor.h>

#include <CppUtil/Basic/StrAPI.h>

#include <iostream>

using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace std;

void SObj::AttachComponent(Ptr<Component> component) {
	auto target = components.find(typeid(*component));
	if (target != components.end())
		target->second->wSObj.reset();

	component->wSObj = This<SObj>();
	components[typeid(*component)] = component;
}

const std::vector<Ptr<Component>> SObj::GetAllComponents() const {
	std::vector<Ptr<Component>> rst;

	for (auto & component : components)
		rst.push_back(component.second);

	return rst;
}

const Transform SObj::GetLocalToWorldMatrix() {
	Transform tsfm(1.0f);

	auto getMatVisitor = Visitor::New();
	getMatVisitor->Reg([&](Ptr<SObj> sobj) {
		auto cmpt = sobj->GetComponent<CmptTransform>();
		if (cmpt != nullptr)
			tsfm = cmpt->GetTransform() * tsfm;
	});

	AscendAccept(getMatVisitor);
	return tsfm;
}

bool SObj::HaveComponentSameTypeWith(Ptr<Component> ptr) const {
	if (ptr == nullptr)
		return false;

	return components.find(typeid(*ptr)) != components.cend();
}

bool SObj::Save(const string & path) {
	auto saver = SObjSaver::New();
	if (StrAPI::IsEndWith(path, ".sobj"))
		saver->Init(path);
	else
		saver->Init(path + ".sobj");

	Accept(saver);
	return true;
}

const Ptr<SObj> SObj::Load(const string & path) {
	if (StrAPI::IsEndWith(path, ".sobj"))
		return SObjLoader::Load(path);

	return AssimpLoader::Load(path);
}

bool SObj::DetachComponent(Ptr<Component> component) {
	auto target = components.find(typeid(*component));
	if (target == components.end())
		return false;

	if (target->second != component)
		return false;

	target->second->wSObj.reset();

	components.erase(target);
	return true;
}
