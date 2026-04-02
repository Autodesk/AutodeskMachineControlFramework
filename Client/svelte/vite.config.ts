import tailwindcss from '@tailwindcss/vite';
import { sveltekit } from '@sveltejs/kit/vite';
import { defineConfig } from 'vite';
import path from 'path';

export default defineConfig({
	plugins: [tailwindcss(), sveltekit()],
	resolve: {
		alias: {
			'@core': path.resolve(__dirname, '../core'),
		},
		// @core files (../core) import packages like axios that live in
		// this project's node_modules.  Without dedupe, Vite walks up from
		// Client/core/ and fails to find them.
		dedupe: ['axios', 'luxon', 'three'],
	},
	server: {
		fs: {
			allow: [path.resolve(__dirname, '..')],
		},
	},
});
