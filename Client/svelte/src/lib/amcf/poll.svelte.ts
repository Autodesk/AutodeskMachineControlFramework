import { getContext, setContext } from 'svelte';
import { createSubscriber } from 'svelte/reactivity';

const TICK_KEY = Symbol('amcf-poll-tick');
const MODULE_CHANGES_KEY = Symbol('amcf-module-changes');

export interface PollTick {
	v: number;
}

/*
 * Application-wide tick: bumped on navigation, status and visibility changes and after
 * asynchronous loads. Every reader re-evaluates.
 */
export function initPollTick (): PollTick {
	const ref: PollTick = $state({ v: 0 });
	setContext(TICK_KEY, ref);
	return ref;
}

export function usePollTick (): PollTick {
	return getContext<PollTick>(TICK_KEY) ?? { v: 0 };
}

/*
 * Per-module change notifications, fed by the core with the uuids of the modules whose
 * pushed state changed, so that one changed attribute only re-renders the affected modules.
 */
export class ModuleChanges {
	#listeners = new Map<string, Set<() => void>>();

	listen (uuid: string, update: () => void): () => void {
		const listeners = this.#listeners.get(uuid) ?? new Set<() => void>();
		this.#listeners.set(uuid, listeners);
		listeners.add(update);

		return () => {
			listeners.delete(update);
			if (listeners.size === 0) this.#listeners.delete(uuid);
		};
	}

	notify (uuids: Iterable<string>) {
		for (const uuid of uuids) {
			const listeners = this.#listeners.get(uuid);
			if (listeners) {
				for (const update of [...listeners]) update();
			}
		}
	}
}

export function initModuleChanges (): ModuleChanges {
	const changes = new ModuleChanges();
	setContext(MODULE_CHANGES_KEY, changes);
	return changes;
}

/*
 * Tick of one module component. Reading `v` re-evaluates when the module's own state changes
 * and on every application-wide tick.
 */
export function useModuleTick (getModule: () => { uuid?: string } | null | undefined): PollTick {
	const appTick = usePollTick();
	const changes = getContext<ModuleChanges | undefined>(MODULE_CHANGES_KEY);

	const subscribe = createSubscriber((update) => {
		const uuid = getModule()?.uuid;
		if (!changes || !uuid) return;
		return changes.listen(uuid, update);
	});

	return {
		get v () {
			subscribe();
			return appTick.v;
		}
	} as PollTick;
}
