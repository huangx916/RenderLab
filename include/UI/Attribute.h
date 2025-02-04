#ifndef _UI_ATTRIBUTE_H_
#define _UI_ATTRIBUTE_H_

#include <CppUtil/Basic/TypeMap.h>

#include <CppUtil/Basic/Ptr.h>

#include <qobject.h>
#include <qtoolbox.h>

#include <map>

namespace CppUtil {
	namespace Engine {
		class SObj;
		class Component;
	}
}


namespace Ui {
	class Grid;

	class Attribute final {
	protected:
		Attribute();

	public:
		static Attribute * GetInstance() {
			static Attribute instance;
			return &instance;
		}

	public:
		void Init(QToolBox * tbox);
		void SetSObj(CppUtil::Basic::Ptr<CppUtil::Engine::SObj> sobj);
		const CppUtil::Basic::Ptr<CppUtil::Engine::SObj> GetCurSObj() const { return curSObj.lock(); }
		template<typename T, typename = std::enable_if_t<std::is_base_of_v<CppUtil::Engine::Component, T>>>
		void SetCurCmpt() {
			auto target = componentType2item.find(typeid(T));
			if (target == componentType2item.end())
				return;

			tbox->setCurrentWidget(target->second);
		}

	private:
		void AddController(CppUtil::Basic::Ptr<CppUtil::Engine::SObj> sobj);

	private:
		class ComponentVisitor;
		friend class ComponentVisitor;

		QToolBox * tbox;

		CppUtil::Basic::TypeMap<QWidget *> componentType2item;
		std::map<QWidget *, CppUtil::Basic::Ptr<Grid>> item2grid;

		CppUtil::Basic::Ptr<ComponentVisitor> visitor;

		CppUtil::Basic::WPtr<CppUtil::Engine::SObj> curSObj;
	};
}

#endif//!_UI_ATTRIBUTE_H_
