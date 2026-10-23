<script lang="ts">
	import { ScrollArea } from '$lib/components/ui/scroll-area/index.js';
	import Search from '@lucide/svelte/icons/search';
	import type { JournalVariable } from '$lib/amcf/journalApi';

	let {
		variables = [],
		selected = [],
		colorFor = undefined,
		onToggle
	}: {
		variables: JournalVariable[];
		selected: string[];
		colorFor?: (name: string) => string;
		onToggle: (name: string) => void;
	} = $props();

	let filter = $state('');

	let filtered = $derived.by(() => {
		const f = filter.trim().toLowerCase();
		if (!f) return variables;
		return variables.filter((v) => v.name.toLowerCase().includes(f));
	});

	function isSelected (name: string): boolean {
		return selected.includes(name);
	}
</script>

<div class="flex flex-col h-full min-h-0 border rounded-md overflow-hidden bg-card">
	<div class="p-2 border-b flex items-center gap-2">
		<Search class="size-4 text-muted-foreground shrink-0" />
		<input
			class="w-full bg-transparent text-sm outline-none"
			placeholder="Filter variables…"
			bind:value={filter}
		/>
	</div>
	<ScrollArea class="flex-1 min-h-0">
		<ul class="p-1">
			{#if filtered.length === 0}
				<li class="px-2 py-4 text-center text-xs text-muted-foreground">No variables found.</li>
			{:else}
				{#each filtered as v (v.name)}
					<li>
						<button
							type="button"
							class="w-full flex items-center gap-2 px-2 py-1.5 rounded text-left text-sm hover:bg-muted/60 transition-colors"
							class:bg-muted={isSelected(v.name)}
							onclick={() => onToggle(v.name)}
						>
							<span
								class="size-3 rounded-sm border shrink-0"
								style={isSelected(v.name) && colorFor ? `background:${colorFor(v.name)}; border-color:${colorFor(v.name)};` : ''}
							></span>
							<span class="flex-1 truncate" title={v.name}>{v.name}</span>
							<span class="text-[10px] uppercase tracking-wide text-muted-foreground shrink-0">{v.type}</span>
						</button>
					</li>
				{/each}
			{/if}
		</ul>
	</ScrollArea>
	<div class="px-2 py-1 border-t text-[11px] text-muted-foreground">
		{selected.length} selected · {variables.length} recorded
	</div>
</div>
