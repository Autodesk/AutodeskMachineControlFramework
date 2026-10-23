/*++

Copyright (C) 2020 Autodesk Inc.

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
 * Neither the name of the Autodesk Inc. nor the
names of its contributors may be used to endorse or promote products
derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL AUTODESK INC. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 */

import axios from "axios";
import { sha256Hex } from "./AMCCrypto.js";

import * as Assert from "./AMCAsserts.js";
import * as Common from "./AMCCommon.js"
import * as GitHash from "./AMCGitHash.js"

import AMCApplicationModule_Content from "../modules/AMCModule_Content.js"
import AMCApplicationModule_GLScene from "../modules/AMCModule_GLScene.js"
import AMCApplicationModule_Graphic from "../modules/AMCModule_Graphic.js"
import AMCApplicationModule_Grid from "../modules/AMCModule_Grid.js"
import AMCApplicationModule_Tabs from "../modules/AMCModule_Tabs.js"
import AMCApplicationModule_Logs from "../modules/AMCModule_Logs.js"
import AMCApplicationModule_LayerView from "../modules/AMCModule_LayerView.js"
import AMCApplicationModule_Custom from "../modules/AMCModule_Custom.js"
import AMCApplicationModule_ParameterList from "../modules/AMCModule_ParameterList.js"
import AMCApplicationModule_StatusList from "../modules/AMCModule_StatusList.js"
import AMCApplicationModule_TogglePanel from "../modules/AMCModule_TogglePanel.js"
import AMCApplicationModule_Form from "../modules/AMCModule_Form.js"
import AMCApplicationModule_Chart from "../modules/AMCModule_Chart.js"
import AMCApplicationModule_JournalHistory from "../modules/AMCModule_JournalHistory.js"
import AMCApplicationModule_Image from "../modules/AMCModule_Image.js"
import AMCApplicationModule_StateMachineGraph from "../modules/AMCModule_StateMachineGraph.js"
import AMCApplicationModule_Paragraph from "../modules/AMCModule_Paragraph.js"
import AMCApplicationModule_Upload from "../modules/AMCModule_Upload.js"
import AMCApplicationModule_BuildList from "../modules/AMCModule_BuildList.js"
import AMCApplicationModule_PartList from "../modules/AMCModule_PartList.js"
import AMCApplicationModule_ExecutionList from "../modules/AMCModule_ExecutionList.js"
import AMCApplicationModule_AlertList from "../modules/AMCModule_AlertList.js"
import AMCApplicationModule_ButtonGroup from "../modules/AMCModule_ButtonGroup.js"
import AMCApplicationModule_ConfigurationList from "../modules/AMCModule_ConfigurationList.js"
import AMCApplicationModule_VideoStream from "../modules/AMCModule_VideoStream.js"
import AMCApplicationModule_Separator from "../modules/AMCModule_Separator.js"
import AMCApplicationModule_StatusBanner from "../modules/AMCModule_StatusBanner.js"
import AMCApplicationModule_Workflow from "../modules/AMCModule_Workflow.js"

import { validateModuleJSON } from "./AMCModuleSchema.js"

import AMCApplicationPage from "./AMCPage.js"
import AMCApplicationCustomPage from "./AMCCustomPage.js"
import AMCUpload from "./AMCImplementation_Upload.js"

import AMCApplicationDialog from "./AMCDialog.js"

const CONFIG_REQUEST_TIMEOUT_MS = 2000;
const DEFAULT_REQUEST_TIMEOUT_MS = 10000;
const MAX_CONSECUTIVE_FAILURES = 5;

// Client reactivity metrics: aggregate /api/frontend roundtrips over this window
// (see SSE reactivity plan, Phase 0) and push a single summary to the backend.
const FRONTEND_METRICS_WINDOW_MS = 5000;

// Attribute names of frontend patches are written onto plain objects and must not reach their prototype.
const FRONTEND_PATCH_FORBIDDEN_NAMES = new Set (["__proto__", "constructor", "prototype"]);

// Live frontend updates: one SSE stream per tab, with the "since" polling endpoint as fallback.
const FRONTEND_POLL_INTERVAL_MS = 600;
const FRONTEND_STREAM_MAX_CONNECT_FAILURES = 3;
const FRONTEND_STREAM_RETRY_BASE_MS = 1000;
const FRONTEND_STREAM_RETRY_MAX_MS = 15000;
const FRONTEND_STREAM_RECOVERY_INTERVAL_MS = 30000;
// Data that is not part of the frontend status (parameter list values) is refreshed by its module.
const FRONTEND_MODULE_REFRESH_INTERVAL_MS = 600;
const FRONTEND_UNTRACKED_REQUEST_PREFIXES = ["/frontend", "/journal"];

export default class AMCApplication extends Common.AMCObject {
	

    constructor(apiBaseURL, uiResizeEvent) {
		
		super ();
		this.registerClass ("amcApplication");
		
		console.log ("AMC Client git hash: " + GitHash.getClientGitHash ());
				
        this.API = {
            baseURL: apiBaseURL,
            authToken: Common.nullToken (),
            unsuccessfulUpdateCounter: 0,
			unsuccessfulFrontendCounter: 0,
			frontendState: null,
			frontendLookup: {},
			frontendTargets: new Map (),
			frontendStoreOwners: new Map (),
			frontendRevision: 0,
			frontendScope: "",
			frontendRequestSerial: 0,
			frontendAppliedSerial: 0,
			// /api/frontend requests sent by retrieveFrontendState that have not settled yet.
			frontendRequestsInFlight: 0,
			navigationRequestSerial: 0,
			frontendMetrics: this._createEmptyFrontendMetrics (),
			userUUID: Common.nullUUID (),
			userLogin: "",
			userDescription: "",
			userRole: "",
			userLanguage: "",
			userPermissions: new Set ()
        }

        this.AppState = {
            currentStatus: "initial", // one of "initial" / "login" / "ready" / "error",
            currentError: "",
            activePage: "",
			appResizeEvent: uiResizeEvent,
            WebGLInstances: new Map()
        }

        this.AppDefinition = {
            TextApplicationName: "",
			ToolbarLogoUUID: "",
            TextCopyRight: "",
            MainPage: "",
            LogoUUID: "",
            DarkLogoUUID: "",
            LogoAspectRatio: 1.0,
			LoginBackgroundImageUUID: "",
			DarkLoginBackgroundImageUUID: "",
			LoginWelcomeMessage: "",
			LoginStyle: "classic",
			LoginSubtitle: "",
			LoginPanelUUID: "",
			FirstLaunchMode: false,
            Colors: {},
            DarkColors: {},
            DefaultTheme: "light"
        }

        this.AppContent = {
            MenuItems: [],
            ToolbarItems: [],
            Pages: [],
            CustomPages: [],
            Dialogs: [],
            PageMap: new Map(),
            CustomPageMap: new Map(),
            DialogMap: new Map(),
			ModuleMap: new Map(),
			ItemMap: new Map(),			
            FormEntityMap: new Map()
        }
		
        this.SnackBar = {
			Visible: false,
			Timeout: -1,
			Text: "",
			Color: "secondary",
			FontColor: "white"			
		}

		// mode is one of "off" / "connecting" / "stream" / "polling".
		this.FrontendUpdates = {
			// Identifies this tab within the session's event stream, which serves each tab its active pages and dialogs.
			clientID: this._createFrontendClientID (),
			enabled: false,
			mode: "off",
			generation: 0,
			source: null,
			connectionHasEvents: false,
			connectFailures: 0,
			revision: 0,
			pendingEvents: null,
			reconnectTimer: null,
			pollTimer: null,
			moduleRefreshTimer: null,
			changeListener: null,
			changeNotificationPending: false
		}

    }

    setStatus(newStatus) {
        this.AppState.currentStatus = newStatus;
        this.AppState.currentError = "";
		if (this.AppState.appResizeEvent)
			this.AppState.appResizeEvent ();
    }

    setStatusToError(message) {
        this.AppState.currentStatus = "error";
        this.AppState.currentError = message || "An unknown error occurred.";
        this.closeAllDialogs();
    }

    extractErrorMessage(err) {
        if (!err) return "An unknown error occurred.";
        if (err.code === "ECONNABORTED")
            return "The server did not respond in time. Please ensure the server is running and reachable.";
        if (err.response && err.response.data && err.response.data.message)
            return err.response.data.message.toString();
        if (err.message)
            return err.message.toString();
        return err.toString();
    }

    axiosGetRequest(subURL, config) {
        let headers = {}
        let authToken = this.API.authToken;

        if (authToken != Common.nullToken ())
            headers.Authorization = "Bearer " + authToken;

        return this._trackRequest(subURL, axios({
            method: "GET",
            "headers": headers,
            url: this.API.baseURL + subURL,
            timeout: DEFAULT_REQUEST_TIMEOUT_MS,
            ...config
        }));
    }

    axiosGetArrayBufferRequest(subURL) {
        let headers = {}
        let authToken = this.API.authToken;

        if (authToken != Common.nullToken ())
            headers.Authorization = "Bearer " + authToken;

        return this._trackRequest(subURL, axios({
            method: "GET",
            "headers": headers,
			"responseType": "arraybuffer",
            url: this.API.baseURL + subURL
        }));
    }


    axiosPostRequest(subURL, data, config) {
        let headers = {}
        let authToken = this.API.authToken;

        if (authToken != Common.nullToken ())
            headers.Authorization = "Bearer " + authToken;

        return this._trackRequest(subURL, axios({
            "method": "POST",
            "url": this.API.baseURL + subURL,
            "headers": headers,
            "data": data,
            timeout: DEFAULT_REQUEST_TIMEOUT_MS,
            ...config
        }));
    }

    axiosPostFormData(subURL, formData, config) {
        let headers = {
            "Content-Type": "multipart/form-data"
        }
        let authToken = this.API.authToken;

        if (authToken != Common.nullToken ())
            headers.Authorization = "Bearer " + authToken;

        return this._trackRequest(subURL, axios({
            "method": "POST",
            "url": this.API.baseURL + subURL,
            "headers": headers,
            "data": formData,
            ...config
        }));
    }

    // Responses of other requests update state asynchronously (e.g. list rows loaded after a head id
    // change), which frontends without deep reactivity only see through a change notification.
    // Frontend status, stream and parameter list requests report their changes themselves, and
    // journal queries only feed the state of the component that issued them.
    _trackRequest(subURL, request) {
        let path = String(subURL);
        if (!FRONTEND_UNTRACKED_REQUEST_PREFIXES.some(prefix => path.startsWith(prefix))) {
            let notify = () => this._scheduleFrontendChangeNotification();
            request.then(notify, notify);
        }
        return request;
    }

    // Retrieves a generic per-user preference stored under (domain, key).
    // Resolves to the parsed JSON document, or null when nothing is stored.
    // Never rejects on "not found"; only genuine transport errors reject.
    async getUserPreference (domain, key) {
        let response = await this.axiosGetRequest ("/userpreferences", {
            params: { domain: domain, key: key }
        });

        if (!response || !response.data || response.data.exists !== true)
            return null;

        let rawValue = response.data.value;
        if (typeof rawValue !== "string" || rawValue.length === 0)
            return null;

        try {
            return JSON.parse (rawValue);
        } catch (parseError) {
            // Stored value is not valid JSON; treat as absent rather than throwing.
            return null;
        }
    }

    // Stores a generic per-user preference document under (domain, key). The
    // document is serialized to JSON before being sent to the backend.
    storeUserPreference (domain, key, valueDocument) {
        let serialized = JSON.stringify (valueDocument === undefined ? null : valueDocument);
        return this.axiosPostRequest ("/userpreferences", {
            "domain": domain,
            "key": key,
            "value": serialized
        });
    }

    // Removes a generic per-user preference stored under (domain, key).
    deleteUserPreference (domain, key) {
        return this.axiosPostRequest ("/userpreferences/delete", {
            "domain": domain,
            "key": key
        });
    }

    retrieveConfiguration(vuetifythemes) {
        this.axiosGetRequest("/config", { timeout: CONFIG_REQUEST_TIMEOUT_MS })
        .then(resultJSON => {
            this.AppDefinition.TextApplicationName = resultJSON.data.appname;
            this.AppDefinition.TextCopyRight = resultJSON.data.copyright;
            this.AppDefinition.MainPage = resultJSON.data.mainpage;
            this.AppDefinition.LogoUUID = resultJSON.data.logouuid;
            this.AppDefinition.DarkLogoUUID = resultJSON.data.darklogouuid || "";
            this.AppDefinition.LoginBackgroundImageUUID = resultJSON.data.loginbackgrounduuid;
            this.AppDefinition.DarkLoginBackgroundImageUUID = resultJSON.data.darkloginbackgrounduuid || "";
			this.AppDefinition.ToolbarLogoUUID = resultJSON.data.toolbarlogouuid;
			this.AppDefinition.LoginWelcomeMessage = resultJSON.data.loginwelcomemessage;
			this.AppDefinition.LoginStyle = resultJSON.data.loginstyle || "classic";
			this.AppDefinition.LoginSubtitle = resultJSON.data.loginsubtitle || "";
			this.AppDefinition.LoginPanelUUID = resultJSON.data.loginpaneluuid || "";

            this.AppDefinition.LogoAspectRatio = resultJSON.data.logoaspectratio;
            if (resultJSON.data.colors) {
                this.AppDefinition.Colors = resultJSON.data.colors;
            } else {
                this.AppDefinition.Colors = {};
            }
            this.AppDefinition.DarkColors = resultJSON.data.darkcolors || {};
            this.AppDefinition.DefaultTheme = resultJSON.data.defaulttheme || "light";
            this.setStatus("login");

            document.title = this.AppDefinition.TextApplicationName;

            if (vuetifythemes) {
                if (this.AppDefinition.Colors.primary) {
                    vuetifythemes.light.primary = this.AppDefinition.Colors.primary;
                    vuetifythemes.dark.primary = this.AppDefinition.Colors.primary;
                }
                if (this.AppDefinition.Colors.secondary) {
                    vuetifythemes.light.secondary = this.AppDefinition.Colors.secondary;
                    vuetifythemes.dark.secondary = this.AppDefinition.Colors.secondary;
                }
                if (this.AppDefinition.Colors.accent) {
                    vuetifythemes.light.accent = this.AppDefinition.Colors.accent;
                    vuetifythemes.dark.accent = this.AppDefinition.Colors.accent;
                }
                if (this.AppDefinition.Colors.error) {
                    vuetifythemes.light.error = this.AppDefinition.Colors.error;
                    vuetifythemes.dark.error = this.AppDefinition.Colors.error;
                }
            }

            this.changePage(this.AppDefinition.MainPage);
        })
        .catch(err => {
            this.setStatusToError(this.extractErrorMessage(err));
        });
    }

    performLogout() {
		this.stopFrontendUpdates ();
        this.API.authToken = Common.nullToken ();
        this.API.unsuccessfulUpdateCounter = 0;
		this.API.userUUID = Common.nullUUID ();
		this.API.userLogin = "";
		this.API.userDescription = "";
		this.API.userRole = "";
		this.API.userLanguage = "";
		this.API.userPermissions = new Set ();
    }

    requestLogin(userName, userPassword) {

		this.performLogout ();

        this.axiosPostRequest("/auth/", {
            "username": userName
        })
        .then(async resultCreateSession => {

            let sessionuuid = resultCreateSession.data.sessionuuid;
            let sessionkey = resultCreateSession.data.sessionkey;
            let loginsalt = resultCreateSession.data.loginsalt;
            let clientkey = sessionkey;

            let saltedpassword = await sha256Hex(loginsalt + userPassword);
            let clientkeyhash = await sha256Hex(clientkey + saltedpassword);
            let sessionkeyhash = await sha256Hex(sessionkey + clientkeyhash);

            let resultAuthenticate = await this.axiosPostRequest("/auth/" + sessionuuid, {
                "clientkey": clientkey,
                "password": sessionkeyhash
            });

			this.API.authToken = Assert.SHA256Value (resultAuthenticate.data.token);
			this.API.userUUID = Assert.UUIDValue (resultAuthenticate.data.useruuid);
			this.API.userLogin = Assert.IdentifierString (resultAuthenticate.data.userlogin);
			this.API.userDescription = Assert.StringValue (resultAuthenticate.data.userdescription);
			this.API.userRole = Assert.IdentifierString (resultAuthenticate.data.userrole);
			//this.API.userLanguage = Assert.IdentifierString (resultAuthenticate.data.userlanguage);
			this.API.userPermissions = new Set ();

			let permissionArray = Assert.ArrayValue (resultAuthenticate.data.userpermissions);
			for (let permission of permissionArray) {
				this.API.userPermissions.add (permission);
			}

			this.setStatus("ready");
			this.retrieveStateUpdate();

        })
        .catch(err => {
            this.setStatusToError(this.extractErrorMessage(err));
        });

    }
	
	
	// Leaf modules that were historically content items and can now be
	// represented as first-class v2 modules.
	_isLeafModuleType (moduleType)
	{
		return (moduleType === "paragraph") || (moduleType === "image") || (moduleType === "chart") || (moduleType === "videostream") ||
			(moduleType === "upload") || (moduleType === "buildlist") || (moduleType === "partlist") || (moduleType === "executionlist") ||
			(moduleType === "alertlist") || (moduleType === "buttongroup") || (moduleType === "parameterlist") ||
			(moduleType === "statuslist") || (moduleType === "togglepanel") ||
			(moduleType === "configurationlist") || (moduleType === "form") || (moduleType === "workflow") || (moduleType === "statemachinegraph");
	}

	// Detect whether a JSON object uses the v2 frontend format.
	_isV2Format (json)
	{
		return (json.moduletype !== undefined && json.type === undefined);
	}

	// Normalize a v2 frontend JSON node into the legacy shape that
	// existing module/item constructors expect.  The conversion is
	// recursive so that nested submodules (tabs, grid sections) are
	// also normalized.
	_normalizeV2ToLegacy (v2)
	{
		if (!v2 || !v2.moduletype) {
			if (v2 && typeof v2 === 'object' && !v2.moduletype && !v2.type)
				console.warn ('[AMC] _normalizeV2ToLegacy: payload has neither moduletype nor type', v2);
			return v2;
		}
		if (!v2.uuid)
			console.warn ('[AMC] _normalizeV2ToLegacy: v2 payload for type "' + v2.moduletype + '" is missing uuid');

		let legacy = {};
		let moduleType = v2.moduletype;

		legacy.type = moduleType;
		legacy.uuid = v2.uuid || "";
		let rawName = (v2.attributes && v2.attributes.name) ? v2.attributes.name : (v2.name || v2.uuid || "");
		legacy.name = /^\d/.test(rawName) ? '_' + rawName : rawName;
		legacy.caption = (v2.attributes && v2.attributes.caption !== undefined) ? v2.attributes.caption : "";

		if (v2.gridcolumn !== undefined)
			legacy.gridcolumn = v2.gridcolumn;
		if (v2.gridrow !== undefined)
			legacy.gridrow = v2.gridrow;
		if (v2.gridcolumnspan !== undefined)
			legacy.gridcolumnspan = v2.gridcolumnspan;
		if (v2.gridrowspan !== undefined)
			legacy.gridrowspan = v2.gridrowspan;

		let attrs = v2.attributes || {};
		let subs = v2.submodules || [];

		if (moduleType === "content") {
			legacy.headline = attrs.headline || "";
			legacy.title = attrs.title || "";
			legacy.subtitle = attrs.subtitle || "";
			legacy.visible = (attrs.visible === "1" || attrs.visible === true || attrs.visible === "true");
			legacy.cardstyle = attrs.cardstyle || "none";
			legacy.cardcolor = attrs.cardcolor || "";
			legacy.spacing = attrs.spacing || 0;
			legacy.elevation = attrs.elevation || 2;
			if (attrs.cssstyle) legacy.cssstyle = attrs.cssstyle;
			legacy.modules = subs.map(sub => this._normalizeV2ToLegacy(sub));

		} else if (moduleType === "tabs") {
			legacy.tabs = subs.map(sub => this._normalizeV2ToLegacy(sub));
			// Optional card framing (mirrors "content").
			legacy.cardstyle = attrs.cardstyle || "none";
			legacy.title = attrs.title || "";
			legacy.subtitle = attrs.subtitle || "";

		} else if (moduleType === "grid") {
			legacy.padding = parseInt(attrs.padding) || 0;
			if (v2.columns && v2.columns.length > 0) {
				legacy.columns = v2.columns;
			} else {
				let colCount = parseInt(attrs.columncount) || 1;
				legacy.columns = [];
				for (let i = 0; i < colCount; i++)
					legacy.columns.push({ width: 1, unit: "free" });
			}

			if (v2.rows && v2.rows.length > 0) {
				legacy.rows = v2.rows;
			} else {
				let rowCount = parseInt(attrs.rowcount) || 1;
				legacy.rows = [];
				for (let i = 0; i < rowCount; i++)
					legacy.rows.push({ height: 1, unit: "free" });
			}

			legacy.sections = subs.map(sub => {
				let section = this._normalizeV2ToLegacy(sub);
				let gc = sub.gridcolumn || 1;
				let gr = sub.gridrow || 1;
				let gcs = sub.gridcolumnspan || 1;
				let grs = sub.gridrowspan || 1;
				section.columnstart = gc;
				section.columnend = gc + gcs - 1;
				section.rowstart = gr;
				section.rowend = gr + grs - 1;
				section.scrollbars = (sub.scrollbars !== undefined) ? sub.scrollbars : (section.scrollbars || false);
				section.columnposition = sub.columnposition || section.columnposition || "stretch";
				section.rowposition = sub.rowposition || section.rowposition || "stretch";
				return section;
			});

		} else if (moduleType === "glscene") {
			legacy.scene = {
				type: "scene",
				uuid: v2.uuid || "",
				instances: Array.isArray(attrs.instances) ? attrs.instances.map(inst => Object.assign({}, inst)) : []
			};

		} else if (moduleType === "graphic") {
			legacy.viewminx = parseFloat(attrs.viewminx) || 0;
			legacy.viewminy = parseFloat(attrs.viewminy) || 0;
			legacy.viewmaxx = parseFloat(attrs.viewmaxx) || 100;
			legacy.viewmaxy = parseFloat(attrs.viewmaxy) || 100;
			legacy.showgrid = (attrs.showgrid === "1" || attrs.showgrid === true || attrs.showgrid === "true");
			legacy.items = subs.map(sub => this._normalizeV2ItemToLegacy(sub));

		} else if (moduleType === "layerview") {
			// Optional card framing (mirrors "content").
			legacy.cardstyle = attrs.cardstyle || "none";
			legacy.title = attrs.title || "";
			legacy.subtitle = attrs.subtitle || "";
			if (subs.length > 0) {
				legacy.items = subs.map(sub => this._normalizeV2ItemToLegacy(sub));
			} else if (attrs.platformuuid) {
				// v2 mode: all platform data is in the module-level attributes.
				// Synthesise a minimal platform item so the JS module can create it.
				legacy.items = [{
					type:               "platform",
					uuid:               attrs.platformuuid,
					currentlayer:       parseInt(attrs.currentlayer)  || 0,
					layercount:         parseInt(attrs.layercount)    || 0,
					builduuid:          attrs.builduuid          || "00000000-0000-0000-0000-000000000000",
					executionuuid:      attrs.executionuuid      || "00000000-0000-0000-0000-000000000000",
					scatterplotuuid:    attrs.scatterplotuuid    || "00000000-0000-0000-0000-000000000000",
					sizex:              parseFloat(attrs.sizex)  || 0,
					sizey:              parseFloat(attrs.sizey)  || 0,
					originx:            parseFloat(attrs.originx) || 0,
					originy:            parseFloat(attrs.originy) || 0,
					paddingx:           parseFloat(attrs.paddingx) || 0,
					paddingy:           parseFloat(attrs.paddingy) || 0,
					transformangle:     parseFloat(attrs.transformangle) || 0,
					rotationcenterx:    parseFloat(attrs.rotationcenterx) || 0,
					rotationcentery:    parseFloat(attrs.rotationcentery) || 0,
					translationx:       parseFloat(attrs.translationx) || 0,
					translationy:       parseFloat(attrs.translationy) || 0,
					showcoordinatesystem: (attrs.showcoordinatesystem === true || attrs.showcoordinatesystem === 1 || attrs.showcoordinatesystem === "1" || attrs.showcoordinatesystem === "true"),
					baseimageresource:  attrs.baseimageresource  || "",
					dark_baseimageresource: attrs.dark_baseimageresource || "",
					labelvisible:       (attrs.labelvisible === true || attrs.labelvisible === "1" || attrs.labelvisible === "true") ? 1 : 0,
					labelcaption:       attrs.labelcaption       || "",
					labelicon:          attrs.labelicon          || "",
					sliderchangeevent:  attrs.sliderchangeevent  || "",
					sliderfixed:        (attrs.sliderfixed === true || attrs.sliderfixed === "1" || attrs.sliderfixed === "true") ? 1 : 0,
				}];
			} else {
				legacy.items = [];
			}

		} else if (moduleType === "logs") {
			legacy.items = [];
			legacy.entries = [];

		} else if (moduleType === "custom") {
			let items = [];
			let propsItem = { type: "properties", uuid: v2.uuid || "" };
			for (let key in attrs) {
				if (key !== "name" && key !== "caption")
					propsItem[key] = attrs[key];
			}
			items.push(propsItem);

			for (let sub of subs) {
				let eventItem = this._normalizeV2ItemToLegacy(sub);
				if (eventItem.type === "event") {
					eventItem.name = (sub.attributes && sub.attributes.eventname) || "";
					if (eventItem.parameters === undefined)
						eventItem.parameters = [];
				}
				items.push(eventItem);
			}
			legacy.items = items;

		} else if (moduleType === "form") {
			legacy.visible = (attrs.visible !== undefined)
				? (attrs.visible === "1" || attrs.visible === true || attrs.visible === "true")
				: true;
			let formSubs = subs;
			if (subs.length === 1 && subs[0].moduletype === "form") {
				formSubs = subs[0].submodules || [];
			}
		legacy.entities = formSubs.map(sub => {
			let a = sub.attributes || {};
			let entity = {
				uuid:                sub.uuid,
				name:                a.name || sub.uuid,
				type:                sub.moduletype || "edit",
				caption:             a.caption || "",
				value:               a.value,
				disabled:            (a.disabled  === true || a.disabled  === "1" || a.disabled  === "true"),
				readonly:            (a.readonly   === true || a.readonly   === "1" || a.readonly   === "true"),
				changeevent:         a.changeevent         || "",
				validation:          a.validation          || "",
				validationmessage:   a.validationmessage   || "",
				minvalue:            a.minvalue,
				maxvalue:            a.maxvalue,
			};

			if ((entity.type === "combobox" || entity.type === "multiselect") && sub.submodules) {
				entity.items = sub.submodules.map(item => {
					let ia = item.attributes || {};
					return { value: String(ia.value !== undefined ? ia.value : ""), text: ia.text || String(ia.value !== undefined ? ia.value : "") };
				});
			}

			return entity;
		});

		} else if (moduleType === "buttongroup") {
			legacy.visible = true;
			let bgSubs = subs;
			let bgAttrs = attrs;
			if (subs.length === 1 && subs[0].moduletype === "buttongroup") {
				bgAttrs = subs[0].attributes || {};
				bgSubs = subs[0].submodules || [];
			}
			legacy.buttondistribution = bgAttrs.buttondistribution || attrs.buttondistribution || "";
			legacy.buttons = bgSubs.map(sub => {
				let a = sub.attributes || {};
				return {
					uuid:        sub.uuid,
					name:        sub.uuid,
					caption:     a.caption     || "",
					disabled:    (a.disabled === true || a.disabled === "1" || a.disabled === "true"),
					visible:     (a.visible === undefined) ? true : (a.visible === true || a.visible === "1" || a.visible === "true"),
					event:       a.event       || "",
					targetpage:  a.targetpage  || "",
					icon:        a.icon        || "",
					color:       a.color       || "",
				};
			});

		} else if (this._isLeafModuleType(moduleType)) {
			// Generic leaf type: flatten module-level attrs then submodule[0] attrs.
			legacy.visible = (attrs.visible !== undefined)
				? (attrs.visible === "1" || attrs.visible === true || attrs.visible === "true")
				: true;
			for (let key in attrs) {
				if (key !== "name" && key !== "caption" && key !== "visible")
					legacy[key] = attrs[key];
			}
			if (subs.length > 0 && subs[0].attributes) {
				let subAttrs = subs[0].attributes;
				for (let key in subAttrs) {
					if (legacy[key] === undefined)
						legacy[key] = subAttrs[key];
				}
			}
			// resource aliases
			if ((moduleType === "image") && (legacy.imageresource === undefined) && (legacy.resource !== undefined))
				legacy.imageresource = legacy.resource;
			if ((moduleType === "videostream") && (legacy.streamresource === undefined) && (legacy.resource !== undefined))
				legacy.streamresource = legacy.resource;

		} else {
			for (let key in attrs)
				legacy[key] = attrs[key];
			if (subs.length > 0)
				legacy.items = subs.map(sub => this._normalizeV2ItemToLegacy(sub));
		}

		return legacy;
	}

	// Normalize a v2 submodule (content item / graphic item / etc.) into a
	// flat legacy item object: { type, uuid, ...attributes }.
	_normalizeV2ItemToLegacy (v2Item)
	{
		if (!v2Item)
			return v2Item;

		let item = {};
		item.type = v2Item.moduletype || "";
		item.uuid = v2Item.uuid || "";

		let attrs = v2Item.attributes || {};
		for (let key in attrs)
			item[key] = attrs[key];

		// v2 image attributes use "resource", legacy item constructor expects "imageresource".
		if ((item.type === "image") && (item.imageresource === undefined) && (item.resource !== undefined))
			item.imageresource = item.resource;

		// Keep a compatibility alias if a v2 stream item uses "resource".
		if ((item.type === "videostream") && (item.streamresource === undefined) && (item.resource !== undefined))
			item.streamresource = item.resource;

		if (v2Item.submodules && v2Item.submodules.length > 0)
			item.submodules = v2Item.submodules;

		return item;
	}

	createModuleInstance (page, moduleDefinitionJSON)
	{
		Assert.ObjectValue (moduleDefinitionJSON);
		Assert.ObjectInstance (page, "amcPage");

		let def = moduleDefinitionJSON;
		if (this._isV2Format(def))
			def = this._normalizeV2ToLegacy(def);

		const validation = validateModuleJSON(def);
		if (!validation.valid) {
			for (const err of validation.errors)
				console.warn ('[AMC] Module validation: ' + err, def);
		}

		if (def.type === "content") 
			return new AMCApplicationModule_Content (page, def);

		if (def.type === "glscene") 
			return new AMCApplicationModule_GLScene (page, def);

		if (def.type === "graphic") 
			return new AMCApplicationModule_Graphic (page, def);

		if (def.type === "grid") 
			return new AMCApplicationModule_Grid (page, def);

		if (def.type === "tabs") 
			return new AMCApplicationModule_Tabs (page, def);

		if (def.type === "logs") 
			return new AMCApplicationModule_Logs (page, def);

		if (def.type === "layerview") 
			return new AMCApplicationModule_LayerView (page, def);

		if (def.type === "custom") 
			return new AMCApplicationModule_Custom (page, def);

		if (def.type === "parameterlist")
			return new AMCApplicationModule_ParameterList (page, def);
		if (def.type === "statuslist")
			return new AMCApplicationModule_StatusList (page, def);
		if (def.type === "togglepanel")
			return new AMCApplicationModule_TogglePanel (page, def);
		if (def.type === "form")
			return new AMCApplicationModule_Form (page, def);
		if (def.type === "chart")
			return new AMCApplicationModule_Chart (page, def);
		if (def.type === "journalhistory")
			return new AMCApplicationModule_JournalHistory (page, def);
		if (def.type === "image")
			return new AMCApplicationModule_Image (page, def);
		if (def.type === "paragraph")
			return new AMCApplicationModule_Paragraph (page, def);
		if (def.type === "upload")
			return new AMCApplicationModule_Upload (page, def);
		if (def.type === "buildlist")
			return new AMCApplicationModule_BuildList (page, def);
		if (def.type === "partlist")
			return new AMCApplicationModule_PartList (page, def);
		if (def.type === "executionlist")
			return new AMCApplicationModule_ExecutionList (page, def);
		if (def.type === "alertlist")
			return new AMCApplicationModule_AlertList (page, def);
		if (def.type === "buttongroup")
			return new AMCApplicationModule_ButtonGroup (page, def);
		if (def.type === "configurationlist")
			return new AMCApplicationModule_ConfigurationList (page, def);
		if (def.type === "videostream")
			return new AMCApplicationModule_VideoStream (page, def);
		if (def.type === "separator")
			return new AMCApplicationModule_Separator (page, def);
		if (def.type === "statusbanner")
			return new AMCApplicationModule_StatusBanner (page, def);
		if (def.type === "workflow")
			return new AMCApplicationModule_Workflow (page, def);
		if (def.type === "statemachinegraph")
			return new AMCApplicationModule_StateMachineGraph (page, def);

		return null;
		
	}
	
	
	addModule (moduleInstance) {			
		if (moduleInstance && moduleInstance.uuid) {			
			this.AppContent.ModuleMap.set (moduleInstance.uuid, moduleInstance);			
			
		}	
	}
	
	addItem (item) {
		if (item && item.uuid) {
			this.AppContent.ItemMap.set (item.uuid, item);
		}
	}
				

    retrieveStateUpdate() {

        this.axiosGetRequest("/frontend")

        .then(resultJSON => {
			
			this.AppContent.Pages = [];
			this.AppContent.CustomPages = [];
			this.AppContent.Dialogs = [];
			this.AppContent.PageMap.clear ();
			this.AppContent.CustomPageMap.clear ();
			this.AppContent.DialogMap.clear ();
			this.AppContent.ModuleMap.clear ();
			this.AppContent.ItemMap.clear ();
			this.AppContent.FormEntityMap.clear ();
			
            this.AppContent.MenuItems = resultJSON.data.menuitems;
            this.AppContent.ToolbarItems = resultJSON.data.toolbaritems;

            for (let pageJSON of resultJSON.data.pages) {
				
				let page = new AMCApplicationPage (this, pageJSON);
				this.AppContent.Pages.push (page);
                this.AppContent.PageMap.set(page.name, page);
            }

			if (resultJSON.data.custompages) {
				for (let customPageJSON of resultJSON.data.custompages) {
					
					let custompage = new AMCApplicationCustomPage (this, customPageJSON);
					
					//alert (custompage.component);
					
					this.AppContent.CustomPages.push (custompage);
					this.AppContent.CustomPageMap.set(custompage.name, custompage);

				}
			}


            for (let dialogJSON of resultJSON.data.dialogs) {
				let dialog = new AMCApplicationDialog (this, dialogJSON);
				this.AppContent.Dialogs.push (dialog);
                this.AppContent.DialogMap.set(dialog.name, dialog);
				
            }

			// The unfiltered response holds the modules of all pages and dialogs, which is the
			// structure the event stream delivers values for.
			this._setFrontendState (resultJSON.data, "*");
			this._applyFrontendUpdate (null);
			this.startFrontendUpdates ();

        })
        .catch(err => {
            this.setStatusToError(this.extractErrorMessage(err));
        });
    }

   
	
	// ====================================================================
	// Phase 1+2: Fetch v2 frontend state and build a flat UUID lookup
	// map so that static items (paragraph, image, upload) can read their
	// attributes directly from /api/frontend instead of legacy polling.
	// ====================================================================

	// The server only writes modules for these pages and dialogs; all other pages and dialogs
	// carry just their header and visibility.
	_frontendActiveScope() {
		let activePageNames = this.AppContent.Pages.concat (this.AppContent.CustomPages)
			.filter (page => this.pageIsActive (page))
			.map (page => page.name);
		let activeDialogNames = this.AppContent.Dialogs
			.filter (dialog => dialog.isActive ())
			.map (dialog => dialog.name);

		return { pages: activePageNames.join (","), dialogs: activeDialogNames.join (",") };
	}

	retrieveFrontendState() {
		if (!this.userIsLoggedIn() || this._frontendUpdatesUseStream ())
			return Promise.resolve();

		this.API.frontendRequestSerial++;
		let requestSerial = this.API.frontendRequestSerial;

		let requestStartMS = this._nowMS ();

		let requestParams = this._frontendActiveScope ();
		let requestScope = requestParams.pages + "|" + requestParams.dialogs;

		// Patches are only meaningful against a local state of the same pages and dialogs.
		if (this.API.frontendState && (this.API.frontendRevision > 0) && (this.API.frontendScope === requestScope))
			requestParams.since = this.API.frontendRevision;

		// Counted for the fallback polling (see _startFrontendPolling) and released however the request
		// ends; axiosGetRequest's default timeout bounds how long a hung request can hold polling back.
		this.API.frontendRequestsInFlight++;
		return this.axiosGetRequest("/frontend", { params: requestParams })
		.finally(() => {
			this.API.frontendRequestsInFlight--;
		})
		.then(resultJSON => {
			this._recordFrontendMetric (requestStartMS, resultJSON);

			// Polls and navigation refreshes overlap; never let an older response replace a newer one.
			if ((requestSerial < this.API.frontendAppliedSerial) || this._frontendUpdatesUseStream ())
				return;

			let data = resultJSON.data;
			if (!data)
				return;

			if (data.changed) {
				if (!this._applyFrontendPatch (data, requestScope))
					return;
			}
			else {
				this._setFrontendState (data, requestScope);
			}

			this.API.frontendAppliedSerial = requestSerial;
			this.API.unsuccessfulFrontendCounter = 0;

			this._applyFrontendVisibility(this.API.frontendState);
			// A response to a request issued before the latest navigation may carry stale
			// visibility, so it must not redirect away from a page or dialog that was just opened.
			if (requestSerial > this.API.navigationRequestSerial)
				this._enforceFrontendVisibility();
		})
		.catch(err => {
			this.API.unsuccessfulFrontendCounter = (this.API.unsuccessfulFrontendCounter || 0) + 1;
			if (this.API.unsuccessfulFrontendCounter > MAX_CONSECUTIVE_FAILURES) {
				console.warn("[v2 frontend] repeated failure:", this.extractErrorMessage(err));
			}
		});
	}

	// ====================================================================
	// Phase 0: Client reactivity measurement. Aggregate the /api/frontend
	// roundtrip durations, response payload sizes and the server-reported
	// build time over a short window, then push a single summary record to
	// POST /api/frontend/metrics for storage in the session journal.
	// ====================================================================

	_nowMS () {
		if ((typeof performance !== "undefined") && performance && (typeof performance.now === "function"))
			return performance.now ();
		return Date.now ();
	}

	_createEmptyFrontendMetrics () {
		return {
			windowStartWallMicros: 0,
			windowStartMS: 0,
			count: 0,
			sumMS: 0,
			minMS: 0,
			maxMS: 0,
			sumSqMS: 0,
			payloadSumBytes: 0,
			payloadMaxBytes: 0,
			serverBuildSumMS: 0
		};
	}

	_frontendResponsePayloadBytes (resultJSON) {
		// Prefer the transport-reported content length; fall back to re-serializing the body.
		if (resultJSON && resultJSON.headers) {
			let sContentLength = resultJSON.headers["content-length"] || resultJSON.headers["Content-Length"];
			let nContentLength = parseInt (sContentLength, 10);
			if (Number.isFinite (nContentLength) && (nContentLength >= 0))
				return nContentLength;
		}
		try {
			if (resultJSON && (resultJSON.data !== undefined))
				return JSON.stringify (resultJSON.data).length;
		}
		catch (err) {
			// Serialization is best-effort only; ignore and report zero.
		}
		return 0;
	}

	_recordFrontendMetric (requestStartMS, resultJSON) {
		let nowMS = this._nowMS ();
		let durationMS = nowMS - requestStartMS;
		if (!(durationMS >= 0))
			durationMS = 0;

		let payloadBytes = this._frontendResponsePayloadBytes (resultJSON);

		let serverBuildMS = 0;
		if (resultJSON && resultJSON.data && (typeof resultJSON.data.servertime === "number"))
			serverBuildMS = resultJSON.data.servertime;

		let metrics = this.API.frontendMetrics;
		if (metrics.count === 0) {
			metrics.windowStartWallMicros = Date.now () * 1000;
			metrics.windowStartMS = nowMS;
			metrics.minMS = durationMS;
			metrics.maxMS = durationMS;
		}
		else {
			if (durationMS < metrics.minMS)
				metrics.minMS = durationMS;
			if (durationMS > metrics.maxMS)
				metrics.maxMS = durationMS;
		}

		metrics.count++;
		metrics.sumMS += durationMS;
		metrics.sumSqMS += durationMS * durationMS;
		metrics.payloadSumBytes += payloadBytes;
		if (payloadBytes > metrics.payloadMaxBytes)
			metrics.payloadMaxBytes = payloadBytes;
		metrics.serverBuildSumMS += serverBuildMS;

		if ((nowMS - metrics.windowStartMS) >= FRONTEND_METRICS_WINDOW_MS)
			this._flushFrontendMetrics ();
	}

	_flushFrontendMetrics () {
		let metrics = this.API.frontendMetrics;
		if (metrics.count === 0)
			return;

		// Reset the accumulator immediately so a slow/failing push does not block the next window.
		this.API.frontendMetrics = this._createEmptyFrontendMetrics ();

		let windowEndWallMicros = Date.now () * 1000;

		let requestBody = {
			"label": "frontend",
			"intervalstart": metrics.windowStartWallMicros,
			"intervalend": windowEndWallMicros,
			"requestcount": metrics.count,
			"sumduration": metrics.sumMS,
			"minduration": metrics.minMS,
			"maxduration": metrics.maxMS,
			"sumsqduration": metrics.sumSqMS,
			"payloadsum": metrics.payloadSumBytes,
			"payloadmax": metrics.payloadMaxBytes,
			"serverbuildsum": metrics.serverBuildSumMS
		};

		this.axiosPostRequest ("/frontend/metrics", requestBody)
		.catch (err => {
			// Metrics are best-effort telemetry; never disrupt the UI on failure.
			console.warn ("[frontend metrics] push failed:", this.extractErrorMessage (err));
		});
	}

	// Copies the session-dependent visibility flags of the v2 frontend state onto the
	// menu items, toolbar items, pages, custom pages and dialogs.
	_applyFrontendVisibility(data) {
		if (!data)
			return;

		let applyItemVisibility = (items, itemsJSON) => {
			if (!items || !itemsJSON)
				return;
			let visibilityMap = new Map();
			for (let itemJSON of itemsJSON)
				visibilityMap.set(itemJSON.uuid, itemJSON.visible !== false);
			for (let item of items) {
				if (visibilityMap.has(item.uuid))
					item.visible = visibilityMap.get(item.uuid);
			}
		};

		let applyPageVisibility = (pageMap, pagesJSON) => {
			if (!pageMap || !pagesJSON)
				return;
			for (let pageJSON of pagesJSON) {
				let page = pageMap.get(pageJSON.name);
				if (page)
					page.visible = (pageJSON.visible !== false);
			}
		};

		applyItemVisibility(this.AppContent.MenuItems, data.menuitems);
		applyItemVisibility(this.AppContent.ToolbarItems, data.toolbaritems);
		applyPageVisibility(this.AppContent.PageMap, data.pages);
		applyPageVisibility(this.AppContent.CustomPageMap, data.custompages);
		applyPageVisibility(this.AppContent.DialogMap, data.dialogs);
	}

	// Leaves a page that became hidden and closes dialogs that became hidden.
	_enforceFrontendVisibility() {
		let activePageName = this.AppState.activePage;
		if (activePageName && !this.pageIsVisible(activePageName)) {
			let fallbackPageName = "";
			if (this.AppDefinition.MainPage && this.pageIsVisible(this.AppDefinition.MainPage)) {
				fallbackPageName = this.AppDefinition.MainPage;
			} else {
				let firstVisiblePage = this.AppContent.Pages.find(page => page.visible !== false);
				if (firstVisiblePage)
					fallbackPageName = firstVisiblePage.name;
			}

			if (fallbackPageName && fallbackPageName !== activePageName)
				this.changePage(fallbackPageName);
		}

		for (let dialog of this.AppContent.Dialogs) {
			if (dialog.dialogIsActive && dialog.visible === false)
				dialog.dialogIsActive = false;
		}
	}

	pageIsVisible(pageName) {
		let page = this.AppContent.PageMap.get(pageName) || this.AppContent.CustomPageMap.get(pageName);
		if (page)
			return page.visible !== false;
		return true;
	}

	dialogIsVisible(dialogName) {
		let dialog = this.AppContent.DialogMap.get(dialogName);
		if (dialog)
			return dialog.visible !== false;
		return true;
	}

	// Menu and toolbar items are hidden if they are hidden themselves or if their target page is hidden.
	navigationItemIsVisible(item) {
		if (!item || item.visible === false)
			return false;
		if (item.targetpage)
			return this.pageIsVisible(item.targetpage);
		return true;
	}

	_setFrontendState(data, scope) {
		this.API.frontendState = data;
		this.API.frontendScope = scope;
		this.API.frontendRevision = Number.isSafeInteger (data.revision) ? data.revision : 0;
		this._indexFrontendState (data);
	}

	// Rebuilds frontendLookup (module uuid -> module JSON), frontendTargets (store uuid -> object that
	// receives patched attributes: the item or page JSON itself, or the attributes of a module) and
	// frontendStoreOwners (module store uuid -> uuids of the module and its containers, which render it).
	_indexFrontendState(data) {
		this.API.frontendLookup = {};
		this.API.frontendTargets = new Map ();
		this.API.frontendStoreOwners = new Map ();

		for (let itemList of [data.menuitems, data.toolbaritems]) {
			if (!itemList)
				continue;
			for (let item of itemList) {
				if (item && item.uuid)
					this.API.frontendTargets.set (item.uuid, item);
			}
		}

		for (let pageList of [data.pages, data.custompages, data.dialogs]) {
			if (!pageList)
				continue;
			for (let page of pageList) {
				if (!page)
					continue;
				if (page.uuid)
					this.API.frontendTargets.set (page.uuid, page);
				if (page.modules) {
					for (let mod of page.modules) {
						this._indexFrontendModule(mod, []);
					}
				}
			}
		}
	}

	// Recursively index a v2 module (and its submodules) by UUID.
	_indexFrontendModule(mod, containerUUIDs) {
		if (!mod || !mod.uuid)
			return;

		if (!mod.attributes)
			mod.attributes = {};

		this.API.frontendLookup[mod.uuid] = mod;
		this.API.frontendTargets.set (mod.uuid, mod.attributes);

		let ownerUUIDs = containerUUIDs.concat ([mod.uuid]);
		this.API.frontendStoreOwners.set (mod.uuid, ownerUUIDs);

		if (mod.submodules) {
			for (let sub of mod.submodules) {
				this._indexFrontendModule(sub, ownerUUIDs);
			}
		}
	}

	// Applies a {revision, base, changed} response onto the local frontend state; null removes an attribute.
	// Patch values are absolute, so a patch also applies to any local revision between base and revision.
	_applyFrontendPatch(data, requestScope) {
		if (!this.API.frontendState || (requestScope !== this.API.frontendScope))
			return false;
		if (!Number.isSafeInteger (data.revision) || !Number.isSafeInteger (data.base) || !data.changed || (typeof data.changed !== "object"))
			return false;

		let localRevision = this.API.frontendRevision;
		if (data.revision < localRevision)
			return false;
		if (data.base > localRevision) {
			// The local state misses changes; the next poll fetches the full state.
			this.API.frontendRevision = 0;
			return false;
		}

		this._writeFrontendChanges (data.changed, false);

		this.API.frontendRevision = data.revision;
		return true;
	}

	// Writes store attributes onto the indexed frontend state. With replaceModuleAttributes, the listed
	// stores carry the complete attribute set of a module, so attributes that are not listed are removed.
	// Returns the uuids of the modules that render a changed store, or null if a menu item, toolbar item,
	// page or dialog changed, which affects the application as a whole.
	_writeFrontendChanges(changedStores, replaceModuleAttributes) {
		let changedModuleUUIDs = new Set ();
		let applicationChanged = false;

		for (let [uuid, attributes] of Object.entries (changedStores)) {
			let target = this.API.frontendTargets.get (uuid);
			if (!target || !attributes || (typeof attributes !== "object"))
				continue;

			let ownerUUIDs = this.API.frontendStoreOwners.get (uuid);
			if (ownerUUIDs) {
				for (let ownerUUID of ownerUUIDs)
					changedModuleUUIDs.add (ownerUUID);

				if (replaceModuleAttributes) {
					for (let name of Object.keys (target)) {
						if (!Object.prototype.hasOwnProperty.call (attributes, name))
							delete target[name];
					}
				}
			}
			else {
				applicationChanged = true;
			}

			for (let [name, value] of Object.entries (attributes)) {
				if (FRONTEND_PATCH_FORBIDDEN_NAMES.has (name))
					continue;
				if (value === null)
					delete target[name];
				else
					target[name] = value;
			}
		}

		return applicationChanged ? null : Array.from (changedModuleUUIDs);
	}

	// Pushes changed frontend state into the module and item objects and notifies the frontend.
	// moduleUUIDs limits the update to the given modules; null updates everything.
	_applyFrontendUpdate(moduleUUIDs) {
		if (moduleUUIDs === null) {
			this.updateModules ();
			this.updateContentItems ();
			this._applyFrontendVisibility (this.API.frontendState);
			this._enforceFrontendVisibility ();
		}
		else {
			for (let uuid of moduleUUIDs) {
				this.updateModule (this.AppContent.ModuleMap.get (uuid));
				this.updateContentItem (this.AppContent.ItemMap.get (uuid));
			}
		}

		this._notifyFrontendChange (moduleUUIDs);
	}


	// ====================================================================
	// Phase 4: Live frontend updates. After login, one SSE stream per tab
	// (/stream/<ticket>) pushes snapshot and patch events for the module
	// stores of all pages and dialogs. If the stream cannot be opened, the
	// client polls GET /api/frontend?since=<rev> instead and retries the
	// stream periodically.
	// ====================================================================

	// The listener is called with the uuids of the modules whose state changed, or with null if
	// the application as a whole may have changed (navigation, visibility, asynchronous loads).
	setFrontendChangeListener(listener) {
		this.FrontendUpdates.changeListener = (typeof listener === "function") ? listener : null;
	}

	_notifyFrontendChange(moduleUUIDs) {
		let listener = this.FrontendUpdates.changeListener;
		if (!listener)
			return;
		try {
			listener (moduleUUIDs);
		}
		catch (err) {
			console.warn ("[frontend updates] change listener failed:", err);
		}
	}

	// Coalesces notifications so that the handlers of a completed request run first.
	_scheduleFrontendChangeNotification() {
		if (this.FrontendUpdates.changeNotificationPending)
			return;
		this.FrontendUpdates.changeNotificationPending = true;
		setTimeout (() => {
			this.FrontendUpdates.changeNotificationPending = false;
			this._notifyFrontendChange (null);
		}, 0);
	}

	// While the event stream is used, the indexed state holds all pages and dialogs and must not be
	// replaced by the scoped responses of the polling endpoint.
	_frontendUpdatesUseStream() {
		let mode = this.FrontendUpdates.mode;
		return (mode === "stream") || (mode === "connecting");
	}

	// "off", "connecting", "stream" or "polling".
	getFrontendUpdateMode() {
		return this.FrontendUpdates.mode;
	}

	// crypto.randomUUID only exists in secure contexts, and machines are often served over plain HTTP.
	_createFrontendClientID() {
		let bytes = new Uint8Array (16);
		if ((typeof crypto !== "undefined") && crypto && (typeof crypto.getRandomValues === "function")) {
			crypto.getRandomValues (bytes);
		}
		else {
			for (let index = 0; index < bytes.length; index++)
				bytes[index] = Math.floor (Math.random () * 256);
		}

		bytes[6] = (bytes[6] & 0x0f) | 0x40;
		bytes[8] = (bytes[8] & 0x3f) | 0x80;

		let hex = Array.from (bytes, value => value.toString (16).padStart (2, "0")).join ("");
		return hex.substring (0, 8) + "-" + hex.substring (8, 12) + "-" + hex.substring (12, 16) + "-" + hex.substring (16, 20) + "-" + hex.substring (20);
	}

	// The stream only carries the modules of the active pages and dialogs; after a change of scope
	// it sends a snapshot with the newly active modules.
	_reportFrontendStreamScope() {
		let updates = this.FrontendUpdates;
		let scope = this._frontendActiveScope ();
		return this.axiosPostRequest ("/frontend/streamscope", { client: updates.clientID, pages: scope.pages, dialogs: scope.dialogs })
		.catch (err => {
			console.warn ("[frontend updates] stream scope update failed:", this.extractErrorMessage (err));
		});
	}

	startFrontendUpdates() {
		this.stopFrontendUpdates ();
		if (!this.userIsLoggedIn ())
			return;

		let updates = this.FrontendUpdates;
		updates.enabled = true;
		updates.connectFailures = 0;
		updates.revision = 0;

		if (typeof EventSource === "undefined") {
			this._startFrontendPolling ();
			return;
		}

		updates.mode = "connecting";
		this._connectFrontendStream ();
	}

	stopFrontendUpdates() {
		let updates = this.FrontendUpdates;
		updates.enabled = false;
		updates.mode = "off";
		updates.revision = 0;
		updates.pendingEvents = null;
		this._closeFrontendStream ();
		this._clearFrontendReconnectTimer ();
		this._stopFrontendPolling ();
		this._stopFrontendModuleRefresh ();
	}

	_closeFrontendStream() {
		let updates = this.FrontendUpdates;
		updates.generation++;
		if (updates.source) {
			updates.source.close ();
			updates.source = null;
		}
	}

	_clearFrontendReconnectTimer() {
		if (this.FrontendUpdates.reconnectTimer) {
			clearTimeout (this.FrontendUpdates.reconnectTimer);
			this.FrontendUpdates.reconnectTimer = null;
		}
	}

	_connectFrontendStream() {
		let updates = this.FrontendUpdates;
		if (!updates.enabled || !this.userIsLoggedIn ())
			return;

		this._closeFrontendStream ();
		let generation = updates.generation;
		updates.connectionHasEvents = false;
		updates.pendingEvents = null;

		let scope = this._frontendActiveScope ();
		this.axiosPostRequest ("/frontend/streamticket", { client: updates.clientID, pages: scope.pages, dialogs: scope.dialogs })
		.then (resultJSON => {
			if ((generation !== updates.generation) || !updates.enabled)
				return;

			// A scope report sent meanwhile may have been handled before the ticket request.
			let currentScope = this._frontendActiveScope ();
			if ((currentScope.pages !== scope.pages) || (currentScope.dialogs !== scope.dialogs))
				this._reportFrontendStreamScope ();

			let ticket = Assert.UUIDValue (resultJSON.data.ticket);
			let streamURL = this.getStreamURL (ticket);
			// A new ticket means a new EventSource, so the resume point is passed explicitly.
			if (updates.revision > 0)
				streamURL += "?lastEventId=" + encodeURIComponent (String (updates.revision));

			let source = new EventSource (streamURL);
			updates.source = source;
			source.addEventListener ("snapshot", event => this._onFrontendStreamEvent (generation, "snapshot", event));
			source.addEventListener ("patch", event => this._onFrontendStreamEvent (generation, "patch", event));
			source.onerror = () => this._onFrontendStreamError (generation);
		})
		.catch (err => {
			if (generation !== updates.generation)
				return;
			console.warn ("[frontend updates] stream ticket request failed:", this.extractErrorMessage (err));
			this._onFrontendStreamError (generation);
		});
	}

	// The browser reconnects an EventSource on its own, but with the consumed ticket. The stream is
	// therefore always closed on error and reopened with a new ticket.
	_onFrontendStreamError(generation) {
		let updates = this.FrontendUpdates;
		if ((generation !== updates.generation) || !updates.enabled)
			return;

		this._closeFrontendStream ();
		updates.pendingEvents = null;

		if (!updates.connectionHasEvents)
			updates.connectFailures++;
		else
			updates.connectFailures = 1;

		let delayMS;
		if (updates.mode === "polling") {
			delayMS = FRONTEND_STREAM_RECOVERY_INTERVAL_MS;
		}
		else if (updates.connectFailures >= FRONTEND_STREAM_MAX_CONNECT_FAILURES) {
			console.warn ("[frontend updates] event stream unavailable, falling back to polling");
			this._startFrontendPolling ();
			delayMS = FRONTEND_STREAM_RECOVERY_INTERVAL_MS;
		}
		else {
			let backoffMS = Math.min (FRONTEND_STREAM_RETRY_MAX_MS, FRONTEND_STREAM_RETRY_BASE_MS * Math.pow (2, updates.connectFailures - 1));
			// Jitter spreads the reconnects of many clients after a server restart.
			delayMS = backoffMS * (0.5 + Math.random () * 0.5);
		}

		this._clearFrontendReconnectTimer ();
		updates.reconnectTimer = setTimeout (() => {
			updates.reconnectTimer = null;
			this._connectFrontendStream ();
		}, delayMS);
	}

	_onFrontendStreamEvent(generation, eventType, event) {
		let updates = this.FrontendUpdates;
		if ((generation !== updates.generation) || !updates.enabled)
			return;

		let data;
		try {
			data = JSON.parse (event.data);
		}
		catch (err) {
			data = null;
		}
		if (!data || (typeof data !== "object")) {
			this._restartFrontendStream ();
			return;
		}

		if (!updates.connectionHasEvents) {
			updates.connectionHasEvents = true;
			updates.connectFailures = 0;
			this._enterFrontendStreamMode ();
		}

		if (updates.pendingEvents) {
			updates.pendingEvents.push ({ eventType: eventType, data: data });
			return;
		}

		this._applyFrontendStreamEvent (eventType, data);
	}

	// Coming back from polling, the indexed state only holds the active pages and dialogs. The stream
	// events are held back until the full structure is loaded again.
	_enterFrontendStreamMode() {
		let updates = this.FrontendUpdates;
		let wasPolling = (updates.mode === "polling");
		if (updates.mode !== "stream")
			console.info ("[frontend updates] receiving live updates through the event stream");
		updates.mode = "stream";
		this._stopFrontendPolling ();
		this._startFrontendModuleRefresh ();

		if (!wasPolling && (this.API.frontendScope === "*"))
			return;

		let generation = updates.generation;
		updates.pendingEvents = [];
		this.axiosGetRequest ("/frontend")
		.then (resultJSON => {
			if ((generation !== updates.generation) || !resultJSON.data)
				return;

			this._setFrontendState (resultJSON.data, "*");
			let pendingEvents = updates.pendingEvents || [];
			updates.pendingEvents = null;
			this._applyFrontendUpdate (null);
			for (let pendingEvent of pendingEvents)
				this._applyFrontendStreamEvent (pendingEvent.eventType, pendingEvent.data);
		})
		.catch (err => {
			if (generation !== updates.generation)
				return;
			console.warn ("[frontend updates] loading the frontend structure failed:", this.extractErrorMessage (err));
			this._onFrontendStreamError (generation);
		});
	}

	_applyFrontendStreamEvent(eventType, data) {
		let updates = this.FrontendUpdates;
		if (!Number.isSafeInteger (data.revision)) {
			this._restartFrontendStream ();
			return;
		}

		if (eventType === "snapshot") {
			if (!data.stores || (typeof data.stores !== "object")) {
				this._restartFrontendStream ();
				return;
			}
			this._writeFrontendChanges (data.stores, true);
			updates.revision = data.revision;
			this._applyFrontendUpdate (null);
			return;
		}

		if (!Number.isSafeInteger (data.base) || !data.changed || (typeof data.changed !== "object")) {
			this._restartFrontendStream ();
			return;
		}

		// A patch only applies to the revision it was computed from; anything else forces a snapshot.
		if (data.base !== updates.revision) {
			updates.revision = 0;
			this._restartFrontendStream ();
			return;
		}

		let changedModuleUUIDs = this._writeFrontendChanges (data.changed, false);
		updates.revision = data.revision;
		this._applyFrontendUpdate (changedModuleUUIDs);
	}

	_restartFrontendStream() {
		this.FrontendUpdates.revision = 0;
		this.FrontendUpdates.pendingEvents = null;
		this._connectFrontendStream ();
	}

	_startFrontendPolling() {
		let updates = this.FrontendUpdates;
		updates.mode = "polling";
		updates.revision = 0;
		this._stopFrontendModuleRefresh ();
		if (updates.pollTimer)
			return;

		// A tick never adds a second request: it is skipped while any /api/frontend request sent by
		// retrieveFrontendState is pending (an earlier poll, a navigation refresh, or a poll still running
		// from before the stream took over), so a server slower than the interval does not get an ever
		// growing number of overlapping polls. Navigation refreshes themselves are not held back; the
		// reply serials keep their results in order. A skipped tick also skips updateModules, so while
		// polling, module data is refreshed once per reply.
		updates.pollTimer = setInterval (() => {
			if (this.API.frontendRequestsInFlight > 0)
				return;
			this.retrieveFrontendState ()
			.finally (() => {
				this.updateModules ();
				this._notifyFrontendChange (null);
			});
		}, FRONTEND_POLL_INTERVAL_MS);
	}

	_stopFrontendPolling() {
		if (this.FrontendUpdates.pollTimer) {
			clearInterval (this.FrontendUpdates.pollTimer);
			this.FrontendUpdates.pollTimer = null;
		}
	}

	// Polling calls updateModules on every cycle, which refreshes such data already.
	_startFrontendModuleRefresh() {
		let updates = this.FrontendUpdates;
		if (updates.moduleRefreshTimer)
			return;

		updates.moduleRefreshTimer = setInterval (() => {
			for (let module of this.AppContent.ModuleMap.values ()) {
				if ((typeof module.refreshLiveData === "function") && module.isActive ())
					module.refreshLiveData ();
			}
		}, FRONTEND_MODULE_REFRESH_INTERVAL_MS);
	}

	_stopFrontendModuleRefresh() {
		if (this.FrontendUpdates.moduleRefreshTimer) {
			clearInterval (this.FrontendUpdates.moduleRefreshTimer);
			this.FrontendUpdates.moduleRefreshTimer = null;
		}
	}

	// Look up a UUID in the v2 frontend map. Returns the entry or null.
	getV2Entry(uuid) {
		if (this.API.frontendLookup) {
			return this.API.frontendLookup[uuid] || null;
		}
		return null;
	}

    updateContentItem(item) {
		
		if (!item)
			return;

        item.refresh = false;
		
		if (item.isActive ()) {

			// Items are updated only from the v2 frontend state; an item without an entry
			// keeps its current state until the next /api/frontend response contains it.
			let v2Entry = this.getV2Entry(item.uuid);
			if (v2Entry && v2Entry.attributes)
				item.updateFromV2Attributes(Object.assign({}, v2Entry.attributes));

			item.setRefreshFlag();
		}

    }


    updateContentItems() {
		
		let uuid, item;
		
		if (this.AppContent.ItemMap) {

			for ([uuid, item] of this.AppContent.ItemMap) {
									
				uuid;
				this.updateContentItem(item);
			}
		
		}
		
	}	

	updateModule(module) {

		// Early return if module is invalid
		if (!module)
			return;

		// Reset refresh flag before attempting update
		module.refresh = false;

		if (module.isActive()) {

			// Item payload (entries, etc.) lives in submodule[0]; display attrs
			// (caption, visible) live at the module level.  Module-level attrs win.
			let v2Entry = this.getV2Entry(module.uuid);
			if (v2Entry) {
				let subAttrs = (v2Entry.submodules && v2Entry.submodules.length > 0)
					? (v2Entry.submodules[0].attributes || {})
					: {};
				let effectiveAttrs = Object.assign({}, subAttrs, v2Entry.attributes || {});
				module.updateFromV2Attributes(effectiveAttrs);
			}
		}
	}

	updateModules() {

		let uuid, module;
		if (this.AppContent.ModuleMap) {
			for ([uuid, module] of this.AppContent.ModuleMap) {
				uuid;
				this.updateModule(module);
			}
		}
	}


	onJobUploadChunkSuccess (application, uploadObject, chunkData, uploadOffset) {
		
		Assert.ObjectInstance (application, "amcApplication");
		Assert.ObjectInstance (uploadObject, "amcUpload");
		Assert.IntegerValue (uploadOffset);
		
		if (!uploadObject.checkIfUploadIsActive())
			return;
		
		uploadObject.setStateMessageToProgress ();
		
		const formData = new FormData();
		formData.append("size", chunkData.byteLength);
		formData.append("offset", uploadOffset);
		formData.append("data", new Blob([chunkData], {
                        type: uploadObject.getMimeType ()
                    }), uploadObject.getFileName ());

		application.axiosPostFormData("/upload/" + uploadObject.streamuuid, formData, {
			onUploadProgress: (progressEvent) => uploadObject.reportChunkProgress (uploadOffset, progressEvent.loaded)
		})
			.then(async resultUploadHandle => {
				
				if (!uploadObject.checkIfUploadIsActive())
					return;
				
				resultUploadHandle;
				if (!uploadObject.readChunk (application, application.onJobUploadChunkSuccess)) {
					
					let checkSum = await uploadObject.calculateChecksum ();
					
					uploadObject.setStateMessageToWaiting ();
					
                    application.axiosPostRequest("/upload/" + uploadObject.streamuuid + "/finish", {
                        "streamuuid": uploadObject.streamuuid,
						"context": "build",
                        "sha256": checkSum
                    })
                    .then(resultUploadFinish => {

						uploadObject.clearUploadState ();

						if (uploadObject.hasSuccessEvent ()) {
							
							let contextuuid = Assert.UUIDValue (resultUploadFinish.data.contextuuid);
							
							let itemuuid = uploadObject.getItemUUID ();
							let eventValues = {}
							eventValues[itemuuid] = contextuuid;

							application.triggerUIEvent(uploadObject.getSuccessEvent (), itemuuid, eventValues); 
						}

						/*uploadObject.setStateMessageToPreparing ();

                        application.axiosPostRequest("/build/prepare", {
                            "builduuid": uploadObject.contextuuid,
                        })
                        .then(resultBuildPrepare => {
							
							if (!uploadObject.checkIfUploadIsActive())
								return;
							
                            resultBuildPrepare;
							uploadObject.clearUploadState ();
							
                            if (uploadObject.hasSuccessEvent ()) {
																
								let itemuuid = uploadObject.getItemUUID ();
								let eventValues = {}
								eventValues[itemuuid] = contextuuid;

								application.triggerUIEvent(uploadObject.getSuccessEvent (), itemuuid, eventValues); 
                            } 

                        })
                        .catch(err => {
                            err;
//                            if (failureevent)
                                //this.triggerUIEvent(failureevent, itemuuid, {});
                        }); */
                    })					
					
					
				}
			})
            .catch(err => {
                err;
            });
		
		
	}
	

    performJobUpload(itemState, successEventName, failureEventName) {
		
		Assert.ObjectInstance (itemState, "amcUploadState");
		
		let chosenfile = itemState.getChosenFile ();	
							
		this.axiosPostRequest("/upload/", {
			"context": "build",
            "name": chosenfile.name,
            "size": chosenfile.size,
            "mimetype": itemState.getMimeType (),
        })

            .then(resultUploadInit => {
				
				Assert.ObjectValue (resultUploadInit);
				Assert.ObjectValue (resultUploadInit.data);				
                let streamuuid = Assert.UUIDValue (resultUploadInit.data.streamuuid);
												
				let currentUpload = new AMCUpload (itemState, streamuuid);
				currentUpload.setSuccessEvent (successEventName);
				currentUpload.setFailureEvent (failureEventName);
				
				currentUpload.readChunk (this, this.onJobUploadChunkSuccess);
				
				
			})
            .catch(err => {
                err;
/*                if (failureeventname)
                    this.triggerUIEvent(failureeventname, itemuuid, {}); */
            });
		
    }


	onImageUploadChunkSuccess (application, uploadObject, chunkData, uploadOffset) {
		
		Assert.ObjectInstance (application, "amcApplication");
		Assert.ObjectInstance (uploadObject, "amcUpload");
		Assert.IntegerValue (uploadOffset);
				
		if (!uploadObject.checkIfUploadIsActive())
			return;
		
		uploadObject.setStateMessageToProgress ();
		
		const formData = new FormData();
		formData.append("size", chunkData.byteLength);
		formData.append("offset", uploadOffset);
		formData.append("data", new Blob([chunkData], {
                        type: uploadObject.getMimeType ()
                    }), uploadObject.getFileName ());

		application.axiosPostFormData("/upload/" + uploadObject.streamuuid, formData, {
			onUploadProgress: (progressEvent) => uploadObject.reportChunkProgress (uploadOffset, progressEvent.loaded)
		})
			.then(async resultUploadHandle => {
				
				if (!uploadObject.checkIfUploadIsActive())
					return;
				
				resultUploadHandle;
				if (!uploadObject.readChunk (application, application.onImageUploadChunkSuccess)) {
					
					let checkSum = await uploadObject.calculateChecksum ();
					
					uploadObject.setStateMessageToWaiting ();
					
                    application.axiosPostRequest("/upload/finish", {
                        "streamuuid": uploadObject.streamuuid,
						"context": "image",
                        "sha256": checkSum
                    })
                    .then(resultUploadFinish => {
                        
						uploadObject.clearUploadState ();

						if (uploadObject.hasSuccessEvent ()) {
							
							let contextuuid = Assert.UUIDValue (resultUploadFinish.data.contextuuid);
							
							let itemuuid = uploadObject.getItemUUID ();
							let eventValues = {}
							eventValues[itemuuid] = contextuuid;

							application.triggerUIEvent(uploadObject.getSuccessEvent (), itemuuid, eventValues); 
						}


						/*uploadObject.setStateMessageToPreparing ();

                        application.axiosPostRequest("/build/prepare", {
                            "builduuid": uploadObject.contextuuid,
                        })
                        .then(resultBuildPrepare => {
							
							if (!uploadObject.checkIfUploadIsActive())
								return;
							
                            resultBuildPrepare;
							uploadObject.clearUploadState ();
							
                            if (uploadObject.hasSuccessEvent ()) {
								
								let itemuuid = uploadObject.getItemUUID ();
                                let eventValues = {}
                                eventValues[itemuuid] = uploadObject.contextuuid;

                                application.triggerUIEvent(uploadObject.getSuccessEvent (), itemuuid, eventValues); 
                            } 

                        })
                        .catch(err => {
                            err;
//                            if (failureevent)
                                //this.triggerUIEvent(failureevent, itemuuid, {});
                        }); */
                    })					
					
					
				}
			})
            .catch(err => {
                err;
            });
		
		
	}
	


    performImageUpload(itemState, successEventName, failureEventName) {
		
		Assert.ObjectInstance (itemState, "amcUploadState");
		
		let chosenfile = itemState.getChosenFile ();		
							
		this.axiosPostRequest("/upload/", {
			"context": "build",
            "name": chosenfile.name,
            "size": chosenfile.size,
            "mimetype": itemState.getMimeType (),
        })

            .then(resultUploadInit => {
				
				Assert.ObjectValue (resultUploadInit);
				Assert.ObjectValue (resultUploadInit.data);				
                let streamuuid = Assert.UUIDValue (resultUploadInit.data.streamuuid);
												
				let currentUpload = new AMCUpload (itemState, streamuuid);
				currentUpload.setSuccessEvent (successEventName);
				currentUpload.setFailureEvent (failureEventName);
				
				//let reader = new FileReader();
				currentUpload.readChunk (this, this.onImageUploadChunkSuccess);
				
				
			})
            .catch(err => {
                err;
/*                if (failureeventname)
                    this.triggerUIEvent(failureeventname, itemuuid, {}); */
            });
		

    }


    changePage(page) {

        let pageString = String(page);
        this.AppState.activePage = pageString;
		this.API.navigationRequestSerial = this.API.frontendRequestSerial;
		
		let pageObject = this.AppContent.PageMap.get(pageString);
		if(pageObject) {
			pageObject.setActive();
		}

		if (this.AppState.appResizeEvent)
			this.AppState.appResizeEvent ();
		
		this.updateContentItems ();
		this._refreshFrontendStateAfterNavigation ();

    }

	// Modules of pages and dialogs that were not active during the last poll have
	// not received data yet, so fetch it immediately instead of waiting for the next poll.
	// The event stream delivers them after the new scope is reported.
	_refreshFrontendStateAfterNavigation () {
		let frontendStateRequest;
		if (this._frontendUpdatesUseStream ()) {
			this._reportFrontendStreamScope ();
			frontendStateRequest = Promise.resolve ();
		}
		else {
			frontendStateRequest = this.retrieveFrontendState ();
		}
		frontendStateRequest
		.then (() => {
			this.updateModules ();
			this.updateContentItems ();
			if (this.AppState.appResizeEvent)
				this.AppState.appResizeEvent ();
			this._notifyFrontendChange (null);
		});
	}

    closeAllDialogs() {
        let dialog;
        for (dialog of this.AppContent.Dialogs) {
            dialog.dialogIsActive = false;
        }
    }

    showDialog(dialog) {

        this.closeAllDialogs();
		this.API.navigationRequestSerial = this.API.frontendRequestSerial;

        if (dialog) {

            if (this.AppContent.DialogMap.has(dialog)) {
                let dialogObject = this.AppContent.DialogMap.get(dialog);
                dialogObject.dialogIsActive = true;
            }
        }
		
		if (this.AppState.appResizeEvent)
			this.AppState.appResizeEvent ();
		
		this.updateContentItems ();
		this._refreshFrontendStateAfterNavigation ();
		
    }
	
	streamDownload (downloadticketuuid) 
	{
				
	
		/*// Start download in Browser
		const a = document.createElement('a');
		a.href = this.getDownloadURL (downloadticketuuid);
		a.download = downloadfilename;
		document.body.appendChild(a);    
		a.click();
		document.body.removeChild(a); */
			
		window.open (this.getDownloadURL (downloadticketuuid), "_blank");
		
	}

    getImageURL(uuid) {
        return this.API.baseURL + '/image/' + uuid;
    }
	
    getChartURL(uuid) {
        return this.API.baseURL + '/dataseries/' + uuid;
    }
	
    getDownloadURL(uuid) {
        return this.API.baseURL + '/download/' + uuid;
    }

    getStreamURL(uuid) {
        let streamBase = this.API.baseURL.replace(/\/api$/, '');
        return streamBase + '/stream/' + uuid;
    }

    triggerUIEvent(eventname, senderuuid, eventValues, executionCallback, eventParameters) {

        let requestBody = {
            "eventname": eventname,
            "senderuuid": senderuuid,
            "formvalues": eventValues
        };
        // Optional event parameters are forwarded as the external event "parameters"
        // object, readable in the handler via UIEnvironment::GetExternalEventParameter.
        if (eventParameters)
            requestBody["parameters"] = eventParameters;

        this.axiosPostRequest("/event", requestBody)
        .then(resultHandleEvent => {
			
			if (resultHandleEvent.data.actions) {
				if (Array.isArray(resultHandleEvent.data.actions)) {
					let action;
					for (action of resultHandleEvent.data.actions) {
						if (action.action === "activatemodaldialog") {
							this.showDialog(action.dialogname);
						}
						if (action.action === "activatepage") {
							this.changePage(action.pagename);
						}
						if (action.action === "closemodaldialog") {
							this.closeAllDialogs();
						}
						if (action.action === "streamdownload") {
							this.streamDownload(action.downloadticket);
						}
						
						//this.updateContentItemResult(item.uuid, item);
					}
				}
			}
			
			if (executionCallback) {
				executionCallback ();
			}				
			
        })
        .catch(err => {
            console.log(err);
        });
    }

    triggerWidgetRequest (widgetuuid, requestType, requestJSON, executionCallback) {

		
        this.axiosPostRequest("/widget/" + Assert.UUIDValue (widgetuuid) + "/" + Assert.StringValue (requestType), Assert.ObjectValue (requestJSON))
        .then(resultHandleWidgetRequest => {
			
			if (resultHandleWidgetRequest.data.actions) {
				if (Array.isArray(resultHandleWidgetRequest.data.actions)) {
					let action;
					for (action of resultHandleWidgetRequest.data.actions) {
						if (action.action === "activatemodaldialog") {
							this.showDialog(action.dialogname);
						}
						if (action.action === "activatepage") {
							this.changePage(action.pagename);
						}
						if (action.action === "closemodaldialog") {
							this.closeAllDialogs();
						}
						if (action.action === "streamdownload") {
							this.streamDownload(action.downloadticket);
						}
						
						//this.updateContentItemResult(item.uuid, item);
					}
				}
			}
			
			if (executionCallback) {
				executionCallback ();
			}				
			
        })
        .catch(err => {
            console.log(err);
        });
    }

    assembleFormValues(formValueUUIDList) {
		
		
		Assert.ArrayValue (formValueUUIDList);
		
        let resultObject = {}
        for (let entityUUID of formValueUUIDList) {
			
			if (this.AppContent.FormEntityMap.has (entityUUID)) {			
				let entity = this.AppContent.FormEntityMap.get (entityUUID);
				if (entity.dataObject) {
					let formValue = entity.dataObject.value;
					resultObject[entityUUID] = formValue;
				}
			}
        }

        return resultObject;

    }

    validateFormValues (eventname, senderuuid, formvalues) {
        return this.axiosPostRequest("/validate", {
            "eventname": eventname,
            "senderuuid": senderuuid,
            "formvalues": formvalues
        })
        .then(response => {
            const data = response.data || {};
            return {
                valid: data.valid === true,
                errors: data.errors || {}
            };
        })
        .catch(err => {
            console.log('[AMC] validateFormValues error:', err);
            return { valid: true, errors: {} };
        });
    }

    retrieveWebGLInstance(uuid) {
        return this.AppState.WebGLInstances[uuid];
    }

    storeWebGLInstance(uuid, instance) {
        this.AppState.WebGLInstances[uuid] = instance;
    }
	
	findPage (pageName) {
		
		if (this.PageMap.has (pageName))
			return this.PageMap.get (pageName);
		
		return null;
		
	}

	findCustomPage (pageName) {
		
		if (this.CustomPageMap.has (pageName))
			return this.CustomPageMap.get (pageName);
		
		return null;
		
	}
	
	userIsLoggedIn ()
	{
		return (this.API.authToken !== Common.nullToken ());
	}

	userUUID ()
	{
		if (!this.userIsLoggedIn ())
			throw "could not get user UUID: user is not logged in";
			
		return this.API.userUUID;
	}

	userLogin ()
	{
		if (!this.userIsLoggedIn ())
			throw "could not get user login: user is not logged in";

		return this.API.userLogin;
	}

	userDescription ()
	{
		if (!this.userIsLoggedIn ())
			throw "could not get user description: user is not logged in";

		return this.API.userDescription;
	}

	userRole ()
	{
		if (!this.userIsLoggedIn ())
			throw "could not get user role: user is not logged in";
		
		return this.API.userRole;
	}

	userLanguage ()
	{
		if (!this.userIsLoggedIn ())
			throw "could not get user language: user is not logged in";
		
		return this.API.userLanguage;
	}

	userPermissions ()
	{
		if (!this.userIsLoggedIn ())
			throw "could not get user permissions: user is not logged in";
		
		return new Set(this.API.userPermissions);
	}
	
	checkPermission (permissionIdentifier)
	{
		if (!this.userIsLoggedIn ())
			return false;
		
		return this.API.userPermissions.has (Assert.IdentifierString (permissionIdentifier));
	}
	
	async generateUserPassword (clearTextPassword)
	{
		if (typeof clearTextPassword !== 'string')
			throw "could not set user password: invalid input string";
		
		let trimmedPassword = clearTextPassword.trim ();
		
		if (trimmedPassword.length < Common.minimumPasswordLength ())
			throw "could not set user password: invalid user password length";
					
		if (!this.userIsLoggedIn ())
			throw "could not get user permissions: user is not logged in";
		
		const randomArray = new Uint32Array(1024);
		crypto.getRandomValues(randomArray);
		
		let randomString = this.API.authToken;
		for (let value of randomArray)
			randomString = randomString + value.toString ();

		let passwordSalt = await sha256Hex(randomString);
		let passwordHash = await sha256Hex(passwordSalt + trimmedPassword);
		
		return {
			"salt": passwordSalt,
			"hash": passwordHash
		}
		
	}
	
	pageIsActive (page)
	{
		if (page) {
					
			if (page.isAlwaysActive)
				return true;
					
			return page.name === this.AppState.activePage;
		}
		
		return false;
	}


}
