import { useState } from "react";
import {
  Copy,
  ExternalLink,
  Trash2,
  Pencil,
  Eye,
  EyeOff,
  Check,
} from "lucide-react";

interface PasswordEntry {
  id: string;
  service: string;
  username: string;
}

interface PasswordListProps {
  entries: PasswordEntry[];
  onEdit: (entry: PasswordEntry) => void;
  onDelete: (id: string) => void;
}

export function PasswordList({ entries, onEdit, onDelete }: PasswordListProps) {
  const [visiblePasswords, setVisiblePasswords] = useState<Set<string>>(new Set());
  const [copiedField, setCopiedField] = useState<string | null>(null);
  const [deleteConfirm, setDeleteConfirm] = useState<string | null>(null);
  const [deleteDoubleConfirm, setDeleteDoubleConfirm] = useState<string | null>(null);

  const toggleVisibility = (id: string) => {
    setVisiblePasswords((prev) => {
      const next = new Set(prev);
      if (next.has(id)) next.delete(id);
      else next.add(id);
      return next;
    });
  };

  const copyToClipboard = async (text: string, fieldKey: string) => {
    await navigator.clipboard.writeText(text);
    setCopiedField(fieldKey);
    setTimeout(() => setCopiedField(null), 1500);
  };

  const handleDeleteClick = (id: string) => {
    if (deleteDoubleConfirm === id) {
      onDelete(id);
      setDeleteConfirm(null);
      setDeleteDoubleConfirm(null);
    } else if (deleteConfirm === id) {
      setDeleteDoubleConfirm(id);
    } else {
      setDeleteConfirm(id);
      setDeleteDoubleConfirm(null);
    }
  };

  const cancelDelete = () => {
    setDeleteConfirm(null);
    setDeleteDoubleConfirm(null);
  };

  if (entries.length === 0) {
    return (
      <div
        className="rounded-lg p-12 text-center"
        style={{
          backgroundColor: "var(--pm-bg-card)",
          border: "1px solid var(--pm-border)",
        }}
      >
        <p className="text-[15px]" style={{ color: "var(--pm-text-dim)" }}>
          No passwords saved yet. Create your first entry above.
        </p>
      </div>
    );
  }

  return (
    <div
      className="rounded-lg overflow-hidden"
      style={{
        backgroundColor: "var(--pm-bg-card)",
        border: "1px solid var(--pm-border)",
      }}
    >
      {/* Header */}
      <div
        className="grid grid-cols-[1fr_1fr_1.2fr_auto] gap-4 px-5 py-3 text-[13px]"
        style={{
          borderBottom: "1px solid var(--pm-border)",
          color: "var(--pm-text-dim)",
        }}
      >
        <span>Service</span>
        <span>Username</span>
        <span>Password</span>
        <span className="w-[140px] text-right">Actions</span>
      </div>

      {/* Rows */}
      {entries.map((entry) => {
        const isVisible = visiblePasswords.has(entry.id);
        const isDeleting = deleteConfirm === entry.id;
        const isDoubleConfirm = deleteDoubleConfirm === entry.id;

        return (
          <div
            key={entry.id}
            className="grid grid-cols-[1fr_1fr_1.2fr_auto] gap-4 px-5 py-3 transition-colors items-center group"
            style={{ borderBottom: "1px solid var(--pm-border)" }}
            onMouseEnter={(e) =>
              (e.currentTarget.style.backgroundColor = "var(--pm-bg-hover)")
            }
            onMouseLeave={(e) => (e.currentTarget.style.backgroundColor = "transparent")}
          >
            {/* Service */}
            <div className="flex items-center gap-2 min-w-0">
              <div
                className="w-7 h-7 rounded-md flex items-center justify-center text-[12px] shrink-0"
                style={{
                  backgroundColor: "var(--pm-bg-badge)",
                  color: "var(--pm-accent-fg)",
                }}
              >
                {entry.service.charAt(0).toUpperCase()}
              </div>
              <div className="min-w-0">
                <div className="truncate text-[14px]" style={{ color: "var(--pm-text)" }}>
                  {entry.service}
                </div>
              </div>
            </div>

            {/* Username */}
            <div className="flex items-center gap-1.5 min-w-0">
              <span
                className="truncate text-[14px] font-mono"
                style={{ color: "var(--pm-text-secondary)" }}
              >
                {entry.username}
              </span>
              <button
                onClick={() => copyToClipboard(entry.username, `user-${entry.id}`)}
                className="transition-colors shrink-0"
                style={{ color: copiedField === `user-${entry.id}` ? "var(--pm-accent-fg)" : "var(--pm-text-dim)" }}
                title="Copy username"
              >
                {copiedField === `user-${entry.id}` ? (
                  <Check size={14} />
                ) : (
                  <Copy size={14} />
                )}
              </button>
            </div>
            {/* Username */}
            <div className="flex items-center gap-1.5 min-w-0">
              <span
                className="truncate text-[14px] font-mono"
                style={{ color: "var(--pm-text-secondary)" }}
              >
                ******
              </span>
              <button
                onClick={() => copyToClipboard(entry.username, `user-${entry.id}`)}
                className="transition-colors shrink-0"
                style={{ color: copiedField === `user-${entry.id}` ? "var(--pm-accent-fg)" : "var(--pm-text-dim)" }}
                title="Copy username"
              >
                {copiedField === `user-${entry.id}` ? (
                  <Check size={14} />
                ) : (
                  <Copy size={14} />
                )}
              </button>
            </div>
           

            {/* Actions */}
            <div className="flex items-center gap-1 w-[140px] justify-end">
              {isDeleting ? (
                <div className="flex items-center gap-1">
                  <button
                    onClick={() => handleDeleteClick(entry.id)}
                    className={`px-2 py-1 rounded text-[12px] transition-colors ${
                      isDoubleConfirm
                        ? "bg-red-600 text-white animate-pulse"
                        : "bg-red-500/20 text-red-400 hover:bg-red-500/30"
                    }`}
                  >
                    {isDoubleConfirm ? "Confirm Delete" : "Are you sure?"}
                  </button>
                  <button
                    onClick={cancelDelete}
                    className="px-1 py-1 text-[12px]"
                    style={{ color: "var(--pm-text-dim)" }}
                  >
                    No
                  </button>
                </div>
              ) : (
                <>
                  <button
                    onClick={() => onEdit(entry)}
                    className="transition-colors p-1.5 rounded"
                    style={{ color: "var(--pm-text-dim)" }}
                    onMouseEnter={(e) => (e.currentTarget.style.color = "var(--pm-accent-fg)")}
                    onMouseLeave={(e) => (e.currentTarget.style.color = "var(--pm-text-dim)")}
                    title="Edit"
                  >
                    <Pencil size={14} />
                  </button>
                  <button
                    onClick={() => handleDeleteClick(entry.id)}
                    className="transition-colors p-1.5 rounded hover:text-red-400"
                    style={{ color: "var(--pm-text-dim)" }}
                    title="Delete"
                  >
                    <Trash2 size={14} />
                  </button>
                </>
              )}
            </div>
          </div>
        );
      })}
    </div>
  );
}