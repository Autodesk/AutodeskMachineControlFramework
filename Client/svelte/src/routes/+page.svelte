<script lang="ts">
	import { onMount, onDestroy } from 'svelte';
	import { Button } from '$lib/components/ui/button/index.js';
	import { ScrollArea } from '$lib/components/ui/scroll-area/index.js';
	import * as Sheet from '$lib/components/ui/sheet/index.js';
	import * as Tooltip from '$lib/components/ui/tooltip/index.js';
	import * as Dialog from '$lib/components/ui/dialog/index.js';
	import ModuleFactory from '$lib/amcf/ModuleFactory.svelte';
	import LoginPage from '$lib/amcf/LoginPage.svelte';
	import Snackbar from '$lib/amcf/Snackbar.svelte';
	import MdiIcon from '$lib/amcf/MdiIcon.svelte';

	import Menu from '@lucide/svelte/icons/menu';
	import PanelLeftClose from '@lucide/svelte/icons/panel-left-close';
	import PanelLeftOpen from '@lucide/svelte/icons/panel-left-open';
	import LogOut from '@lucide/svelte/icons/log-out';
	import RefreshCw from '@lucide/svelte/icons/refresh-cw';
	import AlertTriangle from '@lucide/svelte/icons/triangle-alert';
	import Sun from '@lucide/svelte/icons/sun';
	import Moon from '@lucide/svelte/icons/moon';

	// @ts-ignore — core JS has no type declarations yet
	import AMCApplication from '@core/common/AMCApplication.js';
	import { applyTokens, restoreHighContrastPreference, toggleDarkMode, restoreDarkModePreference, isDarkMode } from '@core/theme/themeLoader.js';
	import { initPollTick, type PollTick } from '$lib/amcf/poll.svelte';

	const POLL_INTERVAL_MS = 600;
	const INITIAL_TICK_DELAY_MS = 1200;
	const POST_LOGIN_TICK_DELAY_MS = 800;

	let app: any = $state(null);
	let timer: ReturnType<typeof setInterval> | null = null;
	let drawerOpen = $state(false);
	let sidebarVisible = $state(true);
	let isLargeScreen = $state(false);
	let darkMode = $state(false);

	const poll: PollTick = initPollTick();

	/*
	 * AMCApplication is a plain JS class — Svelte 5 cannot track deep
	 * property mutations on it.  Every derived value must reference poll.v
	 * so it re-evaluates whenever the polling cycle bumps the counter.
	 * Child components access this via usePollTick() context instead of props.
	 */
	let status      = $derived.by(() => { poll.v; return app?.AppState?.currentStatus || 'initial'; });
	let menuItems   = $derived.by(() => { poll.v; return [...(app?.AppContent?.MenuItems || [])]; });
	let toolbarItems= $derived.by(() => { poll.v; return [...(app?.AppContent?.ToolbarItems || [])]; });
	let pages       = $derived.by(() => { poll.v; return [...(app?.AppContent?.Pages || [])]; });
	let dialogs     = $derived.by(() => { poll.v; return [...(app?.AppContent?.Dialogs || [])]; });
	// The active dialog is tracked as a reactive scalar (like activePage) rather than reading the
	// mutated dialogIsActive flag directly: the dialog objects keep a stable identity across poll
	// ticks, so a per-object property mutation would not re-trigger the {#each} item expressions.
	let activeDialogName = $derived.by(() => { poll.v; return (app?.AppContent?.Dialogs || []).find((d: any) => d.dialogIsActive)?.name || ''; });
	let appName     = $derived.by(() => { poll.v; return app?.AppDefinition?.TextApplicationName || 'AMCF'; });
	let copyright   = $derived.by(() => { poll.v; return app?.AppDefinition?.TextCopyRight || ''; });
	let toolbarLogoUUID = $derived.by(() => { poll.v; return app?.AppDefinition?.ToolbarLogoUUID || ''; });
	let activePage  = $derived.by(() => { poll.v; return app?.AppState?.activePage || ''; });
	let currentError = $derived.by(() => { poll.v; return app?.AppState?.currentError || ''; });
	let serverColors = $derived.by(() => { poll.v; return app?.AppDefinition?.Colors || {}; });
	let serverDarkColors = $derived.by(() => { poll.v; return app?.AppDefinition?.DarkColors || {}; });
	let serverDefaultTheme = $derived.by(() => { poll.v; return app?.AppDefinition?.DefaultTheme || 'light'; });

	function bumpTick () { poll.v++; }

	let mql: MediaQueryList | null = null;
	let smMql: MediaQueryList | null = null;
	let isSmallScreen = $state(false);

	function handleScreenChange (e: MediaQueryListEvent | MediaQueryList) {
		isLargeScreen = e.matches;
	}
	function handleSmallScreenChange (e: MediaQueryListEvent | MediaQueryList) {
		isSmallScreen = !e.matches;
	}

	let colorsApplied = false;

	$effect(() => {
		if (serverColors && !colorsApplied && Object.keys(serverColors).length > 0) {
			applyServerColors(serverColors, serverDarkColors);
			colorsApplied = true;

			darkMode = restoreDarkModePreference(serverDefaultTheme);
		}
	});

	function deriveDarkVariant (hex: string): string {
		hex = hex.replace('#', '');
		if (hex.length === 3) hex = hex.split('').map(c => c + c).join('');
		let r = parseInt(hex.substring(0, 2), 16) / 255;
		let g = parseInt(hex.substring(2, 4), 16) / 255;
		let b = parseInt(hex.substring(4, 6), 16) / 255;
		const lightness = (Math.max(r, g, b) + Math.min(r, g, b)) / 2;

		if (lightness < 0.2) {
			r = 0.85 + r * 0.15;
			g = 0.85 + g * 0.15;
			b = 0.85 + b * 0.15;
		} else if (lightness > 0.8) {
			r *= 0.35;
			g *= 0.35;
			b *= 0.35;
		} else {
			const boost = 1.2;
			r = Math.min(1, r * boost);
			g = Math.min(1, g * boost);
			b = Math.min(1, b * boost);
		}

		const toHex = (v: number) => Math.round(v * 255).toString(16).padStart(2, '0');
		return `#${toHex(r)}${toHex(g)}${toHex(b)}`;
	}

	function buildColorCSS (
		selector: string,
		colors: Record<string, string>,
		derive: boolean
	): string {
		const resolve = (key: string) => colors[key] || '';
		const primary = resolve('primary');
		const secondary = resolve('secondary');
		const accent = resolve('accent');
		const error = resolve('error');
		if (!primary && !secondary && !accent && !error) return '';

		let css = `${selector} {\n`;
		if (primary) {
			css += `  --primary: ${primary};\n`;
			css += `  --ring: ${primary};\n`;
			css += `  --sidebar-primary: ${primary};\n`;
			css += `  --sidebar-ring: ${primary};\n`;
			css += `  --chart-1: ${primary};\n`;
		}
		if (secondary) css += `  --secondary: ${secondary};\n`;
		if (accent) {
			css += `  --accent: ${accent};\n`;
			css += `  --sidebar-accent: ${accent};\n`;
		}
		if (error) css += `  --destructive: ${error};\n`;
		else if (primary) css += `  --destructive: ${primary};\n`;
		css += '}\n';
		return css;
	}

	function applyServerColors (
		lightColors: Record<string, string>,
		darkColorsFromServer: Record<string, string>
	) {
		if (!lightColors) return;

		const hasDarkColors = darkColorsFromServer && Object.keys(darkColorsFromServer).length > 0;
		const darkColors: Record<string, string> = hasDarkColors
			? darkColorsFromServer
			: Object.fromEntries(
				Object.entries(lightColors).map(([k, v]) => [k, deriveDarkVariant(v)])
			);

		let styleEl = document.getElementById('amcf-server-colors') as HTMLStyleElement | null;
		if (!styleEl) {
			styleEl = document.createElement('style');
			styleEl.id = 'amcf-server-colors';
			document.head.appendChild(styleEl);
		}
		styleEl.textContent =
			buildColorCSS(':root', lightColors, false) +
			buildColorCSS('.dark', darkColors, false);

		const tokenMap: Record<string, string> = {};
		if (lightColors.primary) tokenMap['--amcf-color-primary'] = lightColors.primary;
		if (lightColors.secondary) tokenMap['--amcf-color-secondary'] = lightColors.secondary;
		if (lightColors.error) tokenMap['--amcf-color-error'] = lightColors.error;
		applyTokens(tokenMap);
	}

	function handleToggleDarkMode () {
		darkMode = toggleDarkMode();
	}

	onMount(() => {
		restoreHighContrastPreference();
		darkMode = restoreDarkModePreference('system');

		mql = window.matchMedia('(min-width: 1024px)');
		isLargeScreen = mql.matches;
		mql.addEventListener('change', handleScreenChange);

		smMql = window.matchMedia('(min-width: 640px)');
		isSmallScreen = !smMql.matches;
		smMql.addEventListener('change', handleSmallScreenChange);

		let baseURL = '/api';
		if (import.meta.env.DEV) {
			baseURL = 'http://localhost:8869/api';
		}

		app = new AMCApplication(baseURL, bumpTick);
		app.retrieveConfiguration(null);

		setTimeout(bumpTick, INITIAL_TICK_DELAY_MS);

		startPolling();
	});

	function startPolling () {
		if (timer) return;
		timer = setInterval(() => {
			if (app && app.AppState.currentStatus === 'ready') {
				app.retrieveFrontendState()
					.finally(() => {
						app.updateModules();
						poll.v++;
					});
			} else if (app) {
				poll.v++;
			}
		}, POLL_INTERVAL_MS);
	}

	onDestroy(() => {
		if (timer) clearInterval(timer);
		if (mql) mql.removeEventListener('change', handleScreenChange);
		if (smMql) smMql.removeEventListener('change', handleSmallScreenChange);
	});

	function menuClick (item: any) {
		if (item.targetpage) app.changePage(item.targetpage);
		if (item.eventname) app.triggerUIEvent(item.eventname, item.uuid, {});
		if (!isLargeScreen) drawerOpen = false;
	}

	function toolbarClick (item: any) {
		if (item.targetpage) app.changePage(item.targetpage);
		if (item.eventname) app.triggerUIEvent(item.eventname, item.uuid, {});
	}

	function toggleSidebar () {
		if (isLargeScreen) {
			sidebarVisible = !sidebarVisible;
		} else {
			drawerOpen = !drawerOpen;
		}
	}

	function reloadPage () {
		if (app) {
			app.performLogout();
			app.setStatus('initial');
			colorsApplied = false;
			app.retrieveConfiguration(null);
			setTimeout(bumpTick, INITIAL_TICK_DELAY_MS);
		}
	}

	function isMenuActive (item: any): boolean {
		if (!item.targetpage) return false;
		return item.targetpage === activePage;
	}

	function toolbarCaption (caption: string): string {
		if (isSmallScreen) return '';
		return caption;
	}

	function setDialogOpen (dialog: any, open: boolean) {
		if (open) {
			app.showDialog(dialog.name);
		} else {
			dialog.dialogIsActive = false;
			bumpTick();
		}
	}
</script>

<!-- Loading -->
{#if status === 'initial'}
	<div class="flex-1 flex items-center justify-center bg-white dark:bg-black">
		<div class="animate-spin h-8 w-8 border-2 border-black dark:border-white border-t-transparent dark:border-t-transparent rounded-full"></div>
	</div>

<!-- Login -->
{:else if status === 'login'}
	<LoginPage {app} onLogin={() => setTimeout(bumpTick, POST_LOGIN_TICK_DELAY_MS)} />

<!-- Main Application -->
{:else if status === 'ready'}
	<!-- Header bar -->
	<header class="h-12 bg-toolbar text-toolbar-foreground flex items-center px-3 gap-2 shrink-0">
		<!-- Hamburger / sidebar toggle -->
		<button class="p-2 hover:bg-toolbar-foreground/10 rounded" onclick={toggleSidebar}>
			{#if isLargeScreen}
				{#if sidebarVisible}
					<PanelLeftClose class="h-5 w-5" />
				{:else}
					<PanelLeftOpen class="h-5 w-5" />
				{/if}
			{:else}
				<Menu class="h-5 w-5" />
			{/if}
		</button>

		<!-- Mobile overlay drawer -->
		{#if !isLargeScreen}
			<Sheet.Root bind:open={drawerOpen}>
				<Sheet.Content side="left" class="w-72 p-0">
					<Sheet.Header class="p-4 border-b">
						<Sheet.Title>{appName}</Sheet.Title>
					</Sheet.Header>
					<ScrollArea class="flex-1">
						<nav class="flex flex-col">
							{#each menuItems as item (item.uuid)}
								{@const active = isMenuActive(item)}
								<button
									class="flex items-start gap-3 px-4 py-3 text-left hover:bg-accent border-b border-border/50 transition-colors
										{active ? 'bg-primary/5 border-l-[3px] border-l-primary' : ''}"
									onclick={() => menuClick(item)}
								>
									<div class="mt-0.5 shrink-0 {active ? 'text-primary' : 'text-muted-foreground'}">
										<MdiIcon icon={item.icon} class="h-5 w-5" />
									</div>
									<div>
										<div class="text-sm font-medium {active ? 'text-primary' : ''}">{item.caption}</div>
										<div class="text-xs text-muted-foreground">{item.description || ''}</div>
									</div>
								</button>
							{/each}
						</nav>
					</ScrollArea>
				</Sheet.Content>
			</Sheet.Root>
		{/if}

		<!-- Toolbar logo / app name -->
		<button
			class="text-sm font-medium px-3 py-1.5 hover:bg-toolbar-foreground/10 rounded"
			onclick={() => app.changePage(app.AppDefinition.MainPage)}
		>
			{#if toolbarLogoUUID}
				<img src={app.getImageURL(toolbarLogoUUID)} alt={appName} class="h-6 max-w-[150px] object-contain" />
			{:else}
				<span>{appName}</span>
			{/if}
		</button>

		<div class="flex-1"></div>

		<Tooltip.Root>
			<Tooltip.Trigger>
				{#snippet child({ props })}
					<button
						{...props}
						class="p-2 hover:bg-toolbar-foreground/10 rounded"
						onclick={handleToggleDarkMode}
					>
						{#if darkMode}
							<Sun class="h-4 w-4" />
						{:else}
							<Moon class="h-4 w-4" />
						{/if}
					</button>
				{/snippet}
			</Tooltip.Trigger>
			<Tooltip.Content>{darkMode ? 'Switch to light mode' : 'Switch to dark mode'}</Tooltip.Content>
		</Tooltip.Root>

		{#each toolbarItems as item (item.uuid)}
			<Tooltip.Root>
				<Tooltip.Trigger>
					{#snippet child({ props })}
						<button
							{...props}
							class="text-sm px-3 py-1.5 hover:bg-toolbar-foreground/10 rounded flex items-center gap-1.5"
							onclick={() => toolbarClick(item)}
						>
							{#if item.icon}
								<MdiIcon icon={item.icon} class="h-4 w-4" />
							{/if}
							{toolbarCaption(item.caption)}
						</button>
					{/snippet}
				</Tooltip.Trigger>
				<Tooltip.Content>{item.caption}</Tooltip.Content>
			</Tooltip.Root>
		{/each}
	</header>

	<!-- Body: permanent sidebar + page content -->
	<div class="flex flex-1 min-h-0">
		<!-- Permanent sidebar (large screens) -->
		{#if isLargeScreen && sidebarVisible}
			<aside class="w-72 border-r border-border bg-card shrink-0 flex flex-col">
				<div class="p-4 border-b font-semibold text-sm">{appName}</div>
				<ScrollArea class="flex-1">
					<nav class="flex flex-col">
						{#each menuItems as item (item.uuid)}
							{@const active = isMenuActive(item)}
							<button
								class="flex items-start gap-3 px-4 py-3 text-left hover:bg-accent border-b border-border/50 transition-colors
									{active ? 'bg-primary/5 border-l-[3px] border-l-primary' : ''}"
								onclick={() => menuClick(item)}
							>
								<div class="mt-0.5 shrink-0 {active ? 'text-primary' : 'text-muted-foreground'}">
									<MdiIcon icon={item.icon} class="h-5 w-5" />
								</div>
								<div>
									<div class="text-sm font-medium {active ? 'text-primary' : ''}">{item.caption}</div>
									<div class="text-xs text-muted-foreground">{item.description || ''}</div>
								</div>
							</button>
						{/each}
					</nav>
				</ScrollArea>
			</aside>
		{/if}

		<!-- Page content -->
		<main class="flex-1 min-w-0 min-h-0 relative">
			{#each pages as page (page.name)}
				{#if poll.v >= 0 && app.pageIsActive(page)}
					<div class="absolute inset-2">
						{#each page.modules || [] as mod (mod.uuid)}
							<ModuleFactory module={mod} {app} />
						{/each}
					</div>
				{/if}
			{/each}
		</main>
	</div>

	<!-- Dialogs -->
	{#each dialogs as dialog (dialog.name)}
		<Dialog.Root
			open={dialog.name === activeDialogName}
			onOpenChange={(open) => setDialogOpen(dialog, open)}
		>
			<Dialog.Content class="sm:max-w-[50vw] max-h-[80vh] overflow-hidden flex flex-col">
				{#if dialog.title}
					<Dialog.Header class="shrink-0">
						<Dialog.Title>{dialog.title}</Dialog.Title>
					</Dialog.Header>
				{/if}
				<div class="flex-1 overflow-auto">
					{#each dialog.modules || [] as mod (mod.uuid)}
						<ModuleFactory module={mod} {app} />
					{/each}
				</div>
			</Dialog.Content>
		</Dialog.Root>
	{/each}

	<!-- Snackbar -->
	<Snackbar {app} />

	<!-- Footer -->
	<footer class="h-8 bg-toolbar text-toolbar-foreground/60 flex items-center justify-end px-4 text-xs shrink-0">
		{#if copyright}
			&copy; {copyright}
		{/if}
	</footer>

<!-- Error -->
{:else if status === 'error'}
	<div class="flex-1 flex items-center justify-center bg-white dark:bg-black">
		<div class="bg-white dark:bg-neutral-900 border border-red-200 dark:border-red-900/50 rounded-xl p-8 text-center max-w-md shadow-lg">
			<div class="flex justify-center mb-4">
				<AlertTriangle class="h-10 w-10 text-red-500 dark:text-red-400" />
			</div>
			<h2 class="text-lg font-semibold text-neutral-900 dark:text-neutral-100 mb-2">Connection Error</h2>
			<p class="text-sm text-neutral-500 dark:text-neutral-400 mb-4">Unable to connect to the server.</p>
			{#if currentError}
				<p class="text-xs text-neutral-600 dark:text-neutral-400 bg-neutral-100 dark:bg-neutral-800 rounded-lg p-3 mb-5 text-left font-mono break-words leading-relaxed">{currentError}</p>
			{/if}
			<Button onclick={reloadPage} variant="default" class="mt-1">
				<RefreshCw class="h-4 w-4 mr-2" /> Retry connection
			</Button>
		</div>
	</div>
{/if}
