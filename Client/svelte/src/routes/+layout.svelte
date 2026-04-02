<script lang="ts">
	import './layout.css';
	import favicon from '$lib/assets/favicon.svg';
	import * as Tooltip from '$lib/components/ui/tooltip/index.js';

	let { children } = $props();
</script>

<svelte:head><link rel="icon" href={favicon} /></svelte:head>

<Tooltip.Provider>
	<div class="h-screen w-screen flex flex-col overflow-hidden bg-background text-foreground">
		{@render children()}
	</div>
</Tooltip.Provider>
