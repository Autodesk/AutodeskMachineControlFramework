<script lang="ts">
	import { Button } from '$lib/components/ui/button/index.js';

	export type RangeMode = 'live' | 'full' | 'custom';

	let {
		mode = 'live',
		liveWindowSeconds = 60,
		onSelectMode,
		onSelectLiveWindow
	}: {
		mode: RangeMode;
		liveWindowSeconds: number;
		onSelectMode: (mode: RangeMode) => void;
		onSelectLiveWindow: (seconds: number) => void;
	} = $props();

	const liveWindows = [
		{ label: '30s', seconds: 30 },
		{ label: '1m', seconds: 60 },
		{ label: '5m', seconds: 300 },
		{ label: '15m', seconds: 900 }
	];
</script>

<div class="flex flex-wrap items-center gap-2">
	<div class="flex items-center gap-1">
		<Button
			size="sm"
			variant={mode === 'live' ? 'default' : 'outline'}
			onclick={() => onSelectMode('live')}
		>
			Live
		</Button>
		<Button
			size="sm"
			variant={mode === 'full' ? 'default' : 'outline'}
			onclick={() => onSelectMode('full')}
		>
			Full session
		</Button>
		{#if mode === 'custom'}
			<Button size="sm" variant="secondary" disabled>Custom (zoomed)</Button>
		{/if}
	</div>

	{#if mode === 'live'}
		<div class="flex items-center gap-1 pl-2 border-l">
			{#each liveWindows as w (w.seconds)}
				<Button
					size="sm"
					variant={liveWindowSeconds === w.seconds ? 'secondary' : 'ghost'}
					onclick={() => onSelectLiveWindow(w.seconds)}
				>
					{w.label}
				</Button>
			{/each}
		</div>
	{/if}
</div>
