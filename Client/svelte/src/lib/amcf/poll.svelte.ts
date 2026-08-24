import { getContext, setContext } from 'svelte';

const TICK_KEY = Symbol('amcf-poll-tick');

export interface PollTick {
	v: number;
}

export function initPollTick (): PollTick {
	const ref: PollTick = $state({ v: 0 });
	setContext(TICK_KEY, ref);
	return ref;
}

export function usePollTick (): PollTick {
	return getContext<PollTick>(TICK_KEY) ?? { v: 0 };
}
