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

#ifndef USBMGR_USBD_BULKCALLBACK_IMPL_H
#define USBMGR_USBD_BULKCALLBACK_IMPL_H

#include <refbase.h>
#include "iremote_object.h"
#include "v1_0/iusbd_bulk_callback.h"

using OHOS::IRemoteObject;

using OHOS::sptr;

using OHOS::HDI::Usb::V1_0::IUsbdBulkCallback;

class UsbdBulkCallbackImpl : public IUsbdBulkCallback {
public:
    explicit UsbdBulkCallbackImpl(const sptr<IRemoteObject> &cb) : remote_(cb) {}
    int32_t OnBulkWriteCallback(int32_t status, int32_t actLength) override;
    int32_t OnBulkReadCallback(int32_t status, int32_t actLength) override;

private:
    sptr<IRemoteObject> remote_ = nullptr;
};
#endif // USBMGR_USBD_BULKCALLBACK_IMPL_H