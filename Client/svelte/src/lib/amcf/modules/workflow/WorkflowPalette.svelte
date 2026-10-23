<script lang="ts">
	import type { StepCatalogEntry } from './workflowTypes';
	import { Badge } from '$lib/components/ui/badge';
	import { Separator } from '$lib/components/ui/separator';
	import * as ScrollArea from '$lib/components/ui/scroll-area';

	let {
		catalog,
		onadd,
	}: {
		catalog: StepCatalogEntry[];
		onadd?: (stepRef: string) => void;
	} = $props();

	let functionBlocks = $derived(catalog.filter((c) => c.category === 'functionblock'));
	let templates = $derived(catalog.filter((c) => c.category === 'template'));

	const colorMap: Record<string, { bg: string; border: string; text: string; badge: string }> = {
		blue: {
			bg: 'bg-blue-500/10',
			border: 'border-blue-500/40',
			text: 'text-blue-200',
			badge: 'bg-blue-600 text-white',
		},
		amber: {
			bg: 'bg-amber-500/10',
			border: 'border-amber-500/40',
			text: 'text-amber-200',
			badge: 'bg-amber-600 text-white',
		},
	};
</script>

<div class="flex flex-col h-full">
	<h3 class="text-xs font-semibold uppercase tracking-wider text-muted-foreground/60 px-3 py-2">
		Step Palette
	</h3>
	<ScrollArea.Root class="flex-1">
		<ScrollArea.Viewport class="h-full">
			<div class="flex flex-col gap-1 px-2 pb-2">
				{#if functionBlocks.length > 0}
					<span class="text-[10px] uppercase tracking-wider text-muted-foreground/40 px-1 pt-1">
						Function Blocks
					</span>
					{#each functionBlocks as entry}
						{@const colors = colorMap[entry.color] || colorMap.blue}
						<button
							class="w-full flex items-center gap-2 rounded border px-2 py-1.5 text-left text-sm transition-colors
								{colors.bg} {colors.border} hover:shadow-sm"
							onclick={() => onadd?.(entry.id)}
						>
							<Badge class="{colors.badge} text-[9px] px-1 py-0 leading-tight">FB</Badge>
							<span class="{colors.text} text-xs font-medium">{entry.label}</span>
						</button>
					{/each}
				{/if}

				{#if templates.length > 0}
					<Separator class="my-1" />
					<span class="text-[10px] uppercase tracking-wider text-muted-foreground/40 px-1 pt-1">
						Templates
					</span>
					{#each templates as entry}
						{@const colors = colorMap[entry.color] || colorMap.amber}
						<button
							class="w-full flex items-center gap-2 rounded border px-2 py-1.5 text-left text-sm transition-colors
								{colors.bg} {colors.border} hover:shadow-sm"
							onclick={() => onadd?.(entry.id)}
						>
							<Badge class="{colors.badge} text-[9px] px-1 py-0 leading-tight">TPL</Badge>
							<span class="{colors.text} text-xs font-medium">{entry.label}</span>
						</button>
					{/each}
				{/if}
			</div>
		</ScrollArea.Viewport>
	</ScrollArea.Root>
</div>
