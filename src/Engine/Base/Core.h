#pragma once

#include "Base/Log.h"
#include <memory>
#include <functional>

#define CORE_ASSERT(x, ...) { if(!(x)) { LOG_ERROR("Assertion Failed: {0}", __VA_ARGS__); assert(x); } }

#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

template <typename T>
using Ref = std::shared_ptr<T>;

template <typename T>
using Scope = std::unique_ptr<T>;