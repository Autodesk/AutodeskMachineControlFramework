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

async function sha256hex(message) {
	const data = new TextEncoder().encode(message);
	const hashBuffer = await crypto.subtle.digest('SHA-256', data);
	return Array.from(new Uint8Array(hashBuffer))
		.map(b => b.toString(16).padStart(2, '0'))
		.join('');
}

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
import AMCApplicationModule_Image from "../modules/AMCModule_Image.js"
import AMCApplicationModule_StateMachineGraph from "../modules/AMCModule_StateMachineGraph.js"
import AMCApplicationModule_Paragraph from "../modules/AMCModule_Paragraph.js"
import AMCApplicationModule_Upload from "../modules/AMCModule_Upload.js"
import AMCApplicationModule_BuildList from "../modules/AMCModule_BuildList.js"
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
            LogoAspectRatio: 1.0,
			LoginBackgroundImageUUID: "",
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

        return axios({
            method: "GET",
            "headers": headers,
            url: this.API.baseURL + subURL,
            timeout: DEFAULT_REQUEST_TIMEOUT_MS,
            ...config
        });
    }

    axiosGetArrayBufferRequest(subURL) {
        let headers = {}
        let authToken = this.API.authToken;

        if (authToken != Common.nullToken ())
            headers.Authorization = "Bearer " + authToken;

        return axios({
            method: "GET",
            "headers": headers,
			"responseType": "arraybuffer",
            url: this.API.baseURL + subURL
        });
    }


    axiosPostRequest(subURL, data, config) {
        let headers = {}
        let authToken = this.API.authToken;

        if (authToken != Common.nullToken ())
            headers.Authorization = "Bearer " + authToken;

        return axios({
            "method": "POST",
            "url": this.API.baseURL + subURL,
            "headers": headers,
            "data": data,
            timeout: DEFAULT_REQUEST_TIMEOUT_MS,
            ...config
        });
    }

    axiosPostFormData(subURL, formData) {
        let headers = {
            "Content-Type": "multipart/form-data"
        }
        let authToken = this.API.authToken;

        if (authToken != Common.nullToken ())
            headers.Authorization = "Bearer " + authToken;

        return axios({
            "method": "POST",
            "url": this.API.baseURL + subURL,
            "headers": headers,
            "data": formData
        });
    }

    retrieveConfiguration(vuetifythemes) {
        this.axiosGetRequest("/config", { timeout: CONFIG_REQUEST_TIMEOUT_MS })
        .then(resultJSON => {
            this.AppDefinition.TextApplicationName = resultJSON.data.appname;
            this.AppDefinition.TextCopyRight = resultJSON.data.copyright;
            this.AppDefinition.MainPage = resultJSON.data.mainpage;
            this.AppDefinition.LogoUUID = resultJSON.data.logouuid;
            this.AppDefinition.LoginBackgroundImageUUID = resultJSON.data.loginbackgrounduuid;
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

            let saltedpassword = await sha256hex(loginsalt + userPassword);
            let clientkeyhash = await sha256hex(clientkey + saltedpassword);
            let sessionkeyhash = await sha256hex(sessionkey + clientkeyhash);

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
			(moduleType === "upload") || (moduleType === "buildlist") || (moduleType === "executionlist") ||
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
				instances: subs.map(sub => {
					let inst = Object.assign({}, sub.attributes || {});
					inst.uuid = sub.uuid || "";
					inst.type = sub.moduletype || "glsceneinstance";
					return inst;
				})
			};

		} else if (moduleType === "graphic") {
			legacy.viewminx = parseFloat(attrs.viewminx) || 0;
			legacy.viewminy = parseFloat(attrs.viewminy) || 0;
			legacy.viewmaxx = parseFloat(attrs.viewmaxx) || 100;
			legacy.viewmaxy = parseFloat(attrs.viewmaxy) || 100;
			legacy.showgrid = (attrs.showgrid === "1" || attrs.showgrid === true || attrs.showgrid === "true");
			legacy.items = subs.map(sub => this._normalizeV2ItemToLegacy(sub));

		} else if (moduleType === "layerview") {
			if (subs.length > 0) {
				legacy.items = subs.map(sub => this._normalizeV2ItemToLegacy(sub));
			} else if (attrs.platformuuid) {
				// v2 mode: all platform data is in the module-level attributes.
				// Synthesise a minimal platform item so the JS module can create it.
				// layercount starts at 0 and will be refreshed via the legacy polling path.
				legacy.items = [{
					type:               "platform",
					uuid:               attrs.platformuuid,
					currentlayer:       parseInt(attrs.currentlayer)  || 0,
					layercount:         0,
					builduuid:          attrs.builduuid          || "00000000-0000-0000-0000-000000000000",
					executionuuid:      attrs.executionuuid      || "00000000-0000-0000-0000-000000000000",
					scatterplotuuid:    attrs.scatterplotuuid    || "00000000-0000-0000-0000-000000000000",
					sizex:              parseFloat(attrs.sizex)  || 0,
					sizey:              parseFloat(attrs.sizey)  || 0,
					originx:            parseFloat(attrs.originx) || 0,
					originy:            parseFloat(attrs.originy) || 0,
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
			// Copy top-level properties for buildlist / executionlist (buttons, selection UUIDs).
			// These may appear on the parent (v2) or the first submodule (sub0).
			let sub0 = (subs.length > 0) ? subs[0] : {};
			if (v2.entrybuttons || sub0.entrybuttons)
				legacy.entrybuttons = v2.entrybuttons || sub0.entrybuttons;
			if (v2.selectionvalueuuid || sub0.selectionvalueuuid)
				legacy.selectionvalueuuid = v2.selectionvalueuuid || sub0.selectionvalueuuid;
			if (v2.buttonvalueuuid || sub0.buttonvalueuuid)
				legacy.buttonvalueuuid = v2.buttonvalueuuid || sub0.buttonvalueuuid;

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
		if (def.type === "image")
			return new AMCApplicationModule_Image (page, def);
		if (def.type === "paragraph")
			return new AMCApplicationModule_Paragraph (page, def);
		if (def.type === "upload")
			return new AMCApplicationModule_Upload (page, def);
		if (def.type === "buildlist")
			return new AMCApplicationModule_BuildList (page, def);
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

	retrieveFrontendState() {
		if (!this.userIsLoggedIn())
			return Promise.resolve();

		return this.axiosGetRequest("/frontend")
		.then(resultJSON => {
			this.API.frontendState = resultJSON.data;
			this.API.unsuccessfulFrontendCounter = 0;

			// Build a flat uuid -> { moduletype, attributes, submodules } map
			this.API.frontendLookup = {};
			if (resultJSON.data && resultJSON.data.pages) {
				for (let page of resultJSON.data.pages) {
					if (page.modules) {
						for (let mod of page.modules) {
							this._indexFrontendModule(mod);
						}
					}
				}
			}
		})
		.catch(err => {
			this.API.unsuccessfulFrontendCounter = (this.API.unsuccessfulFrontendCounter || 0) + 1;
			if (this.API.unsuccessfulFrontendCounter > MAX_CONSECUTIVE_FAILURES) {
				console.warn("[v2 frontend] repeated failure:", this.extractErrorMessage(err));
			}
		});
	}

	// Recursively index a v2 module (and its submodules) into frontendLookup by UUID.
	_indexFrontendModule(mod) {
		if (!mod || !mod.uuid)
			return;

		this.API.frontendLookup[mod.uuid] = mod;

		if (mod.submodules) {
			for (let sub of mod.submodules) {
				this._indexFrontendModule(sub);
			}
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

			// Phase 2: If item supports v2 and we have v2 data, use it
			// instead of the legacy /ui/contentitem/ call.
			if (item.usesV2Frontend) {
				let v2Entry = this.getV2Entry(item.uuid);
				if (v2Entry && v2Entry.attributes) {
					let attrs = Object.assign({}, v2Entry.attributes, v2Entry.clientvariables || {});
					item.updateFromV2Attributes(attrs);
					item.setRefreshFlag();
					return;
				}
			}

			// Legacy fallback: poll /ui/contentitem/{uuid}
			let headers = {}
			let authToken = this.API.authToken;

			if (authToken != Common.nullToken ())
				headers.Authorization = "Bearer " + authToken;

			let stateidstring = "";
			if (item.stateid > 0)
				stateidstring = "/" + item.stateid;
			
			let url = this.API.baseURL + "/contentitem/" + Assert.UUIDValue (item.uuid) + stateidstring;
			axios({
				method: "GET",
				"headers": headers,
				url: url
			})
			.then(resultJSON => {
							
				if (resultJSON.data) {
					if (resultJSON.data.content) {
						item.updateFromJSON (resultJSON.data.content);					
					}				
				}
							
				this.unsuccessfulUpdateCounter = 0;
				item.setRefreshFlag ();

			})
			.catch(err => {

				this.unsuccessfulUpdateCounter = this.unsuccessfulUpdateCounter + 1;
				if (this.unsuccessfulUpdateCounter > MAX_CONSECUTIVE_FAILURES) {
					this.setStatusToError(err.message);
				} else {
					item.setRefreshFlag ();
				}

			});
		
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

			// If the module supports v2 and we have a v2 entry, build merged effective
			// attributes and use them instead of the legacy /ui/module/ polling endpoint.
			// Item payload (entries, etc.) lives in submodule[0]; display attrs
			// (caption, visible) live at the module level.  Module-level attrs win.
			if (module.usesV2Frontend) {
				let v2Entry = this.getV2Entry(module.uuid);
				if (v2Entry) {
					let subAttrs = (v2Entry.submodules && v2Entry.submodules.length > 0)
						? (v2Entry.submodules[0].attributes || {})
						: {};
				let effectiveAttrs = Object.assign({}, subAttrs, v2Entry.attributes || {}, v2Entry.clientvariables || {});
				module.updateFromV2Attributes(effectiveAttrs);
					return;
				}
			}

			// Legacy fallback: poll /ui/module/{uuid}
			let headers = {};
			let authToken = this.API.authToken;

			if (authToken != Common.nullToken())
				headers.Authorization = "Bearer " + authToken;

			// Optional state segment
			let stateidstring = "";
			if (module.stateid > 0)
				stateidstring = "/" + module.stateid;

			// Build request URL for modules
			let url = this.API.baseURL + "/module/" + Assert.UUIDValue(module.uuid) + stateidstring;

			axios({
				method: "GET",
				"headers": headers,
				url: url
			})
			.then(resultJSON => {

				// Update module from server payload (if present)
				if (resultJSON.data) {
					if (resultJSON.data.content) {
						if (module && typeof module.updateFromJSON === "function") {
							module.updateFromJSON(resultJSON.data.content);
						}
					}
				}

				// Reset failure counter on success
				this.unsuccessfulUpdateCounter = 0;
			})
			.catch(err => {

				// Increment failure counter and react accordingly
				this.unsuccessfulUpdateCounter = this.unsuccessfulUpdateCounter + 1;
				if (this.unsuccessfulUpdateCounter > MAX_CONSECUTIVE_FAILURES) {
					this.setStatusToError(err.message);
				}
			});
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

		application.axiosPostFormData("/upload/" + uploadObject.streamuuid, formData)
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

		application.axiosPostFormData("/upload/" + uploadObject.streamuuid, formData)
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
		
		let pageObject = this.AppContent.PageMap.get(pageString);
		if(pageObject) {
			pageObject.setActive();
		}

		if (this.AppState.appResizeEvent)
			this.AppState.appResizeEvent ();
		
		this.updateContentItems ();

    }

    closeAllDialogs() {
        let dialog;
        for (dialog of this.AppContent.Dialogs) {
            dialog.dialogIsActive = false;
        }
    }

    showDialog(dialog) {

        this.closeAllDialogs();

        if (dialog) {

            if (this.AppContent.DialogMap.has(dialog)) {
                let dialogObject = this.AppContent.DialogMap.get(dialog);
                dialogObject.dialogIsActive = true;
            }
        }
		
		if (this.AppState.appResizeEvent)
			this.AppState.appResizeEvent ();
		
		this.updateContentItems ();
		
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

    triggerUIEvent(eventname, senderuuid, eventValues, executionCallback) {

        this.axiosPostRequest("/event", {
            "eventname": eventname,
            "senderuuid": senderuuid,
            "formvalues": eventValues
        })
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

		let passwordSalt = await sha256hex(randomString);
		let passwordHash = await sha256hex(passwordSalt + trimmedPassword);
		
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
