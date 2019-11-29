#include "ObjectBase.hpp"

namespace boxpp {

	void XObjectBase::SetUpper(XObjectBase* Object) {
		if (Upper) {
			Upper->OnDetachChild(this);
		}

		Upper = Object;

		if (Object) {
			Object->OnAttachChild(this);
		}
	}

	void XObjectBase::OnAttachChild(XObjectBase* Child) {
		Children.Add(Child);
	}
	
	void XObjectBase::OnDetachChild(XObjectBase* Child) {
		Children.Remove(Child);
	}
}