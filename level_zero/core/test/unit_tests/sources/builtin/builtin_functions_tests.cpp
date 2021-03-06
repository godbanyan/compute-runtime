/*
 * Copyright (C) 2020-2021 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "shared/source/built_ins/built_ins.h"
#include "shared/source/execution_environment/root_device_environment.h"
#include "shared/test/unit_test/helpers/debug_manager_state_restore.h"
#include "shared/test/unit_test/mocks/mock_compiler_interface_spirv.h"

#include "test.h"

#include "level_zero/core/source/builtin/builtin_functions_lib_impl.h"
#include "level_zero/core/source/device/device_imp.h"
#include "level_zero/core/source/module/module.h"
#include "level_zero/core/source/module/module_imp.h"
#include "level_zero/core/test/unit_tests/fixtures/device_fixture.h"
#include "level_zero/core/test/unit_tests/mocks/mock_device_for_spirv.h"

#include "gtest/gtest.h"

namespace L0 {
namespace ult {
template <bool useImagesBuiltins>
class TestBuiltinFunctionsLibImpl : public DeviceFixture, public testing::Test {
  public:
    struct MockBuiltinFunctionsLibImpl : BuiltinFunctionsLibImpl {
        using BuiltinFunctionsLibImpl::builtins;
        using BuiltinFunctionsLibImpl::getFunction;
        using BuiltinFunctionsLibImpl::imageBuiltins;
        MockBuiltinFunctionsLibImpl(L0::Device *device, NEO::BuiltIns *builtInsLib) : BuiltinFunctionsLibImpl(device, builtInsLib) {}
        std::unique_ptr<BuiltinData> loadBuiltIn(NEO::EBuiltInOps::Type builtin, const char *builtInName) override {
            ze_result_t res;
            std::unique_ptr<Module> module;
            ze_module_handle_t moduleHandle;
            ze_module_desc_t moduleDesc = {};
            moduleDesc.format = ZE_MODULE_FORMAT_NATIVE;
            moduleDesc.pInputModule = nullptr;
            moduleDesc.inputSize = 0u;
            res = device->createModule(&moduleDesc, &moduleHandle, nullptr, ModuleType::Builtin);
            UNRECOVERABLE_IF(res != ZE_RESULT_SUCCESS);

            module.reset(Module::fromHandle(moduleHandle));

            std::unique_ptr<Kernel> kernel;
            ze_kernel_handle_t kernelHandle;
            ze_kernel_desc_t kernelDesc = {};
            kernelDesc.pKernelName = builtInName;
            res = module->createKernel(&kernelDesc, &kernelHandle);
            DEBUG_BREAK_IF(res != ZE_RESULT_SUCCESS);
            UNUSED_VARIABLE(res);
            kernel.reset(Kernel::fromHandle(kernelHandle));

            return std::unique_ptr<BuiltinData>(new MockBuiltinData{std::move(module), std::move(kernel)});
        }
    };
    struct MockBuiltinData : BuiltinFunctionsLibImpl::BuiltinData {
        using BuiltinFunctionsLibImpl::BuiltinData::func;
        using BuiltinFunctionsLibImpl::BuiltinData::module;
        MockBuiltinData(std::unique_ptr<L0::Module> mod, std::unique_ptr<L0::Kernel> ker) {
            module = std::move(mod);
            func = std::move(ker);
        }
        ~MockBuiltinData() override {
            module.release();
        }
    };
    void SetUp() override {
        DeviceFixture::SetUp();
        mockDevicePtr = std::unique_ptr<MockDeviceForSpv<useImagesBuiltins>>(new MockDeviceForSpv<useImagesBuiltins>(device->getNEODevice(), device->getNEODevice()->getExecutionEnvironment(), driverHandle.get()));
        mockBuiltinFunctionsLibImpl.reset(new MockBuiltinFunctionsLibImpl(mockDevicePtr.get(), neoDevice->getBuiltIns()));
    }
    void TearDown() override {
        mockBuiltinFunctionsLibImpl.reset();
        DeviceFixture::TearDown();
    }

    std::unique_ptr<MockBuiltinFunctionsLibImpl> mockBuiltinFunctionsLibImpl;
    std::unique_ptr<MockDeviceForSpv<useImagesBuiltins>> mockDevicePtr;
};

class TestBuiltinFunctionsLibImplDefault : public TestBuiltinFunctionsLibImpl<false> {};
class TestBuiltinFunctionsLibImplImages : public TestBuiltinFunctionsLibImpl<true> {};

HWTEST_F(TestBuiltinFunctionsLibImplImages, givenInitImageFunctionWhenImageBultinsTableContainNullptrsAndImageSupportedThenBuiltinsImageFunctionsAreLoaded) {
    for (uint32_t builtId = 0; builtId < static_cast<uint32_t>(ImageBuiltin::COUNT); builtId++) {
        EXPECT_EQ(nullptr, mockBuiltinFunctionsLibImpl->imageBuiltins[builtId]);
    }
    if (mockDevicePtr.get()->getHwInfo().capabilityTable.supportsImages) {
        mockBuiltinFunctionsLibImpl->initImageFunctions();
        for (uint32_t builtId = 0; builtId < static_cast<uint32_t>(ImageBuiltin::COUNT); builtId++) {
            EXPECT_NE(nullptr, mockBuiltinFunctionsLibImpl->imageBuiltins[builtId]);
            EXPECT_NE(nullptr, mockBuiltinFunctionsLibImpl->getImageFunction(static_cast<L0::ImageBuiltin>(builtId)));
        }
    }
}

HWTEST_F(TestBuiltinFunctionsLibImplDefault, givenInitFunctionWhenBultinsTableContainNullptrsThenBuiltinsFunctionsAreLoaded) {
    for (uint32_t builtId = 0; builtId < static_cast<uint32_t>(Builtin::COUNT); builtId++) {
        EXPECT_EQ(nullptr, mockBuiltinFunctionsLibImpl->builtins[builtId]);
    }
    mockBuiltinFunctionsLibImpl->initFunctions();
    for (uint32_t builtId = 0; builtId < static_cast<uint32_t>(Builtin::COUNT); builtId++) {
        EXPECT_NE(nullptr, mockBuiltinFunctionsLibImpl->builtins[builtId]);
        EXPECT_NE(nullptr, mockBuiltinFunctionsLibImpl->getFunction(static_cast<L0::Builtin>(builtId)));
    }
}

HWTEST_F(TestBuiltinFunctionsLibImplDefault, givenCompilerInterfaceWhenCreateDeviceAndImageSupportedThenBuiltinsImageFunctionsAreLoaded) {
    neoDevice->getExecutionEnvironment()->rootDeviceEnvironments[neoDevice->getRootDeviceIndex()]->compilerInterface.reset(new NEO::MockCompilerInterfaceSpirv());
    std::unique_ptr<L0::Device> testDevice(Device::create(device->getDriverHandle(), neoDevice, std::numeric_limits<uint32_t>::max(), false));

    if (device->getHwInfo().capabilityTable.supportsImages) {
        for (uint32_t builtId = 0; builtId < static_cast<uint32_t>(ImageBuiltin::COUNT); builtId++) {
            EXPECT_NE(nullptr, testDevice->getBuiltinFunctionsLib()->getImageFunction(static_cast<L0::ImageBuiltin>(builtId)));
        }
    }
}

HWTEST_F(TestBuiltinFunctionsLibImplDefault, givenCompilerInterfaceWhenCreateDeviceThenBuiltinsFunctionsAreLoaded) {
    neoDevice->getExecutionEnvironment()->rootDeviceEnvironments[neoDevice->getRootDeviceIndex()]->compilerInterface.reset(new NEO::MockCompilerInterfaceSpirv());
    std::unique_ptr<L0::Device> testDevice(Device::create(device->getDriverHandle(), neoDevice, std::numeric_limits<uint32_t>::max(), false));

    for (uint32_t builtId = 0; builtId < static_cast<uint32_t>(Builtin::COUNT); builtId++) {
        EXPECT_NE(nullptr, testDevice->getBuiltinFunctionsLib()->getFunction(static_cast<L0::Builtin>(builtId)));
    }
}

HWTEST_F(TestBuiltinFunctionsLibImplDefault, givenRebuildPrecompiledKernelsDebugFlagWhenInitFuctionsThenIntermediateCodeForBuiltinsIsRequested) {
    struct MockDeviceForRebuildBuilins : public Mock<DeviceImp> {
        struct MockModuleForRebuildBuiltins : public ModuleImp {
            MockModuleForRebuildBuiltins(Device *device) : ModuleImp(device, nullptr, ModuleType::Builtin) {}

            ze_result_t createKernel(const ze_kernel_desc_t *desc,
                                     ze_kernel_handle_t *phFunction) override {
                *phFunction = nullptr;
                return ZE_RESULT_SUCCESS;
            }
        };

        MockDeviceForRebuildBuilins(L0::Device *device) : Mock(device->getNEODevice(), static_cast<NEO::ExecutionEnvironment *>(device->getExecEnvironment())) {
            driverHandle = device->getDriverHandle();
            builtins = BuiltinFunctionsLib::create(this, neoDevice->getBuiltIns());
        }

        ze_result_t createModule(const ze_module_desc_t *desc,
                                 ze_module_handle_t *module,
                                 ze_module_build_log_handle_t *buildLog, ModuleType type) override {
            EXPECT_EQ(desc->format, ZE_MODULE_FORMAT_IL_SPIRV);
            EXPECT_GT(desc->inputSize, 0u);
            EXPECT_NE(desc->pInputModule, nullptr);
            createModuleCalled = true;

            *module = new MockModuleForRebuildBuiltins(this);

            return ZE_RESULT_SUCCESS;
        }

        bool createModuleCalled = false;
    };

    DebugManagerStateRestore dgbRestorer;
    NEO::DebugManager.flags.RebuildPrecompiledKernels.set(true);
    MockDeviceForRebuildBuilins testDevice(device);
    testDevice.builtins.reset(new BuiltinFunctionsLibImpl(&testDevice, neoDevice->getBuiltIns()));
    testDevice.getBuiltinFunctionsLib()->initFunctions();

    EXPECT_TRUE(testDevice.createModuleCalled);
}

HWTEST_F(TestBuiltinFunctionsLibImplDefault, givenNotToRebuildPrecompiledKernelsDebugFlagWhenInitFuctionsThenNativeCodeForBuiltinsIsRequested) {
    struct MockDeviceForRebuildBuilins : public Mock<DeviceImp> {
        MockDeviceForRebuildBuilins(L0::Device *device) : Mock(device->getNEODevice(), static_cast<NEO::ExecutionEnvironment *>(device->getExecEnvironment())) {
            driverHandle = device->getDriverHandle();
            builtins = BuiltinFunctionsLib::create(this, neoDevice->getBuiltIns());
        }

        ze_result_t createModule(const ze_module_desc_t *desc,
                                 ze_module_handle_t *module,
                                 ze_module_build_log_handle_t *buildLog, ModuleType type) override {
            EXPECT_EQ(desc->format, ZE_MODULE_FORMAT_NATIVE);
            EXPECT_GT(desc->inputSize, 0u);
            EXPECT_NE(desc->pInputModule, nullptr);
            createModuleCalled = true;

            return DeviceImp::createModule(desc, module, buildLog, type);
        }

        bool createModuleCalled = false;
    };

    DebugManagerStateRestore dgbRestorer;
    NEO::DebugManager.flags.RebuildPrecompiledKernels.set(false);
    MockDeviceForRebuildBuilins testDevice(device);
    L0::Device *testDevicePtr = &testDevice;
    testDevice.builtins.reset(new BuiltinFunctionsLibImpl(testDevicePtr, neoDevice->getBuiltIns()));
    testDevice.getBuiltinFunctionsLib()->initFunctions();

    EXPECT_TRUE(testDevice.createModuleCalled);
}

HWTEST_F(TestBuiltinFunctionsLibImplDefault, GivenBuiltinsWhenInitializingFunctionsThenModulesWithProperTypeAreCreated) {
    struct MockDeviceWithBuilins : public Mock<DeviceImp> {
        MockDeviceWithBuilins(L0::Device *device) : Mock(device->getNEODevice(), static_cast<NEO::ExecutionEnvironment *>(device->getExecEnvironment())) {
            driverHandle = device->getDriverHandle();
            builtins = BuiltinFunctionsLib::create(this, neoDevice->getBuiltIns());
        }

        ze_result_t createModule(const ze_module_desc_t *desc,
                                 ze_module_handle_t *module,
                                 ze_module_build_log_handle_t *buildLog, ModuleType type) override {

            typeCreated = type;
            EXPECT_EQ(ModuleType::Builtin, type);

            return DeviceImp::createModule(desc, module, buildLog, type);
        }

        ModuleType typeCreated = ModuleType::User;
    };

    MockDeviceWithBuilins testDevice(device);
    L0::Device *testDevicePtr = &testDevice;
    testDevice.builtins.reset(new BuiltinFunctionsLibImpl(testDevicePtr, neoDevice->getBuiltIns()));
    testDevice.getBuiltinFunctionsLib()->initFunctions();

    EXPECT_EQ(ModuleType::Builtin, testDevice.typeCreated);
}

} // namespace ult
} // namespace L0
