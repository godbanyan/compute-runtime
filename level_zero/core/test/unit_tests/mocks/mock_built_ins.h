/*
 * Copyright (C) 2019-2020 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once
#include "shared/source/built_ins/built_ins.h"
#include "shared/test/unit_test/mocks/mock_graphics_allocation.h"

namespace L0 {
namespace ult {

class MockBuiltins : public NEO::BuiltIns {
  public:
    MockBuiltins() : BuiltIns() {
        allocation.reset(new NEO::MockGraphicsAllocation());
    }
    const NEO::SipKernel &getSipKernel(NEO::SipKernelType type, NEO::Device &device) override;
    std::unique_ptr<NEO::SipKernel> sipKernel;
    std::unique_ptr<NEO::MockGraphicsAllocation> allocation;
};
} // namespace ult
} // namespace L0
