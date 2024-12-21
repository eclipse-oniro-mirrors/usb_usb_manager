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

#include "usb_function_switch_window.h"

#include <parameters.h>
#include <param_wrapper.h>
#include <semaphore.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>

#include "cJSON.h"

#include "ability_manager_client.h"
#include "bundle_mgr_client.h"
#include "common_event_manager.h"
#include "common_event_support.h"
#include "usb_errors.h"

#define DEFAULT_PARAM_VALUE "charge,mtp,ptp"
using namespace OHOS::AppExecFwk;
using namespace OHOS::EventFwk;

namespace OHOS {
namespace USB {
constexpr int32_t INVALID_USERID = -1;
constexpr int32_t MESSAGE_PARCEL_KEY_SIZE = 3;
constexpr int32_t MAX_RETRY_TIMES = 30;
constexpr int32_t RETRY_INTERVAL_SECONDS = 1;
constexpr uint32_t DELAY_CHECK_DIALOG = 1;

std::shared_ptr<UsbFunctionSwitchWindow> UsbFunctionSwitchWindow::instance_;
std::mutex UsbFunctionSwitchWindow::insMutex_;

std::shared_ptr<UsbFunctionSwitchWindow> UsbFunctionSwitchWindow::GetInstance()
{
    std::lock_guard<std::mutex> guard(insMutex_);
    if (instance_ == nullptr) {
        USB_HILOGI(MODULE_USB_SERVICE, "reset to new instance");
        instance_.reset(new UsbFunctionSwitchWindow());
    }
    return instance_;
}

UsbFunctionSwitchWindow::UsbFunctionSwitchWindow() {}

UsbFunctionSwitchWindow::~UsbFunctionSwitchWindow()
{
    if (windowAction_ == UsbFunctionSwitchWindowAction::FUNCTION_SWITCH_WINDOW_ACTION_SHOW) {
        (void)UnShowFunctionSwitchWindow();
    }
}

int32_t UsbFunctionSwitchWindow::Init()
{
    USB_HILOGI(MODULE_USB_SERVICE, "init: window action=%{public}d,%{public}d", windowAction_, isDialogInstalled_);
    if (isDialogInstalled_) {
        return UEC_OK;
    }

    checkDialogTimer_.Unregister(checkDialogTimerId_);
    checkDialogTimer_.Shutdown();
    // async check dialog install status
    auto task = [this]() {
        CheckDialogInstallStatus();
        checkDialogTimer_.Unregister(checkDialogTimerId_);
        checkDialogTimer_.Shutdown(false);
        USB_HILOGI(MODULE_USB_SERVICE, "dialog check end");
    };
    auto ret = checkDialogTimer_.Setup();
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "set up timer failed %{public}u", ret);
        // fall back to sync
        CheckDialogInstallStatus();
        return isDialogInstalled_ ? UEC_OK : ret;
    }
    checkDialogTimerId_ = checkDialogTimer_.Register(task, DELAY_CHECK_DIALOG, true);
    return UEC_OK;
}

bool UsbFunctionSwitchWindow::PopUpFunctionSwitchWindow()
{
    USB_HILOGI(MODULE_USB_SERVICE, "pop up function switch window");
    bool isPromptEnabeld = OHOS::system::GetBoolParameter("persist.usb.setting.gadget_conn_prompt", false);
    if (!isPromptEnabeld) {
        USB_HILOGE(MODULE_USB_SERVICE, "gadget_conn_prompt is false");
        return false;
    }
    std::string supportedFuncStr = "";
    (void)OHOS::system::GetStringParameter("const.usb.support_functions", supportedFuncStr, DEFAULT_PARAM_VALUE);
    int32_t supportedFuncs = ParseSupposeFuncs(supportedFuncStr);
    USB_HILOGI(MODULE_USB_SERVICE, "%{public}s: supportedFuncs %{public}d", __func__, supportedFuncs);
    if (supportedFuncs < 0) {
        USB_HILOGE(MODULE_USB_SERVICE, "no supported functions: %{public}d", supportedFuncs);
        return false;
    }

    std::lock_guard<std::mutex> guard(opMutex_);
    if (windowAction_ == UsbFunctionSwitchWindowAction::FUNCTION_SWITCH_WINDOW_ACTION_FORBID) {
        USB_HILOGI(MODULE_USB_SERVICE, "forbid: pop up function switch window");
        return false;
    }
    windowAction_ = UsbFunctionSwitchWindowAction::FUNCTION_SWITCH_WINDOW_ACTION_SHOW;
    return ShowFunctionSwitchWindow();
}

bool UsbFunctionSwitchWindow::DismissFunctionSwitchWindow()
{
    USB_HILOGI(MODULE_USB_SERVICE, "dismiss function switch window");
    std::lock_guard<std::mutex> guard(opMutex_);
    if (windowAction_ == UsbFunctionSwitchWindowAction::FUNCTION_SWITCH_WINDOW_ACTION_FORBID) {
        USB_HILOGI(MODULE_USB_SERVICE, "forbid: dismiss function switch window");
        return false;
    }
    windowAction_ = UsbFunctionSwitchWindowAction::FUNCTION_SWITCH_WINDOW_ACTION_DISMISS;
    return UnShowFunctionSwitchWindow();
}

int32_t UsbFunctionSwitchWindow::ParseSupposeFuncs(const std::string &value)
{
    USB_HILOGI(MODULE_USB_SERVICE, "%{public}s: value %{public}s", __func__, value.c_str());

    if (value == "none") {
        return SUPPORTED_FUNC_NONE;
    }
    int32_t mtp = value.find("mtp") != std::string::npos ? SUPPORTED_FUNC_MTP : 0;
    int32_t ptp = value.find("ptp") != std::string::npos ? SUPPORTED_FUNC_PTP : 0;

    return mtp|ptp;
}

void UsbFunctionSwitchWindow::UsbFuncAbilityConn::OnAbilityConnectDone(const AppExecFwk::ElementName &element,
    const sptr<IRemoteObject> &remoteObject, int32_t resultCode)
{
    USB_HILOGI(MODULE_USB_SERVICE, "OnAbilityConnectDone");
    if (remoteObject == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "remoteObject is nullptr");
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInt32(MESSAGE_PARCEL_KEY_SIZE);
    data.WriteString16(u"bundleName");
    data.WriteString16(u"com.usb.right");
    data.WriteString16(u"abilityName");
    data.WriteString16(u"UsbFunctionSwitchExtAbility");
    data.WriteString16(u"parameters");
    cJSON* paramJson = cJSON_CreateObject();
    std::string supportedFuncStr = "";
    (void)OHOS::system::GetStringParameter("const.usb.support_functions", supportedFuncStr, DEFAULT_PARAM_VALUE);
    int32_t supportedFuncs = ParseSupposeFuncs(supportedFuncStr);
    USB_HILOGI(MODULE_USB_SERVICE, "%{public}s: supportedFuncs %{public}d", __func__, supportedFuncs);
    cJSON_AddStringToObject(paramJson, "supportedFuncs", std::to_string(supportedFuncs).c_str());
    std::string uiExtensionTypeStr = "sysDialog/common";
    cJSON_AddStringToObject(paramJson, "ability.want.params.uiExtensionType", uiExtensionTypeStr.c_str());
    char *pParamJson = cJSON_PrintUnformatted(paramJson);
    cJSON_Delete(paramJson);
    paramJson = nullptr;
    if (!pParamJson) {
        USB_HILOGE(MODULE_USB_SERVICE, "Print paramJson error");
        return;
    }
    std::string paramStr(pParamJson);
    data.WriteString16(Str8ToStr16(paramStr));
    cJSON_free(pParamJson);
    pParamJson = NULL;

    const uint32_t cmdCode = 1;
    int32_t ret = remoteObject->SendRequest(cmdCode, data, reply, option);
    if (ret != ERR_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "send request failed: %{public}d", ret);
        return;
    }
    if (!reply.ReadInt32(ret) || ret != ERR_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "show dialog failed: %{public}d", ret);
        return;
    }
    remoteObject_ = remoteObject;
    return;
}

void UsbFunctionSwitchWindow::UsbFuncAbilityConn::OnAbilityDisconnectDone(
    const AppExecFwk::ElementName& element, int resultCode)
{
    USB_HILOGI(MODULE_USB_SERVICE, "OnAbilityDisconnectDone");
    remoteObject_ = nullptr;
    return;
}

void UsbFunctionSwitchWindow::UsbFuncAbilityConn::CloseDialog()
{
    if (remoteObject_ == nullptr) {
        USB_HILOGW(MODULE_USB_SERVICE, "CloseDialog: disconnected");
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    const uint32_t cmdCode = 3;
    int32_t ret = remoteObject_->SendRequest(cmdCode, data, reply, option);
    int32_t replyCode = -1;
    bool success = false;
    if (ret == ERR_OK) {
        success = reply.ReadInt32(replyCode);
    }
    USB_HILOGI(MODULE_USB_SERVICE, "CloseDialog: ret=%{public}d, %{public}d, %{public}d", ret, success, replyCode);
}

bool UsbFunctionSwitchWindow::ShowFunctionSwitchWindow()
{
    USB_HILOGI(MODULE_USB_SERVICE, "show function switch window right now, installed: %{public}d", isDialogInstalled_);
    if (!isDialogInstalled_) {
        return false;
    }

    if (usbFuncAbilityConn == nullptr) {
        usbFuncAbilityConn = sptr<UsbFuncAbilityConn>(new (std::nothrow) UsbFuncAbilityConn());
    }

    auto abilityManager = AAFwk::AbilityManagerClient::GetInstance();
    if (abilityManager == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "AbilityManagerClient is nullptr");
        return false;
    }

    AAFwk::Want want;
    want.SetElementName("com.ohos.sceneboard", "com.ohos.sceneboard.systemdialog");
    auto ret = abilityManager->ConnectAbility(want, usbFuncAbilityConn, INVALID_USERID);
    if (ret != ERR_OK) {
        want.SetElementName("com.ohos.systemui", "com.ohos.systemui.dialog");
        ret = abilityManager->ConnectAbility(want, usbFuncAbilityConn, INVALID_USERID);
        if (ret != ERR_OK) {
            USB_HILOGE(MODULE_USB_SERVICE, "ConnectServiceExtensionAbility systemui failed, ret: %{public}d", ret);
            usbFuncAbilityConn = nullptr;
            return false;
        }
    }
    USB_HILOGI(MODULE_SERVICE, "StartAbility success, ret: %{public}d", ret);
    return true;
}

bool UsbFunctionSwitchWindow::UnShowFunctionSwitchWindow()
{
    if (usbFuncAbilityConn == nullptr) {
        return true;
    }

    auto abmc = AAFwk::AbilityManagerClient::GetInstance();
    if (abmc == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "GetInstance failed");
        return false;
    }
    USB_HILOGI(MODULE_USB_SERVICE, "unshow function switch window");
    usbFuncAbilityConn->CloseDialog();

    auto ret = abmc->DisconnectAbility(usbFuncAbilityConn);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_SERVICE, "DisconnectAbility failed %{public}d", ret);
        return false;
    }
    USB_HILOGD(MODULE_USB_SERVICE, "unshow function switch window success");
    return true;
}

bool UsbFunctionSwitchWindow::CheckDialogInstallStatus()
{
    AppExecFwk::BundleInfo info;
    AppExecFwk::BundleMgrClient bmc;
    int32_t retryTimes = 0;
    while (retryTimes < MAX_RETRY_TIMES) {
        isDialogInstalled_ = bmc.GetBundleInfo(functionSwitchBundleName_,
            AppExecFwk::BundleFlag::GET_BUNDLE_DEFAULT, info, AppExecFwk::Constants::ALL_USERID);
        USB_HILOGI(MODULE_USB_SERVICE, "check dialog, times=%{public}d,res=%{public}d", retryTimes, isDialogInstalled_);
        if (!isDialogInstalled_) {
            retryTimes++;
            sleep(RETRY_INTERVAL_SECONDS);
            continue;
        }

        if (windowAction_ == UsbFunctionSwitchWindowAction::FUNCTION_SWITCH_WINDOW_ACTION_SHOW) {
            ShowFunctionSwitchWindow();
        }
        return true;
    }
    USB_HILOGE(MODULE_USB_SERVICE, "dialog is not installed");
    return false;
}
} // namespace USB
} // namespace OHOS
