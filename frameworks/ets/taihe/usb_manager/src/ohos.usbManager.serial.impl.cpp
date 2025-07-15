/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "ohos.usbManager.serial.impl.hpp"
#include "ohos.usbManager.serial.proj.hpp"
#include "stdexcept"
#include "taihe/runtime.hpp"

#include "hilog_wrapper.h"
#include "usb_errors.h"
#include "usb_serial_type.h"
#include "usb_srv_client.h"

using namespace OHOS;
using namespace OHOS::USB;
using namespace ohos::usbManager::serial;
using namespace taihe;

namespace {
// To be implemented.

static UsbSrvClient &g_usbClient = UsbSrvClient::GetInstance();

const int32_t ERROR = -1;
enum SerialTaiheErrCode : int32_t {
    SYSPARAM_INVALID_INPUT = 401,
    SERIAL_SYSAPI_NOPERMISSION_CALL = 201,
    SERIAL_SYSAPI_PERMISSION_DENIED = 202,
    SERIAL_SERVICE_ABNORMAL = 31400001,
    SERIAL_INTERFACE_PERMISSION_DENIED = 31400002,
    SERIAL_PORT_NOT_EXIST = 31400003,
    SERIAL_PORT_OCCUPIED = 31400004,
    SERIAL_PORT_NOT_OPEN = 31400005,
    SERIAL_TIMED_OUT = 31400006,
    SERIAL_IO_EXCEPTION = 31400007,
    UEC_COMMON_RIGHT_DATABASE_ERROR = 14400005,
};

int32_t ErrorCodeConversion(int32_t value)
{
    static const std::map<int32_t, int32_t> errorMap = {
        {UEC_SERVICE_PERMISSION_DENIED_SYSAPI,        SERIAL_SYSAPI_PERMISSION_DENIED   },
        {UEC_SERVICE_PERMISSION_DENIED,               SERIAL_INTERFACE_PERMISSION_DENIED},
        {UEC_SERVICE_PERMISSION_DENIED_SYSAPI_FAILED, SERIAL_SYSAPI_NOPERMISSION_CALL   },
        {UEC_SERIAL_PORT_REPEAT_OPEN,                 SERIAL_PORT_OCCUPIED              },
        {UEC_SERIAL_PORT_REPEAT_CLOSE,                SERIAL_PORT_OCCUPIED              },
        {UEC_SERIAL_PORT_OCCUPIED,                    SERIAL_PORT_OCCUPIED              },
        {UEC_SERIAL_DEVICENOTOPEN,                    SERIAL_PORT_NOT_OPEN              },
        {UEC_SERIAL_PORT_NOT_OPEN,                    SERIAL_PORT_NOT_OPEN              },
        {UEC_INTERFACE_TIMED_OUT,                     SERIAL_TIMED_OUT                  },
        {UEC_SERIAL_IO_EXCEPTION,                     SERIAL_IO_EXCEPTION               },
        {UEC_SERIAL_PORT_NOT_EXIST,                   SERIAL_PORT_NOT_EXIST             },
        {UEC_SERIAL_DATEBASE_ERROR,                   UEC_COMMON_RIGHT_DATABASE_ERROR   }
    };
    auto it = errorMap.find(value);
    if (it != errorMap.end()) {
        return it->second;
    }
    return SERIAL_SERVICE_ABNORMAL;
}

constexpr uint32_t MAX_READ_BUF_SIZE = 8192;
constexpr uint32_t MAX_WRITE_BUF_SIZE = 128000;

enum AttributeType : uint32_t {
    BAUDRATE = 0,
    DATABITS = 1,
    PARITY = 2,
    STOPBITS = 3,
};

inline bool CheckAndThrowOnError(bool assertion, int errCode, const std::string &errMsg)
{
    if (!assertion) {
        USB_HILOGI(MODULE_USB_INNERKIT, "Throw out ErrCode: [%{public}d], Msg: [%{public}s]", errCode, errMsg.c_str());
        taihe::set_business_error(errCode, errMsg.c_str());
        return false;
    }
    return true;
}

array<SerialPort> GetPortList()
{
    USB_HILOGI(MODULE_USB_INNERKIT, "GetPortList start.");
    std::vector<OHOS::USB::UsbSerialPort> portIds;
    std::vector<SerialPort> tempPortList;
    int32_t ret = g_usbClient.SerialGetPortList(portIds);
    if (ret < 0) {
        USB_HILOGE(MODULE_USB_INNERKIT, "GetPortList failed, ErrCode: %{public}d", ret);
        array<SerialPort> portList(tempPortList);
        return portList;
    }
    for (const auto &port : portIds) {
        SerialPort serialPort {.portId = port.portId_,
            .deviceName = std::to_string(port.busNum_) + "-" + std::to_string(port.devAddr_)};
        tempPortList.push_back(serialPort);
    }
    array<SerialPort> portList(tempPortList);
    return portList;
}

bool HasSerialRight(int32_t portId)
{
    USB_HILOGI(MODULE_USB_INNERKIT, "HasSerialRight start.");
    if (!CheckAndThrowOnError((portId >= 0), SYSPARAM_INVALID_INPUT, "HasSerialRight failed.")) {
        USB_HILOGE(MODULE_USB_INNERKIT, "HasSerialRight failed, invalid portId [%{public}d]", portId);
        return false;
    }
    bool hasRight = false;
    int32_t ret = g_usbClient.HasSerialRight(portId, hasRight);
    CheckAndThrowOnError((ret == 0), ErrorCodeConversion(ret), "HasSerialRight failed.");
    USB_HILOGI(MODULE_USB_INNERKIT, "HasSerialRight finish. ErrCode: %{public}d, ret: %{public}d", ret, hasRight);
    return hasRight;
}

bool RequestSerialRight(int32_t portId)
{
    USB_HILOGI(MODULE_USB_INNERKIT, "RequestSerialRight start.");
    if (!CheckAndThrowOnError((portId >= 0), SYSPARAM_INVALID_INPUT, "RequestSerialRight failed.")) {
        USB_HILOGE(MODULE_USB_INNERKIT, "RequestSerialRight failed, invalid portId [%{public}d]", portId);
        return false;
    }
    bool hasRight = false;
    int32_t ret = g_usbClient.RequestSerialRight(portId, hasRight);
    CheckAndThrowOnError((ret == 0), ErrorCodeConversion(ret), "RequestSerialRight failed.");
    USB_HILOGI(
        MODULE_USB_INNERKIT, "RequestSerialRight finish. ErrCode: %{public}d, ret: %{public}d", ret, hasRight);
    return hasRight;
}

void AddSerialRight(int32_t tokenId, int32_t portId)
{
    USB_HILOGI(MODULE_USB_INNERKIT, "AddSerialRight start.");
    if (!CheckAndThrowOnError((tokenId > 0), SYSPARAM_INVALID_INPUT, "AddSerialRight failed.")) {
        USB_HILOGI(MODULE_USB_INNERKIT, "AddSerialRight failed, invalid tockenId [%{public}d]", tokenId);
        return;
    }
    if (!CheckAndThrowOnError((portId >= 0), SYSPARAM_INVALID_INPUT, "AddSerialRight failed.")) {
        USB_HILOGI(MODULE_USB_INNERKIT, "AddSerialRight failed, invalid portId [%{public}d]", portId);
        return;
    }
    int32_t ret = g_usbClient.AddSerialRight(tokenId, portId);
    CheckAndThrowOnError((ret == 0), ErrorCodeConversion(ret), "AddSerialRight failed.");
    USB_HILOGI(MODULE_USB_INNERKIT, "AddSerialRight finish. ErrCode: %{public}d", ret);
}

void CancelSerialRight(int32_t portId)
{
    USB_HILOGI(MODULE_USB_INNERKIT, "CancelSerialRight start.");
    if (!CheckAndThrowOnError((portId >= 0), SYSPARAM_INVALID_INPUT, "CancelSerialRight failed.")) {
        USB_HILOGI(MODULE_USB_INNERKIT, "CancelSerialRight failed, invalid portId [%{public}d]", portId);
        return;
    }
    int32_t ret = g_usbClient.CancelSerialRight(portId);
    CheckAndThrowOnError((ret == 0), ErrorCodeConversion(ret), "CancelSerialRight failed.");
    USB_HILOGI(MODULE_USB_INNERKIT, "CancelSerialRight finish. ErrCode: %{public}d", ret);
}

void OpenSerial(int32_t portId)
{
    USB_HILOGI(MODULE_USB_INNERKIT, "OpenSerial start.");
    if (!CheckAndThrowOnError((portId >= 0), SYSPARAM_INVALID_INPUT, "OpenSerial failed.")) {
        USB_HILOGI(MODULE_USB_INNERKIT, "OpenSerial failed, invalid portId [%{public}d]", portId);
        return;
    }
    int ret = g_usbClient.SerialOpen(portId);
    CheckAndThrowOnError((ret == 0), ErrorCodeConversion(ret), "OpenSerial failed.");
    USB_HILOGI(MODULE_USB_INNERKIT, "OpenSerial finish. ErrCode: %{public}d", ret);
}

void CloseSerial(int32_t portId)
{
    USB_HILOGI(MODULE_USB_INNERKIT, "CloseSerial start.");
    if (!CheckAndThrowOnError((portId >= 0), SYSPARAM_INVALID_INPUT, "CloseSerial failed.")) {
        USB_HILOGI(MODULE_USB_INNERKIT, "CloseSerial failed, invalid portId [%{public}d]", portId);
        return;
    }
    int ret = g_usbClient.SerialClose(portId);
    CheckAndThrowOnError((ret == 0), ErrorCodeConversion(ret), "CloseSerial failed.");
    USB_HILOGI(MODULE_USB_INNERKIT, "CloseSerial finish. ErrCode: %{public}d", ret);
}

void SerialAttributeToTaihe(const UsbSerialAttr serialAttribute, SerialAttribute &taiheSerialAttribute)
{
    BaudRates baudRates = BaudRates::from_value(serialAttribute.baudRate_);
    taiheSerialAttribute.baudRate = baudRates.get_key();
    DataBits dataBits = DataBits::from_value(serialAttribute.dataBits_);
    taiheSerialAttribute.dataBits = optional<DataBits>::make(DataBits(dataBits.get_key()));
    Parity parity = Parity::from_value(serialAttribute.parity_);
    taiheSerialAttribute.parity = optional<Parity>::make(Parity(parity.get_key()));
    StopBits stopBits = StopBits::from_value(serialAttribute.stopBits_);
    taiheSerialAttribute.stopBits = optional<StopBits>::make(StopBits(stopBits.get_key()));
}

::ohos::usbManager::serial::SerialAttribute GetAttribute(int32_t portId)
{
    USB_HILOGI(MODULE_USB_INNERKIT, "getAttribute start. portId: %{public}d", portId);
    SerialAttribute taiheSerialAttribute = {BaudRates::key_t::BAUDRATE_50};
    if (portId < 0) {
        USB_HILOGE(MODULE_USB_INNERKIT, "portId is invalid!");
        set_business_error(SYSPARAM_INVALID_INPUT, "portId is invalid!");
        return taiheSerialAttribute;
    }

    UsbSerialAttr serialAttribute;
    int32_t ret = g_usbClient.SerialGetAttribute(portId, serialAttribute);
    if (ret != 0) {
        USB_HILOGE(MODULE_USB_INNERKIT, "Failed to get attribute. errorCode: %{public}d", ErrorCodeConversion(ret));
        set_business_error(ErrorCodeConversion(ret), "Failed to get attribute!");
        return taiheSerialAttribute;
    }
    SerialAttributeToTaihe(serialAttribute, taiheSerialAttribute);
    USB_HILOGI(MODULE_USB_INNERKIT, "getAttribute finish, status: %{public}d", ret);
    return taiheSerialAttribute;
}

void SetAttribute(int32_t portId, ::ohos::usbManager::serial::SerialAttribute const& attribute)
{
    USB_HILOGI(MODULE_USB_INNERKIT, "setAttribute start. portId: %{public}d", portId);
    if (portId < 0) {
        USB_HILOGE(MODULE_USB_INNERKIT, "portId is invalid!");
        set_business_error(SYSPARAM_INVALID_INPUT, "portId is invalid!");
        return;
    }

    UsbSerialAttr serialAttribute;
    serialAttribute.baudRate_ = attribute.baudRate.get_value();

    if (attribute.dataBits.has_value()) {
        serialAttribute.dataBits_ = attribute.dataBits.value();
    } else {
        USB_HILOGE(MODULE_USB_INNERKIT, "attribute.dataBits is invalid!");
        set_business_error(SYSPARAM_INVALID_INPUT, "attribute.dataBits is invalid!");
        return;
    }

    if (attribute.parity.has_value()) {
        serialAttribute.parity_ = attribute.parity.value();
    } else {
        USB_HILOGE(MODULE_USB_INNERKIT, "attribute.parity is invalid!");
        set_business_error(SYSPARAM_INVALID_INPUT, "attribute.parity is invalid!");
        return;
    }

    if (attribute.stopBits.has_value()) {
        serialAttribute.stopBits_ = attribute.stopBits.value();
    } else {
        USB_HILOGE(MODULE_USB_INNERKIT, "attribute.stopBits is invalid!");
        set_business_error(SYSPARAM_INVALID_INPUT, "attribute.stopBits is invalid!");
        return;
    }

    int ret = g_usbClient.SerialSetAttribute(portId, serialAttribute);
    if (ret != 0) {
        USB_HILOGE(MODULE_USB_INNERKIT, "Failed to set attribute. errorCode: %{public}d", ErrorCodeConversion(ret));
        set_business_error(ErrorCodeConversion(ret), "Failed to set attribute!");
    }
    USB_HILOGI(MODULE_USB_INNERKIT, "setAttribute finish, status: %{public}d", ret);
}

int32_t ReadSync(int32_t portId, ::taihe::array_view<uint8_t> buffer, ::taihe::optional_view<int32_t> timeout)
{
    USB_HILOGI(MODULE_USB_INNERKIT, "readSync start. portId: %{public}d", portId);
    if (portId < 0) {
        USB_HILOGE(MODULE_USB_INNERKIT, "portId is invalid!");
        set_business_error(SYSPARAM_INVALID_INPUT, "portId is invalid!");
        return ERROR;
    }

    if (buffer.size() > MAX_READ_BUF_SIZE) {
        USB_HILOGE(MODULE_USB_INNERKIT, "buffer size > 8192 byte!");
        set_business_error(SYSPARAM_INVALID_INPUT, "buffer size > 8192 byte!");
        return ERROR;
    }

    std::vector<uint8_t> bufferData(buffer.size(), 0);
    uint32_t actualSize = 0;
    uint32_t utimeout = 0;
    if (timeout.has_value()) {
        utimeout = timeout.value();
    } else {
        USB_HILOGE(MODULE_USB_INNERKIT, "timeout is invalid!");
        set_business_error(SYSPARAM_INVALID_INPUT, "timeout is invalid!");
        return ERROR;
    }
    int32_t ret = g_usbClient.SerialRead(portId, bufferData, buffer.size(), actualSize, utimeout);
    if (ret != 0) {
        USB_HILOGE(MODULE_USB_INNERKIT, "readSync Failed. errorCode: %{public}d", ErrorCodeConversion(ret));
        set_business_error(ErrorCodeConversion(ret), "readSync Failed!");
        return ERROR;
    }
    buffer = bufferData;
    USB_HILOGI(MODULE_USB_INNERKIT, "readSync finish, status: %{public}d", ret);
    return actualSize;
}

int32_t WriteSync(int32_t portId, ::taihe::array_view<uint8_t> buffer, ::taihe::optional_view<int32_t> timeout)
{
    USB_HILOGI(MODULE_USB_INNERKIT, "writeSync start. portId: %{public}d", portId);
    if (portId < 0) {
        USB_HILOGE(MODULE_USB_INNERKIT, "portId is invalid!");
        set_business_error(SYSPARAM_INVALID_INPUT, "portId is invalid!");
        return ERROR;
    }

    if (buffer.size() > MAX_WRITE_BUF_SIZE) {
        USB_HILOGE(MODULE_USB_INNERKIT, "buffer size > 128kb!");
        set_business_error(SYSPARAM_INVALID_INPUT, "buffer size > 128kb!");
        return ERROR;
    }

    uint32_t actualSize = 0;
    std::vector<uint8_t> bufferVector(buffer.data(), buffer.data() + buffer.size());
    uint32_t utimeout = 0;
    if (timeout.has_value()) {
        utimeout = timeout.value();
    } else {
        USB_HILOGE(MODULE_USB_INNERKIT, "timeout is invalid!");
        set_business_error(SYSPARAM_INVALID_INPUT, "timeout is invalid!");
        return ERROR;
    }
    int32_t ret = g_usbClient.SerialWrite(portId, bufferVector, buffer.size(), actualSize, utimeout);
    if (ret != 0) {
        USB_HILOGE(MODULE_USB_INNERKIT, "writeSync Failed. errorCode: %{public}d", ErrorCodeConversion(ret));
        set_business_error(ErrorCodeConversion(ret), "readSync Failed!");
        return ERROR;
    }
    USB_HILOGI(MODULE_USB_INNERKIT, "writeSync finish, status: %{public}d", ret);
    return actualSize;
}
} // namespace

// Since these macros are auto-generate, lint will cause false positive.
// NOLINTBEGIN
TH_EXPORT_CPP_API_getPortList(GetPortList);
TH_EXPORT_CPP_API_hasSerialRight(HasSerialRight);
TH_EXPORT_CPP_API_requestSerialRightSync(RequestSerialRight);
TH_EXPORT_CPP_API_addSerialRight(AddSerialRight);
TH_EXPORT_CPP_API_cancelSerialRight(CancelSerialRight);
TH_EXPORT_CPP_API_open(OpenSerial);
TH_EXPORT_CPP_API_close(CloseSerial);
TH_EXPORT_CPP_API_getAttribute(GetAttribute);
TH_EXPORT_CPP_API_setAttribute(SetAttribute);
TH_EXPORT_CPP_API_readSync(ReadSync);
TH_EXPORT_CPP_API_writeSync(WriteSync);
// NOLINTEND
