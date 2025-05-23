/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef USB_FUNCTION_SWITCH_WINDOW_H
#define USB_FUNCTION_SWITCH_WINDOW_H

#include <mutex>

#include "ability_connect_callback_stub.h"
#include "bundle_mgr_interface.h"
#include "parameter.h"
#include "usb_common.h"
#include "usb_srv_support.h"
#include "timer.h"

namespace OHOS {
namespace USB {

enum UsbFunctionChoose : int32_t {
    FUNCTION_CHOOSE_CHARGE_ONLY = -1,
    FUNCTION_CHOOSE_TRANSFER_FILE = UsbSrvSupport::FUNCTION_MTP,
    FUNCTION_CHOOSE_TRANSFER_PIC = UsbSrvSupport::FUNCTION_PTP,
};

enum UsbFunctionSwitchWindowAction : int32_t {
    FUNCTION_SWITCH_WINDOW_ACTION_DEFAULT = 0,
    FUNCTION_SWITCH_WINDOW_ACTION_SHOW,
    FUNCTION_SWITCH_WINDOW_ACTION_DISMISS,
    FUNCTION_SWITCH_WINDOW_ACTION_FORBID,
};

enum SUPPORTED_FUNC : int32_t {
    SUPPORTED_FUNC_NONE = -1,
    SUPPORTED_FUNC_CHARGE = 0,
    SUPPORTED_FUNC_MTP = 8,
    SUPPORTED_FUNC_PTP = 16,
};

class UsbFunctionSwitchWindow {
public:
    static std::shared_ptr<UsbFunctionSwitchWindow> GetInstance();
    ~UsbFunctionSwitchWindow();
    int32_t Init();
    bool PopUpFunctionSwitchWindow();
    bool DismissFunctionSwitchWindow();
    int32_t SetCurrentFunctionLabel(int32_t func);
    int32_t RemoveCurrentFunctionLabel();
    bool CheckDialogInstallStatus();
private:
    UsbFunctionSwitchWindow();
    DISALLOW_COPY_AND_MOVE(UsbFunctionSwitchWindow);
    class UsbFuncAbilityConn : public OHOS::AAFwk::AbilityConnectionStub {
        void OnAbilityConnectDone(const AppExecFwk::ElementName &element, const sptr<IRemoteObject> &remoteObject,
            int32_t resultCode) override;
        void OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int32_t resultCode) override;
    public:
        void CloseDialog();
    private:
        sptr<IRemoteObject> remoteObject_ = nullptr;
    };
    bool ShowFunctionSwitchWindow();
    bool UnShowFunctionSwitchWindow();
    void SubscribeCommonEvent();
    bool ShouldRejectShowWindow();
    static void BootCompletedEventCallback(const char *key, const char *value, void *context);
    static std::shared_ptr<UsbFunctionSwitchWindow> instance_;
    static int32_t GetSupportedFunctions();
    sptr<UsbFuncAbilityConn> usbFuncAbilityConn = nullptr;
    int32_t windowAction_ = UsbFunctionSwitchWindowAction::FUNCTION_SWITCH_WINDOW_ACTION_DEFAULT;
    static std::mutex insMutex_;
    std::mutex opMutex_;
    const std::string functionSwitchBundleName_ = "com.usb.right";
    const std::string functionSwitchExtAbility_ = "UsbFunctionSwitchExtAbility";
    bool isDialogInstalled_ = false;
    Utils::Timer checkDialogTimer_ {"checkDialogTimer"};
    uint32_t checkDialogTimerId_ {UINT32_MAX};
};
} // namespace USB
} // namespace OHOS

#endif
