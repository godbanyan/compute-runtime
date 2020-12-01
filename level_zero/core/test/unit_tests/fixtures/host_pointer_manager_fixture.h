/*
 * Copyright (C) 2020 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "shared/source/helpers/constants.h"
#include "shared/test/unit_test/helpers/debug_manager_state_restore.h"
#include "shared/test/unit_test/helpers/default_hw_info.h"
#include "shared/test/unit_test/mocks/mock_device.h"

#include "level_zero/core/test/unit_tests/fixtures/device_fixture.h"
#include "level_zero/core/test/unit_tests/mocks/mock_driver_handle.h"
#include "level_zero/core/test/unit_tests/mocks/mock_host_pointer_manager.h"

namespace L0 {
namespace ult {

struct HostPointerManagerFixure {
    void SetUp() {
        NEO::DeviceVector devices;
        neoDevice = NEO::MockDevice::createWithNewExecutionEnvironment<NEO::MockDevice>(NEO::defaultHwInfo.get());
        auto mockBuiltIns = new MockBuiltins();
        neoDevice->executionEnvironment->rootDeviceEnvironments[0]->builtins.reset(mockBuiltIns);
        devices.push_back(std::unique_ptr<NEO::Device>(neoDevice));

        DebugManager.flags.EnableHostPointerImport.set(1);
        hostDriverHandle = std::make_unique<L0::ult::DriverHandle>();
        hostDriverHandle->initialize(std::move(devices));
        device = hostDriverHandle->devices[0];
        EXPECT_NE(nullptr, hostDriverHandle->hostPointerManager.get());
        openHostPointerManager = static_cast<L0::ult::HostPointerManager *>(hostDriverHandle->hostPointerManager.get());
        heapPointer = hostDriverHandle->getMemoryManager()->allocateSystemMemory(4 * MemoryConstants::pageSize, MemoryConstants::pageSize);
        ASSERT_NE(nullptr, heapPointer);
    }

    void TearDown() {
        hostDriverHandle->getMemoryManager()->freeSystemMemory(heapPointer);
    }
    L0::ult::HostPointerManager *openHostPointerManager = nullptr;
    std::unique_ptr<L0::ult::DriverHandle> hostDriverHandle;
    void *heapPointer = nullptr;
    NEO::MockDevice *neoDevice = nullptr;
    L0::Device *device = nullptr;
};

} // namespace ult
} // namespace L0