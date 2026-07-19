import { NoteEntry, PasswordEntry } from "./app/App";

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

    async init (dcCb: Function, listCb: Function, metaCb: Function){
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
            await this.connect(listCb, metaCb);
              console.warn(device);
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

    parseListItem(rawItem: string): PasswordEntry|NoteEntry {
        const splitted = rawItem.split('||');
        const lengthRange = (splitted[4] || '').split('-');
        if (splitted[0].includes('.note'))
            return {
                id: splitted[0],
                title: splitted[1],
                contents: splitted[2]
            };
        if (splitted[0].includes('.card'))
            return {
                id: splitted[0],
                title: splitted[1],
                name: splitted[2],
                number: splitted[3],
                exp: splitted[4],
                cvc: splitted[5],
                pin: splitted[6]
            };
        return {
            id: splitted[0],
            service: splitted[1],
            username: splitted[2],
            ruleWhitelist: splitted[3],
            ruleLimitsMin: parseInt(lengthRange[0]),
            ruleLimitsMax: parseInt(lengthRange[1])
        };
    }

    async connect(listCb, metaCb) {

        try {
            const server = await this.bluetoothDevice.gatt.connect()

            // addClog("Found GATT server");
            this.gattServer = server;
            this.service = await this.gattServer.getPrimaryService(SERVICE_UUID);
            // addClog("Found service");
            this.characteristic = await this.service.getCharacteristic(CHARACTERISTIC_UUID);
            await this.characteristic.startNotifications();
            const feedback = new Promise((resolve): void => {
                let temp = [];

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
                        const isFinal =  parsed.slice(1).filter(it => it === 'endoflist').length !== 0;
                        const newItems = parsed.slice(1).filter(it => it !== 'endoflist')
                        .filter(it => !!it.trim()).map(this.parseListItem);
                        temp = [...temp, ...newItems];
                        if (isFinal) {
                            listCb(temp);
                            temp=[];
                        }
                    } else if(parsed[0].trim() === 'meta') {
                        metaCb(value.trim());
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
                    resolve();  
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

            
            const characteristic = await this.service.getCharacteristic(CHARACTERISTIC_UUID);
            
            console.log("Found the LED characteristic: ", characteristic.uuid);

            console.warn(pwdData);
            let data;
            if(pwdData.type == 'note') {
                data =  new TextEncoder().encode('update\n' +pwdData.type + '\n' + pwdData.id + '\n' + pwdData.title + '\n' + pwdData.contents);
            }else if(pwdData.type == 'card') {
                data =  new TextEncoder().encode('update\n' +pwdData.type + '\n' + pwdData.id + '\n' + pwdData.title + '\n' + pwdData.name + '\n' + pwdData.number + '\n' + pwdData.exp + '\n' + pwdData.cvc + '\n' + pwdData.pin);
            }else {

                let rule = pwdData.ruleWhitelist || 'rnd1';
                if (pwdData.ruleLimitsMin || pwdData.ruleLimitsMax) {
                    if(!pwdData.ruleLimitsMin || !pwdData.ruleLimitsMax) {
                        alert('wrong pwd rule range');
                        return;
                    }
                    rule += '||'+ pwdData.ruleLimitsMin +'-'+ pwdData.ruleLimitsMax;
                }
                data = new TextEncoder().encode('update\n' +pwdData.type + '\n' + pwdData.id + '\n' + pwdData.service + '\n' + pwdData.username + '\n' + rule);
            }
                characteristic.writeValueWithoutResponse(data);
        }, 2000);
    }


    async requestMeta() {
        const data = new TextEncoder().encode('requestmeta');
        this.characteristic.writeValueWithoutResponse(data);

    }

    async requestList(type) {
        const data = new TextEncoder().encode('requestlist_'+type);
        console.warn(type)
        this.characteristic.writeValueWithoutResponse(data);
    }
    async submitMeta(dataStr) {
        setTimeout(() => {

            const data = new TextEncoder().encode('updatemeta\n'+dataStr);
            this.characteristic.writeValueWithoutResponse(data);

        }, 1000)
    }
    async submitSorting(dataStr) {
        console.warn(dataStr);
        setTimeout(() => {

            const data = new TextEncoder().encode('updatesorting\n'+dataStr);
            this.characteristic.writeValueWithoutResponse(data);

        }, 1000)
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
        let data;
        if(pwdData.type == 'note') {
            data =  new TextEncoder().encode('create\n' +pwdData.type + '\n' + 'new\n' + pwdData.title + '\n' + pwdData.contents);
        }else if(pwdData.type == 'card') {
            data =  new TextEncoder().encode('create\n' +pwdData.type + '\n' + 'new\n' + pwdData.title + '\n' + pwdData.name + '\n' + pwdData.number + '\n' + pwdData.exp + '\n' + pwdData.cvc + '\n' + pwdData.pin);
        }else {

            data = new TextEncoder().encode('create\n' +pwdData.type + '\n' + 'new\n' + pwdData.service + '\n' + pwdData.username + '\n' + rule + '\n' + pwdData.password);
        
        }
        characteristic.writeValueWithoutResponse(data);

    }

    async deleteItem(id: string) {
            const data = new TextEncoder().encode('remove\n' + id);
            console.warn('remove\n' + id)
            this.characteristic.writeValueWithoutResponse(data);
        
    }

    handleError(e) {
        console.error(e);

    }
}