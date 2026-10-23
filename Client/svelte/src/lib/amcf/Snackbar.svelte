<script lang="ts">
	import X from '@lucide/svelte/icons/x';
	import { usePollTick } from '$lib/amcf/poll.svelte';

	let { app }: { app: any } = $props();
	const poll = usePollTick();

	let visible   = $derived.by(() => { poll.v; return app?.SnackBar?.Visible || false; });
	let text      = $derived.by(() => { poll.v; return app?.SnackBar?.Text || ''; });
	let color     = $derived.by(() => { poll.v; return app?.SnackBar?.Color || 'secondary'; });
	let fontColor = $derived.by(() => { poll.v; return app?.SnackBar?.FontColor || 'white'; });
	let timeout   = $derived.by(() => { poll.v; return app?.SnackBar?.Timeout || -1; });

	let show = $state(false);
	let autoHideTimer: ReturnType<typeof setTimeout> | null = null;

	$effect(() => {
		if (visible && text) {
			show = true;
			if (autoHideTimer) clearTimeout(autoHideTimer);
			if (timeout > 0) {
				autoHideTimer = setTimeout(() => {
					dismiss();
				}, timeout);
			}
		} else {
			show = false;
		}
	});

	function dismiss () {
		show = false;
		if (app?.SnackBar) app.SnackBar.Visible = false;
		if (autoHideTimer) { clearTimeout(autoHideTimer); autoHideTimer = null; }
	}

	const colorMap: Record<string, string> = {
		primary:   'bg-primary text-primary-foreground',
		secondary: 'bg-[#3C3C3C] text-white',
		success:   'bg-emerald-600 text-white',
		error:     'bg-destructive text-white',
		warning:   'bg-amber-600 text-white',
		info:      'bg-blue-600 text-white',
	};

	let bgClass = $derived(colorMap[color] || 'bg-[#3C3C3C] text-white');
</script>

{#if show}
	<div class="fixed bottom-6 left-1/2 -translate-x-1/2 z-[9999] animate-in slide-in-from-bottom-4 fade-in duration-300">
		<div class="flex items-center gap-3 px-5 py-3 rounded-lg shadow-lg min-w-[280px] max-w-[560px] {bgClass}">
			<span class="text-sm flex-1">{text}</span>
			<button class="p-1 rounded hover:bg-white/20 transition-colors shrink-0" onclick={dismiss}>
				<X class="h-4 w-4" />
			</button>
		</div>
	</div>
{/if}
