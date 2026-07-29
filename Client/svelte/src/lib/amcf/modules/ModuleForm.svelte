<script lang="ts">
	import { Input } from '$lib/components/ui/input/index.js';
	import { Label } from '$lib/components/ui/label/index.js';
	import { Button } from '$lib/components/ui/button/index.js';
	import { Switch } from '$lib/components/ui/switch/index.js';
	import * as Select from '$lib/components/ui/select/index.js';
	import { Badge } from '$lib/components/ui/badge/index.js';
	import ChevronDown from '@lucide/svelte/icons/chevron-down';
	import ChevronUp from '@lucide/svelte/icons/chevron-up';

	import { usePollTick } from '$lib/amcf/poll.svelte';

	let { module, app }: { module: any; app: any } = $props();
	const poll = usePollTick();

	let entities = $derived.by(() => { poll.v; return [...(module.entities || [])]; });
	let visible = $derived.by(() => { poll.v; return module.visible !== false; });

	function d(entity: any): any { poll.v; return entity?.dataObject || {}; }

	function fireChangeEvent (entity: any) {
		if (!entity?.dataObject?.changeevent || !app) return;
		const formvalues = app.assembleFormValues ? app.assembleFormValues([entity.uuid]) : {};
		app.triggerUIEvent(entity.dataObject.changeevent, entity.uuid, formvalues);
	}

	function onEditBlur (entity: any) {
		fireChangeEvent(entity);
	}

	function onEditEnter (entity: any, e: KeyboardEvent) {
		if (e.key === 'Enter') fireChangeEvent(entity);
	}

	function onSwitchChange (entity: any, checked: boolean) {
		entity.dataObject.value = checked;
		fireChangeEvent(entity);
	}

	function onComboChange (entity: any, value: string) {
		entity.dataObject.value = value;
		fireChangeEvent(entity);
	}

	function onActionCancel (entity: any) {
		if (entity.dataObject.cancelevent && app) {
			app.triggerUIEvent(entity.dataObject.cancelevent, module.uuid, {});
		}
	}

	function onActionSubmit (entity: any) {
		if (entity.dataObject.submitevent && app) {
			const formvalues = app.assembleFormValues ? app.assembleFormValues(module.entities.map((e: any) => e.uuid)) : {};
			app.triggerUIEvent(entity.dataObject.submitevent, module.uuid, formvalues);
		}
	}

	function toBool (v: any): boolean {
		return v === true || v === '1' || v === 'true';
	}

	let sectionOpen: Record<string, boolean> = $state({});

	function toggleSection (uuid: string, defaultOpen: boolean) {
		if (sectionOpen[uuid] === undefined) sectionOpen[uuid] = defaultOpen;
		sectionOpen[uuid] = !sectionOpen[uuid];
	}

	function isSectionOpen (uuid: string, defaultOpen: boolean): boolean {
		return sectionOpen[uuid] === undefined ? defaultOpen : sectionOpen[uuid];
	}
</script>

{#snippet formField(entity: any)}
	<!-- edit -->
	{#if entity.type === 'edit'}
		<div class="grid grid-cols-[140px_1fr] items-center gap-2">
			<Label class="text-sm text-muted-foreground">{entity.caption}</Label>
			<div class="flex items-center gap-1">
				{#if d(entity).prefix}
					<span class="text-xs text-muted-foreground">{d(entity).prefix}</span>
				{/if}
				<Input
					value={d(entity).value || ''}
					disabled={d(entity).disabled}
					readonly={d(entity).readonly}
					class="h-8 text-sm"
					onblur={() => onEditBlur(entity)}
					onkeydown={(e: KeyboardEvent) => onEditEnter(entity, e)}
					oninput={(e: Event) => { entity.dataObject.value = (e.target as HTMLInputElement).value; }}
				/>
				{#if d(entity).suffix}
					<span class="text-xs text-muted-foreground">{d(entity).suffix}</span>
				{/if}
			</div>
		</div>

	<!-- switch -->
	{:else if entity.type === 'switch'}
		<div class="grid grid-cols-[140px_1fr] items-center gap-2">
			<Label class="text-sm text-muted-foreground">{entity.caption}</Label>
			<div class="flex items-center gap-2">
				<Switch
					checked={toBool(d(entity).value)}
					onCheckedChange={(v: boolean) => onSwitchChange(entity, v)}
					disabled={d(entity).disabled}
				/>
				<span class="text-sm">{toBool(d(entity).value) ? 'On' : 'Off'}</span>
			</div>
		</div>

	<!-- memo -->
	{:else if entity.type === 'memo'}
		<div class="grid grid-cols-[140px_1fr] items-start gap-2">
			<Label class="text-sm text-muted-foreground pt-2">{entity.caption}</Label>
			<textarea
				class="flex min-h-[80px] w-full rounded-md border border-input bg-background px-3 py-2 text-sm ring-offset-background placeholder:text-muted-foreground focus-visible:outline-none focus-visible:ring-2 focus-visible:ring-ring"
				value={d(entity).value || ''}
				disabled={d(entity).disabled}
				readonly={d(entity).readonly}
				onblur={() => onEditBlur(entity)}
				oninput={(e: Event) => { entity.dataObject.value = (e.target as HTMLTextAreaElement).value; }}
			></textarea>
		</div>

	<!-- combobox / select -->
	{:else if entity.type === 'combobox'}
		<div class="grid grid-cols-[140px_1fr] items-center gap-2">
			<Label class="text-sm text-muted-foreground">{entity.caption}</Label>
			<Select.Root
				type="single"
				value={d(entity).value || ''}
				onValueChange={(v: string) => onComboChange(entity, v)}
				disabled={d(entity).disabled}
			>
			<Select.Trigger class="h-8 text-sm">
				{@const selectedItem = (d(entity).items || []).find((i: any) => (typeof i === 'string' ? i : i.value) === d(entity).value)}
				{selectedItem ? (typeof selectedItem === 'string' ? selectedItem : selectedItem.text || selectedItem.value) : d(entity).value || 'Select...'}
			</Select.Trigger>
			<Select.Content>
				{#each (d(entity).items || entity.items || []) as item}
					<Select.Item value={typeof item === 'string' ? item : item.value}>
						{typeof item === 'string' ? item : item.text || item.value}
					</Select.Item>
				{/each}
			</Select.Content>
			</Select.Root>
		</div>

	<!-- number -->
	{:else if entity.type === 'number'}
		<div class="grid grid-cols-[140px_1fr] items-center gap-2">
			<Label class="text-sm text-muted-foreground">{entity.caption}</Label>
			<div class="flex items-center gap-1">
				<Input
					type="number"
					value={d(entity).value || ''}
					disabled={d(entity).disabled}
					readonly={d(entity).readonly}
					min={d(entity).minvalue}
					max={d(entity).maxvalue}
					step={d(entity).step}
					class="h-8 text-sm"
					onblur={() => onEditBlur(entity)}
					onkeydown={(e: KeyboardEvent) => onEditEnter(entity, e)}
					oninput={(e: Event) => { entity.dataObject.value = (e.target as HTMLInputElement).value; }}
				/>
				{#if d(entity).unit}
					<span class="text-xs text-muted-foreground">{d(entity).unit}</span>
				{/if}
			</div>
		</div>

	<!-- slider -->
	{:else if entity.type === 'slider'}
		<div class="grid grid-cols-[140px_1fr] items-start gap-2">
			<Label class="text-sm text-muted-foreground pt-1">{entity.caption}</Label>
			<div class="flex flex-col">
				<div class="flex items-center gap-3">
					<input
						type="range"
						class="flex-1 accent-primary"
						value={d(entity).value || 0}
						min={parseFloat(d(entity).minvalue) || 0}
						max={parseFloat(d(entity).maxvalue) || 100}
						step={parseFloat(d(entity).step) || 1}
						disabled={d(entity).disabled}
						oninput={(e: Event) => { entity.dataObject.value = (e.target as HTMLInputElement).value; }}
						onchange={() => onEditBlur(entity)}
					/>
					<span class="text-sm tabular-nums w-16 text-right">
						{d(entity).value}{d(entity).unit ? ` ${d(entity).unit}` : ''}
					</span>
				</div>
				<div class="flex text-xs text-muted-foreground pr-[4.5rem]">
					<span>{parseFloat(d(entity).minvalue) || 0}{d(entity).unit ? ` ${d(entity).unit}` : ''}</span>
					<span class="ml-auto">{parseFloat(d(entity).maxvalue) || 100}{d(entity).unit ? ` ${d(entity).unit}` : ''}</span>
				</div>
			</div>
		</div>

	<!-- datetime -->
	{:else if entity.type === 'datetime'}
		<div class="grid grid-cols-[140px_1fr] items-center gap-2">
			<Label class="text-sm text-muted-foreground">{entity.caption}</Label>
			<Input
				type={d(entity).mode === 'time' ? 'time' : d(entity).mode === 'date' ? 'date' : 'datetime-local'}
				value={d(entity).value || ''}
				disabled={d(entity).disabled}
				readonly={d(entity).readonly}
				class="h-8 text-sm"
				onchange={(e: Event) => { entity.dataObject.value = (e.target as HTMLInputElement).value; fireChangeEvent(entity); }}
			/>
		</div>

	<!-- multiselect -->
	{:else if entity.type === 'multiselect'}
		<div class="grid grid-cols-[140px_1fr] items-center gap-2">
			<Label class="text-sm text-muted-foreground">{entity.caption}</Label>
		<div class="flex flex-wrap gap-1 p-1 border rounded-md min-h-[32px]">
			{#each (d(entity).items || entity.items || []) as item}
					{@const val = typeof item === 'string' ? item : item.value}
					{@const selected = (() => { try { const arr = JSON.parse(d(entity).value || '[]'); return Array.isArray(arr) && arr.includes(val); } catch { return false; } })()}
					<button
						class="px-2 py-0.5 rounded text-xs transition-colors {selected ? 'bg-primary text-primary-foreground' : 'bg-muted text-muted-foreground hover:bg-muted/80'}"
						onclick={() => {
							let arr: string[] = [];
							try { arr = JSON.parse(entity.dataObject?.value || '[]'); } catch {}
							if (!Array.isArray(arr)) arr = [];
							if (arr.includes(val)) arr = arr.filter((v: string) => v !== val);
							else arr.push(val);
							entity.dataObject.value = JSON.stringify(arr);
							fireChangeEvent(entity);
						}}
					>
						{typeof item === 'string' ? item : item.text || item.value}
					</button>
				{/each}
			</div>
		</div>

	<!-- calculated (read-only) -->
	{:else if entity.type === 'calculated'}
		<div class="grid grid-cols-[140px_1fr] items-center gap-2">
			<Label class="text-sm text-muted-foreground">{entity.caption}</Label>
			<Badge variant="outline" class="w-fit">
				{d(entity).value || ''}
				{#if d(entity).unit}
					<span class="ml-1 text-muted-foreground">{d(entity).unit}</span>
				{/if}
			</Badge>
		</div>

	<!-- section -->
	{:else if entity.type === 'section'}
		{@const collapsible = d(entity).collapsible !== false}
		{@const defaultOpen = d(entity).defaultOpen !== false}
		{@const open = isSectionOpen(entity.uuid, defaultOpen)}
		<div class="border rounded-md overflow-hidden">
			{#if collapsible}
				<button
					class="w-full flex items-center justify-between px-3 py-2 bg-muted hover:bg-muted/80 transition-colors text-left"
					onclick={() => toggleSection(entity.uuid, defaultOpen)}
				>
					<span class="text-sm font-medium">{entity.caption}</span>
					{#if open}
						<ChevronUp class="h-4 w-4 text-muted-foreground" />
					{:else}
						<ChevronDown class="h-4 w-4 text-muted-foreground" />
					{/if}
				</button>
			{:else}
				<div class="px-3 py-2 bg-muted">
					<span class="text-sm font-medium">{entity.caption}</span>
				</div>
			{/if}
			{#if !collapsible || open}
				<div class="p-3 flex flex-col gap-3">
					{#each (entity.entities || []) as child (child.uuid)}
						{@render formField(child)}
					{/each}
				</div>
			{/if}
		</div>

	<!-- actionbar -->
	{:else if entity.type === 'actionbar'}
		<div class="flex justify-end gap-2 pt-2 border-t">
			{#if d(entity).cancelevent}
				<Button variant="outline" onclick={() => onActionCancel(entity)}>
					{d(entity).cancelcaption || 'Cancel'}
				</Button>
			{/if}
			{#if d(entity).submitevent}
				<Button onclick={() => onActionSubmit(entity)}>
					{d(entity).submitcaption || 'Save'}
				</Button>
			{/if}
		</div>
	{/if}
{/snippet}

{#if visible}
	<div class="flex flex-col gap-3 w-full">
		{#each entities as entity (entity.uuid)}
			{@render formField(entity)}
		{/each}
	</div>
{/if}
