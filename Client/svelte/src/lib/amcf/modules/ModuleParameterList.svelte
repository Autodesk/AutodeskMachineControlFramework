<script lang="ts">
	import * as Table from '$lib/components/ui/table/index.js';
	import { ScrollArea } from '$lib/components/ui/scroll-area/index.js';

	import { usePollTick } from '$lib/amcf/poll.svelte';

	let { module, app }: { module: any; app: any } = $props();
	const poll = usePollTick();

	let visible = $derived.by(() => { poll.v; return module.visible !== false; });
	let headers = $derived.by(() => { poll.v; return module.headers || [
		{ text: 'Parameter', value: 'paramDescription' },
		{ text: 'Value',     value: 'paramValue' },
		{ text: 'Group',     value: 'paramGroup' },
		{ text: 'System',    value: 'paramSystem' },
	]; });

	let entries = $derived.by(() => { poll.v; return [...(module.entries || [])]; });
</script>

{#if visible}
	<div class="w-full min-h-0 flex-1 flex flex-col border rounded-md overflow-hidden">
		<ScrollArea class="flex-1 min-h-0">
			<Table.Root>
				<Table.Header class="sticky top-0 bg-muted z-10">
					<Table.Row>
						{#each headers as h}
							<Table.Head class="text-xs font-semibold uppercase tracking-wider whitespace-nowrap">
								{h.text}
							</Table.Head>
						{/each}
					</Table.Row>
				</Table.Header>
				<Table.Body>
					{#if entries.length === 0}
						<Table.Row>
							<Table.Cell colspan={headers.length} class="text-center py-8 text-muted-foreground text-sm">
								{module.loadingtext || 'No parameters available.'}
							</Table.Cell>
						</Table.Row>
					{:else}
						{#each entries as row, idx (idx)}
							<Table.Row class="hover:bg-muted/50 transition-colors">
								{#each headers as h}
									<Table.Cell class="text-sm py-1.5">
										{row[h.value] ?? ''}
									</Table.Cell>
								{/each}
							</Table.Row>
						{/each}
					{/if}
				</Table.Body>
			</Table.Root>
		</ScrollArea>
	</div>
{/if}
