import { createContext, useContext, useState, useEffect, type ReactNode } from "react";

type Theme = "dark" | "light";

interface ThemeContextType {
  theme: Theme;
  toggleTheme: () => void;
}

const ThemeContext = createContext<ThemeContextType>({
  theme: "dark",
  toggleTheme: () => {},
});

export function useTheme() {
  return useContext(ThemeContext);
}

const darkVars: Record<string, string> = {
  "--pm-bg-page": "#141420",
  "--pm-bg-card": "#1e1e2e",
  "--pm-bg-input": "#161622",
  "--pm-bg-hover": "#232338",
  "--pm-bg-badge": "#2a2a3e",
  "--pm-border": "#2a2a3e",
  "--pm-text": "#e0e0e0",
  "--pm-text-secondary": "#ccc",
  "--pm-text-muted": "#999",
  "--pm-text-dim": "#777",
  "--pm-text-placeholder": "#555",
  "--pm-accent": "#FACC15",
  "--pm-accent-fg": "#FACC15",
  "--pm-accent-hover": "#EAB308",
  "--pm-accent-text": "#111",
};

const lightVars: Record<string, string> = {
  "--pm-bg-page": "#f4f4f7",
  "--pm-bg-card": "#ffffff",
  "--pm-bg-input": "#f0f0f4",
  "--pm-bg-hover": "#f2f2f6",
  "--pm-bg-badge": "#eeeef3",
  "--pm-border": "#dcdce4",
  "--pm-text": "#1a1a2c",
  "--pm-text-secondary": "#333",
  "--pm-text-muted": "#5a5a6e",
  "--pm-text-dim": "#70708a",
  "--pm-text-placeholder": "#999",
  "--pm-accent": "#FACC15",
  "--pm-accent-fg": "#92710A",
  "--pm-accent-hover": "#EAB308",
  "--pm-accent-text": "#111",
};

export function ThemeProvider({ children }: { children: ReactNode }) {
  const [theme, setTheme] = useState<Theme>(localStorage.getItem("theme") === "light" ? "light" : "dark");

  const toggleTheme = () => {
    localStorage.setItem("theme", theme === "dark" ? "light" : "dark");
    setTheme((t) => t === "dark" ? "light" : "dark")
  };

  useEffect(() => {
    const vars = theme === "dark" ? darkVars : lightVars;
    const root = document.documentElement;
    Object.entries(vars).forEach(([key, value]) => {
      root.style.setProperty(key, value);
    });
  }, [theme]);

  return (
    <ThemeContext.Provider value={{ theme, toggleTheme }}>
      {children}
    </ThemeContext.Provider>
  );
}