<script lang="ts">
	import { usePollTick } from '$lib/amcf/poll.svelte';

	let { module }: { module: any; app?: any } = $props();
	const poll = usePollTick();

	let visible = $derived.by(() => { poll.v; return module.visible !== false; });
	let title = $derived.by(() => { poll.v; return module.title || ''; });
	let rows = $derived.by(() => { poll.v; return [...(module.rows || [])]; });

	const severityClass: Record<string, string> = {
		success: 'bg-green-100 text-green-800 dark:bg-green-900/40 dark:text-green-300',
		warning: 'bg-amber-100 text-amber-800 dark:bg-amber-900/40 dark:text-amber-300',
		error: 'bg-red-100 text-red-800 dark:bg-red-900/40 dark:text-red-300',
		info: 'bg-blue-100 text-blue-800 dark:bg-blue-900/40 dark:text-blue-300',
		neutral: 'bg-muted text-muted-foreground',
	};

	function isTruthy(value: string): boolean {
		const v = (value ?? '').toString().trim().toLowerCase();
		return !(v === '' || v === '0' || v === 'false' || v === 'off' || v === 'no');
	}

	interface RowRender {
		pill: boolean;
		text: string;
		cls: string;
	}

	function renderRow(row: any): RowRender {
		const value = (row.value ?? '').toString();
		const display = (row.display || 'text').toString();

		if (display === 'number') {
			const unit = row.unit ? ` ${row.unit}` : '';
			return { pill: false, text: `${value}${unit}`, cls: '' };
		}

		if (display === 'text') {
			return { pill: false, text: value, cls: '' };
		}

		if (display === 'map') {
			const maps = row.maps || [];
			const match = maps.find((m: any) => (m.value ?? '').toString() === value);
			if (match) {
				return { pill: true, text: match.text ?? value, cls: severityClass[match.severity] || severityClass.neutral };
			}
			return { pill: true, text: value, cls: severityClass.neutral };
		}

		// boolean presets
		const on = isTruthy(value);
		if (display === 'yesno') {
			return { pill: true, text: on ? 'Yes' : 'No', cls: on ? severityClass.success : severityClass.neutral };
		}
		if (display === 'openclosed') {
			return { pill: true, text: on ? 'Opened' : 'Closed', cls: on ? severityClass.success : severityClass.neutral };
		}
		if (display === 'leftright') {
			return { pill: true, text: on ? 'Right' : 'Left', cls: severityClass.neutral };
		}
		// default: onoff
		return { pill: true, text: on ? 'On' : 'Off', cls: on ? severityClass.success : severityClass.neutral };
	}
</script>

{#if visible}
	<div class="flex flex-col w-full min-w-0">
		{#if title}
			<div class="text-xs font-semibold text-muted-foreground mb-1">{title}</div>
		{/if}
		<div class="flex flex-col">
			{#if rows.length === 0}
				<div class="text-sm text-muted-foreground py-2">{module.loadingtext || 'No status available.'}</div>
			{:else}
				{#each rows as row (row.label)}
					{@const r = renderRow(row)}
					<div class="flex items-center gap-2 py-1.5 border-b last:border-b-0 border-border/60">
						<span class="flex-1 text-sm text-muted-foreground">{row.label}</span>
						{#if r.pill}
							<span class="inline-block rounded-full px-2 py-0.5 text-xs font-semibold {r.cls}">{r.text}</span>
						{:else}
							<span class="text-sm tabular-nums">{r.text}</span>
						{/if}
					</div>
				{/each}
			{/if}
		</div>
	</div>
{/if}
