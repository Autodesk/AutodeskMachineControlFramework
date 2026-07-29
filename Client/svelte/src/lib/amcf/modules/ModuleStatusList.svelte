<script lang="ts">
	import { usePollTick } from '$lib/amcf/poll.svelte';

	let { module }: { module: any; app?: any } = $props();
	const poll = usePollTick();

	let visible = $derived.by(() => { poll.v; return module.visible !== false; });
	let title = $derived.by(() => { poll.v; return module.title || ''; });
	let rows = $derived.by(() => { poll.v; return [...(module.rows || [])]; });

	// Semantic status colors come from theme tokens (defined per light/dark in
	// core/theme/tokens.css and overridable via a server theme), so the pills
	// follow the active color scheme and stay consistent across clients.
	const severityStyle: Record<string, string> = {
		success: 'background: var(--amcf-status-success-bg); color: var(--amcf-status-success-fg);',
		warning: 'background: var(--amcf-status-warning-bg); color: var(--amcf-status-warning-fg);',
		error: 'background: var(--amcf-status-error-bg); color: var(--amcf-status-error-fg);',
		info: 'background: var(--amcf-status-info-bg); color: var(--amcf-status-info-fg);',
		neutral: 'background: var(--amcf-status-neutral-bg); color: var(--amcf-status-neutral-fg);',
	};

	function isTruthy(value: string): boolean {
		const v = (value ?? '').toString().trim().toLowerCase();
		return !(v === '' || v === '0' || v === 'false' || v === 'off' || v === 'no');
	}

	interface RowRender {
		pill: boolean;
		text: string;
		style: string;
	}

	function renderRow(row: any): RowRender {
		const value = (row.value ?? '').toString();
		const display = (row.display || 'text').toString();

		if (display === 'number') {
			const unit = row.unit ? ` ${row.unit}` : '';
			return { pill: false, text: `${value}${unit}`, style: '' };
		}

		if (display === 'text') {
			return { pill: false, text: value, style: '' };
		}

		if (display === 'map') {
			const maps = row.maps || [];
			const match = maps.find((m: any) => (m.value ?? '').toString() === value);
			if (match) {
				return { pill: true, text: match.text ?? value, style: severityStyle[match.severity] || severityStyle.neutral };
			}
			return { pill: true, text: value, style: severityStyle.neutral };
		}

		// boolean presets
		const on = isTruthy(value);
		if (display === 'yesno') {
			return { pill: true, text: on ? 'Yes' : 'No', style: on ? severityStyle.success : severityStyle.neutral };
		}
		if (display === 'openclosed') {
			return { pill: true, text: on ? 'Opened' : 'Closed', style: on ? severityStyle.success : severityStyle.neutral };
		}
		if (display === 'leftright') {
			return { pill: true, text: on ? 'Right' : 'Left', style: severityStyle.neutral };
		}
		// default: onoff
		return { pill: true, text: on ? 'On' : 'Off', style: on ? severityStyle.success : severityStyle.neutral };
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
							<span class="inline-block rounded-full px-2 py-0.5 text-xs font-semibold" style={r.style}>{r.text}</span>
						{:else}
							<span class="text-sm tabular-nums">{r.text}</span>
						{/if}
					</div>
				{/each}
			{/if}
		</div>
	</div>
{/if}
