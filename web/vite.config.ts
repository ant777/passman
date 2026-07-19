import { defineConfig } from 'vite'
import path from 'path'
import tailwindcss from '@tailwindcss/vite'
import react from '@vitejs/plugin-react'
import {version} from "./src/app/config";
import { insertHtml, h } from 'vite-plugin-insert-html';
import { VitePWA } from 'vite-plugin-pwa'


export default defineConfig({
  plugins: [
    // The React and Tailwind plugins are both required for Make, even if
    // Tailwind is not being actively used – do not remove them
    react(),
    tailwindcss(),
    insertHtml({
      head: [
        h('link', {
          rel: 'manifest',
          href:"/passman/res/site.webmanifest"
        }),
        h('link', {
          rel: 'icon',
          type: "image/png", 
          href:"/passman/res/favicon-96x96.png",
          sizes: "96x96"
        }),
        h('link', {
          rel: 'icon',
          type: "image/svg+xml", 
          href:"/passman/res/favicon.svg"
        }),
        h('link', {
          rel: 'shortcut icon',
          href:"/passman/res/favicon.svg"
        }),
        h('link', {
          rel: 'apple-touch-icon', 
          sizes:"180x180",
          href:"/passman/res/apple-touch-icon.png"
        }),
        h('script', {
          type: 'module', 
          src:"/passman/res/esp-web-tools/install-button.js"
        }),
      ],
      // body: [h('script', { src: 'analytics.js' })],
    }),
    VitePWA({
      manifest: false,
      registerType: 'autoUpdate',
      workbox: {
        globPatterns: ['**/*.{js,css,html,ico,png,svg}', '../res/*.{js,css,html,ico,png,svg,webmanifest}', '../res/esp-web-tools/*.js']
      },
      // scope: '/passman/', // Controls the entire app
    })
  ],
  base: '/passman/' + version + '/',
  build: {
    outDir: '../docs/' + version + '/'
  },
  resolve: {
    alias: {
      // Alias @ to the src directory
      '@': path.resolve(__dirname, './src'),
    },
  },

  // File types to support raw imports. Never add .css, .tsx, or .ts files to this.
  assetsInclude: ['**/*.svg', '**/*.csv'],
})
