<script lang="ts">
	import Home from '@lucide/svelte/icons/house';
	import LayoutDashboard from '@lucide/svelte/icons/layout-dashboard';
	import Settings from '@lucide/svelte/icons/settings';
	import LogOut from '@lucide/svelte/icons/log-out';
	import UserIcon from '@lucide/svelte/icons/user';
	import FileIcon from '@lucide/svelte/icons/file';
	import FolderIcon from '@lucide/svelte/icons/folder';
	import BarChart from '@lucide/svelte/icons/bar-chart-2';
	import Play from '@lucide/svelte/icons/play';
	import Square from '@lucide/svelte/icons/square';
	import Pause from '@lucide/svelte/icons/pause';
	import RefreshCw from '@lucide/svelte/icons/refresh-cw';
	import X from '@lucide/svelte/icons/x';
	import Check from '@lucide/svelte/icons/check';
	import AlertTriangle from '@lucide/svelte/icons/triangle-alert';
	import Info from '@lucide/svelte/icons/info';
	import Download from '@lucide/svelte/icons/download';
	import Upload from '@lucide/svelte/icons/upload';
	import Trash2 from '@lucide/svelte/icons/trash-2';
	import Pencil from '@lucide/svelte/icons/pencil';
	import Plus from '@lucide/svelte/icons/plus';
	import Minus from '@lucide/svelte/icons/minus';
	import Search from '@lucide/svelte/icons/search';
	import Eye from '@lucide/svelte/icons/eye';
	import Monitor from '@lucide/svelte/icons/monitor';
	import Wrench from '@lucide/svelte/icons/wrench';
	import Layers from '@lucide/svelte/icons/layers';
	import Box from '@lucide/svelte/icons/box';
	import List from '@lucide/svelte/icons/list';
	import Table from '@lucide/svelte/icons/table';
	import ImageIcon from '@lucide/svelte/icons/image';
	import Video from '@lucide/svelte/icons/video';
	import Camera from '@lucide/svelte/icons/camera';
	import Bell from '@lucide/svelte/icons/bell';
	import Printer from '@lucide/svelte/icons/printer';
	import Power from '@lucide/svelte/icons/power';
	import CircleDot from '@lucide/svelte/icons/circle-dot';
	import Gauge from '@lucide/svelte/icons/gauge';
	import Workflow from '@lucide/svelte/icons/workflow';
	import Server from '@lucide/svelte/icons/server';
	import Activity from '@lucide/svelte/icons/activity';
	import Cpu from '@lucide/svelte/icons/cpu';
	import Zap from '@lucide/svelte/icons/zap';
	import Shield from '@lucide/svelte/icons/shield';
	import Clock from '@lucide/svelte/icons/clock';
	import Calendar from '@lucide/svelte/icons/calendar';
	import Map from '@lucide/svelte/icons/map';
	import Navigation from '@lucide/svelte/icons/navigation';
	import Send from '@lucide/svelte/icons/send';
	import Mail from '@lucide/svelte/icons/mail';
	import Terminal from '@lucide/svelte/icons/terminal';
	import Code from '@lucide/svelte/icons/code';
	import Database from '@lucide/svelte/icons/database';
	import HardDrive from '@lucide/svelte/icons/hard-drive';
	import Wifi from '@lucide/svelte/icons/wifi';
	import CircleIcon from '@lucide/svelte/icons/circle';

	let { icon = '', class: className = 'h-4 w-4' }: { icon?: string; class?: string } = $props();

	const mdiMap: Record<string, any> = {
		'mdi-home': Home,
		'mdi-home-outline': Home,
		'mdi-view-dashboard': LayoutDashboard,
		'mdi-view-dashboard-outline': LayoutDashboard,
		'mdi-cog': Settings,
		'mdi-cog-outline': Settings,
		'mdi-settings': Settings,
		'mdi-logout': LogOut,
		'mdi-account': UserIcon,
		'mdi-account-outline': UserIcon,
		'mdi-account-circle': UserIcon,
		'mdi-file': FileIcon,
		'mdi-file-outline': FileIcon,
		'mdi-file-document': FileIcon,
		'mdi-file-document-outline': FileIcon,
		'mdi-folder': FolderIcon,
		'mdi-folder-outline': FolderIcon,
		'mdi-chart-bar': BarChart,
		'mdi-chart-line': BarChart,
		'mdi-poll': BarChart,
		'mdi-play': Play,
		'mdi-play-circle': Play,
		'mdi-play-circle-outline': Play,
		'mdi-stop': Square,
		'mdi-stop-circle': Square,
		'mdi-pause': Pause,
		'mdi-pause-circle': Pause,
		'mdi-refresh': RefreshCw,
		'mdi-reload': RefreshCw,
		'mdi-close': X,
		'mdi-close-circle': X,
		'mdi-check': Check,
		'mdi-check-circle': Check,
		'mdi-alert': AlertTriangle,
		'mdi-alert-outline': AlertTriangle,
		'mdi-alert-circle': AlertTriangle,
		'mdi-alert-circle-outline': AlertTriangle,
		'mdi-information': Info,
		'mdi-information-outline': Info,
		'mdi-download': Download,
		'mdi-upload': Upload,
		'mdi-delete': Trash2,
		'mdi-delete-outline': Trash2,
		'mdi-trash-can': Trash2,
		'mdi-pencil': Pencil,
		'mdi-pencil-outline': Pencil,
		'mdi-plus': Plus,
		'mdi-plus-circle': Plus,
		'mdi-minus': Minus,
		'mdi-minus-circle': Minus,
		'mdi-magnify': Search,
		'mdi-eye': Eye,
		'mdi-eye-outline': Eye,
		'mdi-monitor': Monitor,
		'mdi-desktop-mac': Monitor,
		'mdi-wrench': Wrench,
		'mdi-wrench-outline': Wrench,
		'mdi-tools': Wrench,
		'mdi-layers': Layers,
		'mdi-layers-outline': Layers,
		'mdi-cube': Box,
		'mdi-cube-outline': Box,
		'mdi-format-list-bulleted': List,
		'mdi-format-list-numbered': List,
		'mdi-view-list': List,
		'mdi-table': Table,
		'mdi-table-large': Table,
		'mdi-image': ImageIcon,
		'mdi-image-outline': ImageIcon,
		'mdi-video': Video,
		'mdi-video-outline': Video,
		'mdi-camera': Camera,
		'mdi-camera-outline': Camera,
		'mdi-bell': Bell,
		'mdi-bell-outline': Bell,
		'mdi-printer': Printer,
		'mdi-printer-3d': Printer,
		'mdi-printer-3d-nozzle': Printer,
		'mdi-power': Power,
		'mdi-power-standby': Power,
		'mdi-record-circle': CircleDot,
		'mdi-gauge': Gauge,
		'mdi-speedometer': Gauge,
		'mdi-state-machine': Workflow,
		'mdi-sitemap': Workflow,
		'mdi-server': Server,
		'mdi-server-network': Server,
		'mdi-pulse': Activity,
		'mdi-heart-pulse': Activity,
		'mdi-chip': Cpu,
		'mdi-memory': Cpu,
		'mdi-flash': Zap,
		'mdi-lightning-bolt': Zap,
		'mdi-shield': Shield,
		'mdi-shield-outline': Shield,
		'mdi-clock': Clock,
		'mdi-clock-outline': Clock,
		'mdi-timer': Clock,
		'mdi-calendar': Calendar,
		'mdi-calendar-outline': Calendar,
		'mdi-map': Map,
		'mdi-map-outline': Map,
		'mdi-map-marker': Navigation,
		'mdi-map-marker-outline': Navigation,
		'mdi-send': Send,
		'mdi-email': Mail,
		'mdi-email-outline': Mail,
		'mdi-console': Terminal,
		'mdi-console-line': Terminal,
		'mdi-code-tags': Code,
		'mdi-xml': Code,
		'mdi-database': Database,
		'mdi-database-outline': Database,
		'mdi-harddisk': HardDrive,
		'mdi-wifi': Wifi,
		'mdi-laser-pointer': Zap,
	};

	let Icon = $derived(mdiMap[icon] || null);
</script>

{#if Icon}
	{@const ResolvedIcon = Icon}
	<ResolvedIcon class={className} />
{:else if icon}
	<CircleIcon class={className} />
{/if}
