const SERVICE_UUID = '6fe56dde-8e9b-45c5-96ee-7c8aea360a94';
const CHARACTERISTIC_UUID = '46017b0a-60bd-4d8d-9fd9-22d15de3ae0f';
export class PassManagerBLE {
    constructor() {

    }

    bluetoothDevice = null;

    async init (){
        try {
            var deviceOptions = {
                optionalServices: [SERVICE_UUID, CHARACTERISTIC_UUID, 0xfe95, 0x1f10],
                acceptAllDevices: true,
            };
            const hideUnknown = false;//document.getElementById('hideUnknown').checked;
            const namePrefix = false;//document.getElementById('namePrefix').value;
            if (hideUnknown) {
                deviceOptions.acceptAllDevices = false;
                deviceOptions.filters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz"
                    .split("")
                    .map((x) => ({ namePrefix: x }));
            }
            if (namePrefix) {
                deviceOptions.acceptAllDevices = false;
                deviceOptions.filters = namePrefix.split(",")
                    .map((x) => ({ namePrefix: x }));
            }


            console.log(deviceOptions)

            if (this.bluetoothDevice != null) this.bluetoothDevice.gatt.disconnect();
            // resetVariables();
            // addLog("Searching for devices");
            // connectTrys = 0;
            const device = await navigator.bluetooth.requestDevice(deviceOptions)
            this.bluetoothDevice = device;
                // catchAdvertisement(device);
                // bluetoothDevice.addEventListener('gattserverdisconnected', onDisconnected);
                // addLog("Connecting to: " + bluetoothDevice.name);
            await this.connect();
        } catch(e) {
            this.handleError(e)
        }
    }


     handleCharacteristicChange(event){
        const newValueReceived = new TextDecoder().decode(event.target.value);
        console.warn("Characteristic value changed: ", newValueReceived);
        // retrievedValue.innerHTML = newValueReceived;
        // timestampContainer.innerHTML = getDateTime();
    }


    async connect() {

        try {
            const server = await this.bluetoothDevice.gatt.connect()

            // addClog("Found GATT server");
            this.gattServer = server;
            this.service = await this.gattServer.getPrimaryService(SERVICE_UUID);
            // addClog("Found service");
            this.characteristic = await this.service.getCharacteristic(CHARACTERISTIC_UUID);
            await this.characteristic.startNotifications();
            this.characteristic.addEventListener('characteristicvaluechanged', (event) => {
                this.handleCharacteristicChange(event);
                const decoder = new TextDecoder('utf-8');
                const stringValue = decoder.decode(event.target.value);
                const value = stringValue;
                // if(value.startsWith('remove_')) {
                //     return;
                // }
                // Process the received data (e.g., as a DataView or ArrayBuffer)
                console.log('Characteristic value changed:', value);
                const parsed = value.split('\n');
                window.currentPwdServiceName.value = parsed[0];
                window.currentPwdLogin.value = parsed[1];
                const ruleLimits = parsed[2];
                const rule = ruleLimits.split('||')[0];
                window.currentPwdPasswordRuleMinLength.value = "";
                window.currentPwdPasswordRuleMaxLength.value = "";
                if(ruleLimits.split('||')[1]) {
                    window.currentPwdPasswordRuleMinLength.value = ruleLimits.split('||')[1].split('-')[0];
                    window.currentPwdPasswordRuleMaxLength.value =  ruleLimits.split('||')[1].split('-')[1];

                }
                window.currentPwdPasswordRule.value = rule;
                if (stringValue.trim()) {

                    document.body.classList.remove('show-create');
                    document.body.classList.add('show-update');

                } else {

                    document.body.classList.add('show-create');
                    document.body.classList.remove('show-update');

                }
                console.log(`Value as String: ${stringValue}`);
                // Example: Read as an integer
                // const intValue = value.getInt16(0, true); // little-endian
                // console.log('Integer value:', intValue);
            });
            const data = new TextEncoder().encode('request');
            this.characteristic.writeValueWithoutResponse(data);

        }catch(e) {
            this.handleError(e)

        }
    }

    async submitUpdate() {
        const characteristic = await this.service.getCharacteristic(CHARACTERISTIC_UUID);
        
        console.log("Found the LED characteristic: ", characteristic.uuid);
        let rule = window.currentPwdServiceName.value || 'rnd1';
        if (window.currentPwdPasswordRuleMinLength.value || window.currentPwdPasswordRuleMaxLength.value) {
            if(!window.currentPwdPasswordRuleMinLength.value || !window.currentPwdPasswordRuleMaxLength.value) {
                alert('wrong pwd rule range');
                return;
            }
            rule += '||'+ window.currentPwdPasswordRuleMinLength.value +'-'+ window.currentPwdPasswordRuleMaxLength.value;
        }
        const data = new TextEncoder().encode('update\n' + window.currentPwdServiceName.value + '\n' + window.currentPwdLogin.value + '\n' + rule);
            characteristic.writeValueWithoutResponse(data);
    }

    async submit() {
        const characteristic = await this.service.getCharacteristic(CHARACTERISTIC_UUID);
        
        console.log("Found the LED characteristic: ", characteristic.uuid);
        let rule = window.pwdPasswordRule.value || 'rnd1';
        if (window.pwdPasswordRuleMinLength.value || window.pwdPasswordRuleMaxLength.value) {
            if(!window.pwdPasswordRuleMinLength.value || !window.pwdPasswordRuleMaxLength.value) {
                alert('wrong pwd rule range');
                return;
            }
            rule += '||'+ window.pwdPasswordRuleMinLength.value +'-'+ window.pwdPasswordRuleMaxLength.value;
        }
        const data = new TextEncoder().encode('create\n' + window.pwdServiceName.value + '\n' + window.pwdLogin.value + '\n' + rule + '\n' + window.pwdPassword.value);
            characteristic.writeValueWithoutResponse(data);

window.pwdServiceName.value = '';
window.pwdLogin.value = '';
window.pwdPassword.value = '';
window.pwdPasswordRule.value = '';
window.pwdPasswordRuleMinLength.value = '';
window.pwdPasswordRuleMaxLength.value = '';
    }

    async deleteItem() {
            
        let rule = window.currentPwdPasswordRule.value || 'rnd1';
        if (window.currentPwdPasswordRuleMinLength.value || window.currentPwdPasswordRuleMaxLength.value) {
            if(!window.currentPwdPasswordRuleMinLength.value || !window.currentPwdPasswordRuleMaxLength.value) {
                alert('wrong pwd rule range');
                return;
            }
            rule += '||'+ window.currentPwdPasswordRuleMinLength.value +'-'+ window.currentPwdPasswordRuleMaxLength.value;
        }
            console.warn('remove_'+ (window.currentPwdServiceName.value + '\n' + window.currentPwdLogin.value + '\n' + rule));
            const data = new TextEncoder().encode('remove_'+ (window.currentPwdServiceName.value + '\n' + window.currentPwdLogin.value + '\n' + rule));
            this.characteristic.writeValueWithoutResponse(data);
        
    }

    handleError(e) {
        console.error(e);

    }
}