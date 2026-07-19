import { useEffect, useMemo, useState } from "react";
import {
  Copy,
  ExternalLink,
  Trash2,
  Pencil,
  Eye,
  EyeOff,
  Check,
} from "lucide-react";
import { NoteEntry } from "../App";

interface PasswordEntry {
  id: string;
  service: string;
  username: string;
}

export interface PasswordListTitleEntry {
  label: string;
}
export interface PasswordListUserEntry {
  label: string;
  copyCb: Function;
  copied: boolean
}

export interface ListProps {
  entries: (PasswordEntry|NoteEntry)[];
  onEdit: (entry: PasswordEntry|NoteEntry) => void;
  onDelete: (id: string) => void;
  type: "pwd" | "note" | "card"
}
const SERVICE = 'Service';
const TITLE = 'Title';
const USERNAME = 'Username';
const NOTE = 'Note';
const PASSWORD = "Password";
const HOLDER_NAME = "Holder Name";
const NUMBER = "Number";
const CVC = "CVC";
const EXP_AT = "Exp at";
const PIN = "PIN";

const keyMap = {
  [SERVICE]: "service",
  [TITLE]: "title",
  [USERNAME]: "username",
  [NOTE]: "contents",
  [PASSWORD]: "password",
  [HOLDER_NAME]: "name",
  [NUMBER]: "number",
  [CVC]: "cvc",
  [EXP_AT]: "exp",
  [PIN]: "pin"
}


const cols = {
  pwd: [
    SERVICE,
    USERNAME,
    PASSWORD
  ],
  note: [
    TITLE,
    NOTE
  ],
  card: [
    TITLE,
    HOLDER_NAME,
    NUMBER,
    CVC,
    EXP_AT,
    PIN
  ]
};
const colSizeMap = {
  card: "1fr_1fr_1fr_0.5fr_0.5fr_0.5fr",
  pwd: "1fr_1fr_0.5fr",
  note: "1fr_1fr"
};

export function PasswordListUsernameEntry({ label = "pwd",copyCb, copied }: PasswordListUserEntry) {
  return (
    <div className="flex items-center gap-1.5 min-w-0">
      <span
        className="truncate text-[14px] font-mono"
        style={{ color: "var(--pm-text-secondary)" }}
      >
        {label}
      </span>
      <button
        onClick={copyCb}
        className="transition-colors shrink-0"
        style={{ color: copied ? "var(--pm-accent-fg)" : "var(--pm-text-dim)" }}
        title="Copy username"
      >
        {copied ? (
          <Check size={14} />
        ) : (
          <Copy size={14} />
        )}
      </button>
    </div>
  )
};
export function PasswordListPasswordEntry({ label = "pwd",copyCb, copied }: PasswordListUserEntry) {
  return (
    
            <div className="flex items-center gap-1.5 min-w-0">
              <span
                className="truncate text-[14px] font-mono"
                style={{ color: "var(--pm-text-secondary)" }}
              >
                ******
              </span>
              <button
                onClick={copyCb}
                className="transition-colors shrink-0"
                style={{ color: copied ? "var(--pm-accent-fg)" : "var(--pm-text-dim)" }}
                title="Copy username"
              >
                {copied ? (
                  <Check size={14} />
                ) : (
                  <Copy size={14} />
                )}
              </button>
            </div>
  )
};

export function PasswordListTitleEntry({ label = "pwd" }: PasswordListTitleEntry) {
  return  <div className="flex items-center gap-2 min-w-0">
    <div
      className="w-7 h-7 rounded-md flex items-center justify-center text-[12px] shrink-0"
      style={{
        backgroundColor: "var(--pm-bg-badge)",
        color: "var(--pm-accent-fg)",
      }}
    >
      {label.charAt(0).toUpperCase()}
    </div>
    <div className="min-w-0">
      <div className="truncate text-[14px]" style={{ color: "var(--pm-text)" }}>
        {label}
      </div>
    </div>
  </div>
};

export function PasswordList({ entries, onEdit, onDelete, onSort, type = "pwd" }: ListProps) {
  const [visiblePasswords, setVisiblePasswords] = useState<Set<string>>(new Set());
  const [copiedField, setCopiedField] = useState<string | null>(null);
  const [sorting, setSorting] = useState({});
  const [sortedEntries, setSortedEntries] = useState(entries);
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

  const sorted = useMemo(() => {
    const filtered = entries.filter((ent) => {
      return Object.values(sorting).map(it => `/${it}.${type}`).includes(ent.id);
    });
    const plain = entries.filter((ent) => {
      return !Object.values(sorting).map(it => `/${it}.${type}`).includes(ent.id);
    });
    Object.keys(sorting).forEach((it, id) => {
      const ind = plain.findIndex(x => x.id === `/${it}.${type}` );

      plain.splice(ind, 0, filtered.filter(sit => sit.id === `/${sorting[it]}.${type}`)[0]);
    }, []);
    return plain;
  }, [entries, sorting]);


  const cancelDelete = () => {
    setDeleteConfirm(null);
    setDeleteDoubleConfirm(null);
  };

  useEffect(() => {
    setSortedEntries(entries);
  }, [entries]);
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
      className="rounded-b-lg overflow-y-auto max-h-full"
      style={{
        backgroundColor: "var(--pm-bg-card)",
        border: "1px solid var(--pm-border)",
      }}
    >
      <div className={`grid grid-cols-[1fr_1fr_0.5fr_auto] grid-cols-[1fr_1fr_auto] grid-cols-[1fr_1fr_1fr_0.5fr_0.5fr_0.5fr_auto]`}></div>
      {/* Header */}
      <div
        className={`grid grid-cols-[${colSizeMap[type]}_auto] gap-4 px-5 py-3 text-[13px]`}
        style={{
          borderBottom: "1px solid var(--pm-border)",
          color: "var(--pm-text-dim)",
        }}
      >
        {cols[type].map((col) => <span>{col}</span>)}
        
        <span className="w-[80px] text-right">Actions</span>
      </div>

      {/* Rows */}
      <div style={{display: "flex", flexDirection: "column"}}>
          
        {sortedEntries.map((entry, ind) => {
          const isVisible = visiblePasswords.has(entry.id);
          const isDeleting = deleteConfirm === entry.id;
          const isDoubleConfirm = deleteDoubleConfirm === entry.id;

          return (
            <div
              key={entry.id}
              data-index={parseInt(entry.id.substring(1))}
              className={`grid grid-cols-[${colSizeMap[type]}_auto] gap-4 px-5 py-3 transition-colors items-center group`}
              style={{ borderBottom: "1px solid var(--pm-border)" }}
              onMouseEnter={(e) =>
                (e.currentTarget.style.backgroundColor = "var(--pm-bg-hover)")
              }
              onDragOver={(e) => {
                e.preventDefault();
                // "event.preventDefault(); evt.currentTarget.classList.add('state-dragover')"
                e.currentTarget.classList.add('state-dragover')
              }
              }
              onDragLeave={
                (e) => {
                  // "event.preventDefault(); evt.currentTarget.classList.add('state-dragover')"
                  e.currentTarget.classList.remove('state-dragover')
                }
              }
              onDragStart={
                (event) => {
                  event.dataTransfer.setData('text/plain', entry.id)
                }
              }
              onDrop={
                (event) => {
                
                  const dragIndex = parseInt(event.dataTransfer.getData('text/plain').substring(1), 10)
                  event.currentTarget.classList.remove('state-dragover');
                  const dropIndex = parseInt(event.target.closest('[data-index]').dataset.index, 10);
                  if(dragIndex === dropIndex) return;
                  setSortedEntries(entries.map((it) => {
                    const parsedInd = parseInt(it.id.substring(1), 10);
                    if (parsedInd === dragIndex) {
                      it.id= `/${dropIndex}.${type}`;
                    }
                    else if(dragIndex > dropIndex){
                      if (parsedInd >= dropIndex && parsedInd < dragIndex) {

                        it.id = `/${parsedInd + 1}.${type}`;
                      }
                    }
                    else if(dragIndex < dropIndex){
                      if (parsedInd <= dropIndex && parsedInd > dragIndex) {

                        it.id = `/${parsedInd - 1}.${type}`;
                      }
                    }
                    return it;
                  }).sort((a,b) => parseInt(a.id.substring(1), 10) > parseInt(b.id.substring(1), 10) ? 1: -1));
                  const newSortingValue = {
                    ...Object.keys(sorting).reduce((acc, ind) => {
                      ind = parseInt(ind, 10);
                      if(ind >= dragIndex) {

                        acc[ind+1] = sorting[ind];
                      }else {
                        acc[ind] = sorting[ind];
                        
                      }
                      return acc;
                      
                    }, {}),
                    [dropIndex]: dragIndex

                  };
                  onSort(`${type}\n${dropIndex}=${dragIndex}`);

                  setSorting(newSortingValue)

                }
              }
              draggable="true"
              onMouseLeave={(e) => (e.currentTarget.style.backgroundColor = "transparent")}
            >

              {cols[type].map((col) => {
                const key = keyMap[col]
                if (col === TITLE || col === SERVICE) {
                  return <PasswordListTitleEntry label={entry[key]}/>
                }
                if (col === USERNAME || col === NOTE) {
                  return <PasswordListUsernameEntry label={entry[key]} copyCb={() => copyToClipboard(entry[key], `user-${entry.id}`)} copied={copiedField === `user-${entry.id}`}/>
                }
                if (col === PASSWORD) {
                  return <PasswordListPasswordEntry label={entry[key]} copyCb={() => copyToClipboard(entry[key], `user-${entry.id}`)} copied={copiedField === `user-${entry.id}`}/>
                } else {

                  return <PasswordListUsernameEntry label={entry[key]} copyCb={() => copyToClipboard(entry[key], `user-${entry.id}`)} copied={copiedField === `user-${entry.id}`}/>
                }
              })}

            

              {/* Actions */}
              <div className="flex items-center gap-1 w-[80px] justify-end">
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
    </div>
  );
}