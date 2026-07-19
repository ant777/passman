const { app, BrowserWindow } = require('electron')

const createWindow = () => {
  const win = new BrowserWindow({
    width: 800,
    height: 600,
        // frame: false,

    // titleBarStyle: 'hidden',
    webPreferences: {
      webviewTag:true,
      nodeIntegration: false,
      contextIsolation: true,
      experimentalFeatures: true // Включает экспериментальные веб-функции
    }
  })
win.removeMenu();
  win.loadFile('docs/index.html');
  return win;
}

app.whenReady().then(() => {
  const mainWindow = createWindow()

  mainWindow.webContents.on('select-bluetooth-device', (event, deviceList, callback) => {
    event.preventDefault(); // Останавливаем стандартное поведение
    

    console.warn(deviceList);
    // В deviceList находятся все доступные Bluetooth-устройства
    if (deviceList && deviceList.length > 0) {
      // Пример: берем первое попавшееся устройство
      const deviceToSelect = deviceList[0];
      callback(deviceToSelect.deviceId);
    } else {
      callback(''); // Отмена, если ничего не найдено
    }
  });
});
