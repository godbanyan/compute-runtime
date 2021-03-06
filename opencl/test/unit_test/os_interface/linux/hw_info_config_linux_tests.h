/*
 * Copyright (C) 2017-2020 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "shared/source/execution_environment/execution_environment.h"
#include "shared/source/os_interface/hw_info_config.h"
#include "shared/source/os_interface/os_interface.h"
#include "shared/source/utilities/cpu_info.h"
#include "shared/test/unit_test/helpers/default_hw_info.h"

#include "opencl/test/unit_test/os_interface/hw_info_config_tests.h"
#include "opencl/test/unit_test/os_interface/linux/drm_mock.h"

using namespace NEO;

struct HwInfoConfigTestLinux : public HwInfoConfigTest {
    void SetUp() override;
    void TearDown() override;

    OSInterface *osInterface;
    std::unique_ptr<ExecutionEnvironment> executionEnvironment;
    DrmMock *drm;

    void (*rt_cpuidex_func)(int *, int, int);
};
