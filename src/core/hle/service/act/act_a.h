// Copyright 2016 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#pragma once

#include <memory>
#include "core/hle/service/act/act.h"

namespace Service {
namespace ACT {

class ACT_A final : public Module::Interface {
public:
    explicit ACT_A(std::shared_ptr<Module> act);
};

} // namespace ACT
} // namespace Service
