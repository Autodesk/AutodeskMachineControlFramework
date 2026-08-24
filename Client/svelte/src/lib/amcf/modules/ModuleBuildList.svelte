<script lang="ts">
	import * as Table from '$lib/components/ui/table/index.js';
	import * as Dialog from '$lib/components/ui/dialog/index.js';
	import { ScrollArea } from '$lib/components/ui/scroll-area/index.js';
	import { Button } from '$lib/components/ui/button/index.js';
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

	function selectBuild (build: any) {
		if (module.selectevent && app) {
			const formvalues: Record<string, string> = {};
			if (module.selectionvalueuuid)
				formvalues[module.selectionvalueuuid] = build.buildUUID;
			app.triggerUIEvent(module.selectevent, module.uuid, formvalues);
		}
	}

	function triggerButton (btn: any, build: any) {
		if (btn.selectevent && app) {
			const formvalues: Record<string, string> = {};
			if (module.selectionvalueuuid)
				formvalues[module.selectionvalueuuid] = build.buildUUID;
			if (module.buttonvalueuuid)
				formvalues[module.buttonvalueuuid] = build.buildUUID;
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
						<Table.Head class="text-xs font-semibold uppercase tracking-wider">Build</Table.Head>
						<Table.Head class="text-xs font-semibold uppercase tracking-wider w-[80px] text-right">Layers</Table.Head>
						<Table.Head class="text-xs font-semibold uppercase tracking-wider w-[140px]">Date</Table.Head>
						{#if entrybuttons.length > 0}
							<Table.Head class="text-xs font-semibold uppercase tracking-wider w-[220px]">Actions</Table.Head>
						{/if}
					</Table.Row>
				</Table.Header>
				<Table.Body>
					{#if entries.length === 0}
						<Table.Row>
							<Table.Cell colspan={entrybuttons.length > 0 ? 5 : 4} class="text-center py-8 text-muted-foreground text-sm">
								{loaded ? 'No builds available.' : (module.loadingtext || 'Loading…')}
							</Table.Cell>
						</Table.Row>
					{:else}
						{#each entries as build, idx (build.buildUUID || idx)}
						<Table.Row
							class="hover:bg-muted/50 transition-colors cursor-pointer"
							onclick={() => { if (entrybuttons.length > 0) { detailItem = build; } else { selectBuild(build); } }}
						>
								<Table.Cell class="py-1.5">
									{#if thumbnailURL(build.buildThumbnail)}
										<img src={thumbnailURL(build.buildThumbnail)} alt="" class="h-12 w-16 object-cover rounded border" />
									{:else}
										<div class="h-12 w-16 bg-muted rounded border flex items-center justify-center">
											<Image class="h-5 w-5 text-muted-foreground" />
										</div>
									{/if}
								</Table.Cell>
								<Table.Cell class="text-sm py-1.5">
									<div class="font-medium">{build.buildName}</div>
									<div class="text-xs text-muted-foreground">{build.buildUser}</div>
								</Table.Cell>
								<Table.Cell class="text-sm text-right py-1.5 tabular-nums">{build.buildLayers}</Table.Cell>
								<Table.Cell class="text-xs text-muted-foreground py-1.5">{formatDate(build.buildTimestamp)}</Table.Cell>
								{#if entrybuttons.length > 0}
									<Table.Cell class="py-1.5">
										<div class="flex gap-1">
											{#each entrybuttons as btn (btn.uuid)}
												<Button
													variant="outline"
													size="sm"
													class="h-7 text-xs"
													onclick={(e: MouseEvent) => { e.stopPropagation(); triggerButton(btn, build); }}
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
				<Dialog.Title>{detailItem?.buildName || 'Build Details'}</Dialog.Title>
				<Dialog.Description>Build file details and actions</Dialog.Description>
			</Dialog.Header>
			{#if detailItem}
				<div class="grid grid-cols-[100px_1fr] gap-y-2 gap-x-4 text-sm mt-2">
					{#if thumbnailURL(detailItem.buildThumbnail)}
						<div class="col-span-2 flex justify-center mb-2">
							<img src={thumbnailURL(detailItem.buildThumbnail)} alt="" class="max-h-40 object-contain rounded border" />
						</div>
					{/if}
					<span class="text-muted-foreground">Name</span><span>{detailItem.buildName}</span>
					<span class="text-muted-foreground">Layers</span><span>{detailItem.buildLayers}</span>
					<span class="text-muted-foreground">User</span><span>{detailItem.buildUser}</span>
					<span class="text-muted-foreground">Date</span><span>{formatDate(detailItem.buildTimestamp)}</span>
					<span class="text-muted-foreground">Executions</span><span>{detailItem.buildExecutionCount}</span>
					<span class="text-muted-foreground">UUID</span><span class="text-xs font-mono break-all">{detailItem.buildUUID}</span>
				</div>
			{/if}
			<Dialog.Footer class="mt-4">
				<Button variant="outline" onclick={() => { detailItem = null; }}>Close</Button>
				{#each entrybuttons as btn (btn.uuid)}
					{#if btn.selectevent}
						<Button onclick={() => { triggerButton(btn, detailItem); detailItem = null; }}>
							{btn.caption}
						</Button>
					{/if}
				{/each}
			</Dialog.Footer>
		</Dialog.Content>
	</Dialog.Root>
{/if}
