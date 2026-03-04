import { PasswordEntry } from "./app/App";

const SERVICE_UUID = '6fe56dde-8e9b-45c5-96ee-7c8aea360a94';
const CHARACTERISTIC_UUID = '46017b0a-60bd-4d8d-9fd9-22d15de3ae0f';

interface BluetoothDevice {
    addEventListener: Function;

}
export class PassManagerBLE {
    constructor() {
        this.bluetoothDevice = null;
    }

    bluetoothDevice: BluetoothDevice | null;

    service = null;
    gattServer = {};

    async init (dcCb: Function, listCb: Function){
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

            if (this.bluetoothDevice !== null) this.bluetoothDevice.gatt.disconnect();
            // resetVariables();
            // addLog("Searching for devices");
            // connectTrys = 0;
            const device = await navigator.bluetooth.requestDevice(deviceOptions)
            this.bluetoothDevice = device;
                // catchAdvertisement(device);
            this.bluetoothDevice.addEventListener('gattserverdisconnected', dcCb);
                // addLog("Connecting to: " + bluetoothDevice.name);
            await this.connect(listCb);
            return device;
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

    parseListItem(rawItem: string): PasswordEntry {
        const splitted = rawItem.split('||');
        const lengthRange = (splitted[4] || '').split('-');
        return {
            id: splitted[0],
            service: splitted[1],
            username: splitted[2],
            ruleWhitelist: splitted[3],
            ruleLimitsMin: parseInt(lengthRange[0]),
            ruleLimitsMax: parseInt(lengthRange[1])
        };
    }

    async connect(listCb) {

        try {
            const server = await this.bluetoothDevice.gatt.connect()

            // addClog("Found GATT server");
            this.gattServer = server;
            this.service = await this.gattServer.getPrimaryService(SERVICE_UUID);
            // addClog("Found service");
            this.characteristic = await this.service.getCharacteristic(CHARACTERISTIC_UUID);
            await this.characteristic.startNotifications();
            const feedback = new Promise((resolve): void => {

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
                    const parsed = value.trim().split('\n');
                    if (parsed[0].trim() === 'list') {
                        listCb(parsed.slice(1).map(this.parseListItem));
                        resolve();
                    } else {

                        // window.currentPwdServiceName.value = parsed[0];
                        // window.currentPwdLogin.value = parsed[1];
                        // const ruleLimits = parsed[2];
                        // const rule = ruleLimits.split('||')[0];
                        // window.currentPwdPasswordRuleMinLength.value = "";
                        // window.currentPwdPasswordRuleMaxLength.value = "";
                        // if(ruleLimits.split('||')[1]) {
                        //     window.currentPwdPasswordRuleMinLength.value = ruleLimits.split('||')[1].split('-')[0];
                        //     window.currentPwdPasswordRuleMaxLength.value =  ruleLimits.split('||')[1].split('-')[1];

                        // }
                        // window.currentPwdPasswordRule.value = rule;
                        // if (stringValue.trim()) {

                        //     document.body.classList.remove('show-create');
                        //     document.body.classList.add('show-update');

                        // } else {

                        //     document.body.classList.add('show-create');
                        //     document.body.classList.remove('show-update');

                        // }
                        // console.log(`Value as String: ${stringValue}`);
                    }
                    // Example: Read as an integer
                    // const intValue = value.getInt16(0, true); // little-endian
                    // console.log('Integer value:', intValue);
                });
            })
            const data = new TextEncoder().encode('request');
            this.characteristic.writeValueWithoutResponse(data);
            await feedback;

        }catch(e) {
            this.handleError(e)

        }
    }

    async submitUpdate(pwdData: PasswordEntry) {
        setTimeout(async () => {

            console.warn(pwdData);
            const characteristic = await this.service.getCharacteristic(CHARACTERISTIC_UUID);
            
            console.log("Found the LED characteristic: ", characteristic.uuid);

            let rule = pwdData.ruleWhitelist || 'rnd1';
            if (pwdData.ruleLimitsMin || pwdData.ruleLimitsMax) {
                if(!pwdData.ruleLimitsMin || !pwdData.ruleLimitsMax) {
                    alert('wrong pwd rule range');
                    return;
                }
                rule += '||'+ pwdData.ruleLimitsMin +'-'+ pwdData.ruleLimitsMax;
            }
            const data = new TextEncoder().encode('update\n' + pwdData.id + '\n' + pwdData.service + '\n' + pwdData.username + '\n' + rule);
                characteristic.writeValueWithoutResponse(data);
        }, 2000);
    }

    async submit(pwdData: PasswordEntry) {
        const characteristic = await this.service.getCharacteristic(CHARACTERISTIC_UUID);
        
        console.log("Found the LED characteristic: ", characteristic.uuid, pwdData  );
        let rule = pwdData.ruleWhitelist || 'rnd1';
        if (pwdData.ruleLimitsMin || pwdData.ruleLimitsMax) {
            if(!pwdData.ruleLimitsMin || !pwdData.ruleLimitsMax) {
                alert('wrong pwd rule range');
                return;
            }
            rule += '||'+ pwdData.ruleLimitsMin +'-'+ pwdData.ruleLimitsMax;
        }
        const data = new TextEncoder().encode('create\n' + '\n' + pwdData.service + '\n' + pwdData.username + '\n' + rule + '\n' + pwdData.password);
            characteristic.writeValueWithoutResponse(data);

    }

    async deleteItem(id: string) {
            
            const data = new TextEncoder().encode('remove\n' + id);
            this.characteristic.writeValueWithoutResponse(data);
        
    }

    handleError(e) {
        console.error(e);

    }
}