<script lang="ts">
	import * as Table from '$lib/components/ui/table/index.js';
	import { ScrollArea } from '$lib/components/ui/scroll-area/index.js';
	import type { Snippet } from 'svelte';

	let {
		headers,
		entries,
		rowKey = 'uuid',
		emptyMessage = 'No data available.',
		maxHeight = '400px',
		rowSnippet,
		onRowClick,
	}: {
		headers: { text: string; value: string; width?: string; align?: string }[];
		entries: any[];
		rowKey?: string;
		emptyMessage?: string;
		maxHeight?: string;
		rowSnippet?: Snippet<[any]>;
		onRowClick?: (row: any) => void;
	} = $props();
</script>

<div class="w-full border rounded-md overflow-hidden">
	<ScrollArea style="max-height: {maxHeight}">
		<Table.Root>
			<Table.Header class="sticky top-0 bg-muted z-10">
				<Table.Row>
					{#each headers as h}
						<Table.Head
							class="text-xs font-semibold uppercase tracking-wider whitespace-nowrap"
							style={h.width ? `width: ${h.width}` : ''}
						>
							{h.text}
						</Table.Head>
					{/each}
				</Table.Row>
			</Table.Header>
			<Table.Body>
				{#if entries.length === 0}
					<Table.Row>
						<Table.Cell colspan={headers.length} class="text-center py-8 text-muted-foreground text-sm">
							{emptyMessage}
						</Table.Cell>
					</Table.Row>
				{:else}
					{#each entries as row, idx (row[rowKey] || idx)}
						{#if rowSnippet}
							{@render rowSnippet(row)}
						{:else}
							<Table.Row
								class={onRowClick ? 'cursor-pointer hover:bg-muted/50 transition-colors' : ''}
								onclick={() => onRowClick?.(row)}
							>
								{#each headers as h}
									<Table.Cell class="text-sm py-2">
										{row[h.value] ?? ''}
									</Table.Cell>
								{/each}
							</Table.Row>
						{/if}
					{/each}
				{/if}
			</Table.Body>
		</Table.Root>
	</ScrollArea>
</div>
