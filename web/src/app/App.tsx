import { useState, useEffect } from "react";
import { PasswordForm } from "./components/PasswordForm";
import { PasswordList } from "./components/PasswordList";
import { ThemeProvider, useTheme } from "./components/ThemeProvider";
import { Shield, Search, Sun, Moon, Plus, Settings, ChevronDown, CheckIcon } from "lucide-react";
import { PassManagerBLE } from "../pass-manager";
import { Dialog, DialogContent } from "./components/ui/dialog";
import { SettingsForm } from "./components/SettingsForm";
import { DialogTitle } from "@radix-ui/react-dialog";
import * as DropdownMenu from "@radix-ui/react-dropdown-menu";
import * as ToggleGroup from "@radix-ui/react-toggle-group";
import { maintainableVersions, version, maintainableFirmwareVersions } from "./config";

export interface PasswordEntry {
  id: string;
  type?:'pwd' | 'note' | 'card';
  service: string;
  username: string;
  password?:string;
  ruleWhitelist?: string;
  ruleLimitsMin?: number;
  ruleLimitsMax?: number;
}
export interface NoteEntry {
  id: string;
  title: string;
  contents: string;
}
export interface CardEntry {
  id: string;
  title: string;
  name: string;
  number: string;
  exp: string;
  cvc: string;
}

const toggleGroupItemClasses =
	"flex  w-1/3 py-2 items-center justify-center leading-4 text-mauve11 first:rounded-l last:rounded-r hover:bg-violet3 focus:z-10 focus:outline-none data-[state=on]:bg-violet6 data-[state=on]:bg-border pointer";

const passManagerBLE = new PassManagerBLE();


function PasswordManager() {
  const { theme, toggleTheme } = useTheme();
  const [entries, setEntries] = useState<(PasswordEntry|NoteEntry)[]>([]);
  const [editEntry, setEditEntry] = useState<PasswordEntry|NoteEntry | null>(null);
  const [search, setSearch] = useState("");
  const [showForm, setShowForm] = useState(false);
  const [connected, setConnected] = useState(false);
  const [dialogVisible, setDialogVisible] = useState("");
  const [firmwareVersion, setFirmwareVersion] = useState(maintainableFirmwareVersions[maintainableFirmwareVersions.length-1]);
  const [selected, setSelected] = useState("pwd");

  function pickNextUID() {
      return '/' + (entries.reduce((acc, it) => {
        const num = it.id.replace(/\/+([0-9]+)\.(pwd|card|note)/gi, '$1');
        console.warn(num);
        if(acc < +num) {
          acc = +num;
        }
        return acc;
      }, 0) + 1) + '.'+selected
      // return crypto.randomUUID();
  }
  useEffect(() => {
    if (editEntry) setShowForm(true);
  }, [editEntry]);

  const handleSave = (data: PasswordEntry) => {
    if (editEntry) {
      passManagerBLE.submitUpdate({id: editEntry.id, ...data})
      setEntries((prev) =>
        prev.map((e) => (e.id === editEntry.id ? { ...e, ...data } : e))
      );
      setEditEntry(null);
    } else {
      passManagerBLE.submit(data)
      setEntries((prev) => [ ...prev, { ...data, id: pickNextUID() }]);
    }
    setShowForm(false);
  };

  const handleSort = (data: string) => {
    passManagerBLE.submitSorting(data);
  };
  const handleDelete = (id: string) => {
    passManagerBLE.deleteItem(id);
    setEntries((prev) => prev.filter((e) => e.id !== id));
    if (editEntry?.id === id) {
      setEditEntry(null);
      setShowForm(false);
    }
  };

  const handleEdit = (entry: PasswordEntry|NoteEntry) => {
    setEditEntry(entry);
  };

  const handleCancelEdit = () => {
    setEditEntry(null);
    setShowForm(false);
  };

  const filtered = entries.filter(
    (e) =>{
      if (!search.trim())
        return true;

      switch(selected) {
        case "pwd":
          
            return e.service.toLowerCase().includes(search.toLowerCase()) || e.username.toLowerCase().includes(search.toLowerCase());
          
        case "card":
          
            return e.title.toLowerCase().includes(search.toLowerCase()) || e.name.toLowerCase().includes(search.toLowerCase());
        case "note":
          
            return e.title.toLowerCase().includes(search.toLowerCase()) || e.contents.toLowerCase().includes(search.toLowerCase());
      }
      return true;
    }
  );

  return (
    <div
      className="flex flex-col h-screen transition-colors duration-300"
      style={{ backgroundColor: "var(--pm-bg-page)", color: "var(--pm-text)" }}
    >
        <Dialog open={dialogVisible && connected} onOpenChange={(a) => {
          if(!a) {
            setDialogVisible('')
          }
        }}>
          <DialogContent
          className="overflow-auto max-h-screen"
      style={{ backgroundColor: "var(--pm-bg-page)", color: "var(--pm-text)" }} >
            <DialogTitle>Settings</DialogTitle>
            <SettingsForm onSave={(data) => {
              passManagerBLE.submitMeta(data);
            }} meta={dialogVisible}></SettingsForm>
          </DialogContent>
        </Dialog>

      {/* Header */}
      <header
        className="transition-colors duration-300"
        style={{
          backgroundColor: theme === "dark" ? "#1a1a2c" : "#ffffff",
          borderBottom: "1px solid var(--pm-border)",
        }}
      >
        <div className="max-w-6xl mx-auto px-6 py-4 flex items-center justify-between">
          <div className="flex items-center gap-3">
            <div
              className="w-9 h-9 rounded-lg flex items-center justify-center"
              style={{ backgroundColor: "var(--pm-accent)" }}
            >
              <Shield size={20} style={{ color: "var(--pm-accent-text)" }} />
            </div>
            <div>
              <h1 className="text-[18px]" style={{ textAlign:"right", lineHeight: "14px", color: "var(--pm-text)" }}>
                Pass<span style={{ color: "var(--pm-accent)" }}>word</span><br/> Man<span style={{ color: "var(--pm-accent)" }}>ager</span>
              </h1>
            </div>
          </div>
          <div className="flex items-center gap-3">
            

	<DropdownMenu.Root>
			<DropdownMenu.Trigger asChild>
				<button
					className="inline-flex items-center justify-center rounded-full text-violet11  shadow-blackA4 outline-none hover:bg-violet3 "
					aria-label="Customise options"
				><span className="text-[12px]" style={{ color: "var(--pm-text-dim)" }}>
              ver {version}
              <ChevronDown style={{display: "inline-block"}} size="16"/>
            </span>
				</button>
			</DropdownMenu.Trigger>

			<DropdownMenu.Portal>
				<DropdownMenu.Content              
        style={{
                backgroundColor: "var(--pm-bg-badge)",
                color: "var(--pm-text-muted)",
              }}
					className="min-w-[120px] rounded-md bg-white p-[5px] shadow-[0px_10px_38px_-10px_rgba(22,_23,_24,_0.35),_0px_10px_20px_-15px_rgba(22,_23,_24,_0.2)] will-change-[opacity,transform] data-[side=bottom]:animate-slideUpAndFade data-[side=left]:animate-slideRightAndFade data-[side=right]:animate-slideLeftAndFade data-[side=top]:animate-slideDownAndFade"
					sideOffset={5}
				>
					<DropdownMenu.Item onClick={() => {location.href = "/passman/"}} className="group relative flex h-[25px] select-none items-center rounded-[3px] pl-[25px] pr-[5px] text-[13px] leading-none text-violet11 outline-none data-[disabled]:pointer-events-none data-[highlighted]:bg-violet9 data-[disabled]:text-mauve8 data-[highlighted]:text-violet1">
						Latest
					</DropdownMenu.Item>
          {maintainableVersions.map((it) => (
					<DropdownMenu.Item onClick={() => {location.href = `/passman/${it}/`}} className="group relative flex h-[25px] select-none items-center rounded-[3px] pl-[25px] pr-[5px] text-[13px] leading-none text-violet11 outline-none data-[disabled]:pointer-events-none data-[highlighted]:bg-violet9 data-[disabled]:text-mauve8 data-[highlighted]:text-violet1">
						{it}
					</DropdownMenu.Item>))}

					<DropdownMenu.Arrow             style={{
                fill: "var(--pm-bg-badge)",
                color: "var(--pm-text-muted)",
              }}/>
				</DropdownMenu.Content>
			</DropdownMenu.Portal>
		</DropdownMenu.Root>

              {/* <p className="text-[12px]" style={{ color: "var(--pm-text-dim)" }}>
                {entries.length} entries stored
              </p> */}
            {/* Theme Toggle */}
            <button
              onClick={toggleTheme}
              className="w-9 h-9 rounded-lg flex items-center justify-center transition-colors cursor-pointer"
              style={{
                backgroundColor: "var(--pm-bg-badge)",
                color: "var(--pm-text-muted)",
              }}
              title={`Switch to ${theme === "dark" ? "light" : "dark"} mode`}
            >
              {theme === "dark" ? <Sun size={18} /> : <Moon size={18} />}
            </button>
        {connected && (
          <>
            <button
              onClick={() => {
                setTimeout(() => {

                passManagerBLE.requestMeta();
                },100)
              }}
              className="fixed z-20 bottom-25 left-10 xl:left-auto xl:right-10 px-4 py-4 rounded-full transition-colors text-[14px]"
              style={
                dialogVisible
                  ? {
                      backgroundColor: "var(--pm-bg-badge)",
                      color: "var(--pm-text-muted)",
                    }
                  : {
                      backgroundColor: "var(--pm-accent)",
                      color: "var(--pm-accent-text)",
                    }
              }
            >
              <Settings size="32"/>
            </button>
            <button
              onClick={() => {
                setShowForm(!showForm);
                if (showForm) setEditEntry(null);
              }}
              className="fixed z-20 bottom-5 left-10 xl:left-auto xl:right-10 px-4 py-4 rounded-full transition-colors text-[14px]"
              style={
                showForm
                  ? {
                      backgroundColor: "var(--pm-bg-badge)",
                      color: "var(--pm-text-muted)",
                    }
                  : {
                      backgroundColor: "var(--pm-accent)",
                      color: "var(--pm-accent-text)",
                    }
              }
            >
               <Plus size={32} />
              {/* {showForm ? "Close Form" : "+ New Password"} */}
            </button></>
            )}
          </div>
        </div>
      </header>

      <main className="flex-grow-1 h-1 max-w-6xl mx-auto px-6 my-6 space-y-6" style={{width: "100%",    backgroundColor: "var(--pm-bg-page)"}}>

        {!connected && (
          <div className="relative flex flex-col justify-center items-center min-h-[300px] overflow-auto">
            <button
              onClick={async () => {
                const device = await passManagerBLE.init(() => {
                    setConnected(false);
                    location.reload();
                    setDialogVisible('');
                  }, (entries: PasswordEntry[]) => {
                    setEntries(entries)
                  }, (meta) => {
                    setDialogVisible(meta)
                  });
                if(device){
                  setConnected(true);
                  passManagerBLE.requestList(selected)
                }
              }}
              className="px-8 py-4 mb-12 mt-12 min-w-52 rounded-md transition-colors text-[14px]  cursor-pointer"
              style={{
                      backgroundColor: "var(--pm-accent)",
                      color: "var(--pm-accent-text)",
                    }
              }
            >Connect to Passman device</button>
            <div style={{border: "1px solid var(--pm-border)  ", padding: "20px",backgroundSize: "90% auto", backgroundRepeat: "no-repeat", backgroundImage: "var(--tutorial)", backgroundPosition: "center bottom", width: "100%", maxWidth: "720px"}} className="relative text-center mb-4">
              <span className="absolute " style={{left: '50%', top: "-14px",transform: "translateX(-50%)", background: "var(--pm-bg-page)", padding: "0 12px"}}>or</span>
<div className="flex justify-center"><esp-web-install-button manifest={`/passman/res/firmware/${firmwareVersion}/manifest.json`}>
  <button slot="activate"
              className="px-4 py-2 min-w-4 rounded-md transition-colors text-[14px]  cursor-pointer rounded-r-none"
              style={{
                      backgroundColor: "#03a9f4",
                      color: "#fff",
                    }
              }
            >Upload firmware</button> </esp-web-install-button>	<DropdownMenu.Root>
			<DropdownMenu.Trigger asChild>
				<button            
        style={{
                      backgroundColor: "#03a9f4",
                      color: "#fff",
              }}
					className="inline-flex items-center justify-center rounded-md rounded-l-none text-violet11  shadow-blackA4 outline-none hover:bg-violet3 border-l border-white px-4"
					aria-label="Customise options"
				><span className="text-[12px]" style={{ color: "#fff" }}>
              v. {firmwareVersion}
              <ChevronDown style={{display: "inline-block"}} size="16"/>
            </span>
				</button>
			</DropdownMenu.Trigger>

			<DropdownMenu.Portal>
				<DropdownMenu.Content     
        align="end"         
        style={{
                      backgroundColor: "#03a9f4",
                      color: "#fff",
              }}
					className="min-w-[120px] rounded-md bg-white p-[5px] shadow-[0px_10px_38px_-10px_rgba(22,_23,_24,_0.35),_0px_10px_20px_-15px_rgba(22,_23,_24,_0.2)] will-change-[opacity,transform] data-[side=bottom]:animate-slideUpAndFade data-[side=left]:animate-slideRightAndFade data-[side=right]:animate-slideLeftAndFade data-[side=top]:animate-slideDownAndFade"
					sideOffset={5}
				>
					<DropdownMenu.RadioGroup value={firmwareVersion} onValueChange={setFirmwareVersion}>
          {maintainableFirmwareVersions.map((it) => (
					<DropdownMenu.RadioItem value={it} className="group relative flex h-[25px] select-none items-center rounded-[3px] pl-[25px] pr-[5px] text-[13px] leading-none text-violet11 outline-none data-[disabled]:pointer-events-none data-[highlighted]:bg-violet9 data-[disabled]:text-mauve8 data-[highlighted]:text-violet1">
						
						<DropdownMenu.ItemIndicator className="absolute left-0 inline-flex w-[25px] items-center justify-center">
							<CheckIcon size="12"/>
						</DropdownMenu.ItemIndicator>{it}
					</DropdownMenu.RadioItem>))}
					</DropdownMenu.RadioGroup>

					<DropdownMenu.Arrow             style={{
                fill: "var(--pm-bg-badge)",
                color: "var(--pm-text-muted)",
              }}/>
				</DropdownMenu.Content>
			</DropdownMenu.Portal>
		</DropdownMenu.Root>
</div>

            <img src="/passman/res/tutorial.svg" alt="" className="max-w-150" style={{opacity: 0, width: "100%"}}/>
            </div>

          </div>
        )}
        {connected && (<>

  <div className={`grid relative h-full grid-cols-1 gap-3 ${showForm ? 'xl:grid-cols-2' : ''}`}><div className="flex  flex-col overflow-hidden h-full">{/* Search */}
                  <div className="relative mb-3">
                    <Search
                      size={16}
                      className="absolute left-3 top-1/2 -translate-y-1/2"
                      style={{ color: "var(--pm-text-dim)" }}
                    />
                    <input
                      type="text"
                      value={search}
                      onChange={(e) => setSearch(e.target.value)}
                      placeholder="Search by service, title or username..."
                      className="w-full rounded-md pl-9 pr-4 py-2.5 transition-colors focus:outline-none"
                      style={{
                        backgroundColor: "var(--pm-bg-card)",
                        border: "1px solid var(--pm-border)",
                        color: "var(--pm-text)",
                      }}
                      onFocus={(e) => (e.target.style.borderColor = "var(--pm-accent)")}
                      onBlur={(e) => (e.target.style.borderColor = "var(--pm-border)")}
                    />
                  </div>
<div>

<ToggleGroup.Root
		className="flex  justify-center items-center space-x-px rounded-t-md   cursor-pointer m-auto w-1/1"
		type="single"
    style={{
                      backgroundColor: "var(--pm-bg-card)",
        border: "1px solid var(--pm-border)",
        borderBottom: 0,
                      color: "var(--pm-text-dim)",
                    }
              }
		defaultValue="pwd"
      value={selected}
      onValueChange={(val) => { 
        if (val && val !== selected) { 
          setEntries([]);
          setSelected(val); 
          passManagerBLE.requestList(val);

        }
      }}
		aria-label="Text alignment"
	>
		<ToggleGroup.Item
			className={toggleGroupItemClasses}
			value="pwd"
			aria-label="Left aligned"
		>Passwords
		</ToggleGroup.Item>
		<ToggleGroup.Item
			className={toggleGroupItemClasses}
			value="card"
			aria-label="Center aligned"
		>Cards
		</ToggleGroup.Item>
		<ToggleGroup.Item
			className={toggleGroupItemClasses}
			value="note"
			aria-label="Right aligned"
		>Notes
		</ToggleGroup.Item>
	</ToggleGroup.Root>
</div>
                  <PasswordList
                    entries={filtered}
                    onEdit={handleEdit}
                    onDelete={handleDelete}
                    onSort={handleSort}
                    type={selected}
                  />
                  </div>{showForm && (
                    <PasswordForm
                      key={editEntry?.id ?? "new"}
                      defaultType={selected}
                      onSave={handleSave}
                      editEntry={editEntry}
                      onCancelEdit={handleCancelEdit}
                    />
                  )}
                  
                </div>
          

                  
        </>)}
        
      </main>
    </div>
  );
}

export default function App() {
  return (
    <ThemeProvider>
      <PasswordManager />
    </ThemeProvider>
  );
}
