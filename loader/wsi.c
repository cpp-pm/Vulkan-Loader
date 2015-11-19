/*
 *
 * Copyright (C) 2015 Valve Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Author: Ian Elliott <ian@lunarg.com>
 * Author: Ian Elliott <ianelliott@google.com>
 */

// FIXME/TODO: DEVELOP A BETTER APPROACH FOR SETTING THE DEFAULT VALUES FOR
// THESE PLATFORM-SPECIFIC MACROS APPROPRIATELY:
#ifdef _WIN32
// The Win32 default is to support the WIN32 platform:
#ifndef VK_USE_PLATFORM_WIN32_KHR
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#else // _WIN32 (i.e. Linux)
// The Linux default is to support the XCB platform:
#if (!defined(VK_USE_PLATFORM_MIR_KHR) && \
     !defined(VK_USE_PLATFORM_WAYLAND_KHR) && \
     !defined(VK_USE_PLATFORM_XCB_KHR) && \
     !defined(VK_USE_PLATFORM_XLIB_KHR))
#define VK_USE_PLATFORM_XCB_KHR
#endif
#endif // _WIN32

//#define _ISOC11_SOURCE /* for aligned_alloc() */
#define _GNU_SOURCE
#include <stdlib.h>
#include <string.h>
#include "vk_loader_platform.h"
#include "loader.h"
#include "wsi.h"
#include <vulkan/vk_icd.h>

static const VkExtensionProperties wsi_surface_extension_info = {
        .extensionName = VK_KHR_SURFACE_EXTENSION_NAME,
        .specVersion = VK_KHR_SURFACE_REVISION,
};

#ifdef _WIN32
#ifdef VK_USE_PLATFORM_WIN32_KHR
static const VkExtensionProperties wsi_win32_surface_extension_info = {
        .extensionName = VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
        .specVersion = VK_KHR_WIN32_SURFACE_REVISION,
};
#endif/ VK_USE_PLATFORM_WIN32_KHR
#else // _WIN32
#ifdef VK_USE_PLATFORM_MIR_KHR
static const VkExtensionProperties wsi_mir_surface_extension_info = {
        .extensionName = VK_KHR_MIR_SURFACE_EXTENSION_NAME,
        .specVersion = VK_KHR_MIR_SURFACE_REVISION,
};
#endif // VK_USE_PLATFORM_MIR_KHR

#ifdef VK_USE_PLATFORM_WAYLAND_KHR
static const VkExtensionProperties wsi_wayland_surface_extension_info = {
        .extensionName = VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME,
        .specVersion = VK_KHR_WAYLAND_SURFACE_REVISION,
};
#endif // VK_USE_PLATFORM_WAYLAND_KHR

#ifdef VK_USE_PLATFORM_XCB_KHR
static const VkExtensionProperties wsi_xcb_surface_extension_info = {
        .extensionName = VK_KHR_XCB_SURFACE_EXTENSION_NAME,
        .specVersion = VK_KHR_XCB_SURFACE_REVISION,
};
#endif // VK_USE_PLATFORM_XCB_KHR

#ifdef VK_USE_PLATFORM_XLIB_KHR
static const VkExtensionProperties wsi_xlib_surface_extension_info = {
        .extensionName = VK_KHR_XLIB_SURFACE_EXTENSION_NAME,
        .specVersion = VK_KHR_XLIB_SURFACE_REVISION,
};
#endif // VK_USE_PLATFORM_XLIB_KHR
#endif // _WIN32

void wsi_add_instance_extensions(
        const struct loader_instance *inst,
        struct loader_extension_list *ext_list)
{
    loader_add_to_ext_list(inst, ext_list, 1, &wsi_surface_extension_info);
#ifdef _WIN32
#ifdef VK_USE_PLATFORM_WIN32_KHR
    loader_add_to_ext_list(inst, ext_list, 1, &wsi_win32_surface_extension_info);
#endif/ VK_USE_PLATFORM_WIN32_KHR
#else // _WIN32
#ifdef VK_USE_PLATFORM_MIR_KHR
    loader_add_to_ext_list(inst, ext_list, 1, &wsi_mir_surface_extension_info);
#endif // VK_USE_PLATFORM_MIR_KHR
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
    loader_add_to_ext_list(inst, ext_list, 1, &wsi_wayland_surface_extension_info);
#endif // VK_USE_PLATFORM_WAYLAND_KHR
#ifdef VK_USE_PLATFORM_XCB_KHR
    loader_add_to_ext_list(inst, ext_list, 1, &wsi_xcb_surface_extension_info);
#endif // VK_USE_PLATFORM_XCB_KHR
#ifdef VK_USE_PLATFORM_XLIB_KHR
    loader_add_to_ext_list(inst, ext_list, 1, &wsi_xlib_surface_extension_info);
#endif // VK_USE_PLATFORM_XLIB_KHR
#endif // _WIN32
}

void wsi_create_instance(
        struct loader_instance *ptr_instance,
        const VkInstanceCreateInfo *pCreateInfo)
{
    ptr_instance->wsi_surface_enabled = false;
#ifdef _WIN32
    ptr_instance->wsi_surface_enabled = true;
#else // _WIN32
    ptr_instance->wsi_mir_surface_enabled = false;
    ptr_instance->wsi_wayland_surface_enabled = false;
    ptr_instance->wsi_xcb_surface_enabled = false;
    ptr_instance->wsi_xlib_surface_enabled = false;
#endif // _WIN32
    ptr_instance->wsi_swapchain_enabled = false;

    for (uint32_t i = 0; i < pCreateInfo->enabledExtensionNameCount; i++) {
        if (strcmp(pCreateInfo->ppEnabledExtensionNames[i], VK_KHR_SURFACE_EXTENSION_NAME) == 0) {
            ptr_instance->wsi_surface_enabled = true;
            continue;
        }
#ifdef _WIN32
#ifdef VK_USE_PLATFORM_WIN32_KHR
        if (strcmp(pCreateInfo->ppEnabledExtensionNames[i], VK_KHR_WIN32_SURFACE_EXTENSION_NAME) == 0) {
            ptr_instance->wsi_surface_enabled = true;
            continue;
        }
#endif/ VK_USE_PLATFORM_WIN32_KHR
#else // _WIN32
#ifdef VK_USE_PLATFORM_MIR_KHR
        if (strcmp(pCreateInfo->ppEnabledExtensionNames[i], VK_KHR_MIR_SURFACE_EXTENSION_NAME) == 0) {
            ptr_instance->wsi_mir_surface_enabled = true;
            continue;
        }
#endif // VK_USE_PLATFORM_MIR_KHR
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
        if (strcmp(pCreateInfo->ppEnabledExtensionNames[i], VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME) == 0) {
            ptr_instance->wsi_wayland_surface_enabled = true;
            continue;
        }
#endif // VK_USE_PLATFORM_WAYLAND_KHR
#ifdef VK_USE_PLATFORM_XCB_KHR
        if (strcmp(pCreateInfo->ppEnabledExtensionNames[i], VK_KHR_XCB_SURFACE_EXTENSION_NAME) == 0) {
            ptr_instance->wsi_xcb_surface_enabled = true;
            continue;
        }
#endif // VK_USE_PLATFORM_XCB_KHR
#ifdef VK_USE_PLATFORM_XLIB_KHR
        if (strcmp(pCreateInfo->ppEnabledExtensionNames[i], VK_KHR_XLIB_SURFACE_EXTENSION_NAME) == 0) {
            ptr_instance->wsi_xlib_surface_enabled = true;
            continue;
        }
#endif // VK_USE_PLATFORM_XLIB_KHR
#endif // _WIN32
    }
}

/*
 * Functions for the VK_KHR_surface extension:
 */

/*
 * This is the trampoline entrypoint
 * for DestroySurfaceKHR
 *
 * Note: There is no terminator for this function.
 */
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroySurfaceKHR(
    VkInstance                                   instance,
    VkSurfaceKHR                                 surface,
    const VkAllocationCallbacks*                 pAllocator)
{
    if (pAllocator) {
        pAllocator->pfnFree(pAllocator->pUserData, surface);
    } else {
        free(surface);
    }
}

/*
 * This is the trampoline entrypoint
 * for GetPhysicalDeviceSurfaceSupportKHR
 */
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceSurfaceSupportKHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t                                    queueFamilyIndex,
    VkSurfaceKHR                                surface,
    VkBool32*                                   pSupported)
{
    const VkLayerInstanceDispatchTable *disp;
    disp = loader_get_instance_dispatch(physicalDevice);
    VkResult res = disp->GetPhysicalDeviceSurfaceSupportKHR(
                                                      physicalDevice,
                                                      queueFamilyIndex,
                                                      surface,
                                                      pSupported);
    return res;
}

/*
 * This is the instance chain terminator function
 * for GetPhysicalDeviceSurfaceSupportKHR
 */
VKAPI_ATTR VkResult VKAPI_CALL loader_GetPhysicalDeviceSurfaceSupportKHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t                                    queueFamilyIndex,
    VkSurfaceKHR                                surface,
    VkBool32*                                   pSupported)
{
    struct loader_physical_device *phys_dev = (struct loader_physical_device *) physicalDevice;
    struct loader_icd *icd = phys_dev->this_icd;

    assert(pSupported && "GetPhysicalDeviceSurfaceSupportKHR: Error, null pSupported");
    *pSupported = false;

    assert(icd->GetPhysicalDeviceSurfaceSupportKHR && "loader: null GetPhysicalDeviceSurfaceSupportKHR ICD pointer");

    return icd->GetPhysicalDeviceSurfaceSupportKHR(phys_dev->phys_dev,
                                                   queueFamilyIndex,
                                                   surface,
                                                   pSupported);
}

/*
 * This is the trampoline entrypoint
 * for GetPhysicalDeviceSurfaceCapabilitiesKHR
 */
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
    VkPhysicalDevice                            physicalDevice,
    VkSurfaceKHR                                surface,
    VkSurfaceCapabilitiesKHR*                   pSurfaceCapabilities)
{
    const VkLayerInstanceDispatchTable *disp;
    disp = loader_get_instance_dispatch(physicalDevice);
    VkResult res = disp->GetPhysicalDeviceSurfaceCapabilitiesKHR(
                                                      physicalDevice,
                                                      surface,
                                                      pSurfaceCapabilities);
    return res;
}

/*
 * This is the instance chain terminator function
 * for GetPhysicalDeviceSurfaceCapabilitiesKHR
 */
VKAPI_ATTR VkResult VKAPI_CALL loader_GetPhysicalDeviceSurfaceCapabilitiesKHR(
    VkPhysicalDevice                            physicalDevice,
    VkSurfaceKHR                                surface,
    VkSurfaceCapabilitiesKHR*                   pSurfaceCapabilities)
{
    struct loader_physical_device *phys_dev = (struct loader_physical_device *) physicalDevice;
    struct loader_icd *icd = phys_dev->this_icd;

    assert(pSurfaceCapabilities && "GetPhysicalDeviceSurfaceCapabilitiesKHR: Error, null pSurfaceCapabilities");

    assert(icd->GetPhysicalDeviceSurfaceCapabilitiesKHR && "loader: null GetPhysicalDeviceSurfaceCapabilitiesKHR ICD pointer");

    return icd->GetPhysicalDeviceSurfaceCapabilitiesKHR(phys_dev->phys_dev,
                                                      surface,
                                                      pSurfaceCapabilities);
}

/*
 * This is the trampoline entrypoint
 * for GetPhysicalDeviceSurfaceFormatsKHR
 */
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceSurfaceFormatsKHR(
    VkPhysicalDevice                            physicalDevice,
    VkSurfaceKHR                                surface,
    uint32_t*                                   pSurfaceFormatCount,
    VkSurfaceFormatKHR*                         pSurfaceFormats)
{
    const VkLayerInstanceDispatchTable *disp;
    disp = loader_get_instance_dispatch(physicalDevice);
    VkResult res = disp->GetPhysicalDeviceSurfaceFormatsKHR(
                                                      physicalDevice,
                                                      surface,
                                                      pSurfaceFormatCount,
                                                      pSurfaceFormats);
    return res;
}

/*
 * This is the instance chain terminator function
 * for GetPhysicalDeviceSurfaceFormatsKHR
 */
VKAPI_ATTR VkResult VKAPI_CALL loader_GetPhysicalDeviceSurfaceFormatsKHR(
    VkPhysicalDevice                            physicalDevice,
    VkSurfaceKHR                                surface,
    uint32_t*                                   pSurfaceFormatCount,
    VkSurfaceFormatKHR*                         pSurfaceFormats)
{
    struct loader_physical_device *phys_dev = (struct loader_physical_device *) physicalDevice;
    struct loader_icd *icd = phys_dev->this_icd;

    assert(pSurfaceFormatCount && "GetPhysicalDeviceSurfaceFormatsKHR: Error, null pSurfaceFormatCount");

    assert(icd->GetPhysicalDeviceSurfaceFormatsKHR && "loader: null GetPhysicalDeviceSurfaceFormatsKHR ICD pointer");

    return icd->GetPhysicalDeviceSurfaceFormatsKHR(phys_dev->phys_dev,
                                                      surface,
                                                      pSurfaceFormatCount,
                                                      pSurfaceFormats);
}

/*
 * This is the trampoline entrypoint
 * for GetPhysicalDeviceSurfacePresentModesKHR
 */
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceSurfacePresentModesKHR(
    VkPhysicalDevice                            physicalDevice,
    VkSurfaceKHR                                surface,
    uint32_t*                                   pPresentModeCount,
    VkPresentModeKHR*                           pPresentModes)
{
    const VkLayerInstanceDispatchTable *disp;
    disp = loader_get_instance_dispatch(physicalDevice);
    VkResult res = disp->GetPhysicalDeviceSurfacePresentModesKHR(
                                                      physicalDevice,
                                                      surface,
                                                      pPresentModeCount,
                                                      pPresentModes);
    return res;
}

/*
 * This is the instance chain terminator function
 * for GetPhysicalDeviceSurfacePresentModesKHR
 */
VKAPI_ATTR VkResult VKAPI_CALL loader_GetPhysicalDeviceSurfacePresentModesKHR(
    VkPhysicalDevice                            physicalDevice,
    VkSurfaceKHR                                surface,
    uint32_t*                                   pPresentModeCount,
    VkPresentModeKHR*                           pPresentModes)
{
    struct loader_physical_device *phys_dev = (struct loader_physical_device *) physicalDevice;
    struct loader_icd *icd = phys_dev->this_icd;

    assert(pPresentModeCount && "GetPhysicalDeviceSurfacePresentModesKHR: Error, null pPresentModeCount");

    assert(icd->GetPhysicalDeviceSurfacePresentModesKHR && "loader: null GetPhysicalDeviceSurfacePresentModesKHR ICD pointer");

    return icd->GetPhysicalDeviceSurfacePresentModesKHR(phys_dev->phys_dev,
                                                      surface,
                                                      pPresentModeCount,
                                                      pPresentModes);
}


/*
 * Functions for the VK_KHR_swapchain extension:
 */

/*
 * This is the trampoline entrypoint
 * for CreateSwapchainKHR
 */
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateSwapchainKHR(
    VkDevice                                     device,
    const VkSwapchainCreateInfoKHR*              pCreateInfo,
    const VkAllocationCallbacks*                 pAllocator,
    VkSwapchainKHR*                              pSwapchain)
{
    const VkLayerDispatchTable *disp;
    disp = loader_get_dispatch(device);
    VkResult res = disp->CreateSwapchainKHR(
                                            device,
                                            pCreateInfo,
                                            pAllocator,
                                            pSwapchain);
    return res;
}

/*
 * This is the trampoline entrypoint
 * for DestroySwapchainKHR
 */
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroySwapchainKHR(
    VkDevice                                     device,
    VkSwapchainKHR                               swapchain,
    const VkAllocationCallbacks*                 pAllocator)
{
    const VkLayerDispatchTable *disp;
    disp = loader_get_dispatch(device);
    disp->DestroySwapchainKHR(device, swapchain, pAllocator);
}

/*
 * This is the trampoline entrypoint
 * for GetSwapchainImagesKHR
 */
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetSwapchainImagesKHR(
    VkDevice                                     device,
    VkSwapchainKHR                               swapchain,
    uint32_t*                                    pSwapchainImageCount,
    VkImage*                                     pSwapchainImages)
{
    const VkLayerDispatchTable *disp;
    disp = loader_get_dispatch(device);
    VkResult res = disp->GetSwapchainImagesKHR(
                                            device,
                                            swapchain,
                                            pSwapchainImageCount,
                                            pSwapchainImages);
    return res;
}

/*
 * This is the trampoline entrypoint
 * for AcquireNextImageKHR
 */
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkAcquireNextImageKHR(
    VkDevice                                     device,
    VkSwapchainKHR                               swapchain,
    uint64_t                                     timeout,
    VkSemaphore                                  semaphore,
    VkFence                                      fence,
    uint32_t*                                    pImageIndex)
{
    const VkLayerDispatchTable *disp;
    disp = loader_get_dispatch(device);
    VkResult res = disp->AcquireNextImageKHR(
                                            device,
                                            swapchain,
                                            timeout,
                                            semaphore,
                                            fence,
                                            pImageIndex);
    return res;
}

/*
 * This is the trampoline entrypoint
 * for QueuePresentKHR
 */
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkQueuePresentKHR(
    VkQueue                                      queue,
    const VkPresentInfoKHR*                      pPresentInfo)
{
    const VkLayerDispatchTable *disp;
    disp = loader_get_dispatch(queue);
    VkResult res = disp->QueuePresentKHR(queue, pPresentInfo);
    return res;
}


#ifdef _WIN32

#ifdef VK_USE_PLATFORM_WIN32_KHR

/*
 * Functions for the VK_KHR_win32_surface extension:
 */

/*
 * This is the trampoline entrypoint
 * for CreateWin32SurfaceKHR
 */
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateWin32SurfaceKHR(
    VkInstance                                  instance,
    HINSTANCE                                   hinstance,
    HWND                                        hwnd,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface)
{
    VkIcdSurfaceWin32 *pIcdSurface = NULL;

    if (pAllocator) {
        pIcdSurface = (VkIcdSurfaceWin32 *) pAllocator->pfnAllocation(
                           pAllocator->pUserData,
                           sizeof(VkIcdSurfaceWin32),
                           sizeof(VkSurfaceKHR),
                           VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
    } else {
        pIcdSurface = (VkIcdSurfaceWin32 *) malloc(sizeof(VkIcdSurfaceWin32));
    }
    if (pIcdSurface == NULL) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    pIcdSurface->base.platform = VK_ICD_WSI_PLATFORM_WIN32;
    pIcdSurface->hinstance = hinstance;
    pIcdSurface->hwnd = hwnd;

    *pSurface = (VkSurfaceKHR) pIcdSurface;

    return VK_SUCCESS;
}
#endif/ VK_USE_PLATFORM_WIN32_KHR

#else // _WIN32 (i.e. Linux)

#ifdef VK_USE_PLATFORM_MIR_KHR

/*
 * Functions for the VK_KHR_mir_surface extension:
 */

/*
 * This is the trampoline entrypoint
 * for CreateMirSurfaceKHR
 */
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateMirSurfaceKHR(
    VkInstance                                  instance,
    MirConnection*                              connection,
    MirSurface*                                 mirSurface,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface)
{
    VkIcdSurfaceMir *pIcdSurface = NULL;

    if (pAllocator) {
        pIcdSurface = (VkIcdSurfaceMir *) pAllocator->pfnAllocation(
                           pAllocator->pUserData,
                           sizeof(VkIcdSurfaceMir),
                           sizeof(VkSurfaceKHR),
                           VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
    } else {
        pIcdSurface = (VkIcdSurfaceMir *) malloc(sizeof(VkIcdSurfaceMir));
    }
    if (pIcdSurface == NULL) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    pIcdSurface->base.platform = VK_ICD_WSI_PLATFORM_MIR;
    pIcdSurface->connection = connection;
    pIcdSurface->mirSurface = mirSurface;

    *pSurface = (VkSurfaceKHR) pIcdSurface;

    return VK_SUCCESS;
}
#endif // VK_USE_PLATFORM_MIR_KHR

#ifdef VK_USE_PLATFORM_WAYLAND_KHR

/*
 * Functions for the VK_KHR_wayland_surface extension:
 */

/*
 * This is the trampoline entrypoint
 * for CreateWaylandSurfaceKHR
 */
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateWaylandSurfaceKHR(
    VkInstance                                  instance,
    struct wl_display*                          display,
    struct wl_surface*                          surface,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface)
{
    VkIcdSurfaceWayland *pIcdSurface = NULL;

    if (pAllocator) {
        pIcdSurface = (VkIcdSurfaceWayland *) pAllocator->pfnAllocation(
                           pAllocator->pUserData,
                           sizeof(VkIcdSurfaceWayland),
                           sizeof(VkSurfaceKHR),
                           VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
    } else {
        pIcdSurface = (VkIcdSurfaceWayland *) malloc(sizeof(VkIcdSurfaceWayland));
    }
    if (pIcdSurface == NULL) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    pIcdSurface->base.platform = VK_ICD_WSI_PLATFORM_WAYLAND;
    pIcdSurface->display = display;
    pIcdSurface->surface = surface;

    *pSurface = (VkSurfaceKHR) pIcdSurface;

    return VK_SUCCESS;
}
#endif // VK_USE_PLATFORM_WAYLAND_KHR

#ifdef VK_USE_PLATFORM_XCB_KHR

/*
 * Functions for the VK_KHR_xcb_surface extension:
 */

/*
 * This is the trampoline entrypoint
 * for CreateXcbSurfaceKHR
 */
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateXcbSurfaceKHR(
    VkInstance                                  instance,
    xcb_connection_t*                           connection,
    xcb_window_t                                window,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface)
{
    VkIcdSurfaceXcb *pIcdSurface = NULL;

    if (pAllocator) {
        pIcdSurface = (VkIcdSurfaceXcb *) pAllocator->pfnAllocation(
                           pAllocator->pUserData,
                           sizeof(VkIcdSurfaceXcb),
                           sizeof(VkSurfaceKHR),
                           VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
    } else {
        pIcdSurface = (VkIcdSurfaceXcb *) malloc(sizeof(VkIcdSurfaceXcb));
    }
    if (pIcdSurface == NULL) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    pIcdSurface->base.platform = VK_ICD_WSI_PLATFORM_XCB;
    pIcdSurface->connection = connection;
    pIcdSurface->window = window;

    *pSurface = (VkSurfaceKHR) pIcdSurface;

    return VK_SUCCESS;
}
#endif // VK_USE_PLATFORM_XCB_KHR

#ifdef VK_USE_PLATFORM_XLIB_KHR

/*
 * Functions for the VK_KHR_xlib_surface extension:
 */

/*
 * This is the trampoline entrypoint
 * for CreateXlibSurfaceKHR
 */
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateXlibSurfaceKHR(
    VkInstance                                  instance,
    Display*                                    dpy,
    Window                                      window,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface)
{
    VkIcdSurfaceXlib *pIcdSurface = NULL;

    if (pAllocator) {
        pIcdSurface = (VkIcdSurfaceXlib *) pAllocator->pfnAllocation(
                           pAllocator->pUserData,
                           sizeof(VkIcdSurfaceXlib),
                           sizeof(VkSurfaceKHR),
                           VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
    } else {
        pIcdSurface = (VkIcdSurfaceXlib *) malloc(sizeof(VkIcdSurfaceXlib));
    }
    if (pIcdSurface == NULL) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    pIcdSurface->base.platform = VK_ICD_WSI_PLATFORM_XLIB;
    pIcdSurface->dpy = dpy;
    pIcdSurface->window = window;

    *pSurface = (VkSurfaceKHR) pIcdSurface;

    return VK_SUCCESS;
}
#endif // VK_USE_PLATFORM_XLIB_KHR

#endif // _WIN32


bool wsi_swapchain_instance_gpa(struct loader_instance *ptr_instance,
                                 const char* name, void **addr)
{
    *addr = NULL;

    /*
     * Functions for the VK_KHR_surface extension:
     */
    if (!strcmp("vkDestroySurfaceKHR", name)) {
        *addr = ptr_instance->wsi_surface_enabled ? (void *) vkDestroySurfaceKHR : NULL;
        return true;
    }
    if (!strcmp("vkGetPhysicalDeviceSurfaceSupportKHR", name)) {
        *addr = ptr_instance->wsi_surface_enabled ? (void *) vkGetPhysicalDeviceSurfaceSupportKHR : NULL;
        return true;
    }
    if (!strcmp("vkGetPhysicalDeviceSurfaceCapabilitiesKHR", name)) {
        *addr = ptr_instance->wsi_surface_enabled ? (void *) vkGetPhysicalDeviceSurfaceCapabilitiesKHR : NULL;
        return true;
    }
    if (!strcmp("vkGetPhysicalDeviceSurfaceFormatsKHR", name)) {
        *addr = ptr_instance->wsi_surface_enabled ? (void *) vkGetPhysicalDeviceSurfaceFormatsKHR : NULL;
        return true;
    }
    if (!strcmp("vkGetPhysicalDeviceSurfacePresentModesKHR", name)) {
        *addr = ptr_instance->wsi_surface_enabled ? (void *) vkGetPhysicalDeviceSurfacePresentModesKHR : NULL;
        return true;
    }

    /*
     * Functions for the VK_KHR_swapchain extension:
     *
     * Note: This is a device extension, and its functions are statically
     * exported from the loader.  Per Khronos decisions, the the loader's GIPA
     * function will return the trampoline function for such device-extension
     * functions, regardless of whether the extension has been enabled.
     */
    if (!strcmp("vkCreateSwapchainKHR", name)) {
        *addr = (void *) vkCreateSwapchainKHR;
        return true;
    }
    if (!strcmp("vkDestroySwapchainKHR", name)) {
        *addr = (void *) vkDestroySwapchainKHR;
        return true;
    }
    if (!strcmp("vkGetSwapchainImagesKHR", name)) {
        *addr = (void *) vkGetSwapchainImagesKHR;
        return true;
    }
    if (!strcmp("vkAcquireNextImageKHR", name)) {
        *addr = (void *) vkAcquireNextImageKHR;
        return true;
    }
    if (!strcmp("vkQueuePresentKHR", name)) {
        *addr = (void *) vkQueuePresentKHR;
        return true;
    }

#ifdef _WIN32
#ifdef VK_USE_PLATFORM_WIN32_KHR
    /*
     * Functions for the VK_KHR_win32_surface extension:
     */
    if (!strcmp("vkCreateWin32SurfaceKHR", name)) {
        *addr = ptr_instance->wsi_win32_surface_enabled ? (void *) vkCreateWin32SurfaceKHR : NULL;
        return true;
    }
#endif // VK_USE_PLATFORM_WIN32_KHR
#else // _WIN32 (i.e. Linux)
#ifdef VK_USE_PLATFORM_MIR_KHR
    /*
     * Functions for the VK_KHR_mir_surface extension:
     */
    if (!strcmp("vkCreateMirSurfaceKHR", name)) {
        *addr = ptr_instance->wsi_mir_surface_enabled ? (void *) vkCreateMirSurfaceKHR : NULL;
        return true;
    }
#endif // VK_USE_PLATFORM_MIR_KHR
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
    /*
     * Functions for the VK_KHR_wayland_surface extension:
     */
    if (!strcmp("vkCreateWaylandSurfaceKHR", name)) {
        *addr = ptr_instance->wsi_wayland_surface_enabled ? (void *) vkCreateWaylandSurfaceKHR : NULL;
        return true;
    }
#endif // VK_USE_PLATFORM_WAYLAND_KHR
#ifdef VK_USE_PLATFORM_XCB_KHR
    /*
     * Functions for the VK_KHR_xcb_surface extension:
     */
    if (!strcmp("vkCreateXcbSurfaceKHR", name)) {
        *addr = ptr_instance->wsi_xcb_surface_enabled ? (void *) vkCreateXcbSurfaceKHR : NULL;
        return true;
    }
#endif // VK_USE_PLATFORM_XCB_KHR
#ifdef VK_USE_PLATFORM_XLIB_KHR
    /*
     * Functions for the VK_KHR_xlib_surface extension:
     */
    if (!strcmp("vkCreateXlibSurfaceKHR", name)) {
        *addr = ptr_instance->wsi_xlib_surface_enabled ? (void *) vkCreateXlibSurfaceKHR : NULL;
        return true;
    }
#endif // VK_USE_PLATFORM_XLIB_KHR
#endif // _WIN32

    return false;
}
