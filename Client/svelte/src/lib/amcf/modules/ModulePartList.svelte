<script lang="ts">
	import * as Table from '$lib/components/ui/table/index.js';
	import { ScrollArea } from '$lib/components/ui/scroll-area/index.js';
	import { Badge } from '$lib/components/ui/badge/index.js';
	import Box from '@lucide/svelte/icons/box';
	import Ban from '@lucide/svelte/icons/ban';
	import Image from '@lucide/svelte/icons/image';

	import { useModuleTick } from '$lib/amcf/poll.svelte';

	let { module, app }: { module: any; app: any } = $props();
	const poll = useModuleTick(() => module);

	const nullUUID = '00000000-0000-0000-0000-000000000000';

	// The build details are fetched by the model class (AMCModule_PartList.js) whenever the
	// selected build changes, so this component only renders what the model exposes.
	let visible = $derived.by(() => { poll.v; return module.visible !== false; });
	let loadingtext = $derived.by(() => { poll.v; return module.loadingtext || 'Loading build details...'; });
	let showdetails = $derived.by(() => { poll.v; return module.showdetails !== false; });
	let loading = $derived.by(() => { poll.v; return module.loading === true; });
	let errorMessage = $derived.by(() => { poll.v; return module.errorMessage || ''; });
	let details = $derived.by(() => { poll.v; return module.details || null; });
	let parts = $derived.by(() => { poll.v; return [...(module.parts || [])]; });
	let selectevent = $derived.by(() => { poll.v; return module.selectevent || ''; });
	let builduuid = $derived.by(() => { poll.v; return module.builduuid || ''; });

	// Remembered per build, so a newly selected build starts without a selected part.
	let selection = $state<{ build: string; part: string }>({ build: '', part: '' });
	let selectedPartUUID = $derived(selection.build === builduuid ? selection.part : '');

	// Clicking the selected row again deselects it and sends the null UUID.
	function selectPart(part: any) {
		if (!selectevent || !app || !part?.uuid) return;
		const deselect = part.uuid === selectedPartUUID;
		selection = { build: builduuid, part: deselect ? '' : part.uuid };

		const formvalues: Record<string, string> = {};
		if (module.selectionvalueuuid)
			formvalues[module.selectionvalueuuid] = deselect ? nullUUID : part.uuid;
		app.triggerUIEvent(selectevent, module.uuid, formvalues);
	}

	function onPartKeyDown(event: KeyboardEvent, part: any) {
		if (event.key === 'Enter' || event.key === ' ') {
			event.preventDefault();
			selectPart(part);
		}
	}

	function formatBytes(bytes: number | undefined): string {
		if (!bytes || bytes <= 0) return '—';
		if (bytes >= 1048576) return (bytes / 1048576).toFixed(1) + ' MB';
		if (bytes >= 1024) return (bytes / 1024).toFixed(1) + ' KB';
		return bytes + ' B';
	}

	function layerThickness(d: any): string {
		if (!d) return '—';
		if (d.variablelayers) return 'Variable';
		if (typeof d.globallayerthickness === 'number' && d.globallayerthickness > 0)
			return d.globallayerthickness.toFixed(3) + ' mm';
		return '—';
	}

	function thumbnailURL(uuid: string | undefined): string {
		if (!uuid || uuid === nullUUID || !app) return '';
		return app.getImageURL(uuid);
	}
</script>

{#if visible}
	<div class="flex flex-col gap-3 w-full min-h-0">
		{#if loading && !details}
			<p class="text-sm text-muted-foreground">{loadingtext}</p>
		{:else if errorMessage}
			<p class="text-sm text-destructive">{errorMessage}</p>
		{:else if !details}
			<p class="text-sm text-muted-foreground">No build selected.</p>
		{:else}
			{#if showdetails}
			<!-- Build detail header -->
			<div class="flex gap-3">
				{#if thumbnailURL(details.thumbnail)}
					<img
						src={thumbnailURL(details.thumbnail)}
						alt=""
						class="h-24 w-32 object-cover rounded border shrink-0"
					/>
				{:else}
					<div class="h-24 w-32 bg-muted rounded border flex items-center justify-center shrink-0">
						<Image class="h-6 w-6 text-muted-foreground" />
					</div>
				{/if}
				<dl class="grid grid-cols-[auto_1fr] gap-x-4 gap-y-1 text-sm content-start">
					<dt class="text-muted-foreground">Name</dt>
					<dd class="font-medium break-all">{details.name || '—'}</dd>
					<dt class="text-muted-foreground">Layers</dt>
					<dd class="tabular-nums">{details.layercount ?? '—'}</dd>
					<dt class="text-muted-foreground">Layer thickness</dt>
					<dd class="tabular-nums">{layerThickness(details)}</dd>
					<dt class="text-muted-foreground">File size</dt>
					<dd class="tabular-nums">{formatBytes(details.size)}</dd>
				</dl>
			</div>
			{/if}

			<!-- Part list -->
			<div class="text-xs font-semibold uppercase tracking-wider text-muted-foreground">
				Parts ({parts.length})
			</div>
			<div class="w-full border rounded-md overflow-hidden">
				<ScrollArea class="max-h-[360px]">
					<Table.Root>
						<Table.Header class="sticky top-0 bg-muted z-10">
							<Table.Row>
								<Table.Head class="text-xs font-semibold uppercase tracking-wider w-[40px]"></Table.Head>
								<Table.Head class="text-xs font-semibold uppercase tracking-wider">Part name</Table.Head>
								<Table.Head class="text-xs font-semibold uppercase tracking-wider w-[160px]">Part number</Table.Head>
							</Table.Row>
						</Table.Header>
						<Table.Body>
							{#if parts.length === 0}
								<Table.Row>
									<Table.Cell colspan={3} class="text-center py-8 text-muted-foreground text-sm">
										No parts in this build.
									</Table.Cell>
								</Table.Row>
							{:else}
								{#each parts as part, idx (part.uuid || idx)}
									<Table.Row
										class={selectevent ? 'cursor-pointer' : ''}
										data-state={part.uuid && part.uuid === selectedPartUUID ? 'selected' : undefined}
										aria-selected={selectevent ? part.uuid === selectedPartUUID : undefined}
										tabindex={selectevent ? 0 : undefined}
										onclick={() => selectPart(part)}
										onkeydown={(event: KeyboardEvent) => onPartKeyDown(event, part)}
									>
										<Table.Cell class="py-1.5">
											{#if part.disabled}
												<Ban class="h-4 w-4 text-destructive" />
											{:else}
												<Box class="h-4 w-4 text-muted-foreground" />
											{/if}
										</Table.Cell>
										<Table.Cell class="text-sm py-1.5 font-medium">
											<span class={part.disabled ? 'text-muted-foreground line-through' : ''}>{part.name || '—'}</span>
											{#if part.disabled}
												<Badge variant="destructive" class="ml-2 text-[10px] px-1.5 py-0 uppercase">Disabled</Badge>
											{/if}
										</Table.Cell>
										<Table.Cell class="text-sm py-1.5 text-muted-foreground tabular-nums">{part.partnumber || '—'}</Table.Cell>
									</Table.Row>
								{/each}
							{/if}
						</Table.Body>
					</Table.Root>
				</ScrollArea>
			</div>
		{/if}
	</div>
{/if}
