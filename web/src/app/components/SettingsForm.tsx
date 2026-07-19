import { useState } from "react";
import { RefreshCw, Plus, Save } from "lucide-react";
import { PasswordEntry } from "../App";
import { Select, 
  SelectContent,
  SelectGroup,
  SelectItem,
  SelectLabel,
  SelectTrigger,
  SelectValue, } from "../components/ui/select";


interface SettingsFormProps {
  onSave: () => void;
  meta?: string | null;
  onCancelEdit?: () => void;
}

const rgbToHex = (r, g, b) => 
  "#" + [r, g, b].map(x => parseInt(x, 10).toString(16).padStart(2, '0')).join('');

function hexToRgb(hex) {
  // Expand shorthand form (e.g. "03F") to full form (e.g. "0033FF")
  const shorthandRegex = /^#?([a-f\d])([a-f\d])([a-f\d])$/i;
  hex = hex.replace(shorthandRegex, (m, r, g, b) => r + r + g + g + b + b);

  const result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
  return result ? {
    r: parseInt(result[1], 16),
    g: parseInt(result[2], 16),
    b: parseInt(result[3], 16)
  } : null;
}
export function SettingsForm({ onSave, meta, onCancelEdit }: SettingsFormProps) {
  console.warn(meta);
  let parsed =  (meta?.split('\n') || []).reduce((acc, it) => {
    const key = it?.split(':')[0];
    let value = it?.split(':')[1];
    if (key === 'PASS_LIST_ENABLED') {
      value = true;
    }
    if (key === 'CONNECTION_CONFIRMATION_DISABLED') {
      value = true;
    }
    if (key === 'AUTOSTART_BLUETOOTH') {
      value = true;
    }
    if (key === 'AUTOPRESS_ENTER') {
      value = true;
    }
    if (key === 'THEME_CUSTOM') {
      const splitted = value.split(',');
      value = {
        bgR: splitted[0],
        bgG: splitted[1],
        bgB: splitted[2],
        fontR: splitted[3],
        fontG: splitted[4],
        fontB: splitted[5],
        hilitR: splitted[6],
        hilitG: splitted[7],
        hilitB: splitted[8],
        titleR: splitted[9],
        titleG: splitted[10],
        titleB: splitted[11],
      };
    }
    acc[key] = value;
    return acc;
  }, {});
  const [requestDevicePin, setRequestDevicePin] = useState(parsed.REQUEST_DEVICE_PIN !== undefined ? parsed.REQUEST_DEVICE_PIN : false);
  const [autstartBluetooth, setAutstartBluetooth] = useState(parsed.AUTOSTART_BLUETOOTH !== undefined ? parsed.AUTOSTART_BLUETOOTH : false);
  const [autopressEnter, setAutopressEnter] = useState(parsed.AUTOPRESS_ENTER !== undefined ? parsed.AUTOPRESS_ENTER : false);
  const [connectionConfirmation, setConnectionConfirmation] = useState(parsed.CONNECTION_CONFIRMATION_DISABLED !== undefined ? !parsed.CONNECTION_CONFIRMATION_DISABLED : true);
  const [passListOutput, setPassListOutput] = useState(!!parsed.PASS_LIST_ENABLED);
  const [theme, setTheme] = useState(parsed.THEME_CUSTOM ? 'custom' : 'default');

  const [bgR, setBgR] = useState(parsed.THEME_CUSTOM?.bgR || 128);
  const [bgG, setBgG] = useState(parsed.THEME_CUSTOM?.bgG || 128);
  const [bgB, setBgB] = useState(parsed.THEME_CUSTOM?.bgB || 128);
  const [fontR, setFontR] = useState(parsed.THEME_CUSTOM?.fontR || 255);
  const [fontG, setFontG] = useState(parsed.THEME_CUSTOM?.fontG || 255);
  const [fontB, setFontB] = useState(parsed.THEME_CUSTOM?.fontB || 255);
  const [hilitR, setHilitR] = useState(parsed.THEME_CUSTOM?.hilitR || 255);
  const [hilitG, setHilitG] = useState(parsed.THEME_CUSTOM?.hilitG || 255);
  const [hilitB, setHilitB] = useState(parsed.THEME_CUSTOM?.hilitB || 0);
  const [titleR, setTitleR] = useState(parsed.THEME_CUSTOM?.titleR || 255);
  const [titleG, setTitleG] = useState(parsed.THEME_CUSTOM?.titleG || 0);
  const [titleB, setTitleB] = useState(parsed.THEME_CUSTOM?.titleB || 255);
  const handleSubmit = (data) => {
    onSave(data);
  };


  const inputCls =
    "w-full rounded-md px-3 py-2 transition-colors focus:outline-none text-[var(--pm-text)] placeholder-[var(--pm-text-placeholder)]";
  const inputStyle = {
    backgroundColor: "var(--pm-bg-input)",
    borderWidth: 1,
    borderStyle: "solid" as const,
    borderColor: "var(--pm-border)",
  };
  const focusStyle = "var(--pm-accent)";

  return (
    <div
      className="rounded-lg p-6  grid grid-cols-3 text-sm"
      style={{
        backgroundColor: "var(--pm-bg-card)",
        border: "1px solid var(--pm-border)",
      }}
    >

      <div
        className="flex flex-col items-start gap-2 cursor-pointer select-none  mb-6">
          Dummy typing TBD
          <label
              key="passList"
              className="flex items-center gap-2 cursor-pointer select-none"
            >
              <input
                disabled
                type="checkbox"
                checked={requestDevicePin}
                onChange={(e) => setRequestDevicePin(e.target.checked)}
                className="accent-[#FACC15] w-4 h-4"
              /> Enable
            </label></div>
      <div
        className="flex flex-col items-start gap-2 cursor-pointer select-none  mb-6">
          Dbl press consequentially inserts username and pwd TBD
          <label
              key="passList"
              className="flex items-center gap-2 cursor-pointer select-none"
            >
              <input
                disabled
                type="checkbox"
                checked={requestDevicePin}
                onChange={(e) => setRequestDevicePin(e.target.checked)}
                className="accent-[#FACC15] w-4 h-4"
              /> Enable
            </label></div>
      <div
        className="flex flex-col items-start gap-2 cursor-pointer select-none  mb-6">
          Request device pin TBD
          <label
              key="passList"
              className="flex items-center gap-2 cursor-pointer select-none"
            >
              <input
                disabled
                type="checkbox"
                checked={requestDevicePin}
                onChange={(e) => setRequestDevicePin(e.target.checked)}
                className="accent-[#FACC15] w-4 h-4"
              /> Enable
            </label></div>
      <div
        className="flex flex-col items-start gap-2 cursor-pointer select-none  mb-6">
          Bluetooth autodeactivation timeout TBD
          <label
              key="passList"
              className="flex items-center gap-2 cursor-pointer select-none"
            >
              <input
                disabled
                type="checkbox"
                checked={requestDevicePin}
                onChange={(e) => setRequestDevicePin(e.target.checked)}
                className="accent-[#FACC15] w-4 h-4"
              /> Enable
            </label></div>
      <div
        className="flex flex-col items-start gap-2 cursor-pointer select-none  mb-6">
          Autopress Enter after password inserted
          <label
              key="passList"
              className="flex items-center gap-2 cursor-pointer select-none"
            >
              <input
                type="checkbox"
                checked={autopressEnter}
                onChange={(e) => setAutopressEnter(e.target.checked)}
                className="accent-[#FACC15] w-4 h-4"
              /> Enable
            </label></div>

      <div
        className="flex flex-col items-start gap-2 cursor-pointer select-none  mb-6">
          Autostart Bluetooth
          <label
              key="passList"
              className="flex items-center gap-2 cursor-pointer select-none"
            >
              <input
                type="checkbox"
                checked={autstartBluetooth}
                onChange={(e) => setAutstartBluetooth(e.target.checked)}
                className="accent-[#FACC15] w-4 h-4"
              /> Enable
            </label></div>
      <div
        className="flex flex-col items-start gap-2 cursor-pointer select-none  mb-6">
          Bluetooth connection confirmation required
          <label
              key="passList"
              className="flex items-center gap-2 cursor-pointer select-none"
            >
              <input
                type="checkbox"
                checked={connectionConfirmation}
                onChange={(e) => setConnectionConfirmation(e.target.checked)}
                className="accent-[#FACC15] w-4 h-4"
              /> Enable
            </label></div>
      <div
        className="flex flex-col items-start gap-2 cursor-pointer select-none  mb-6">
          Password List Output
          <label
              key="passList"
              className="flex items-center gap-2 cursor-pointer select-none"
            >
              <input
                type="checkbox"
                checked={passListOutput}
                onChange={(e) => setPassListOutput(e.target.checked)}
                className="accent-[#FACC15] w-4 h-4"
              /> Enable
            </label></div>
      <div
        className="flex flex-col items-start gap-2 cursor-pointer select-none  mb-6">
          Theme config
          <Select onValueChange={setTheme} defaultValue={theme}>
            <SelectTrigger
      style={{ backgroundColor: "var(--pm-bg-page)", color: "var(--pm-text)" }} className="w-full max-w-48">
              <SelectValue placeholder="Select a theme" />
            </SelectTrigger>
            <SelectContent>
                <SelectItem value="default">Default</SelectItem>
                <SelectItem value="custom">Custom</SelectItem>
            </SelectContent>
          </Select>
          {theme === 'custom' && <><div>
            <h5>Background color</h5>
            <div className="flex gap-2 items-center"> 
              <input type="text" placeholder="red" value={bgR} onChange={(e) => setBgR(e.target.value)} className="border-1 p-1 w-12" />
              <input type="text" placeholder="green" value={bgG} onChange={(e) => setBgG(e.target.value)} className="border-1 p-1 w-12" />
              <input type="text" placeholder="blue" value={bgB} onChange={(e) => setBgB(e.target.value)} className="border-1 p-1 w-12" />
              <input type="color" id="head" name="head" onChange={(e) => {
                const rgb = hexToRgb(e.target.value);
                setBgR(rgb.r);
                setBgG(rgb.g);
                setBgB(rgb.b);
              }} value={rgbToHex(bgR,bgG,bgB)} style={{width: '25px'}} />
            </div>
          </div>
          <div>
            <h5>Font color</h5>
            <div className="flex gap-2 items-center"> 
              <input type="text" placeholder="red" value={fontR} onChange={(e) => setFontR(e.target.value)} className="border-1 p-1 w-12" />
              <input type="text" placeholder="green" value={fontG} onChange={(e) => setFontG(e.target.value)} className="border-1 p-1 w-12" />
              <input type="text" placeholder="blue" value={fontB} onChange={(e) => setFontB(e.target.value)} className="border-1 p-1 w-12" />
              <input type="color" id="head" name="head" onChange={(e) => {
                const rgb = hexToRgb(e.target.value);
                setFontR(rgb.r);
                setFontG(rgb.g);
                setFontB(rgb.b);
              }} value={rgbToHex(fontR,fontG,fontB)} style={{width: '25px'}} />
            </div>
          </div>
          <div>
            <h5>Highlight color</h5>
            <div className="flex gap-2 items-center"> 
              <input type="text" placeholder="red" value={hilitR} onChange={(e) => setHilitR(e.target.value)} className="border-1 p-1 w-12" />
              <input type="text" placeholder="green" value={hilitG} onChange={(e) => setHilitG(e.target.value)} className="border-1 p-1 w-12" />
              <input type="text" placeholder="blue" value={hilitB} onChange={(e) => setHilitB(e.target.value)} className="border-1 p-1 w-12" />
              <input type="color" id="head" name="head" onChange={(e) => {
                const rgb = hexToRgb(e.target.value);
                setHilitR(rgb.r);
                setHilitG(rgb.g);
                setHilitB(rgb.b);
              }} value={rgbToHex(hilitR,hilitG,hilitB)} style={{width: '25px'}} />
            </div>
          </div>
          <div>
            <h5>Title color</h5>
            <div className="flex gap-2 items-center"> 
              <input type="text" placeholder="red" value={titleR} onChange={(e) => setTitleR(e.target.value)} className="border-1 p-1 w-12" />
              <input type="text" placeholder="green" value={titleG} onChange={(e) => setTitleG(e.target.value)} className="border-1 p-1 w-12" />
              <input type="text" placeholder="blue" value={titleB} onChange={(e) => setTitleB(e.target.value)} className="border-1 p-1 w-12" />  
              <input type="color" id="head" name="head" onChange={(e) => {
                const rgb = hexToRgb(e.target.value);
                setTitleR(rgb.r);
                setTitleG(rgb.g);
                setTitleB(rgb.b);
              }} value={rgbToHex(titleR,titleG,titleB)} style={{width: '25px'}} />
            </div>
          </div></>}
      </div>
      

      {/* Show listed output */}

      <div
        className="flex flex-row gap-2">
      <button
        onClick={() => {
          let result = '';
          if(theme === 'custom') {
            result += `THEME_CUSTOM:${bgR||0},${bgG||0},${bgB||0},${fontR||0},${fontG||0},${fontB||0},${hilitR||0},${hilitG||0},${hilitB||0},${titleR||0},${titleG||0},${titleB||0}\n`;
          }
          if (passListOutput) {
            result += 'PASS_LIST_ENABLED\n';
          }
          if (autstartBluetooth) {
            result += 'AUTOSTART_BLUETOOTH\n';
          }
          if (autopressEnter) {
            result += 'AUTOPRESS_ENTER\n';
          }
          if (!connectionConfirmation) {
            result += 'CONNECTION_CONFIRMATION_DISABLED\n';
          }
          handleSubmit(result)
        }}
        className="disabled:opacity-40 disabled:cursor-not-allowed px-6 py-2.5 rounded-md transition-colors flex items-center gap-2"
        style={{
          backgroundColor: "var(--pm-accent)",
          color: "var(--pm-accent-text)",
        }}
      >
        Update settings
      </button>
      <button
        onClick={() => {
          let result = '';
          handleSubmit(result)
        }}
        className="disabled:opacity-40 disabled:cursor-not-allowed px-6 py-2.5 rounded-md transition-colors flex items-center gap-2"
        style={{
          backgroundColor: "var(--pm-bg-page)",
          color: "var(--pm-accent-text)",
        }}
      >
        Reset settings
      </button></div>
    </div>
  );
}