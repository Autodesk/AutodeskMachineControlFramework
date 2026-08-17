<script lang="ts">
	import * as Table from '$lib/components/ui/table/index.js';
	import { ScrollArea } from '$lib/components/ui/scroll-area/index.js';
	import Check from '@lucide/svelte/icons/check';
	import X from '@lucide/svelte/icons/x';

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
	let editEvent = $derived.by(() => { poll.v; return module.editevent || ''; });

	// Only one row is edited at a time. editValue is a local buffer so live polling
	// never clobbers what the user is typing.
	let editingKey = $state<string | null>(null);
	let editValue = $state('');

	function rowKey(row: any): string {
		return `${row.paramInstance ?? ''}.${row.paramGroupName ?? ''}.${row.paramName ?? ''}`;
	}
	function isEditable(row: any): boolean {
		return !!editEvent && (row.paramEditable === true || row.paramEditable === 'true');
	}
	function isNumeric(row: any): boolean {
		return row.paramType === 'integer' || row.paramType === 'double';
	}
	function startEdit(row: any) {
		editingKey = rowKey(row);
		editValue = String(row.paramValue ?? '');
	}
	function cancelEdit() {
		editingKey = null;
	}
	function acceptEdit(row: any) {
		const params: Record<string, string> = {
			instance: row.paramInstance ?? '',
			group: row.paramGroupName ?? '',
			parameter: row.paramName ?? '',
			value: String(editValue ?? ''),
			type: row.paramType ?? '',
		};
		if (row.paramMin) params.min = String(row.paramMin);
		if (row.paramMax) params.max = String(row.paramMax);
		app.triggerUIEvent(editEvent, module.uuid, {}, undefined, params);
		editingKey = null;
	}
	function onKeydown(e: KeyboardEvent, row: any) {
		if (e.key === 'Enter') { e.preventDefault(); acceptEdit(row); }
		else if (e.key === 'Escape') { e.preventDefault(); cancelEdit(); }
	}
	function autofocus(node: HTMLInputElement) {
		node.focus();
		node.select();
	}
</script>

{#if visible}
	<div class="w-full min-h-0 flex-1 flex flex-col border rounded-md overflow-hidden">
		<ScrollArea class="flex-1 min-h-0">
			<Table.Root>
				<Table.Header class="sticky top-0 bg-muted z-10">
					<Table.Row>
						{#each headers as h (h.value)}
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
								{#each headers as h (h.value)}
									<Table.Cell class="text-sm py-1.5">
										{#if h.value === 'paramValue' && isEditable(row)}
											{#if editingKey === rowKey(row)}
												<div class="flex items-center gap-1">
													<input
														class="w-24 px-2 py-1 text-sm border rounded bg-background"
														type={isNumeric(row) ? 'number' : 'text'}
														min={row.paramMin || undefined}
														max={row.paramMax || undefined}
														step={row.paramStep || undefined}
														bind:value={editValue}
														onkeydown={(e) => onKeydown(e, row)}
														{@attach autofocus}
													/>
													<button
														class="text-green-600 hover:text-green-700"
														aria-label="Accept"
														onclick={() => acceptEdit(row)}
													>
														<Check class="size-4" />
													</button>
													<button
														class="text-red-600 hover:text-red-700"
														aria-label="Cancel"
														onclick={cancelEdit}
													>
														<X class="size-4" />
													</button>
												</div>
											{:else}
												<button
													class="text-left cursor-pointer hover:underline decoration-dotted underline-offset-4"
													title="Click to edit"
													onclick={() => startEdit(row)}
												>
													{row[h.value] ?? ''}
												</button>
											{/if}
										{:else}
											{row[h.value] ?? ''}
										{/if}
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
