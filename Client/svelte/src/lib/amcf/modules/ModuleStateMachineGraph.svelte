<script lang="ts">
	import { onMount } from 'svelte';
	import { usePollTick } from '$lib/amcf/poll.svelte';

	let { module, app }: { module: any; app: any } = $props();
	const poll = usePollTick();

	let statemachineName = $derived.by(() => { poll.v; return module?.statemachine || ''; });
	let activeState = $derived.by(() => { poll.v; return module?.activestate || ''; });
	let nodePositionsStr = $derived.by(() => { poll.v; return module?.nodepositions || ''; });

	interface GraphNode {
		name: string;
		x: number;
		y: number;
		outstates: string[];
	}

	let nodes: GraphNode[] = $state([]);
	let structureLoaded = $state(false);
	let svgWidth = $state(800);
	let svgHeight = $state(400);

	const NODE_W = 140;
	const NODE_H = 44;
	const NODE_RX = 8;
	const H_GAP = 60;
	const V_GAP = 70;
	const PAD = 40;

	function parseNodePositions (posStr: string): Map<string, {x: number, y: number}> {
		const map = new Map();
		if (!posStr) return map;
		for (const part of posStr.split(';')) {
			const segs = part.split(':');
			if (segs.length >= 3) {
				const x = parseFloat(segs[1]);
				const y = parseFloat(segs[2]);
				if (!isNaN(x) && !isNaN(y) && x >= 0 && y >= 0)
					map.set(segs[0], { x, y });
			}
		}
		return map;
	}

	function rectEdgeIntersection (cx: number, cy: number, tx: number, ty: number, w: number, h: number): {x: number; y: number} {
		const hw = w / 2;
		const hh = h / 2;
		const dx = tx - cx;
		const dy = ty - cy;
		if (dx === 0 && dy === 0) return { x: cx + hw, y: cy };

		const absDx = Math.abs(dx);
		const absDy = Math.abs(dy);
		let t: number;
		if (absDx * hh > absDy * hw) {
			t = hw / absDx;
		} else {
			t = hh / absDy;
		}
		return { x: cx + dx * t, y: cy + dy * t };
	}

	function autoLayout (nodeList: {name: string; outstates: string[]}[], overrides: Map<string, {x: number, y: number}>): GraphNode[] {
		if (nodeList.length === 0) return [];

		const nameSet = new Set(nodeList.map(n => n.name));
		const adj = new Map<string, string[]>();
		for (const n of nodeList)
			adj.set(n.name, n.outstates.filter(t => t !== n.name && nameSet.has(t)));

		const levels = new Map<string, number>();
		const queue: string[] = [nodeList[0].name];
		levels.set(nodeList[0].name, 0);

		while (queue.length > 0) {
			const cur = queue.shift()!;
			const curLevel = levels.get(cur)!;
			for (const next of (adj.get(cur) || [])) {
				if (!levels.has(next)) {
					levels.set(next, curLevel + 1);
					queue.push(next);
				}
			}
		}

		for (const n of nodeList) {
			if (!levels.has(n.name))
				levels.set(n.name, (levels.size > 0 ? Math.max(...levels.values()) + 1 : 0));
		}

		const byLevel = new Map<number, string[]>();
		for (const [name, level] of levels) {
			if (!byLevel.has(level)) byLevel.set(level, []);
			byLevel.get(level)!.push(name);
		}

		const result: GraphNode[] = [];

		for (const [level, names] of byLevel) {
			for (let i = 0; i < names.length; i++) {
				const name = names[i];
				const node = nodeList.find(n => n.name === name)!;
				const override = overrides.get(name);

				const autoX = PAD + level * (NODE_W + H_GAP);
				const autoY = PAD + i * (NODE_H + V_GAP);

				result.push({
					name,
					x: override ? override.x : autoX,
					y: override ? override.y : autoY,
					outstates: node.outstates,
				});
			}
		}

		const maxX = Math.max(...result.map(n => n.x + NODE_W)) + PAD;
		const maxY = Math.max(...result.map(n => n.y + NODE_H)) + PAD;
		svgWidth = Math.max(600, maxX);
		svgHeight = Math.max(200, maxY);

		return result;
	}

	async function fetchGraphStructure () {
		if (!statemachineName || !app) return;
		try {
			const resp = await app.axiosGetRequest('/setup');
			const data = resp?.data;

			const instances = data?.instances || [];
			const instance = instances.find((inst: any) => inst.name === statemachineName);
			if (!instance || !instance.states) return;

			const nodeList = instance.states.map((s: any) => ({
				name: s.name,
				outstates: s.outstates || [],
			}));

			const overrides = parseNodePositions(nodePositionsStr);
			nodes = autoLayout(nodeList, overrides);
			structureLoaded = true;
		} catch (e) {
			console.warn('[StateMachineGraph] Failed to fetch structure:', e);
		}
	}

	onMount(() => {
		fetchGraphStructure();
	});

	$effect(() => {
		if (statemachineName && !structureLoaded)
			fetchGraphStructure();
	});

	function nodeCenter (name: string): {cx: number; cy: number} | null {
		const n = nodes.find(nd => nd.name === name);
		if (!n) return null;
		return { cx: n.x + NODE_W / 2, cy: n.y + NODE_H / 2 };
	}

	interface Edge {
		from: string;
		to: string;
		path: string;
		labelX: number;
		labelY: number;
	}

	let edges = $derived.by((): Edge[] => {
		const result: Edge[] = [];
		const pairSet = new Set<string>();
		const edgeSet = new Set<string>();

		for (const node of nodes) {
			for (const target of node.outstates) {
				edgeSet.add(`${node.name}->${target}`);
			}
		}

		for (const node of nodes) {
			for (const target of node.outstates) {
				if (target === node.name) continue;
				const key = `${node.name}->${target}`;
				if (pairSet.has(key)) continue;
				pairSet.add(key);

				const from = nodeCenter(node.name);
				const to = nodeCenter(target);
				if (!from || !to) continue;

				const dx = to.cx - from.cx;
				const dy = to.cy - from.cy;
				const dist = Math.sqrt(dx * dx + dy * dy);
				if (dist === 0) continue;

				const reverseKey = `${target}->${node.name}`;
				const isBidirectional = edgeSet.has(reverseKey);
				const curveOffset = isBidirectional ? 0.2 : 0.08;

				const nx = dx / dist;
				const ny = dy / dist;

				const perpX = -ny;
				const perpY = nx;

				const mx = (from.cx + to.cx) / 2 + perpX * dist * curveOffset;
				const my = (from.cy + to.cy) / 2 + perpY * dist * curveOffset;

				const p1 = rectEdgeIntersection(from.cx, from.cy, mx, my, NODE_W, NODE_H);
				const p2 = rectEdgeIntersection(to.cx, to.cy, mx, my, NODE_W, NODE_H);

				result.push({
					from: node.name,
					to: target,
					path: `M ${p1.x} ${p1.y} Q ${mx} ${my} ${p2.x} ${p2.y}`,
					labelX: mx,
					labelY: my,
				});
			}
		}
		return result;
	});
</script>

{#if !structureLoaded}
	<div class="flex items-center justify-center h-40 text-muted-foreground text-sm">
		Loading state machine...
	</div>
{:else if nodes.length === 0}
	<div class="flex items-center justify-center h-40 text-muted-foreground text-sm">
		No states found for "{statemachineName}"
	</div>
{:else}
	<div class="w-full overflow-auto rounded-lg">
		<svg
			viewBox="0 0 {svgWidth} {svgHeight}"
			class="w-full h-auto min-h-[200px]"
			style="max-height: 600px;"
			xmlns="http://www.w3.org/2000/svg"
		>
			<defs>
				<marker id="smg-arrow" markerWidth="8" markerHeight="6" refX="7" refY="3" orient="auto" markerUnits="strokeWidth">
					<path d="M 0 0 L 8 3 L 0 6 Z" fill="var(--muted-foreground)" opacity="0.5" />
				</marker>
				<marker id="smg-arrow-active" markerWidth="8" markerHeight="6" refX="7" refY="3" orient="auto" markerUnits="strokeWidth">
					<path d="M 0 0 L 8 3 L 0 6 Z" fill="var(--primary)" />
				</marker>
				<filter id="smg-shadow" x="-10%" y="-10%" width="120%" height="130%">
					<feDropShadow dx="0" dy="1" stdDeviation="2" flood-opacity="0.1" />
				</filter>
				<filter id="smg-glow" x="-20%" y="-20%" width="140%" height="140%">
					<feDropShadow dx="0" dy="0" stdDeviation="4" flood-color="var(--primary)" flood-opacity="0.3" />
				</filter>
			</defs>

			{#each edges as edge (edge.from + '->' + edge.to)}
				{@const isActive = edge.from === activeState}
				<path
					d={edge.path}
					fill="none"
					stroke="var(--muted-foreground)"
					stroke-opacity={isActive ? 1 : 0.35}
					stroke-width={isActive ? 2 : 1.5}
					marker-end={isActive ? 'url(#smg-arrow-active)' : 'url(#smg-arrow)'}
					style={isActive ? 'stroke: var(--primary);' : ''}
				/>
			{/each}

			{#each nodes as node (node.name)}
				{@const isActive = node.name === activeState}
				<g filter={isActive ? 'url(#smg-glow)' : 'url(#smg-shadow)'}>
					<rect
						x={node.x}
						y={node.y}
						width={NODE_W}
						height={NODE_H}
						rx={NODE_RX}
						fill={isActive ? 'var(--primary)' : 'var(--card)'}
						stroke={isActive ? 'var(--primary)' : 'var(--border)'}
						stroke-width={isActive ? 2 : 1}
					/>
					{#if isActive}
						<rect
							x={node.x - 4}
							y={node.y - 4}
							width={NODE_W + 8}
							height={NODE_H + 8}
							rx={NODE_RX + 3}
							fill="none"
							stroke="var(--primary)"
							stroke-opacity="0.25"
							stroke-width="2"
							stroke-dasharray="4 2"
						/>
					{/if}
					<text
						x={node.x + NODE_W / 2}
						y={node.y + NODE_H / 2}
						text-anchor="middle"
						dominant-baseline="central"
						fill={isActive ? 'var(--primary-foreground)' : 'var(--foreground)'}
						font-size="12"
						font-weight={isActive ? '600' : '500'}
						font-family="system-ui, sans-serif"
					>
						{node.name}
					</text>
				</g>
			{/each}
		</svg>
	</div>
{/if}
