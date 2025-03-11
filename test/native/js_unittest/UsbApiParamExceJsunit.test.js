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

import usbManager from '@ohos.usbManager';
import { UiDriver, BY } from '@ohos.UiTest';
import CheckEmptyUtils from '../../../../../ohos_master/test/xts/acts/usb/usb_standard/entry/src/ohosTest/js/test/CheckEmptyUtils.js';
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from '@ohos/hypium'


export default function UsbApiParamExceJsunitTest() {
describe('UsbApiParamExceJsunitTest', function () {

    const TAG = "[UsbApiParamExceJsunitTest]";
    const PARAM_NULL = null;
    const PARAM_UNDEFINED = undefined;
    const PARAM_NULLSTRING = "";
    const PARAM_NUMBEREX = 123;
    let gDeviceList;
    let devices;
    let gPipe;
    let isDeviceConnected;
    let tmpPipe = {
        busNum: null,
        devAddress: null
    };
    function deviceConnected() {
        if (gDeviceList.length > 0) {
            console.info(TAG, "Test USB device is connected");
            return true;
        }
        console.info(TAG, "Test USB device is not connected");
        return false;
    }

    beforeAll(async function () {
        console.log(TAG, '*************Usb Unit UsbApiParamExceJsunitTest Begin*************');
        const Version = usbManager.getVersion();
        console.info(TAG, 'usb unit begin test getversion :' + Version);

        // version > 17  host currentMode = 2 device currentMode = 1
        gDeviceList = usbManager.getDevices();
        isDeviceConnected = deviceConnected();
        if (isDeviceConnected) {
            let hasRight = usbManager.hasRight(gDeviceList[0].name);
            if (!hasRight) {
                console.info(TAG, `beforeAll: usb requestRight start`);
                await getPermission();
                CheckEmptyUtils.sleep(1000);
                await driveFn();
                CheckEmptyUtils.sleep(1000);
            }

            tmpPipe.busNum = gDeviceList[0].busNum;
            tmpPipe.devAddress = gDeviceList[0].devAddress;
        }
    })

    beforeEach(function () {
        console.info(TAG, 'beforeEach: *************Usb Unit Test CaseEx*************');
        gDeviceList = usbManager.getDevices();
        if (isDeviceConnected) {
            devices = gDeviceList[0];
            console.info(TAG, 'beforeEach return devices : ' + JSON.stringify(devices));
        }
    })

    afterEach(function () {
        console.info(TAG, 'afterEach: *************Usb Unit Test CaseEx*************');
        devices = null;
        gPipe = null;
        console.info(TAG, 'afterEach return devices : ' + JSON.stringify(devices));
    })

    afterAll(function () {
        console.log(TAG, '*************Usb Unit UsbApiParamExceJsunitTest End*************');
    })

    async function driveFn() {
        console.info('**************driveFn**************');
        try {
            let driver = await UiDriver.create();
            console.info(TAG, ` come in driveFn`);
            console.info(TAG, `driver is ${JSON.stringify(driver)}`);
            CheckEmptyUtils.sleep(1000);
            let button = await driver.findComponent(BY.text('允许'));
            console.info(TAG, `button is ${JSON.stringify(button)}`);
            CheckEmptyUtils.sleep(1000);
            await button.click();
        } catch (err) {
            console.info(TAG, 'err is ' + err);
            return;
        }
    }

    async function getPermission() {
        console.info('**************getPermission**************');
        try {
            usbManager.requestRight(gDeviceList[0].name).then(hasRight => {
                console.info(TAG, `usb requestRight success, hasRight: ${hasRight}`);
            })
        } catch (err) {
            console.info(TAG, `usb getPermission to requestRight hasRight fail: `, err);
            return
        }
    }

    function getPipe(testCaseName) {
        gPipe = usbManager.connectDevice(devices);
        console.info(TAG, `usb ${testCaseName} connectDevice getPipe ret: ${JSON.stringify(gPipe)}`);
        expect(gPipe !== null).assertTrue();
    }

    function toReleaseInterface(testCaseName, tInterface) {
        let ret = usbManager.releaseInterface(tmpPipe, tInterface);
        console.info(TAG, `usb ${testCaseName} releaseInterface ret: ${ret}`);
        expect(ret).assertEqual(0);
    }
    
    function toClosePipe(testCaseName) {
        let isPipClose = usbManager.closePipe(tmpPipe);
        console.info(TAG, `usb ${testCaseName} closePipe ret: ${isPipClose}`);
        expect(isPipClose).assertEqual(0);
    }

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_0100
     * @tc.name     : testHasRightParamEx001
     * @tc.desc     : Negative test: Param is null string
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testHasRightParamEx001', 0, function () {
        console.info(TAG, 'usb testHasRightParamEx001 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            let isHasRight = usbManager.hasRight(PARAM_NULLSTRING);
            console.info(TAG, 'usb case hasRight ret : ' + isHasRight);
            expect(isHasRight).assertFalse();
        } catch (err) {
            console.info(TAG, 'testHasRightParamEx001 catch err code: ', err.code, ', message: ', err.message);
            expect(err !== null).assertFalse();
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_0200
     * @tc.name     : testHasRightParamEx002
     * @tc.desc     : Negative test: Param add number '123'
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testHasRightParamEx002', 0, function () {
        console.info(TAG, 'usb testHasRightParamEx002 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            for (var i = 0; i < gDeviceList.length; i++) {
                let deviceName = gDeviceList[i].name;
                deviceName = deviceName + "123";
                let isHasRight = usbManager.hasRight(deviceName);
                console.info(TAG, 'usb [', deviceName, '] hasRight ret : ' + isHasRight);
                expect(isHasRight).assertFalse();
            }
        } catch (err) {
            console.info(TAG, 'testHasRightParamEx002 catch err code: ', err.code, ', message: ', err.message);
            expect(err !== null).assertFalse();
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_0300
     * @tc.name     : testRequestRightParamEx001
     * @tc.desc     : Negative test: Param is null string
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testRequestRightParamEx001', 0, async function () {
        console.info(TAG, 'usb testRequestRightParamEx001 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            let isHasRight = await usbManager.requestRight(PARAM_NULLSTRING);
            console.info(TAG, 'usb case requestRight ret : ' + isHasRight);
            expect(isHasRight).assertFalse();
        } catch (err) {
            console.info(TAG, 'testRequestRightParamEx001 catch err code: ', err.code, ', message: ', err.message);
            expect(err !== null).assertFalse();
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_0400
     * @tc.name     : testRequestRightParamEx002
     * @tc.desc     : Negative test: Param add number 'abc'
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testRequestRightParamEx002', 0, async function () {
        console.info(TAG, 'usb testRequestRightParamEx002 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            for (var i = 0; i < gDeviceList.length; i++) {
                let deviceName = gDeviceList[i].name;
                deviceName = deviceName + "abc";
                let isHasRight = await usbManager.requestRight(deviceName);
                console.info(TAG, 'usb [', deviceName, '] requestRight ret : ' + isHasRight);
                expect(isHasRight).assertFalse();
            }
        } catch (err) {
            console.info(TAG, 'testRequestRightParamEx002 catch err code: ', err.code, ', message: ', err.message);
            expect(err !== null).assertFalse();
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_0500
     * @tc.name     : testRemoveRightParamEx001
     * @tc.desc     : Negative test: Param is null string
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testRemoveRightParamEx001', 0, function () {
        console.info(TAG, 'usb testRemoveRightParamEx001 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            let remRight = usbManager.removeRight(PARAM_NULLSTRING);
            console.info(TAG, 'usb case removeRight ret : ' + remRight);
            expect(remRight).assertFalse();
        } catch (err) {
            console.info(TAG, 'testRemoveRightParamEx001 catch err code: ', err.code, ', message: ', err.message);
            expect(err !== null).assertFalse();
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_0600
     * @tc.name     : testRemoveRightParamEx002
     * @tc.desc     : Negative test: Param add letter 'abc'
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testRemoveRightParamEx002', 0, function () {
        console.info(TAG, 'usb testRemoveRightParamEx002 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            for (var i = 0; i < gDeviceList.length; i++) {
                let deviceName = gDeviceList[i].name;
                deviceName = deviceName + "abc";
                let remRight = usbManager.removeRight(deviceName);
                console.info(TAG, 'usb [', deviceName, '] removeRight ret : ', remRight);
                expect(remRight).assertFalse();
            }
        } catch (err) {
            console.info(TAG, 'testRemoveRightParamEx002 catch err code: ', err.code, ', message: ', err.message);
            expect(err !== null).assertFalse();
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_0700
     * @tc.name     : testRemoveRightParamEx003
     * @tc.desc     : Negative test: Param add special characters '@#'
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testRemoveRightParamEx003', 0, function () {
        console.info(TAG, 'usb testRemoveRightParamEx003 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            for (var i = 0; i < gDeviceList.length; i++) {
                let deviceName = gDeviceList[i].name;
                deviceName = deviceName + "@#";
                let remRight = usbManager.removeRight(deviceName);
                console.info(TAG, 'usb [', deviceName, '] removeRight ret : ', remRight);
                expect(remRight).assertFalse();
            }
        } catch (err) {
            console.info(TAG, 'testRemoveRightParamEx003 catch err code: ', err.code, ', message: ', err.message);
            expect(err !== null).assertFalse();
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_0800
     * @tc.name     : testRemoveRightParamEx004
     * @tc.desc     : Negative test: Param add number '123'
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testRemoveRightParamEx004', 0, function () {
        console.info(TAG, 'usb testRemoveRightParamEx004 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            for (var i = 0; i < gDeviceList.length; i++) {
                let deviceName = gDeviceList[i].name;
                deviceName = deviceName + "123";
                let remRight = usbManager.removeRight(deviceName);
                console.info(TAG, 'usb [', deviceName, '] removeRight ret : ', remRight);
                expect(remRight).assertFalse();
            }
        } catch (err) {
            console.info(TAG, 'testRemoveRightParamEx004 catch err code: ', err.code, ', message: ', err.message);
            expect(err !== null).assertFalse();
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_0900
     * @tc.name     : testConnectDeviceParamEx001
     * @tc.desc     : Negative test: Param add number '123'
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamEx001', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamEx001 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            let deviceName = devices.name + "123";
            devices.name = deviceName;
            let gPipe = usbManager.connectDevice(devices);

            console.info(TAG, 'usb [', devices.name, '] connectDevice ret : ', JSON.stringify(gPipe));
            expect(CheckEmptyUtils.isEmpty(gPipe)).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamEx001 catch err code: ', err.code, ', message: ', err.message);
            expect(err !== null).assertFalse();
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_1000
     * @tc.name     : testConnectDeviceParamEx002
     * @tc.desc     : Negative test: Param add letter 'abc'
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamEx002', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamEx002 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            let deviceName = devices.name + "abc";
            devices.name = deviceName;
            let gPipe = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [', devices.name, '] connectDevice ret : ', JSON.stringify(gPipe));
            expect(CheckEmptyUtils.isEmpty(gPipe)).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamEx002 catch err code: ', err.code, ', message: ', err.message);
            expect(err !== null).assertFalse();
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_1100
     * @tc.name     : testConnectDeviceParamEx003
     * @tc.desc     : Negative test: Param add special characters '@#'
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamEx003', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamEx003 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            let deviceName = devices.name + "@#";
            devices.name = deviceName;
            let gPipe = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [', devices.name, '] connectDevice ret : ', JSON.stringify(gPipe));
            expect(CheckEmptyUtils.isEmpty(gPipe)).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamEx003 catch err code: ', err.code, ', message: ', err.message);
            expect(err !== null).assertFalse();
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_1200
     * @tc.name     : testConnectDeviceParamEx004
     * @tc.desc     : Negative test: devices name is null string ""
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamEx004', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamEx004 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            devices.name = PARAM_NULLSTRING;
            let gPipe = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [', devices.name, '] connectDevice ret : ', JSON.stringify(gPipe));
            expect(CheckEmptyUtils.isEmpty(gPipe)).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamEx004 catch err code: ', err.code, ', message: ', err.message);
            expect(err !== null).assertFalse();
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_1300
     * @tc.name     : testConnectDeviceParamEx005
     * @tc.desc     : Negative test: devices serial is null string ""
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamEx005', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamEx005 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            devices.serial = PARAM_NULLSTRING;
            let gPipe = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [', devices.serial, '] connectDevice ret : ', JSON.stringify(gPipe));
            expect(CheckEmptyUtils.isEmpty(gPipe)).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamEx005 catch err code: ', err.code, ', message: ', err.message);
            expect(err !== null).assertFalse();
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_1400
     * @tc.name     : testConnectDeviceParamEx006
     * @tc.desc     : Negative test: devices serial add letter abc
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamEx006', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamEx006 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            let devSerial = devices.serial + "abc";
            devices.serial = devSerial;
            let gPipe = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [', devices.serial, '] connectDevice ret : ', JSON.stringify(gPipe));
            expect(CheckEmptyUtils.isEmpty(gPipe)).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamEx006 catch err code: ', err.code, ', message: ', err.message);
            expect(err !== null).assertFalse();
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_1500
     * @tc.name     : testConnectDeviceParamEx007
     * @tc.desc     : Negative test: devices serial add number 123
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamEx007', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamEx007 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            let devSerial = devices.serial + "123";
            devices.serial = devSerial;
            let gPipe = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [', devices.serial, '] connectDevice ret : ', JSON.stringify(gPipe));
            expect(CheckEmptyUtils.isEmpty(gPipe)).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamEx007 catch err code: ', err.code, ', message: ', err.message);
            expect(err !== null).assertFalse();
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_1600
     * @tc.name     : testConnectDeviceParamEx008
     * @tc.desc     : Negative test: devices serial add special characters '@#'
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamEx008', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamEx008 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            let devSerial = devices.serial + "@#";
            devices.serial = devSerial;
            let gPipe = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [', devices.serial, '] connectDevice ret : ', JSON.stringify(gPipe));
            expect(CheckEmptyUtils.isEmpty(gPipe)).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamEx008 catch err code: ', err.code, ', message: ', err.message);
            expect(err !== null).assertFalse();
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_1700
     * @tc.name     : testConnectDeviceParamEx009
     * @tc.desc     : Negative test: devices manufacturerName add special characters '@#'
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamEx009', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamEx009 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            let devManufacturerName = devices.manufacturerName + "@#";
            devices.manufacturerName = devManufacturerName;
            let gPipe = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [', devices.manufacturerName, '] connectDevice ret : ', JSON.stringify(gPipe));
            expect(CheckEmptyUtils.isEmpty(gPipe)).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamEx009 catch err code: ', err.code, ', message: ', err.message);
            expect(err !== null).assertFalse();
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_1800
     * @tc.name     : testConnectDeviceParamEx010
     * @tc.desc     : Negative test: devices manufacturerName add special characters 'abc'
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamEx010', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamEx010 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            let devManufacturerName = devices.manufacturerName + "abc";
            devices.manufacturerName = devManufacturerName;
            let gPipe = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [', devices.manufacturerName, '] connectDevice ret : ', JSON.stringify(gPipe));
            expect(CheckEmptyUtils.isEmpty(gPipe)).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamEx010 catch err code: ', err.code, ', message: ', err.message);
            expect(err !== null).assertFalse();
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_1900
     * @tc.name     : testConnectDeviceParamEx011
     * @tc.desc     : Negative test: devices manufacturerName add special characters '123'
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamEx011', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamEx011 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            let devManufacturerName = devices.manufacturerName + "123";
            devices.manufacturerName = devManufacturerName;
            let gPipe = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [', devices.manufacturerName, '] connectDevice ret : ', JSON.stringify(gPipe));
            expect(CheckEmptyUtils.isEmpty(gPipe)).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamEx011 catch err code: ', err.code, ', message: ', err.message);
            expect(err !== null).assertFalse();
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_2000
     * @tc.name     : testConnectDeviceParamEx012
     * @tc.desc     : Negative test: devices manufacturerName add special characters ""
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamEx012', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamEx012 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            devices.manufacturerName = PARAM_NULLSTRING;
            let gPipe = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [', devices.manufacturerName, '] connectDevice ret : ', JSON.stringify(gPipe));
            expect(CheckEmptyUtils.isEmpty(gPipe)).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamEx012 catch err code: ', err.code, ', message: ', err.message);
            expect(err !== null).assertFalse();
        }
    })
    
    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_2100
     * @tc.name     : testConnectDeviceParamEx013
     * @tc.desc     : Negative test: devices productName add special characters '@#'
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamEx013', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamEx013 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            let devProductName = devices.productName + "@#";
            devices.productName = devProductName;
            let gPipe = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [', devices.productName, '] connectDevice ret : ', JSON.stringify(gPipe));
            expect(CheckEmptyUtils.isEmpty(gPipe)).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamEx013 catch err code: ', err.code, ', message: ', err.message);
            expect(err !== null).assertFalse();
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_2200
     * @tc.name     : testConnectDeviceParamEx014
     * @tc.desc     : Negative test: devices productName add special characters 'abc'
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamEx014', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamEx014 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            let devProductName = devices.productName + "abc";
            devices.productName = devProductName;
            let gPipe = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [', devices.productName, '] connectDevice ret : ', JSON.stringify(gPipe));
            expect(CheckEmptyUtils.isEmpty(gPipe)).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamEx014 catch err code: ', err.code, ', message: ', err.message);
            expect(err !== null).assertFalse();
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_2300
     * @tc.name     : testConnectDeviceParamEx015
     * @tc.desc     : Negative test: devices productName add special characters '123'
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamEx015', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamEx015 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            let devProductName = devices.productName + "123";
            devices.productName = devProductName;
            let gPipe = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [', devices.productName, '] connectDevice ret : ', JSON.stringify(gPipe));
            expect(CheckEmptyUtils.isEmpty(gPipe)).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamEx015 catch err code: ', err.code, ', message: ', err.message);
            expect(err !== null).assertFalse();
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_2400
     * @tc.name     : testConnectDeviceParamEx016
     * @tc.desc     : Negative test: devices productName is null string ""
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamEx016', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamEx016 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            devices.productName = PARAM_NULLSTRING;
            let gPipe = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [', devices.productName, '] connectDevice ret : ', JSON.stringify(gPipe));
            expect(CheckEmptyUtils.isEmpty(gPipe)).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamEx016 catch err code: ', err.code, ', message: ', err.message);
            expect(err !== null).assertFalse();
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_2500
     * @tc.name     : testConnectDeviceParamEx017
     * @tc.desc     : Negative test: devices version add special characters '@#'
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamEx017', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamEx017 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            let devVersion = devices.version + "@#";
            devices.version = devVersion;
            let gPipe = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [', devices.version, '] connectDevice ret : ', JSON.stringify(gPipe));
            expect(CheckEmptyUtils.isEmpty(gPipe)).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamEx017 catch err code: ', err.code, ', message: ', err.message);
            expect(err !== null).assertFalse();
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_2600
     * @tc.name     : testConnectDeviceParamEx018
     * @tc.desc     : Negative test: devices version add special characters 'abc'
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamEx018', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamEx018 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            let devVersion = devices.version + "abc";
            devices.version = devVersion;
            let gPipe = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [', devices.version, '] connectDevice ret : ', JSON.stringify(gPipe));
            expect(CheckEmptyUtils.isEmpty(gPipe)).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamEx018 catch err code: ', err.code, ', message: ', err.message);
            expect(err !== null).assertFalse();
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_2700
     * @tc.name     : testConnectDeviceParamEx019
     * @tc.desc     : Negative test: devices version add special characters '123'
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamEx019', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamEx019 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            let devVersion = devices.version + "123";
            devices.version = devVersion;
            let gPipe = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [', devices.version, '] connectDevice ret : ', JSON.stringify(gPipe));
            expect(CheckEmptyUtils.isEmpty(gPipe)).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamEx019 catch err code: ', err.code, ', message: ', err.message);
            expect(err !== null).assertFalse();
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_2800
     * @tc.name     : testConnectDeviceParamEx020
     * @tc.desc     : Negative test: devices version is null string ""
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamEx020', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamEx020 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            devices.version = PARAM_NULLSTRING;
            let gPipe = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [', devices.version, '] connectDevice ret : ', JSON.stringify(gPipe));
            expect(CheckEmptyUtils.isEmpty(gPipe)).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamEx020 catch err code: ', err.code, ', message: ', err.message);
            expect(err !== null).assertFalse();
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_2900
     * @tc.name     : testConnectDeviceParamEx021
     * @tc.desc     : Negative test: devices vendorId is add number 1000
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamEx021', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamEx021 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            let devVendorId = devices.vendorId + 1000;
            devices.vendorId = devVendorId;
            let gPipe = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [', devices.vendorId, '] connectDevice ret : ', JSON.stringify(gPipe));
            expect(CheckEmptyUtils.isEmpty(gPipe)).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamEx021 catch err code: ', err.code, ', message: ', err.message);
            expect(err !== null).assertFalse();
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_3000
     * @tc.name     : testConnectDeviceParamEx022
     * @tc.desc     : Negative test: devices productId is add number 1000
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamEx022', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamEx022 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            let devProductId = devices.productId + 1000;
            devices.productId = devProductId;
            let gPipe = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [', devices.productId, '] connectDevice ret : ', JSON.stringify(gPipe));
            expect(CheckEmptyUtils.isEmpty(gPipe)).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamEx022 catch err code: ', err.code, ', message: ', err.message);
            expect(err !== null).assertFalse();
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_3100
     * @tc.name     : testConnectDeviceParamEx023
     * @tc.desc     : Negative test: devices clazz is add number 1000
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamEx023', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamEx023 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            let devClazz = devices.clazz + 1000;
            devices.clazz = devClazz;
            let gPipe = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [', devices.clazz, '] connectDevice ret : ', JSON.stringify(gPipe));
            expect(CheckEmptyUtils.isEmpty(gPipe)).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamEx023 catch err code: ', err.code, ', message: ', err.message);
            expect(err !== null).assertFalse();
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_3200
     * @tc.name     : testConnectDeviceParamEx024
     * @tc.desc     : Negative test: devices subClass is add number 1000
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamEx024', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamEx024 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            let devSubClass = devices.subClass + 1000;
            devices.subClass = devSubClass;
            let gPipe = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [', devices.subClass, '] connectDevice ret : ', JSON.stringify(gPipe));
            expect(CheckEmptyUtils.isEmpty(gPipe)).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamEx024 catch err code: ', err.code, ', message: ', err.message);
            expect(err !== null).assertFalse();
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_3300
     * @tc.name     : testConnectDeviceParamEx025
     * @tc.desc     : Negative test: devices protocol is add number 1000
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamEx025', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamEx025 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            let devProtocol = devices.protocol + 1000;
            devices.protocol = devProtocol;
            let gPipe = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [', devices.protocol, '] connectDevice ret : ', JSON.stringify(gPipe));
            expect(CheckEmptyUtils.isEmpty(gPipe)).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamEx025 catch err code: ', err.code, ', message: ', err.message);
            expect(err !== null).assertFalse();
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_3400
     * @tc.name     : testConnectDeviceParamEx026
     * @tc.desc     : Negative test: devices busNum is add number 1000
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamEx026', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamEx026 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            let devBusNum = devices.busNum + 1000;
            devices.busNum = devBusNum;
            let gPipe = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [', devices.busNum, '] connectDevice ret : ', JSON.stringify(gPipe));
            expect(gPipe !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamEx026 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(14400001);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_3500
     * @tc.name     : testConnectDeviceParamEx027
     * @tc.desc     : Negative test: devices devAddress is add number 10000
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamEx027', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamEx027 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            let devDevAddress = devices.devAddress + 1000;
            devices.devAddress = devDevAddress;
            let gPipe = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [', devices.devAddress, '] connectDevice ret : ', JSON.stringify(gPipe));
            expect(gPipe !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamEx027 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(14400001);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_3600
     * @tc.name     : testClosePipeParamEx001
     * @tc.desc     : Negative test: pipe busNum add number 1000
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testClosePipeParamEx001', 0, function () {
        console.info(TAG, 'usb testClosePipeParamEx001 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        getPipe("testClosePipeParamEx001");
        try {
            gPipe.busNum = gPipe.busNum + 1000;
            let ret = usbManager.closePipe(gPipe);
            console.info(TAG, 'usb [', gPipe.busNum, '] closePipe ret : ', ret);
            expect(ret !== 0).assertTrue();
        } catch (err) {
            console.info(TAG, 'testClosePipeParamEx001 catch err code: ', err);
            expect(err !== null).assertFalse();
        }
        toClosePipe("testClosePipeParamEx001");
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_3700
     * @tc.name     : testClosePipeParamEx002
     * @tc.desc     : Negative test: pipe devAddress add number 10000
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testClosePipeParamEx002', 0, function () {
        console.info(TAG, 'usb testClosePipeParamEx002 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        getPipe("testClosePipeParamEx002");
        try {
            let pipDevAdd = gPipe.devAddress + 10000;
            gPipe.devAddress = pipDevAdd;
            let ret = usbManager.closePipe(gPipe);
            console.info(TAG, 'usb [', gPipe.devAddress, '] closePipe ret : ', ret);
            expect(ret !== 0).assertTrue();
        } catch (err) {
            console.info(TAG, 'testClosePipeParamEx002 catch err code: ', err);
            expect(err !== null).assertFalse();
        }
        toClosePipe("testClosePipeParamEx002");
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_3800
     * @tc.name     : testGetRawDescriptorParamEx001
     * @tc.desc     : Negative test: pipe busNum add number 1000
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testGetRawDescriptorParamEx001', 0, function () {
        console.info(TAG, 'usb testGetRawDescriptorParamEx001 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        getPipe("testGetRawDescriptorParamEx001");
        try {
            let pipBusNum = gPipe.busNum + 1000;
            gPipe.busNum = pipBusNum;
            let ret = usbManager.getRawDescriptor(gPipe);
            console.info(TAG, 'usb [', gPipe.busNum, '] getRawDescriptor ret : ', ret);
            expect(ret).assertEqual(undefined);
        } catch (err) {
            console.info(TAG, 'testGetRawDescriptorParamEx001 catch err code: ', err);
            expect(err !== null).assertFalse();
        }
        toClosePipe("testGetRawDescriptorParamEx001");
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_3900
     * @tc.name     : testGetRawDescriptorParamEx002
     * @tc.desc     : Negative test: pipe devAddress add number 10000
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testGetRawDescriptorParamEx002', 0, function () {
        console.info(TAG, 'usb testGetRawDescriptorParamEx002 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        getPipe("testGetRawDescriptorParamEx002");
        try {
            let pipDevAdd = gPipe.devAddress + 10000;
            gPipe.devAddress = pipDevAdd;
            let ret = usbManager.getRawDescriptor(gPipe);
            console.info(TAG, 'usb [', gPipe.devAddress, '] getRawDescriptor ret : ', ret);
            expect(ret).assertEqual(undefined);
        } catch (err) {
            console.info(TAG, 'testGetRawDescriptorParamEx002 catch err code: ', err);
            expect(err !== null).assertFalse();
        }
        toClosePipe("testGetRawDescriptorParamEx002");
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_4000
     * @tc.name     : testGetRawDescriptorParamEx003
     * @tc.desc     : Negative test: pipe busNum -23
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testGetRawDescriptorParamEx003', 0, function () {
        console.info(TAG, 'usb testGetRawDescriptorParamEx003 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        getPipe("testGetRawDescriptorParamEx003");
        try {
            gPipe.busNum = -23;
            let ret = usbManager.getRawDescriptor(gPipe);
            console.info(TAG, 'usb [', gPipe.busNum, '] getRawDescriptor ret : ', ret);
            expect(ret).assertEqual(undefined);
        } catch (err) {
            console.info(TAG, 'testGetRawDescriptorParamEx003 catch err code: ', err);
            expect(err !== null).assertFalse();
        }
        toClosePipe("testGetRawDescriptorParamEx003");
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_4100
     * @tc.name     : testGetRawDescriptorParamEx004
     * @tc.desc     : Negative test: pipe devAddress -23
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testGetRawDescriptorParamEx004', 0, function () {
        console.info(TAG, 'usb testGetRawDescriptorParamEx004 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        getPipe("testGetRawDescriptorParamEx004");
        try {
            gPipe.devAddress = -23;
            let ret = usbManager.getRawDescriptor(gPipe);
            console.info(TAG, 'usb [', gPipe.devAddress, '] getRawDescriptor ret : ', ret);
            expect(ret).assertEqual(undefined);
        } catch (err) {
            console.info(TAG, 'testGetRawDescriptorParamEx004 catch err code: ', err);
            expect(err !== null).assertFalse();
        }
        toClosePipe("testGetRawDescriptorParamEx004");
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_4200
     * @tc.name     : testGetRawDescriptorParamEx005
     * @tc.desc     : Negative test: pipe busNum -23, devAddress -23
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testGetRawDescriptorParamEx005', 0, function () {
        console.info(TAG, 'usb testGetRawDescriptorParamEx005 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        getPipe("testGetRawDescriptorParamEx005");
        try {
            gPipe.busNum = -23;
            gPipe.devAddress = -23;
            let ret = usbManager.getRawDescriptor(gPipe);
            console.info(TAG, 'usb [', gPipe.devAddress, '] getRawDescriptor ret : ', ret);
            expect(ret).assertEqual(undefined);
        } catch (err) {
            console.info(TAG, 'testGetRawDescriptorParamEx005 catch err code: ', err);
            expect(err !== null).assertFalse();
        }
        toClosePipe("testGetRawDescriptorParamEx005");
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_4300
     * @tc.name     : testGetFileDescriptorParamEx001
     * @tc.desc     : Negative test: pipe busNum add number 1000
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testGetFileDescriptorParamEx001', 0, function () {
        console.info(TAG, 'usb testGetFileDescriptorParamEx001 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        getPipe("testGetFileDescriptorParamEx001");
        try {
            let pipBusNum = gPipe.busNum + 1000;
            gPipe.busNum = pipBusNum;
            let ret = usbManager.getFileDescriptor(gPipe);
            console.info(TAG, 'usb [', gPipe.busNum, '] getFileDescriptor ret : ', ret);
            expect(ret).assertEqual(-1);
        } catch (err) {
            console.info(TAG, 'testGetFileDescriptorParamEx001 catch err code: ', err);
            expect(err !== null).assertFalse();
        }
        toClosePipe("testGetFileDescriptorParamEx001");
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_4400
     * @tc.name     : testGetFileDescriptorParamEx002
     * @tc.desc     : Negative test: pipe devAddress add number 10000
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testGetFileDescriptorParamEx002', 0, function () {
        console.info(TAG, 'usb testGetFileDescriptorParamEx002 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        getPipe("testGetFileDescriptorParamEx002");
        try {
            let pipDevAdd = gPipe.devAddress + 10000;
            gPipe.devAddress = pipDevAdd;
            let ret = usbManager.getFileDescriptor(gPipe);
            console.info(TAG, 'usb [', gPipe.devAddress, '] getFileDescriptor ret : ', ret);
            expect(ret).assertEqual(-1);
        } catch (err) {
            console.info(TAG, 'testGetFileDescriptorParamEx002 catch err code: ', err);
            expect(err !== null).assertFalse();
        }
        toClosePipe("testGetFileDescriptorParamEx002");
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_4500
     * @tc.name     : testGetFileDescriptorParamEx003
     * @tc.desc     : Negative test: pipe busNum -23
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testGetFileDescriptorParamEx003', 0, function () {
        console.info(TAG, 'usb testGetFileDescriptorParamEx003 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        getPipe("testGetFileDescriptorParamEx003");
        try {
            gPipe.busNum = -23;
            let ret = usbManager.getFileDescriptor(gPipe);
            console.info(TAG, 'usb [', gPipe.busNum, '] getFileDescriptor ret : ', ret);
            expect(ret).assertEqual(-1);
        } catch (err) {
            console.info(TAG, 'testGetFileDescriptorParamEx003 catch err code: ', err);
            expect(err !== null).assertFalse();
        }
        toClosePipe("testGetFileDescriptorParamEx003");
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_4600
     * @tc.name     : testGetFileDescriptorParamEx004
     * @tc.desc     : Negative test: pipe devAddress -23
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testGetFileDescriptorParamEx004', 0, function () {
        console.info(TAG, 'usb testGetFileDescriptorParamEx004 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        getPipe("testGetFileDescriptorParamEx004");
        try {
            gPipe.devAddress = -23;
            let ret = usbManager.getFileDescriptor(gPipe);
            console.info(TAG, 'usb [', gPipe.devAddress, '] getFileDescriptor ret : ', ret);
            expect(ret).assertEqual(-1);
        } catch (err) {
            console.info(TAG, 'testGetFileDescriptorParamEx004 catch err code: ', err);
            expect(err !== null).assertFalse();
        }
        toClosePipe("testGetFileDescriptorParamEx004");
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_4700
     * @tc.name     : testGetFileDescriptorParamEx005
     * @tc.desc     : Negative test: pipe busNum -23, devAddress -23
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testGetFileDescriptorParamEx005', 0, function () {
        console.info(TAG, 'usb testGetFileDescriptorParamEx005 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        getPipe("testGetFileDescriptorParamEx005");
        try {
            gPipe.busNum = -23;
            gPipe.devAddress = -23;
            let ret = usbManager.getFileDescriptor(gPipe);
            console.info(TAG, 'usb [', gPipe.devAddress, '] getFileDescriptor ret : ', ret);
            expect(ret).assertEqual(-1);
        } catch (err) {
            console.info(TAG, 'testGetFileDescriptorParamEx005 catch err code: ', err);
            expect(err !== null).assertFalse();
        }
        toClosePipe("testGetFileDescriptorParamEx005");
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_4800
     * @tc.name     : testClaimInterfaceParamEx001
     * @tc.desc     : Negative test: interfaces id add 123
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testClaimInterfaceParamEx001', 0, function () {
        console.info(TAG, 'usb testClaimInterfaceParamEx001 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        getPipe("testClaimInterfaceParamEx001");
        try {
            let tmpInterface = devices.configs[0].interfaces[0];
            tmpInterface.id = tmpInterface.id + 123;
            let ret = usbManager.claimInterface(gPipe, tmpInterface);
            console.info(TAG, 'usb [', tmpInterface.id, '] claimInterface ret : ', ret);
            expect(ret !== 0).assertTrue();
        } catch (err) {
            console.info(TAG, 'testClaimInterfaceParamEx001 catch err code: ', err);
            expect(err !== null).assertFalse();
        }
        toClosePipe("testClaimInterfaceParamEx001");
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_4900
     * @tc.name     : testClaimInterfaceParamEx002
     * @tc.desc     : Negative test: interfaces id -1
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testClaimInterfaceParamEx002', 0, function () {
        console.info(TAG, 'usb testClaimInterfaceParamEx002 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        getPipe("testClaimInterfaceParamEx002");
        try {
            let tmpInterface = devices.configs[0].interfaces[0];
            tmpInterface.id = -1;
            let ret = usbManager.claimInterface(gPipe, tmpInterface);
            console.info(TAG, 'usb [', tmpInterface.id, '] claimInterface ret : ', ret);
            expect(ret !== 0).assertTrue();
        } catch (err) {
            console.info(TAG, 'testClaimInterfaceParamEx002 catch err code: ', err);
            expect(err !== null).assertFalse();
        }
        toClosePipe("testClaimInterfaceParamEx002");
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_5000
     * @tc.name     : testClaimInterfaceParamEx003
     * @tc.desc     : Negative test: pipe busNum add 1000
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testClaimInterfaceParamEx003', 0, function () {
        console.info(TAG, 'usb testClaimInterfaceParamEx003 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        getPipe("testClaimInterfaceParamEx003");
        try {
            gPipe.busNum = gPipe.busNum + 1000;
            let tmpInterface = devices.configs[0].interfaces[0];
            let ret = usbManager.claimInterface(gPipe, tmpInterface);
            console.info(TAG, 'usb [', gPipe.busNum, '] claimInterface ret : ', ret);
            expect(ret !== 0).assertTrue();
        } catch (err) {
            console.info(TAG, 'testClaimInterfaceParamEx003 catch err code: ', err);
            expect(err !== null).assertFalse();
        }
        toClosePipe("testClaimInterfaceParamEx003");
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_5100
     * @tc.name     : testClaimInterfaceParamEx004
     * @tc.desc     : Negative test: pipe busNum -1
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testClaimInterfaceParamEx004', 0, function () {
        console.info(TAG, 'usb testClaimInterfaceParamEx004 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        getPipe("testClaimInterfaceParamEx004");
        try {
            gPipe.busNum = -1;
            let tmpInterface = devices.configs[0].interfaces[0];
            let ret = usbManager.claimInterface(gPipe, tmpInterface);
            console.info(TAG, 'usb [', gPipe.busNum, '] claimInterface ret : ', ret);
            expect(ret !== 0).assertTrue();
        } catch (err) {
            console.info(TAG, 'testClaimInterfaceParamEx004 catch err code: ', err);
            expect(err !== null).assertFalse();
        }
        toClosePipe("testClaimInterfaceParamEx004");
    })
})
}
