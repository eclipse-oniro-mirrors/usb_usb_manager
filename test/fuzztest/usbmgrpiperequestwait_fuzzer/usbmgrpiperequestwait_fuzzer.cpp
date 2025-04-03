/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "usbmgrpiperequestwait_fuzzer.h"

#include "usb_errors.h"
#include "usb_srv_client.h"

namespace OHOS {
const uint32_t OFFSET = 4;
const uint32_t OFFSET_BYTE = 8;
constexpr size_t THRESHOLD = 10;
namespace USB {
    bool UsbMgrPipeRequestWaitFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr || size < sizeof(USBDevicePipe) || size < OFFSET + sizeof(int64_t) ||
            size < OFFSET_BYTE + sizeof(UsbRequest)) {
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

        USBDevicePipe pipe;
        UsbDevice device = devList.front();
        usbSrvClient.RequestRight(device.GetName());
        ret = usbSrvClient.OpenDevice(device, pipe);
        if (ret != UEC_OK) {
            USB_HILOGE(MODULE_USB_SERVICE, "open device failed ret=%{public}d", ret);
            return false;
        }

        UsbInterface interface = devList.front().GetConfigs().front().GetInterfaces().front();
        ret = usbSrvClient.ClaimInterface(pipe, interface, true);
        if (ret != UEC_OK) {
            USB_HILOGE(MODULE_USB_SERVICE, "ClaimInterface failed ret=%{public}d", ret);
            return false;
        }

        ret = usbSrvClient.PipeRequestWait(reinterpret_cast<USBDevicePipe &>(data),
            *reinterpret_cast<const int64_t *>(std::move(data + OFFSET)),
            reinterpret_cast<UsbRequest &>(std::move(data + OFFSET_BYTE)));
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
    OHOS::USB::UsbMgrPipeRequestWaitFuzzTest(data, size);
    return 0;
}

