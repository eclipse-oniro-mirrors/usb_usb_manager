/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "usbmgrmanagedevice_fuzzer.h"
#include "usb_srv_client.h"
#include "usb_errors.h"

namespace {
    const int32_t MAX_FUNC_NUM = 6;
}

namespace OHOS {
constexpr int32_t OFFSET = 4;
constexpr size_t THRESHOLD = 10;
namespace USB {
    bool UsbMgrManageDeviceFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr || size < OFFSET + sizeof(int32_t)) {
            USB_HILOGE(MODULE_USB_SERVICE, "data size is insufficient!");
            return false;
        }
        std::vector<UsbDevice> devList;
        auto &usbSrvClient = UsbSrvClient::GetInstance();
        auto ret = usbSrvClient.GetDevices(devList);
        if (ret != UEC_OK || devList.empty()) {
            USB_HILOGE(MODULE_USB_SERVICE, "get devices failed ret=%{public}d", ret);
            return false;
        }
        ret = usbSrvClient.ManageDevice(
            *reinterpret_cast<const int32_t *>(data), *reinterpret_cast<const int32_t *>(data + OFFSET), true);
        if (ret == UEC_OK) {
            return false;
        }
        return true;
    }
} // USB
} // OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (size < OHOS::THRESHOLD) {
        return 0;
    }
    /* Run your code on data */
    OHOS::USB::UsbMgrManageDeviceFuzzTest(data, size);
    return 0;
}

