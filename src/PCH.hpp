#pragma once

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/msvc_sink.h>
#include <spdlog/spdlog.h>

#include <fstream>
#include <nlohmann/json.hpp>

#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"

#define IMGUI_DEFINE_MATH_OPERATORS

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

namespace logger = SKSE::log;
using namespace std::literals;

using FormID = RE::FormID;
using RefID = RE::FormID;

constexpr RefID player_refid = 0x14;

#define IF_FIND(array, value, it) \
    auto it = array.find(value);  \
    if (it != array.end())
