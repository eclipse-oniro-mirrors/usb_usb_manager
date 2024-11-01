/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef USB_ERRORS_H
#define USB_ERRORS_H

#include <errors.h>
#include <cstdint>
#include <string>

namespace OHOS {
namespace USB {
enum UsbModuleType {
    USB_MODULE_TYPE_SERVICE = 0,
    USB_MODULE_TYPE_KIT = 1,
};

const std::string PERMISSION_DENIED_SYSAPI = "PERMISSION_DENIED_SYSAPI";

#define USB_OFFSET 100

constexpr int32_t USBFWK_SERVICE_ERR_OFFSET = ErrCodeOffset(SUBSYS_USB, USB_MODULE_TYPE_SERVICE);

enum UsbErrCode {
    UEC_OK = 0,

    UEC_INTERFACE_NO_MEMORY = USBFWK_SERVICE_ERR_OFFSET,
    UEC_INTERFACE_INVALID_OPERATION,
    UEC_INTERFACE_INVALID_VALUE,
    UEC_INTERFACE_NAME_NOT_FOUND,
    UEC_INTERFACE_PERMISSION_DENIED,
    UEC_INTERFACE_NO_INIT,
    UEC_INTERFACE_ALREADY_EXISTS,
    UEC_INTERFACE_DEAD_OBJECT,
    UEC_INTERFACE_OVERFLOW,
    UEC_INTERFACE_ENOUGH_DATA,
    UEC_INTERFACE_WOULD_BLOCK,
    UEC_INTERFACE_TIMED_OUT,
    UEC_INTERFACE_WRITE_PARCEL_ERROR,
    UEC_INTERFACE_READ_PARCEL_ERROR,
    UEC_INTERFACE_GET_SYSTEM_ABILITY_MANAGER_FAILED,
    UEC_INTERFACE_GET_USB_SERVICE_FAILED,
    UEC_INTERFACE_ADD_DEATH_RECIPIENT_FAILED,
    UEC_INTERFACE_INNER_ERR,
    UEC_INTREFACE_END,

    UEC_SERVICE_NO_MEMORY = USBFWK_SERVICE_ERR_OFFSET + USB_OFFSET,
    UEC_SERVICE_INVALID_OPERATION,
    UEC_SERVICE_INVALID_VALUE,
    UEC_SERVICE_NAME_NOT_FOUND,
    UEC_SERVICE_PERMISSION_DENIED,
    UEC_SERVICE_NO_INIT,
    UEC_SERVICE_ALREADY_EXISTS,
    UEC_SERVICE_DEAD_OBJECT,
    UEC_SERVICE_OVERFLOW,
    UEC_SERVICE_ENOUGH_DATA,
    UEC_SERVICE_WOULD_BLOCK,
    UEC_SERVICE_TIMED_OUT,
    UEC_SERVICE_WRITE_PARCEL_ERROR,
    UEC_SERVICE_READ_PARCEL_ERROR,
    UEC_SERVICE_GET_SYSTEM_ABILITY_MANAGER_FAILED,
    UEC_SERVICE_GET_USB_SERVICE_FAILED,
    UEC_SERVICE_ADD_DEATH_RECIPIENT_FAILED,
    UEC_SERVICE_INNER_ERR,
    UEC_SERVICE_PERMISSION_DENIED_SYSAPI,
    UEC_SERVICE_PERMISSION_CHECK_HDC,
    UEC_SERVICE_NOT_SUPPORT_SWITCH_PORT,
    UEC_SERVICE_END,
    UEC_SERVICE_PRE_MANAGE_INTERFACE_FAILED,
    UEC_SERVICE_EXECUTE_POLICY_FAILED,
    UEC_SERVICE_PREPARE_EDM_SA_FAILED,
    UEC_SERVICE_GET_EDM_SERVICE_FAILED,
    UEC_SERVICE_EDM_DEVICE_SIZE_EXCEED,
    UEC_SERVICE_EDM_SA_TIME_OUT_FAILED,
    UEC_SERVICE_EDM_SEND_REQUEST_FAILED,
    UEC_SERVICE_OBJECT_CREATE_FAILED,
    UEC_SERVICE_GET_TOKEN_INFO_FAILED,
    UEC_SERVICE_ACCESSORY_NOT_MATCH,
    UEC_SERVICE_ACCESSORY_NOT_SUPPORT,
    UEC_SERVICE_ACCESSORY_REOPEN,
    UEC_SERVICE_DATABASE_OPERATOR_FAILED,
};

enum UsbRightErrCode {
    USB_RIGHT_OK = 0,
    USB_RIGHT_FAILURE = -1,
    USB_RIGHT_RDB_EXECUTE_FAILTURE = -2,
    USB_RIGHT_RDB_NO_INIT = -3,
    USB_RIGHT_RDB_EMPTY = -4,
    USB_RIGHT_PERMISSION_DENIED = -5,
    USB_RIGHT_NOP = -6,
    USB_RIGHT_OVERFLOW = -7,
};

} // namespace USB
} // namespace OHOS
#endif // USB_ERRORS_H
