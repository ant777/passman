import { useState } from "react";
import { RefreshCw, Plus, Save } from "lucide-react";
import { PasswordEntry } from "../App";


interface PasswordFormProps {
  onSave: (entry: Omit<PasswordEntry, "id">) => void;
  editEntry?: PasswordEntry | null;
  onCancelEdit?: () => void;
}

function generatePassword(options: {
  length: number;
  uppercase: boolean;
  lowercase: boolean;
  numbers: boolean;
  symbols: boolean;
}): string {
  let chars = "";
  if (options.uppercase) chars += "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  if (options.lowercase) chars += "abcdefghijklmnopqrstuvwxyz";
  if (options.numbers) chars += "0123456789";
  if (options.symbols) chars += "!@#$%^&*()_+-=[]{}|;:,.<>?";
  if (!chars) chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

  let result = "";
  for (let i = 0; i < options.length; i++) {
    result += chars.charAt(Math.floor(Math.random() * chars.length));
  }
  return result;
}

export function PasswordForm({ onSave, editEntry, onCancelEdit }: PasswordFormProps) {
  const [service, setService] = useState(editEntry?.service ?? "");
  const [username, setUsername] = useState(editEntry?.username ?? "");
  const [password, setPassword] = useState(editEntry?.password ?? "");
  const [minLength, setMinLength] = useState(editEntry?.ruleLimitsMin || 12);
  const [maxLength, setMaxLength] = useState(editEntry?.ruleLimitsMin || 20);
  const [uppercase, setUppercase] = useState(editEntry?.ruleWhitelist?.includes('A-Z') || false);
  const [lowercase, setLowercase] = useState(editEntry?.ruleWhitelist?.includes('a-z') || false);
  const [numbers, setNumbers] = useState(editEntry?.ruleWhitelist?.includes('0-9') || false);
  const [symbols, setSymbols] = useState(true);

  const handleGenerate = () => {
    const actualMin = Math.max(4, Math.min(minLength, maxLength));
    const actualMax = Math.max(actualMin, maxLength);
    const len = Math.floor(Math.random() * (actualMax - actualMin + 1)) + actualMin;
    setPassword(generatePassword({ length: len, uppercase, lowercase, numbers, symbols }));
  };

  const handleSubmit = () => {
    if (!service.trim() || !username.trim()) return;
    let ruleWhitelist = "";
    if(uppercase) {
      ruleWhitelist += 'A-Z';
    }
    if(lowercase) {
      ruleWhitelist += 'a-z';
    }
    if(numbers) {
      ruleWhitelist += '0-9';
    }
    if(symbols) {
      ruleWhitelist += 'A-Z';
    }
    const ruleLimitsMin = minLength;
    const ruleLimitsMax = maxLength;
    onSave({ service: service.trim(), username: username.trim(), password, ruleWhitelist, ruleLimitsMin, ruleLimitsMax });
    if (!editEntry) {
      setService("");
      setUsername("");
      setPassword("");
    }
  };

  const isEditing = !!editEntry;

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
      className="rounded-lg p-6"
      style={{
        backgroundColor: "var(--pm-bg-card)",
        border: "1px solid var(--pm-border)",
      }}
    >
      <div className="flex items-center justify-between mb-6">
        <h2 style={{ color: "var(--pm-text)" }}>
          {isEditing ? "Edit Password" : "Create New Password"}
        </h2>
        {isEditing && onCancelEdit && (
          <button
            onClick={onCancelEdit}
            className="transition-colors px-3 py-1 rounded"
            style={{
              color: "var(--pm-text-muted)",
              border: "1px solid var(--pm-border)",
            }}
          >
            Cancel
          </button>
        )}
      </div>

      <div className="grid grid-cols-1 md:grid-cols-1 gap-4 mb-6">
        <div>
          <label
            className="block mb-1.5 text-[13px]"
            style={{ color: "var(--pm-text-muted)" }}
          >
            Service Name
          </label>
          <input
            type="text"
            value={service}
            onChange={(e) => setService(e.target.value)}
            placeholder="e.g. GitHub, Google, Netflix"
            className={inputCls}
            style={inputStyle}
            onFocus={(e) => (e.target.style.borderColor = focusStyle)}
            onBlur={(e) => (e.target.style.borderColor = "var(--pm-border)")}
          />
        </div>
        <div>
          <label
            className="block mb-1.5 text-[13px]"
            style={{ color: "var(--pm-text-muted)" }}
          >
            Username / Email
          </label>
          <input
            type="text"
            value={username}
            onChange={(e) => setUsername(e.target.value)}
            placeholder="user@example.com"
            className={inputCls}
            style={inputStyle}
            onFocus={(e) => (e.target.style.borderColor = focusStyle)}
            onBlur={(e) => (e.target.style.borderColor = "var(--pm-border)")}
          />
        </div>
        <div>
          <label
            className="block mb-1.5 text-[13px]"
            style={{ color: "var(--pm-text-muted)" }}
          >
            Password
          </label>
          <div className="flex gap-2">
            <input
              type="text"
              value={password}
              onChange={(e) => setPassword(e.target.value)}
              placeholder="Enter or generate a password"
              className={`flex-1 rounded-md px-3 py-2 transition-colors focus:outline-none font-mono text-[var(--pm-text)] placeholder-[var(--pm-text-placeholder)]`}
              style={inputStyle}
              onFocus={(e) => (e.target.style.borderColor = focusStyle)}
              onBlur={(e) => (e.target.style.borderColor = "var(--pm-border)")}
            />
            <button
              onClick={handleGenerate}
              className="px-3 py-2 rounded-md transition-colors flex items-center gap-1.5"
              style={{
                backgroundColor: "var(--pm-bg-badge)",
                color: "var(--pm-accent-fg)",
              }}
              title="Generate password"
            >
              <RefreshCw size={16} />
            </button>
          </div>
        </div>
      </div>

      {/* Generation Rules */}
      <div
        className="rounded-md p-4 mb-6"
        style={{
          backgroundColor: "var(--pm-bg-input)",
          border: "1px solid var(--pm-border)",
        }}
      >
        <h3 className="mb-3 text-[13px]" style={{ color: "var(--pm-text-muted)" }}>
          Password Generation Rules
        </h3>
        <div className="flex flex-wrap items-center gap-x-6 gap-y-3">
          <div className="flex items-center gap-2">
            <label className="text-[13px]" style={{ color: "var(--pm-text-dim)" }}>
              Min Length:
            </label>
            <input
              type="number"
              min={4}
              max={64}
              value={minLength}
              onChange={(e) => setMinLength(Math.min(Number(e.target.value), maxLength))}
              className="w-16 rounded px-2 py-1 text-center focus:outline-none transition-colors"
              style={{
                backgroundColor: "var(--pm-bg-card)",
                border: "1px solid var(--pm-border)",
                color: "var(--pm-text)",
              }}
              onFocus={(e) => (e.target.style.borderColor = focusStyle)}
              onBlur={(e) => (e.target.style.borderColor = "var(--pm-border)")}
            />
          </div>
          <div className="flex items-center gap-2">
            <label className="text-[13px]" style={{ color: "var(--pm-text-dim)" }}>
              Max Length:
            </label>
            <input
              type="number"
              min={4}
              max={64}
              value={maxLength}
              onChange={(e) => setMaxLength(Math.max(Number(e.target.value), minLength))}
              className="w-16 rounded px-2 py-1 text-center focus:outline-none transition-colors"
              style={{
                backgroundColor: "var(--pm-bg-card)",
                border: "1px solid var(--pm-border)",
                color: "var(--pm-text)",
              }}
              onFocus={(e) => (e.target.style.borderColor = focusStyle)}
              onBlur={(e) => (e.target.style.borderColor = "var(--pm-border)")}
            />
          </div>
          {[
            { label: "A-Z", checked: uppercase, set: setUppercase },
            { label: "a-z", checked: lowercase, set: setLowercase },
            { label: "0-9", checked: numbers, set: setNumbers },
            { label: "!@#", checked: symbols, set: setSymbols },
          ].map((opt) => (
            <label
              key={opt.label}
              className="flex items-center gap-2 cursor-pointer select-none"
            >
              <input
                type="checkbox"
                checked={opt.checked}
                onChange={(e) => opt.set(e.target.checked)}
                className="accent-[#FACC15] w-4 h-4"
              />
              <span className="text-[13px] font-mono" style={{ color: "var(--pm-text-dim)" }}>
                {opt.label}
              </span>
            </label>
          ))}
        </div>
      </div>

      <button
        onClick={handleSubmit}
        disabled={!service.trim() || !username.trim()}
        className="disabled:opacity-40 disabled:cursor-not-allowed px-6 py-2.5 rounded-md transition-colors flex items-center gap-2"
        style={{
          backgroundColor: "var(--pm-accent)",
          color: "var(--pm-accent-text)",
        }}
      >
        {isEditing ? <Save size={16} /> : <Plus size={16} />}
        {isEditing ? "Update Password" : "Create Password"}
      </button>
    </div>
  );
}