<script lang="ts">
	import * as Table from '$lib/components/ui/table/index.js';
	import * as Dialog from '$lib/components/ui/dialog/index.js';
	import { ScrollArea } from '$lib/components/ui/scroll-area/index.js';
	import { Button } from '$lib/components/ui/button/index.js';
	import { Badge } from '$lib/components/ui/badge/index.js';
	import Image from '@lucide/svelte/icons/image';

	import { usePollTick } from '$lib/amcf/poll.svelte';

	let { module, app }: { module: any; app: any } = $props();
	const poll = usePollTick();

	let visible = $derived.by(() => { poll.v; return module.visible !== false; });
	let entries = $derived.by(() => { poll.v; return [...(module.entries || [])]; });
	let entrybuttons = $derived.by(() => { poll.v; return [...(module.entrybuttons || [])]; });
	let loaded = $derived.by(() => { poll.v; return module.loaded === true; });
	let detailItem: any = $state(null);

	const nullUUID = '00000000-0000-0000-0000-000000000000';

	function thumbnailURL (uuid: string): string {
		if (!uuid || uuid === nullUUID || !app) return '';
		return app.getImageURL(uuid);
	}

	function formatDate (ts: string): string {
		if (!ts) return '';
		try {
			const d = new Date(ts.replace(' UTC', 'Z').replace('Z UTC', 'Z'));
			return d.toLocaleString([], { dateStyle: 'short', timeStyle: 'short' });
		} catch { return ts; }
	}

	function formatDuration (seconds: number): string {
		if (!seconds || seconds <= 0) return '-';
		const h = Math.floor(seconds / 3600);
		const m = Math.floor((seconds % 3600) / 60);
		const s = Math.floor(seconds % 60);
		if (h > 0) return `${h}h ${m}m ${s}s`;
		if (m > 0) return `${m}m ${s}s`;
		return `${s}s`;
	}

	function statusVariant (status: string): 'default' | 'destructive' | 'secondary' | 'outline' {
		const s = (status || '').toLowerCase();
		if (s === 'running' || s === 'inprocess') return 'default';
		if (s === 'failed' || s === 'error') return 'destructive';
		return 'secondary';
	}

	function selectExecution (exec: any) {
		if (module.selectevent && app) {
			const formvalues: Record<string, string> = {};
			if (module.selectionvalueuuid)
				formvalues[module.selectionvalueuuid] = exec.executionUUID;
			app.triggerUIEvent(module.selectevent, module.uuid, formvalues);
		}
	}

	function triggerButton (btn: any, exec: any) {
		if (btn.selectevent && app) {
			const formvalues: Record<string, string> = {};
			if (module.buttonvalueuuid)
				formvalues[module.buttonvalueuuid] = exec.executionUUID;
			app.triggerUIEvent(btn.selectevent, module.uuid, formvalues);
		}
	}
</script>

{#if visible}
	<div class="w-full border rounded-md overflow-hidden">
		<ScrollArea class="max-h-[500px]">
			<Table.Root>
				<Table.Header class="sticky top-0 bg-muted z-10">
					<Table.Row>
						<Table.Head class="text-xs font-semibold uppercase tracking-wider w-[80px]"></Table.Head>
						<Table.Head class="text-xs font-semibold uppercase tracking-wider">Execution</Table.Head>
						<Table.Head class="text-xs font-semibold uppercase tracking-wider w-[140px]">Started</Table.Head>
						<Table.Head class="text-xs font-semibold uppercase tracking-wider w-[100px]">Status</Table.Head>
						{#if entrybuttons.length > 0}
							<Table.Head class="text-xs font-semibold uppercase tracking-wider w-[140px]">Actions</Table.Head>
						{/if}
					</Table.Row>
				</Table.Header>
				<Table.Body>
					{#if entries.length === 0}
						<Table.Row>
							<Table.Cell colspan={entrybuttons.length > 0 ? 5 : 4} class="text-center py-8 text-muted-foreground text-sm">
								{loaded ? 'No executions available.' : (module.loadingtext || 'Loading…')}
							</Table.Cell>
						</Table.Row>
					{:else}
						{#each entries as exec, idx (exec.executionUUID || idx)}
							<Table.Row
								class="hover:bg-muted/50 transition-colors cursor-pointer"
								onclick={() => { detailItem = exec; }}
							>
								<Table.Cell class="py-1.5">
									{#if thumbnailURL(exec.executionThumbnail)}
										<img src={thumbnailURL(exec.executionThumbnail)} alt="" class="h-12 w-16 object-cover rounded border" />
									{:else}
										<div class="h-12 w-16 bg-muted rounded border flex items-center justify-center">
											<Image class="h-5 w-5 text-muted-foreground" />
										</div>
									{/if}
								</Table.Cell>
								<Table.Cell class="text-sm py-1.5">
									<div class="font-medium">{exec.executionName}</div>
									{#if exec.executionDescription}
										<div class="text-xs text-muted-foreground">{exec.executionDescription}</div>
									{/if}
								</Table.Cell>
								<Table.Cell class="text-xs text-muted-foreground py-1.5">{formatDate(exec.executionStartTimestamp)}</Table.Cell>
								<Table.Cell class="py-1.5">
									<Badge variant={statusVariant(exec.executionStatus)}>{exec.executionStatus || 'unknown'}</Badge>
								</Table.Cell>
								{#if entrybuttons.length > 0}
									<Table.Cell class="py-1.5">
										<div class="flex gap-1">
											{#each entrybuttons as btn (btn.uuid)}
												<Button
													variant="outline"
													size="sm"
													class="h-7 text-xs"
													onclick={(e: MouseEvent) => { e.stopPropagation(); triggerButton(btn, exec); }}
												>
													{btn.caption}
												</Button>
											{/each}
										</div>
									</Table.Cell>
								{/if}
							</Table.Row>
						{/each}
					{/if}
				</Table.Body>
			</Table.Root>
		</ScrollArea>
	</div>

	<!-- Detail dialog -->
	<Dialog.Root open={!!detailItem} onOpenChange={(v) => { if (!v) detailItem = null; }}>
		<Dialog.Content class="sm:max-w-[500px]">
			<Dialog.Header>
				<Dialog.Title>{detailItem?.executionName || 'Execution Details'}</Dialog.Title>
				<Dialog.Description>Execution details and history</Dialog.Description>
			</Dialog.Header>
			{#if detailItem}
				<div class="grid grid-cols-[100px_1fr] gap-y-2 gap-x-4 text-sm mt-2">
					{#if thumbnailURL(detailItem.executionThumbnail)}
						<div class="col-span-2 flex justify-center mb-2">
							<img src={thumbnailURL(detailItem.executionThumbnail)} alt="" class="max-h-40 object-contain rounded border" />
						</div>
					{/if}
					<span class="text-muted-foreground">Name</span><span>{detailItem.executionName}</span>
					<span class="text-muted-foreground">Status</span><span><Badge variant={statusVariant(detailItem.executionStatus)}>{detailItem.executionStatus}</Badge></span>
					<span class="text-muted-foreground">Started</span><span>{formatDate(detailItem.executionStartTimestamp)}</span>
					{#if detailItem.executionEndTimestamp}
						<span class="text-muted-foreground">Ended</span><span>{formatDate(detailItem.executionEndTimestamp)}</span>
					{/if}
					<span class="text-muted-foreground">Duration</span><span>{formatDuration(detailItem.executionDuration)}</span>
					<span class="text-muted-foreground">Layers</span><span>{detailItem.executionLayerCount}</span>
					<span class="text-muted-foreground">UUID</span><span class="text-xs font-mono break-all">{detailItem.executionUUID}</span>
				</div>
			{/if}
			<Dialog.Footer class="mt-4">
				<Button variant="outline" onclick={() => { detailItem = null; }}>Close</Button>
			</Dialog.Footer>
		</Dialog.Content>
	</Dialog.Root>
{/if}
