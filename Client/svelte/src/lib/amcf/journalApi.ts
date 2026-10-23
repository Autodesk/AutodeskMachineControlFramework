// Service layer for the AMCF process-parameter journal history REST API (/api/journal/*).
//
// All requests go through the shared AMCApplication axios wrapper so that the
// bearer token and base URL handling stay consistent with the rest of the client.

export interface JournalInfo {
	startTime: string;
	lifetimeInMicroseconds: number;
	currentTime: number;
}

export interface JournalVariable {
	name: string;
	type: string;
	units: number;
}

export interface EnvelopeBucket {
	t: number; // microseconds, relative to journal start
	min: number;
	max: number;
	avg: number;
	last: number;
}

export interface EnvelopeSeries {
	name: string;
	available: boolean;
	buckets: EnvelopeBucket[];
}

export interface EnvelopeResult {
	startTimeStamp: number;
	endTimeStamp: number;
	series: EnvelopeSeries[];
}

// Retrieves metadata about the current live journal session.
export async function getJournalInfo (app: any): Promise<JournalInfo> {
	const response = await app.axiosGetRequest('/journal/current');
	const data = response.data || {};
	return {
		startTime: data.starttime || '',
		lifetimeInMicroseconds: Number(data.lifetimeinmicroseconds || 0),
		currentTime: Number(data.currenttime || 0)
	};
}

// Retrieves the list of variables recorded in the current live journal.
export async function getVariables (app: any): Promise<JournalVariable[]> {
	const response = await app.axiosGetRequest('/journal/variables');
	const variables = (response.data && response.data.variables) || [];
	return variables.map((entry: any) => ({
		name: String(entry.name),
		type: String(entry.type),
		units: Number(entry.units || 0)
	}));
}

// Retrieves downsampled min/max/avg envelopes for one or more variables over a
// time range. `count` should be roughly the pixel width of the chart so that the
// server produces exactly the resolution needed (multi-scale).
export async function getEnvelope (
	app: any,
	variables: string[],
	startTimeStamp: number,
	endTimeStamp: number,
	count: number
): Promise<EnvelopeResult> {
	if (variables.length === 0)
		return { startTimeStamp, endTimeStamp, series: [] };

	const encodedVariables = variables.map((name) => encodeURIComponent(name)).join(',');
	const query =
		`/journal/envelope?variables=${encodedVariables}` +
		`&starttimestamp=${Math.floor(startTimeStamp)}` +
		`&endtimestamp=${Math.floor(endTimeStamp)}` +
		`&count=${Math.floor(count)}`;

	const response = await app.axiosGetRequest(query);
	const data = response.data || {};

	const series: EnvelopeSeries[] = ((data.series as any[]) || []).map((entry) => ({
		name: String(entry.name),
		available: entry.available !== false,
		buckets: ((entry.buckets as any[]) || []).map((bucket) => ({
			t: Number(bucket.t),
			min: Number(bucket.min),
			max: Number(bucket.max),
			avg: Number(bucket.avg),
			last: Number(bucket.last)
		}))
	}));

	return {
		startTimeStamp: Number(data.starttimestamp || startTimeStamp),
		endTimeStamp: Number(data.endtimestamp || endTimeStamp),
		series
	};
}
