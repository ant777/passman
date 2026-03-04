import { useState, useEffect } from "react";
import { PasswordForm } from "./components/PasswordForm";
import { PasswordList } from "./components/PasswordList";
import { ThemeProvider, useTheme } from "./components/ThemeProvider";
import { Shield, Search, Sun, Moon } from "lucide-react";
import { PassManagerBLE } from "../pass-manager";

export interface PasswordEntry {
  id: string;
  service: string;
  username: string;
  ruleWhitelist?: string;password?:string;
  ruleLimitsMin?: number;
  ruleLimitsMax?: number;
}

const SAMPLE_ENTRIES: PasswordEntry[] = [
  {
    id: "1",
    service: "GitHub",
    username: "dev@example.com",
  },
  {
    id: "2",
    service: "Google",
    username: "user@gmail.com",
  },
  {
    id: "3",
    service: "Netflix",
    username: "viewer@email.com",
  },
  {
    id: "4",
    service: "AWS Console",
    username: "admin@company.io",
  },
  {
    id: "5",
    service: "Slack",
    username: "team@workspace.com",
  },
];

const passManagerBLE = new PassManagerBLE();

function PasswordManager() {
  const { theme, toggleTheme } = useTheme();
  const [entries, setEntries] = useState<PasswordEntry[]>(SAMPLE_ENTRIES);
  const [editEntry, setEditEntry] = useState<PasswordEntry | null>(null);
  const [search, setSearch] = useState("");
  const [showForm, setShowForm] = useState(false);
  const [connected, setConnected] = useState(false);

  useEffect(() => {
    if (editEntry) setShowForm(true);
  }, [editEntry]);

  const handleSave = (data: Omit<PasswordEntry, "id">) => {
    if (editEntry) {
      passManagerBLE.submitUpdate()
      setEntries((prev) =>
        prev.map((e) => (e.id === editEntry.id ? { ...e, ...data } : e))
      );
      setEditEntry(null);
    } else {
      passManagerBLE.submit(data)
      setEntries((prev) => [{ ...data, id: crypto.randomUUID() }, ...prev]);
    }
    setShowForm(false);
  };

  const handleDelete = (id: string) => {
    setEntries((prev) => prev.filter((e) => e.id !== id));
    if (editEntry?.id === id) {
      setEditEntry(null);
      setShowForm(false);
    }
  };

  const handleEdit = (entry: PasswordEntry) => {
    setEditEntry(entry);
  };

  const handleCancelEdit = () => {
    setEditEntry(null);
    setShowForm(false);
  };

  const filtered = entries.filter(
    (e) =>
      e.service.toLowerCase().includes(search.toLowerCase()) ||
      e.username.toLowerCase().includes(search.toLowerCase())
  );

  return (
    <div
      className="min-h-screen transition-colors duration-300"
      style={{ backgroundColor: "var(--pm-bg-page)", color: "var(--pm-text)" }}
    >
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
              <h1 className="text-[18px]" style={{ color: "var(--pm-text)" }}>
                Password Vault
              </h1>
              <p className="text-[12px]" style={{ color: "var(--pm-text-dim)" }}>
                {entries.length} entries stored
              </p>
            </div>
          </div>
          <div className="flex items-center gap-3">
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
            <button
              onClick={() => {
                setShowForm(!showForm);
                if (showForm) setEditEntry(null);
              }}
              className="px-4 py-2 rounded-md transition-colors text-[14px]"
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
              {showForm ? "Close Form" : "+ New Password"}
            </button>)}
          </div>
        </div>
      </header>

      <main className="max-w-6xl mx-auto px-6 py-6 space-y-6">

        {!connected && (
          <div className="relative flex justify-center items-center min-h-[300px]">
            <button
              onClick={async () => {
                const device = await passManagerBLE.init(() => {
                  setConnected(false)
                }, (entries) => {
                  setEntries(entries)
                });
                if(device)
                setConnected(true);
              }}
              className="px-4 py-2 rounded-md transition-colors text-[14px]  cursor-pointer"
              style={{
                      backgroundColor: "var(--pm-accent)",
                      color: "var(--pm-accent-text)",
                    }
              }
            >Connect to Passman device</button>
          </div>
        )}
        {connected && (<>
          {showForm && (
                    <PasswordForm
                      key={editEntry?.id ?? "new"}
                      onSave={handleSave}
                      editEntry={editEntry}
                      onCancelEdit={handleCancelEdit}
                    />
                  )}

                  {/* Search */}
                  <div className="relative">
                    <Search
                      size={16}
                      className="absolute left-3 top-1/2 -translate-y-1/2"
                      style={{ color: "var(--pm-text-dim)" }}
                    />
                    <input
                      type="text"
                      value={search}
                      onChange={(e) => setSearch(e.target.value)}
                      placeholder="Search by service or username..."
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

                  <PasswordList
                    entries={filtered}
                    onEdit={handleEdit}
                    onDelete={handleDelete}
                  />
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
