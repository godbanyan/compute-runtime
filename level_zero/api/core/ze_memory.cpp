/*
 * Copyright (C) 2019-2020 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "level_zero/core/source/driver/driver_handle.h"
#include <level_zero/ze_api.h>

extern "C" {

__zedllexport ze_result_t __zecall
zeDriverAllocSharedMem(
    ze_driver_handle_t hDriver,
    const ze_device_mem_alloc_desc_t *deviceDesc,
    const ze_host_mem_alloc_desc_t *hostDesc,
    size_t size,
    size_t alignment,
    ze_device_handle_t hDevice,
    void **pptr) {
    return L0::DriverHandle::fromHandle(hDriver)->allocSharedMem(hDevice, deviceDesc->flags, hostDesc->flags, size, alignment, pptr);
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zeMemAllocShared(ze_context_handle_t hContext,
                 const ze_device_mem_alloc_desc_t *deviceDesc,
                 const ze_host_mem_alloc_desc_t *hostDesc,
                 size_t size,
                 size_t alignment,
                 ze_device_handle_t hDevice,
                 void **pptr) {
    return L0::Context::fromHandle(hContext)->allocSharedMem(hDevice, deviceDesc->flags, hostDesc->flags, size, alignment, pptr);
}

__zedllexport ze_result_t __zecall
zeDriverAllocDeviceMem(
    ze_driver_handle_t hDriver,
    const ze_device_mem_alloc_desc_t *deviceDesc,
    size_t size,
    size_t alignment,
    ze_device_handle_t hDevice,
    void **pptr) {
    return L0::DriverHandle::fromHandle(hDriver)->allocDeviceMem(hDevice, deviceDesc->flags, size, alignment, pptr);
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zeMemAllocDevice(ze_context_handle_t hContext,
                 const ze_device_mem_alloc_desc_t *deviceDesc,
                 size_t size,
                 size_t alignment,
                 ze_device_handle_t hDevice,
                 void **pptr) {
    return L0::Context::fromHandle(hContext)->allocDeviceMem(hDevice, deviceDesc->flags, size, alignment, pptr);
}

__zedllexport ze_result_t __zecall
zeDriverAllocHostMem(
    ze_driver_handle_t hDriver,
    const ze_host_mem_alloc_desc_t *hostDesc,
    size_t size,
    size_t alignment,
    void **pptr) {
    return L0::DriverHandle::fromHandle(hDriver)->allocHostMem(hostDesc->flags, size, alignment, pptr);
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zeMemAllocHost(ze_context_handle_t hContext,
               const ze_host_mem_alloc_desc_t *hostDesc,
               size_t size,
               size_t alignment,
               void **pptr) {
    return L0::Context::fromHandle(hContext)->allocHostMem(hostDesc->flags, size, alignment, pptr);
}

__zedllexport ze_result_t __zecall
zeDriverFreeMem(
    ze_driver_handle_t hDriver,
    void *ptr) {
    return L0::DriverHandle::fromHandle(hDriver)->freeMem(ptr);
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zeMemFree(ze_context_handle_t hContext,
          void *ptr) {
    return L0::Context::fromHandle(hContext)->freeMem(ptr);
}

__zedllexport ze_result_t __zecall
zeDriverGetMemAllocProperties(
    ze_driver_handle_t hDriver,
    const void *ptr,
    ze_memory_allocation_properties_t *pMemAllocProperties,
    ze_device_handle_t *phDevice) {
    return L0::DriverHandle::fromHandle(hDriver)->getMemAllocProperties(ptr, pMemAllocProperties, phDevice);
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zeMemGetAllocProperties(
    ze_context_handle_t hContext,
    const void *ptr,
    ze_memory_allocation_properties_t *pMemAllocProperties,
    ze_device_handle_t *phDevice) {
    return L0::Context::fromHandle(hContext)->getMemAllocProperties(ptr, pMemAllocProperties, phDevice);
}

__zedllexport ze_result_t __zecall
zeDriverGetMemAddressRange(
    ze_driver_handle_t hDriver,
    const void *ptr,
    void **pBase,
    size_t *pSize) {
    return L0::DriverHandle::fromHandle(hDriver)->getMemAddressRange(ptr, pBase, pSize);
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zeMemGetAddressRange(
    ze_context_handle_t hContext,
    const void *ptr,
    void **pBase,
    size_t *pSize) {
    return L0::Context::fromHandle(hContext)->getMemAddressRange(ptr, pBase, pSize);
}

__zedllexport ze_result_t __zecall
zeDriverGetMemIpcHandle(
    ze_driver_handle_t hDriver,
    const void *ptr,
    ze_ipc_mem_handle_t *pIpcHandle) {
    return L0::DriverHandle::fromHandle(hDriver)->getIpcMemHandle(ptr, pIpcHandle);
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zeMemGetIpcHandle(
    ze_context_handle_t hContext,
    const void *ptr,
    ze_ipc_mem_handle_t *pIpcHandle) {
    return L0::Context::fromHandle(hContext)->getIpcMemHandle(ptr, pIpcHandle);
}

__zedllexport ze_result_t __zecall
zeDriverOpenMemIpcHandle(
    ze_driver_handle_t hDriver,
    ze_device_handle_t hDevice,
    ze_ipc_mem_handle_t handle,
    ze_ipc_memory_flag_t flags,
    void **pptr) {
    return L0::DriverHandle::fromHandle(hDriver)->openIpcMemHandle(hDevice, handle, flags, pptr);
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zeMemOpenIpcHandle(
    ze_context_handle_t hContext,
    ze_device_handle_t hDevice,
    ze_ipc_mem_handle_t handle,
    ze_ipc_memory_flags_t flags,
    void **pptr) {
    return L0::Context::fromHandle(hContext)->openIpcMemHandle(hDevice, handle, flags, pptr);
}

__zedllexport ze_result_t __zecall
zeDriverCloseMemIpcHandle(
    ze_driver_handle_t hDriver,
    const void *ptr) {
    return L0::DriverHandle::fromHandle(hDriver)->closeIpcMemHandle(ptr);
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zeMemCloseIpcHandle(
    ze_context_handle_t hContext,
    const void *ptr) {
    return L0::Context::fromHandle(hContext)->closeIpcMemHandle(ptr);
}

} // extern "C"
