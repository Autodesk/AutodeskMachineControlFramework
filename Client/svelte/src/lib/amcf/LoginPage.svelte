<script lang="ts">
	import { onMount, tick } from 'svelte';
	import LogIn from '@lucide/svelte/icons/log-in';
	import User from '@lucide/svelte/icons/user';
	import Lock from '@lucide/svelte/icons/lock';
	import Sun from '@lucide/svelte/icons/sun';
	import Moon from '@lucide/svelte/icons/moon';

	import { usePollTick } from '$lib/amcf/poll.svelte';
	import { toggleDarkMode, isDarkMode } from '@core/theme/themeLoader.js';

	let { app, onLogin }: { app: any; onLogin: () => void } = $props();
	const poll = usePollTick();

	let loginUser = $state('');
	let loginPassword = $state('');
	let darkMode = $state(false);

	let loginStyle   = $derived.by(() => { poll.v; return (app?.AppDefinition?.LoginStyle || 'classic').toLowerCase(); });
	let appName      = $derived.by(() => { poll.v; return app?.AppDefinition?.TextApplicationName || 'AMCF'; });
	let welcomeMsg   = $derived.by(() => { poll.v; return app?.AppDefinition?.LoginWelcomeMessage || ''; });
	let subtitle     = $derived.by(() => { poll.v; return app?.AppDefinition?.LoginSubtitle || ''; });
	let logoUUID     = $derived.by(() => { poll.v; return app?.AppDefinition?.LogoUUID || ''; });
	let logoAspect   = $derived.by(() => { poll.v; return app?.AppDefinition?.LogoAspectRatio || 3; });
	let panelUUID    = $derived.by(() => { poll.v; return app?.AppDefinition?.LoginPanelUUID || ''; });
	let bgUUID       = $derived.by(() => { poll.v; return app?.AppDefinition?.LoginBackgroundImageUUID || ''; });
	let primaryColor = $derived.by(() => { poll.v; return app?.AppDefinition?.Colors?.primary || '#1a1a1a'; });
	let accentColor  = $derived.by(() => { poll.v; return app?.AppDefinition?.Colors?.accent || ''; });

	let logoURL     = $derived(logoUUID && app ? app.getImageURL(logoUUID) : '');
	let panelURL    = $derived(panelUUID && app ? app.getImageURL(panelUUID) : '');
	let bgURL       = $derived(bgUUID && app ? app.getImageURL(bgUUID) : '');

	function hexLightness (hex: string): number {
		hex = hex.replace('#', '');
		if (hex.length === 3) hex = hex.split('').map(c => c + c).join('');
		const r = parseInt(hex.substring(0, 2), 16) / 255;
		const g = parseInt(hex.substring(2, 4), 16) / 255;
		const b = parseInt(hex.substring(4, 6), 16) / 255;
		return (Math.max(r, g, b) + Math.min(r, g, b)) / 2;
	}

	let isDarkPrimary = $derived(hexLightness(primaryColor) < 0.25);

	let ctaColor = $derived(
		isDarkPrimary && accentColor && accentColor !== primaryColor
			? accentColor : primaryColor
	);
	let ctaTextDark = $derived(hexLightness(ctaColor) > 0.55);

	let bgStyle = $derived.by(() => {
		if (bgURL) return `background: url("${bgURL}") center/cover no-repeat;`;
		return 'background: linear-gradient(160deg, #1e1e24 0%, #141418 50%, #0c0c0f 100%);';
	});

	function autofocus (node: HTMLElement) { tick().then(() => node.focus()); }

	onMount(() => {
		darkMode = isDarkMode();
		tick().then(() => {
			const el = document.querySelector<HTMLInputElement>('[data-login-user], [data-login-first]');
			el?.focus();
		});
	});

	function doLogin () {
		if (loginUser && loginPassword && app) {
			app.requestLogin(loginUser, loginPassword);
			loginPassword = '';
			onLogin();
		}
	}

	function onEnter () { if (loginUser && loginPassword) doLogin(); }

	function handleToggleTheme () {
		darkMode = toggleDarkMode();
	}
</script>

<!-- Theme toggle (all styles) -->
<button
	class="fixed top-4 right-4 z-50 p-2.5 rounded-full bg-black/5 dark:bg-white/10 backdrop-blur-sm border border-black/10 dark:border-white/20 text-gray-600 dark:text-white/70 hover:text-gray-900 dark:hover:text-white hover:bg-black/10 dark:hover:bg-white/20 transition shadow-sm"
	onclick={handleToggleTheme}
	title={darkMode ? 'Switch to light mode' : 'Switch to dark mode'}
>
	{#if darkMode}
		<Sun class="h-4 w-4" />
	{:else}
		<Moon class="h-4 w-4" />
	{/if}
</button>

<!-- ========== CLASSIC ========== -->
{#if loginStyle === 'classic' || !['industrial', 'luminous', 'split'].includes(loginStyle)}
	<div class="flex-1 flex items-center justify-center" style={bgStyle}>
		<div class="w-full max-w-[400px] mx-4">
			<div class="rounded-xl bg-white dark:bg-zinc-900 shadow-2xl shadow-black/40 overflow-hidden">
				<div class="h-[3px]" style="background: {ctaColor}"></div>
				<div class="px-8 pt-8 pb-9">
					<div class="text-center mb-6">
						{#if logoURL}
							<img src={logoURL} alt="Logo" class="max-h-14 mx-auto mb-4 object-contain" style="aspect-ratio: {logoAspect}" />
						{/if}
						<h1 class="text-xl font-semibold text-gray-900 dark:text-gray-100">{appName}</h1>
						{#if subtitle}<p class="text-sm text-gray-500 dark:text-gray-400 mt-0.5">{subtitle}</p>{/if}
						{#if welcomeMsg}<p class="text-xs text-gray-400 dark:text-gray-500 mt-2 leading-relaxed max-w-[280px] mx-auto">{welcomeMsg}</p>{/if}
					</div>
					<div class="flex flex-col gap-3">
						<div class="relative">
							<User class="absolute left-3 top-1/2 -translate-y-1/2 h-4 w-4 text-gray-400 dark:text-gray-500 pointer-events-none" />
							<input data-login-user type="text" placeholder="User name" bind:value={loginUser} use:autofocus
								class="h-10 w-full rounded-md border border-gray-300 dark:border-zinc-700 bg-white dark:bg-zinc-800 pl-10 pr-3 text-sm text-gray-900 dark:text-gray-100 placeholder:text-gray-400 dark:placeholder:text-gray-500 outline-none transition focus:border-gray-900 dark:focus:border-gray-400 focus:ring-1 focus:ring-gray-900 dark:focus:ring-gray-400"
								onkeydown={(e) => e.key === 'Enter' && onEnter()} />
						</div>
						<div class="relative">
							<Lock class="absolute left-3 top-1/2 -translate-y-1/2 h-4 w-4 text-gray-400 dark:text-gray-500 pointer-events-none" />
							<input data-login-password type="password" placeholder="Password" bind:value={loginPassword}
								class="h-10 w-full rounded-md border border-gray-300 dark:border-zinc-700 bg-white dark:bg-zinc-800 pl-10 pr-3 text-sm text-gray-900 dark:text-gray-100 placeholder:text-gray-400 dark:placeholder:text-gray-500 outline-none transition focus:border-gray-900 dark:focus:border-gray-400 focus:ring-1 focus:ring-gray-900 dark:focus:ring-gray-400"
								onkeydown={(e) => e.key === 'Enter' && onEnter()} />
						</div>
						<button onclick={doLogin} disabled={!loginUser || !loginPassword}
							class="h-10 w-full rounded-md text-sm font-medium transition mt-1 disabled:opacity-30 disabled:cursor-not-allowed hover:brightness-110 active:scale-[0.99] flex items-center justify-center gap-2"
							style="background:{ctaColor}; color:{ctaTextDark ? '#000' : '#fff'}">
							<LogIn class="h-4 w-4" /> Sign In
						</button>
					</div>
				</div>
			</div>
			{#if app?.AppDefinition?.TextCopyRight}
				<p class="text-center text-[11px] text-white/35 mt-4">{app.AppDefinition.TextCopyRight}</p>
			{/if}
		</div>
	</div>

<!-- ========== INDUSTRIAL ========== -->
{:else if loginStyle === 'industrial'}
	<div class="flex-1 flex items-center justify-center" style={bgStyle}>
		<div class="w-full max-w-[420px] mx-4">
			<div class="rounded-xl bg-[rgba(255,255,255,0.07)] backdrop-blur-md border border-white/10 shadow-2xl shadow-black/50 overflow-hidden">
				<div class="h-[2px] opacity-80" style="background:linear-gradient(90deg,transparent,{ctaColor},transparent)"></div>
				<div class="px-10 pt-10 pb-11">
					<div class="text-center mb-8">
						{#if logoURL}
							<img src={logoURL} alt="Logo" class="max-h-12 mx-auto mb-5 object-contain" />
						{/if}
						<h1 class="text-2xl font-semibold tracking-wide text-white">{appName}</h1>
						{#if subtitle}<p class="text-[13px] text-white/55 mt-1">{subtitle}</p>{/if}
						{#if welcomeMsg}<p class="text-xs text-white/35 mt-3 leading-relaxed max-w-[300px] mx-auto">{welcomeMsg}</p>{/if}
					</div>
					<div class="flex flex-col gap-4">
						<div class="relative">
							<User class="absolute left-3.5 top-1/2 -translate-y-1/2 h-4 w-4 text-white/35 pointer-events-none" />
							<input data-login-user type="text" placeholder="User name" bind:value={loginUser} use:autofocus
								class="h-11 w-full rounded-md border border-white/15 bg-white/[0.05] pl-11 pr-4 text-sm text-white placeholder:text-white/30 outline-none transition focus:border-white/30 focus:bg-white/[0.08]"
								onkeydown={(e) => e.key === 'Enter' && onEnter()} />
						</div>
						<div class="relative">
							<Lock class="absolute left-3.5 top-1/2 -translate-y-1/2 h-4 w-4 text-white/35 pointer-events-none" />
							<input data-login-password type="password" placeholder="Password" bind:value={loginPassword}
								class="h-11 w-full rounded-md border border-white/15 bg-white/[0.05] pl-11 pr-4 text-sm text-white placeholder:text-white/30 outline-none transition focus:border-white/30 focus:bg-white/[0.08]"
								onkeydown={(e) => e.key === 'Enter' && onEnter()} />
						</div>
						<button onclick={doLogin} disabled={!loginUser || !loginPassword}
							class="h-11 w-full rounded-md text-sm font-semibold tracking-wide transition mt-2 disabled:opacity-25 disabled:cursor-not-allowed hover:brightness-110 active:scale-[0.99] flex items-center justify-center gap-2"
							style="background:{ctaColor}; color:{ctaTextDark ? '#000' : '#fff'}">
							<LogIn class="h-4 w-4" /> Sign In
						</button>
					</div>
				</div>
			</div>
			{#if app?.AppDefinition?.TextCopyRight}
				<p class="text-center text-[11px] text-white/25 mt-5">{app.AppDefinition.TextCopyRight}</p>
			{/if}
		</div>
	</div>

<!-- ========== LUMINOUS ========== -->
{:else if loginStyle === 'luminous'}
	<div class="flex-1 flex items-center justify-center" style={bgStyle}>
		<div class="w-full max-w-[420px] mx-4">
			<div class="rounded-xl bg-white/[0.82] dark:bg-white/[0.08] backdrop-blur-md border border-white/50 dark:border-white/10 shadow-2xl shadow-black/25 overflow-hidden">
				<div class="h-[3px]" style="background:{ctaColor}"></div>
				<div class="px-10 pt-10 pb-11">
					<div class="text-center mb-8">
						{#if logoURL}
							<img src={logoURL} alt="Logo" class="max-h-12 mx-auto mb-5 object-contain" />
						{/if}
						<h1 class="text-2xl font-semibold tracking-wide text-gray-900 dark:text-white">{appName}</h1>
						{#if subtitle}<p class="text-[13px] text-gray-500 dark:text-white/55 mt-1">{subtitle}</p>{/if}
						{#if welcomeMsg}<p class="text-xs text-gray-400 dark:text-white/35 mt-3 leading-relaxed max-w-[300px] mx-auto">{welcomeMsg}</p>{/if}
					</div>
					<div class="flex flex-col gap-4">
						<div class="relative">
							<User class="absolute left-3.5 top-1/2 -translate-y-1/2 h-4 w-4 text-gray-400 dark:text-white/35 pointer-events-none" />
							<input data-login-first type="text" placeholder="User name" bind:value={loginUser} use:autofocus
								class="h-11 w-full rounded-md border border-gray-300 dark:border-white/15 bg-white/70 dark:bg-white/[0.05] pl-11 pr-4 text-sm text-gray-900 dark:text-white placeholder:text-gray-400 dark:placeholder:text-white/30 outline-none transition focus:border-gray-900 dark:focus:border-white/30 focus:ring-1 focus:ring-gray-900 dark:focus:ring-white/30 focus:bg-white dark:focus:bg-white/[0.08]"
								onkeydown={(e) => e.key === 'Enter' && onEnter()} />
						</div>
						<div class="relative">
							<Lock class="absolute left-3.5 top-1/2 -translate-y-1/2 h-4 w-4 text-gray-400 dark:text-white/35 pointer-events-none" />
							<input type="password" placeholder="Password" bind:value={loginPassword}
								class="h-11 w-full rounded-md border border-gray-300 dark:border-white/15 bg-white/70 dark:bg-white/[0.05] pl-11 pr-4 text-sm text-gray-900 dark:text-white placeholder:text-gray-400 dark:placeholder:text-white/30 outline-none transition focus:border-gray-900 dark:focus:border-white/30 focus:ring-1 focus:ring-gray-900 dark:focus:ring-white/30 focus:bg-white dark:focus:bg-white/[0.08]"
								onkeydown={(e) => e.key === 'Enter' && onEnter()} />
						</div>
						<button onclick={doLogin} disabled={!loginUser || !loginPassword}
							class="h-11 w-full rounded-md text-sm font-semibold tracking-wide transition mt-2 disabled:opacity-30 disabled:cursor-not-allowed hover:brightness-110 active:scale-[0.99] flex items-center justify-center gap-2"
							style="background:{ctaColor}; color:{ctaTextDark ? '#000' : '#fff'}">
							<LogIn class="h-4 w-4" /> Sign In
						</button>
					</div>
				</div>
			</div>
			{#if app?.AppDefinition?.TextCopyRight}
				<p class="text-center text-[11px] text-white/30 mt-5">{app.AppDefinition.TextCopyRight}</p>
			{/if}
		</div>
	</div>

<!-- ========== SPLIT ========== -->
{:else if loginStyle === 'split'}
	<div class="flex-1 flex flex-row h-full" style={bgStyle}>
		<div class="hidden md:flex w-5/12 items-center justify-center text-white relative overflow-hidden"
			style="background:{primaryColor}">
			<div class="text-center px-10 max-w-[420px]">
				{#if logoURL}
					<img src={logoURL} alt="Logo" class="max-h-14 mx-auto mb-6 object-contain" />
				{/if}
				<h1 class="text-3xl font-bold tracking-wide mb-2">{appName}</h1>
				{#if subtitle}<p class="text-lg opacity-85 mb-1">{subtitle}</p>{/if}
				{#if welcomeMsg}<p class="text-sm opacity-70 leading-relaxed">{welcomeMsg}</p>{/if}
				{#if panelURL}
					<img src={panelURL} alt="" class="mt-8 max-h-48 mx-auto object-contain opacity-90" />
				{/if}
			</div>
		</div>
		<div class="flex-1 flex items-center justify-center bg-white dark:bg-zinc-900">
			<div class="w-full max-w-[380px] px-8 py-12 text-center">
				{#if logoURL}
					<img src={logoURL} alt="Logo" class="max-h-12 mx-auto mb-4 object-contain md:hidden" />
				{/if}
				<h1 class="text-2xl font-semibold text-gray-900 dark:text-gray-100">Sign In</h1>
				<p class="text-sm text-gray-500 dark:text-gray-400 mt-1 mb-6">Enter your credentials to continue</p>
				<div class="flex flex-col gap-4">
					<div class="relative">
						<User class="absolute left-3.5 top-1/2 -translate-y-1/2 h-4 w-4 text-gray-400 dark:text-gray-500 pointer-events-none" />
						<input data-login-first type="text" placeholder="User name" bind:value={loginUser} use:autofocus
							class="h-11 w-full rounded-md border border-gray-300 dark:border-zinc-700 bg-white dark:bg-zinc-800 pl-11 pr-4 text-sm text-gray-900 dark:text-gray-100 placeholder:text-gray-400 dark:placeholder:text-gray-500 outline-none transition focus:border-gray-900 dark:focus:border-gray-400 focus:ring-1 focus:ring-gray-900 dark:focus:ring-gray-400"
							onkeydown={(e) => e.key === 'Enter' && onEnter()} />
					</div>
					<div class="relative">
						<Lock class="absolute left-3.5 top-1/2 -translate-y-1/2 h-4 w-4 text-gray-400 dark:text-gray-500 pointer-events-none" />
						<input type="password" placeholder="Password" bind:value={loginPassword}
							class="h-11 w-full rounded-md border border-gray-300 dark:border-zinc-700 bg-white dark:bg-zinc-800 pl-11 pr-4 text-sm text-gray-900 dark:text-gray-100 placeholder:text-gray-400 dark:placeholder:text-gray-500 outline-none transition focus:border-gray-900 dark:focus:border-gray-400 focus:ring-1 focus:ring-gray-900 dark:focus:ring-gray-400"
							onkeydown={(e) => e.key === 'Enter' && onEnter()} />
					</div>
					<button onclick={doLogin} disabled={!loginUser || !loginPassword}
						class="h-11 w-full rounded-md text-sm font-semibold tracking-wide transition mt-2 disabled:opacity-30 disabled:cursor-not-allowed hover:brightness-110 active:scale-[0.99] flex items-center justify-center gap-2"
						style="background:{ctaColor}; color:{ctaTextDark ? '#000' : '#fff'}">
						<LogIn class="h-4 w-4" /> Sign In
					</button>
				</div>
			</div>
		</div>
	</div>
{/if}
