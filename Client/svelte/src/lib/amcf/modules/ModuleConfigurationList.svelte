<script lang="ts">
	import * as Table from '$lib/components/ui/table/index.js';
	import { ScrollArea } from '$lib/components/ui/scroll-area/index.js';
	import { Button } from '$lib/components/ui/button/index.js';
	import { Checkbox } from '$lib/components/ui/checkbox/index.js';
	import Check from '@lucide/svelte/icons/check';
	import Upload from '@lucide/svelte/icons/upload';
	import Trash2 from '@lucide/svelte/icons/trash-2';

	import { usePollTick } from '$lib/amcf/poll.svelte';

	let { module, app }: { module: any; app: any } = $props();
	const poll = usePollTick();

	let visible = $derived.by(() => { poll.v; return module.visible !== false; });
	let entries = $derived.by(() => { poll.v; return [...(module.entries || [])]; });
	let entrybuttons = $derived.by(() => { poll.v; return module.entrybuttons || []; });

	function formatDate (ts: string): string {
		if (!ts) return '';
		try {
			const d = new Date(ts.replace(' UTC', 'Z').replace('Z UTC', 'Z'));
			return d.toLocaleString([], { dateStyle: 'short', timeStyle: 'short' });
		} catch { return ts; }
	}

	function getActions (item: any) {
		if (entrybuttons.length > 0) return entrybuttons;
		return [
			{ uuid: 'load',     caption: 'Load',       selectevent: 'loadconfiguration' },
			{ uuid: 'activate', caption: 'Set Active',  selectevent: 'setactiveconfiguration' },
			{ uuid: 'delete',   caption: 'Delete',      selectevent: 'deleteconfiguration' },
		];
	}

	function triggerAction (action: any, config: any) {
		if (action.selectevent && app) {
			const formvalues: Record<string, string> = {};
			if (module.selectionvalueuuid)
				formvalues[module.selectionvalueuuid] = config.configurationUUID;
			if (module.buttonvalueuuid)
				formvalues[module.buttonvalueuuid] = action.uuid;
			app.triggerUIEvent(action.selectevent, module.uuid, formvalues);
		}
	}
</script>

{#if visible}
	<div class="w-full flex-1 min-h-0 border rounded-md overflow-hidden">
		<ScrollArea class="h-full max-h-[400px]">
			<Table.Root>
				<Table.Header class="sticky top-0 bg-muted z-10">
					<Table.Row>
						<Table.Head class="text-xs font-semibold uppercase tracking-wider w-[70px] text-center">Active</Table.Head>
						<Table.Head class="text-xs font-semibold uppercase tracking-wider w-[80px] text-center">Version</Table.Head>
						<Table.Head class="text-xs font-semibold uppercase tracking-wider w-[140px]">Upload Time</Table.Head>
						<Table.Head class="text-xs font-semibold uppercase tracking-wider w-[100px]">User</Table.Head>
						<Table.Head class="text-xs font-semibold uppercase tracking-wider">Actions</Table.Head>
					</Table.Row>
				</Table.Header>
				<Table.Body>
					{#if entries.length === 0}
						<Table.Row>
							<Table.Cell colspan={5} class="text-center py-8 text-muted-foreground text-sm">
								{module.loadingtext || 'No configurations available.'}
							</Table.Cell>
						</Table.Row>
					{:else}
						{#each entries as config, idx (config.configurationUUID || idx)}
							<Table.Row class="hover:bg-muted/50 transition-colors">
								<Table.Cell class="text-center py-1.5">
									{#if config.configurationActive}
										<Check class="h-4 w-4 text-green-600 inline-block" />
									{/if}
								</Table.Cell>
								<Table.Cell class="text-sm text-center py-1.5 tabular-nums">{config.configurationVersion}</Table.Cell>
								<Table.Cell class="text-xs text-muted-foreground py-1.5">{formatDate(config.configurationTimestamp)}</Table.Cell>
								<Table.Cell class="text-sm py-1.5">{config.userName}</Table.Cell>
								<Table.Cell class="py-1.5">
									<div class="flex gap-1">
										{#each getActions(config) as action (action.uuid)}
											<Button
												variant="outline"
												size="sm"
												class="h-7 text-xs"
												onclick={() => triggerAction(action, config)}
											>
												{action.caption}
											</Button>
										{/each}
									</div>
								</Table.Cell>
							</Table.Row>
						{/each}
					{/if}
				</Table.Body>
			</Table.Root>
		</ScrollArea>
	</div>
{/if}
