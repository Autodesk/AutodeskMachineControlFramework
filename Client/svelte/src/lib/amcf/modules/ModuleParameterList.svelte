<script lang="ts">
	import * as Table from '$lib/components/ui/table/index.js';
	import * as DropdownMenu from '$lib/components/ui/dropdown-menu/index.js';
	import * as Dialog from '$lib/components/ui/dialog/index.js';
	import { Button } from '$lib/components/ui/button/index.js';
	import { Input } from '$lib/components/ui/input/index.js';
	import { ScrollArea } from '$lib/components/ui/scroll-area/index.js';
	import Check from '@lucide/svelte/icons/check';
	import X from '@lucide/svelte/icons/x';
	import Star from '@lucide/svelte/icons/star';
	import Download from '@lucide/svelte/icons/download';
	import SlidersHorizontal from '@lucide/svelte/icons/sliders-horizontal';
	import ArrowUp from '@lucide/svelte/icons/arrow-up';
	import ArrowDown from '@lucide/svelte/icons/arrow-down';
	import ChevronsUpDown from '@lucide/svelte/icons/chevrons-up-down';

	import { useModuleTick } from '$lib/amcf/poll.svelte';
	import { useDisplayed } from '$lib/amcf/display';

	let { module, app }: { module: any; app: any } = $props();
	const poll = useModuleTick(() => module);
	const isDisplayed = useDisplayed();

	// Bumped whenever the client-side view state (favorites/sort/filters/presets)
	// changes, so derived values recompute even though `module` is a plain JS
	// object outside Svelte's reactivity graph. The core module invokes
	// onPreferencesChanged on every mutation and after the async preferences load.
	let stateTick = $state(0);

	$effect(() => {
		module.onPreferencesChanged = () => { stateTick++; };
		// Values are fetched asynchronously after the poll tick, so re-render when they arrive.
		module.onEntriesChanged = () => { stateTick++; };
		// The restore is driven from the core module's polled update once the stable
		// key and auth token are available; this is just a best-effort early nudge.
		if (typeof module.maybeLoadPreferences === 'function') module.maybeLoadPreferences();
		return () => {
			module.onPreferencesChanged = null;
			module.onEntriesChanged = null;
		};
	});

	// The core module only polls values while the list is shown (e.g. not in an inactive tab).
	$effect(() => {
		const displayed = isDisplayed();
		if (typeof module.setDisplayed === 'function') module.setDisplayed(displayed);
		return () => {
			if (typeof module.setDisplayed === 'function') module.setDisplayed(false);
		};
	});

	let visible = $derived.by(() => { poll.v; return module.visible !== false; });
	let headers = $derived.by(() => { poll.v; return module.headers || [
		{ text: 'Parameter', value: 'paramDescription' },
		{ text: 'Value',     value: 'paramValue' },
		{ text: 'Group',     value: 'paramGroup' },
		{ text: 'System',    value: 'paramSystem' },
	]; });

	// The displayed rows: raw server entries transformed by the shared view logic
	// (favorites-first ordering, category filters, favorites-only, sorting).
	let entries = $derived.by(() => {
		poll.v; stateTick;
		const raw = module.entries || [];
		return (typeof module.applyView === 'function') ? module.applyView(raw) : [...raw];
	});
	let editEvent = $derived.by(() => { poll.v; return module.editevent || ''; });

	let sortState = $derived.by(() => { poll.v; stateTick; return module.sort || { column: '', direction: 'asc' }; });
	let showOnlyFavorites = $derived.by(() => { poll.v; stateTick; return !!module.showOnlyFavorites; });
	let groupOptions = $derived.by(() => { poll.v; stateTick; return (typeof module.distinctGroups === 'function') ? module.distinctGroups() : []; });
	let systemOptions = $derived.by(() => { poll.v; stateTick; return (typeof module.distinctSystems === 'function') ? module.distinctSystems() : []; });
	let activeGroups = $derived.by(() => { poll.v; stateTick; return (module.filters && module.filters.groups) ? module.filters.groups : []; });
	let activeSystems = $derived.by(() => { poll.v; stateTick; return (module.filters && module.filters.systems) ? module.filters.systems : []; });
	let presetNames = $derived.by(() => { poll.v; stateTick; return (typeof module.listPresets === 'function') ? module.listPresets() : []; });
	let hasPreferences = $derived.by(() => { poll.v; return !!(module.preferenceKey); });

	// Session-only column width overrides from drag-resizing, keyed by column value.
	let widthOverrides = $state<Record<string, string>>({});

	// A configured width is fixed unless it is empty or "auto" (= flexible).
	function isFixedWidth(width: string | undefined): boolean {
		return !!width && width !== 'auto';
	}
	function colWidth(h: any): string {
		const override = widthOverrides[h.value];
		if (isFixedWidth(override)) return override;
		return isFixedWidth(h.width) ? h.width : '';
	}
	// Fixed table layout is only needed once a column pins an explicit width;
	// otherwise the table keeps its natural content-based sizing.
	let useFixedLayout = $derived.by(() => {
		poll.v;
		return headers.some((h: any) => isFixedWidth(colWidth(h)));
	});

	function startResize(e: PointerEvent, h: any) {
		e.preventDefault();
		e.stopPropagation();
		const handle = e.currentTarget as HTMLElement;
		const th = handle.closest('th') as HTMLElement | null;
		if (!th) return;
		const startX = e.clientX;
		const startWidth = th.getBoundingClientRect().width;
		const onMove = (ev: PointerEvent) => {
			const newWidth = Math.max(40, startWidth + (ev.clientX - startX));
			widthOverrides = { ...widthOverrides, [h.value]: `${Math.round(newWidth)}px` };
		};
		const onUp = () => {
			window.removeEventListener('pointermove', onMove);
			window.removeEventListener('pointerup', onUp);
		};
		window.addEventListener('pointermove', onMove);
		window.addEventListener('pointerup', onUp);
	}

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

	// Quote a CSV field only when needed, doubling embedded quotes (RFC 4180).
	function csvEscape(value: any): string {
		const s = String(value ?? '');
		return /[",\r\n]/.test(s) ? '"' + s.replace(/"/g, '""') + '"' : s;
	}

	// Exports the full, unfiltered parameter set (description, value, state machine,
	// group) as a CSV download, regardless of the active view filters.
	async function downloadCsv() {
		// Filtered-out rows are not polled, so refresh all values before exporting.
		if (typeof module.loadAllValues === 'function') await module.loadAllValues();
		const header = ['Parameter', 'Value', 'State Machine', 'Group'];
		const rows = (module.entries || []).map((e: any) =>
			[e.paramDescription, e.paramValue, e.paramSystem, e.paramGroup].map(csvEscape).join(','));
		const csv = [header.join(','), ...rows].join('\r\n');
		const blob = new Blob([csv], { type: 'text/csv;charset=utf-8;' });
		const url = URL.createObjectURL(blob);
		const a = document.createElement('a');
		a.href = url;
		a.download = (module.name || 'parameters') + '.csv';
		document.body.appendChild(a);
		a.click();
		document.body.removeChild(a);
		URL.revokeObjectURL(url);
	}

	// -- view-state actions (delegated to the shared core module) --
	function toggleFavorite(row: any) {
		if (typeof module.toggleFavorite === 'function') module.toggleFavorite(row);
	}
	function isFavorite(row: any): boolean {
		return (typeof module.isFavorite === 'function') ? module.isFavorite(row) : false;
	}
	function sortBy(h: any) {
		if (typeof module.toggleSort === 'function') module.toggleSort(h.value);
	}
	function toggleGroupFilter(group: string) {
		const next = activeGroups.includes(group)
			? activeGroups.filter((g: string) => g !== group)
			: [...activeGroups, group];
		if (typeof module.setGroupFilter === 'function') module.setGroupFilter(next);
	}
	function toggleSystemFilter(system: string) {
		const next = activeSystems.includes(system)
			? activeSystems.filter((s: string) => s !== system)
			: [...activeSystems, system];
		if (typeof module.setSystemFilter === 'function') module.setSystemFilter(next);
	}
	function clearFilters() {
		if (typeof module.clearFilters === 'function') module.clearFilters();
	}
	function setShowOnlyFavorites(flag: boolean) {
		if (typeof module.setShowOnlyFavorites === 'function') module.setShowOnlyFavorites(flag);
	}

	// -- presets --
	let saveOpen = $state(false);
	let presetName = $state('');
	function openSaveDialog() {
		presetName = (module.activePreset || '');
		saveOpen = true;
	}
	function confirmSavePreset() {
		const name = presetName.trim();
		if (name.length === 0) return;
		if (typeof module.savePreset === 'function') module.savePreset(name);
		saveOpen = false;
		presetName = '';
	}
	function loadPreset(name: string) {
		if (typeof module.loadPreset === 'function') module.loadPreset(name);
	}
	function deletePreset(name: string) {
		if (typeof module.deletePreset === 'function') module.deletePreset(name);
	}
</script>

{#if visible}
	<div class="w-full min-h-0 flex-1 flex flex-col border rounded-md overflow-hidden">
		<div class="flex items-center justify-end gap-1 px-2 py-1 border-b bg-muted/40">
			{#if hasPreferences && (showOnlyFavorites || activeGroups.length > 0 || activeSystems.length > 0)}
				<span class="mr-auto text-xs text-muted-foreground">
					{#if showOnlyFavorites}Favorites only{/if}
					{#if activeGroups.length > 0}{showOnlyFavorites ? ' · ' : ''}{activeGroups.length} group filter(s){/if}
					{#if activeSystems.length > 0}{(showOnlyFavorites || activeGroups.length > 0) ? ' · ' : ''}{activeSystems.length} system filter(s){/if}
				</span>
			{/if}
			<Button variant="ghost" size="sm" class="h-7 px-2 gap-1" title="Download parameters as CSV" onclick={downloadCsv}>
				<Download class="h-3.5 w-3.5" />
				<span class="text-xs">CSV</span>
			</Button>
			{#if hasPreferences}
				<DropdownMenu.Root>
					<DropdownMenu.Trigger>
						{#snippet child({ props })}
							<Button {...props} variant="ghost" size="icon" class="size-7" aria-label="Parameter list options">
								<SlidersHorizontal class="size-4" />
							</Button>
						{/snippet}
					</DropdownMenu.Trigger>
					<DropdownMenu.Content align="end" class="w-56">
						<DropdownMenu.Label>Sort by</DropdownMenu.Label>
						{#each headers as h (h.value)}
							<DropdownMenu.Item onSelect={() => sortBy(h)}>
								<span class="flex-1">{h.text}</span>
								{#if sortState.column === h.value}
									{#if sortState.direction === 'asc'}
										<ArrowUp class="size-4" />
									{:else}
										<ArrowDown class="size-4" />
									{/if}
								{/if}
							</DropdownMenu.Item>
						{/each}

						<DropdownMenu.Separator />
						<DropdownMenu.CheckboxItem
							checked={showOnlyFavorites}
							closeOnSelect={false}
							onCheckedChange={(v: boolean) => setShowOnlyFavorites(v)}
						>
							Show only favorites
						</DropdownMenu.CheckboxItem>
						<DropdownMenu.Item onSelect={() => setShowOnlyFavorites(false)}>
							Show everything
						</DropdownMenu.Item>

						{#if groupOptions.length > 0}
							<DropdownMenu.Separator />
							<DropdownMenu.Sub>
								<DropdownMenu.SubTrigger>Filter by group</DropdownMenu.SubTrigger>
								<DropdownMenu.SubContent class="max-h-64 overflow-y-auto">
									{#each groupOptions as group (group)}
										<DropdownMenu.CheckboxItem
											checked={activeGroups.includes(group)}
											closeOnSelect={false}
											onCheckedChange={() => toggleGroupFilter(group)}
										>
											{group}
										</DropdownMenu.CheckboxItem>
									{/each}
								</DropdownMenu.SubContent>
							</DropdownMenu.Sub>
						{/if}

						{#if systemOptions.length > 0}
							<DropdownMenu.Sub>
								<DropdownMenu.SubTrigger>Filter by system</DropdownMenu.SubTrigger>
								<DropdownMenu.SubContent class="max-h-64 overflow-y-auto">
									{#each systemOptions as system (system)}
										<DropdownMenu.CheckboxItem
											checked={activeSystems.includes(system)}
											closeOnSelect={false}
											onCheckedChange={() => toggleSystemFilter(system)}
										>
											{system}
										</DropdownMenu.CheckboxItem>
									{/each}
								</DropdownMenu.SubContent>
							</DropdownMenu.Sub>
						{/if}

						{#if activeGroups.length > 0 || activeSystems.length > 0}
							<DropdownMenu.Item onSelect={() => clearFilters()}>Clear filters</DropdownMenu.Item>
						{/if}

						<DropdownMenu.Separator />
						<DropdownMenu.Item onSelect={() => openSaveDialog()}>Save view as preset…</DropdownMenu.Item>
						{#if presetNames.length > 0}
							<DropdownMenu.Sub>
								<DropdownMenu.SubTrigger>Load preset</DropdownMenu.SubTrigger>
								<DropdownMenu.SubContent class="max-h-64 overflow-y-auto">
									{#each presetNames as name (name)}
										<DropdownMenu.Item onSelect={() => loadPreset(name)}>
											<span class="flex-1">{name}</span>
											{#if module.activePreset === name}<Check class="size-4" />{/if}
										</DropdownMenu.Item>
									{/each}
								</DropdownMenu.SubContent>
							</DropdownMenu.Sub>
							<DropdownMenu.Sub>
								<DropdownMenu.SubTrigger>Delete preset</DropdownMenu.SubTrigger>
								<DropdownMenu.SubContent class="max-h-64 overflow-y-auto">
									{#each presetNames as name (name)}
										<DropdownMenu.Item onSelect={() => deletePreset(name)}>
											<span class="flex-1">{name}</span>
											<X class="size-4" />
										</DropdownMenu.Item>
									{/each}
								</DropdownMenu.SubContent>
							</DropdownMenu.Sub>
						{/if}
					</DropdownMenu.Content>
				</DropdownMenu.Root>
			{/if}
		</div>
		<ScrollArea class="flex-1 min-h-0">
			<Table.Root style={useFixedLayout ? 'table-layout: fixed; width: 100%;' : ''}>
				<Table.Header class="sticky top-0 bg-muted z-10">
					<Table.Row>
						{#if hasPreferences}
							<Table.Head class="w-8" style="width: 2rem;"></Table.Head>
						{/if}
						{#each headers as h (h.value)}
							<Table.Head
								class="relative text-xs font-semibold uppercase tracking-wider overflow-hidden text-ellipsis whitespace-nowrap"
								style={colWidth(h) ? `width: ${colWidth(h)};` : ''}
							>
								{#if hasPreferences}
									<button
										type="button"
										class="inline-flex items-center gap-1 hover:text-primary"
										onclick={() => sortBy(h)}
										title="Sort by {h.text}"
									>
										{h.text}
										{#if sortState.column === h.value}
											{#if sortState.direction === 'asc'}
												<ArrowUp class="size-3" />
											{:else}
												<ArrowDown class="size-3" />
											{/if}
										{:else}
											<ChevronsUpDown class="size-3 opacity-40" />
										{/if}
									</button>
								{:else}
									{h.text}
								{/if}
								{#if h.sizeable}
									<span
										class="absolute top-0 right-0 h-full w-1.5 cursor-col-resize select-none hover:bg-primary/40"
										role="separator"
										aria-orientation="vertical"
										aria-label="Resize column"
										onpointerdown={(e) => startResize(e, h)}
									></span>
								{/if}
							</Table.Head>
						{/each}
					</Table.Row>
				</Table.Header>
				<Table.Body>
					{#if entries.length === 0}
						<Table.Row>
							<Table.Cell colspan={headers.length + (hasPreferences ? 1 : 0)} class="text-center py-8 text-muted-foreground text-sm">
								{module.loadingtext || 'No parameters available.'}
							</Table.Cell>
						</Table.Row>
					{:else}
						{#each entries as row (rowKey(row))}
							<Table.Row class="hover:bg-muted/50 transition-colors">
								{#if hasPreferences}
									<Table.Cell class="py-1.5 pr-0" style="width: 2rem;">
										<button
											type="button"
											class="text-muted-foreground hover:text-yellow-500"
											aria-label={isFavorite(row) ? 'Remove favorite' : 'Add favorite'}
											title={isFavorite(row) ? 'Remove favorite' : 'Add favorite'}
											onclick={() => toggleFavorite(row)}
										>
											<Star class={isFavorite(row) ? 'size-4 fill-yellow-400 text-yellow-500' : 'size-4'} />
										</button>
									</Table.Cell>
								{/if}
								<!-- Auto-sized columns wrap: unwrapped long descriptions widen the table past the
								     vertical-only ScrollArea and hide the other columns. Pinned widths keep the ellipsis. -->
								{#each headers as h (h.value)}
									<Table.Cell
										class="text-sm py-1.5 overflow-hidden text-ellipsis {colWidth(h) ? '' : 'whitespace-normal wrap-anywhere'}"
										style={colWidth(h) ? `width: ${colWidth(h)};` : ''}
									>
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

	<Dialog.Root bind:open={saveOpen}>
		<Dialog.Content class="sm:max-w-md">
			<Dialog.Header>
				<Dialog.Title>Save view preset</Dialog.Title>
				<Dialog.Description>
					Store the current favorites, sorting and filters as a named preset.
				</Dialog.Description>
			</Dialog.Header>
			<div class="py-2">
				<Input
					placeholder="Preset name"
					bind:value={presetName}
					onkeydown={(e: KeyboardEvent) => { if (e.key === 'Enter') { e.preventDefault(); confirmSavePreset(); } }}
				/>
			</div>
			<Dialog.Footer>
				<Button variant="outline" onclick={() => (saveOpen = false)}>Cancel</Button>
				<Button onclick={() => confirmSavePreset()} disabled={presetName.trim().length === 0}>Save</Button>
			</Dialog.Footer>
		</Dialog.Content>
	</Dialog.Root>
{/if}
