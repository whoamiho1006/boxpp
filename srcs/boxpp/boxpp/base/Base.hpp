#pragma once
#include <boxpp/base/BaseMacros.hpp>
#include <boxpp/base/BaseTypes.hpp>

/* opacity impls. */
#include <boxpp/base/opacities/posix.hpp>
#include <boxpp/base/opacities/windows.hpp>
#include <boxpp/base/opacities/vsprintf.hpp>

/* placement new from std. */
#include <new>

/* type traits. */
#include <boxpp/base/tpls/BaseTraits.hpp>

#include <boxpp/base/tpls/traits/IsPodType.hpp>
#include <boxpp/base/tpls/traits/IsSameType.hpp>
#include <boxpp/base/tpls/traits/IsDerivedType.hpp>
#include <boxpp/base/tpls/traits/IsConstructibleType.hpp>
#include <boxpp/base/tpls/traits/IsConvertibleType.hpp>
#include <boxpp/base/tpls/traits/IsReferenceType.hpp>

#include <boxpp/base/tpls/traits/BypassType.hpp>
#include <boxpp/base/tpls/traits/PickType.hpp>
#include <boxpp/base/tpls/traits/EnableIf.hpp>

#include <boxpp/base/tpls/traits/IsCallableType.hpp>

#include <boxpp/base/tpls/traits/DecayRef.hpp>
#include <boxpp/base/tpls/traits/DecayConst.hpp>

#include <boxpp/base/tpls/traits/Forward.hpp>
#include <boxpp/base/tpls/traits/Movable.hpp>

#include <boxpp/base/tpls/traits/AbstractedOperators.hpp>

/* shared pointers. */
#include <boxpp/base/tpls/sharedptrs/SharedCount.hpp>
#include <boxpp/base/tpls/sharedptrs/SharedHolder.hpp>
#include <boxpp/base/tpls/sharedptrs/SharedPtr.hpp>

/* containers. */
#include <boxpp/base/tpls/containers/Iterator.hpp>
#include <boxpp/base/tpls/containers/Comparator.hpp>

#include <boxpp/base/tpls/containers/ArrayBase.hpp>
#include <boxpp/base/tpls/containers/EdgeBase.hpp>

#include <boxpp/base/tpls/containers/Node.hpp>
#include <boxpp/base/tpls/containers/SingleNode.hpp>

#include <boxpp/base/tpls/containers/Array.hpp>
#include <boxpp/base/tpls/containers/SortedArray.hpp>

#include <boxpp/base/tpls/containers/LinkedList.hpp>
#include <boxpp/base/tpls/containers/Queue.hpp>
#include <boxpp/base/tpls/containers/Stack.hpp>

#include <boxpp/base/tpls/strings/Constants.hpp>
#include <boxpp/base/tpls/strings/Operations.hpp>
#include <boxpp/base/tpls/strings/Converter.hpp>

#include <boxpp/base/tpls/strings/String.hpp>

/* boilerplates. */
#include <boxpp/base/boilerplates/Boilerplate.hpp>