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


#ifndef __AMC_API_CONSTANTS
#define __AMC_API_CONSTANTS

#ifndef __AMCIMPL_API_CONSTANTS
#error this header should only be included in the implementation CPP files.
#endif

#define AMC_API_HTTP_SUCCESS 200
#define AMC_API_HTTP_BADREQUEST 400
#define AMC_API_HTTP_FORBIDDEN 403
#define AMC_API_HTTP_NOTFOUND 404

#define AMC_API_PROTOCOL_VERSION "2.0.0"
#define AMC_API_PROTOCOL_ERROR "com.autodesk.error"
#define AMC_API_PROTOCOL_LOGS "com.autodesk.machinecontrol.logs"
#define AMC_API_PROTOCOL_STATUS "com.autodesk.machinecontrol.status"
#define AMC_API_PROTOCOL_SETUP "com.autodesk.machinecontrol.setup"
#define AMC_API_PROTOCOL_UPLOAD "com.autodesk.machinecontrol.upload"
#define AMC_API_PROTOCOL_BUILD "com.autodesk.machinecontrol.build"
#define AMC_API_PROTOCOL_SIGNAL "com.autodesk.machinecontrol.signal"
#define AMC_API_PROTOCOL_AUTH "com.autodesk.machinecontrol.auth"
#define AMC_API_PROTOCOL_UI "com.autodesk.machinecontrol.ui"
#define AMC_API_PROTOCOL_TOKEN "com.autodesk.machinecontrol.token"

#define AMC_API_CONTENTTYPE "application/json"

#define AMC_API_KEY_PROTOCOL "protocol"
#define AMC_API_KEY_VERSION "version"
#define AMC_API_KEY_MESSAGE "message"
#define AMC_API_KEY_ERRORCODE "errorcode"
#define AMC_API_KEY_SETUPINSTANCES "instances"
#define AMC_API_KEY_SETUPINSTANCE_NAME "name"
#define AMC_API_KEY_SETUPINSTANCE_DESCRIPTION "description"
#define AMC_API_KEY_SETUPSTATES "states"
#define AMC_API_KEY_SETUPPARAMETERGROUPS "parametergroups"
#define AMC_API_KEY_SETUPSTATE_NAME "name"
#define AMC_API_KEY_SETUPSTATE_OUTSTATES "outstates"
#define AMC_API_KEY_SETUPPARAMETERGROUP_NAME "name"
#define AMC_API_KEY_SETUPPARAMETERGROUP_DESCRIPTION "description"
#define AMC_API_KEY_SETUPPARAMETERGROUP_PARAMETERS "parameters"
#define AMC_API_KEY_SETUPPARAMETER_NAME "name"
#define AMC_API_KEY_SETUPPARAMETER_DESCRIPTION "description"
#define AMC_API_KEY_UPLOAD_STREAMUUID "streamuuid"
#define AMC_API_KEY_UPLOAD_CONTEXT "context"
#define AMC_API_KEY_UPLOAD_UUID "uuid"
#define AMC_API_KEY_UPLOAD_NAME "name"
#define AMC_API_KEY_UPLOAD_MIMETYPE "mimetype"
#define AMC_API_KEY_UPLOAD_DATATYPE "datatype"
#define AMC_API_KEY_UPLOAD_TIMESTAMP "timestamp"
#define AMC_API_KEY_UPLOAD_SIZE "size"
#define AMC_API_KEY_UPLOAD_SHA256 "sha256"
#define AMC_API_KEY_UPLOAD_SHA256BLOCK64K "sha256block64k"
#define AMC_API_KEY_UPLOAD_DATA "data"
#define AMC_API_KEY_UPLOAD_DATASIZE "size"
#define AMC_API_KEY_UPLOAD_DATAOFFSET "offset"
#define AMC_API_KEY_UPLOAD_CONTEXTUUID "contextuuid"
#define AMC_API_KEY_UPLOAD_BUILDJOBARRAY "buildjobs"
#define AMC_API_KEY_UPLOAD_BUILDDATAARRAY "builddata"
#define AMC_API_KEY_UPLOAD_BUILDJOBNAME "name"
#define AMC_API_KEY_UPLOAD_BUILDJOBLAYERCOUNT "layercount"
#define AMC_API_KEY_UPLOAD_BUILDJOBUUID "uuid"
#define AMC_API_KEY_UPLOAD_BUILDJOBSTORAGESTREAM "storagestream"

#define AMC_API_KEY_SIGNAL_INSTANCENAME "instancename"
#define AMC_API_KEY_SIGNAL_SIGNALNAME "signalname"
#define AMC_API_KEY_SIGNAL_SIGNALUUID "signaluuid"

#define AMC_API_KEY_UI_APPNAME "appname"
#define AMC_API_KEY_UI_COPYRIGHT "copyright"
#define AMC_API_KEY_UI_MAINPAGE "mainpage"
#define AMC_API_KEY_UI_LOGOUUID "logouuid"
#define AMC_API_KEY_UI_COLORS "colors"
#define AMC_API_KEY_UI_LOGOASPECTRATIO "logoaspectratio"
#define AMC_API_KEY_UI_SESSIONID "sessionid"
#define AMC_API_KEY_UI_SESSIONKEY "sessionkey"
#define AMC_API_KEY_UI_ICON "icon"
#define AMC_API_KEY_UI_ID "id"
#define AMC_API_KEY_UI_CAPTION "caption"
#define AMC_API_KEY_UI_VERTICALSPLIT "verticalsplit"
#define AMC_API_KEY_UI_HORIZONTALSPLIT "horiizontalsplit"
#define AMC_API_KEY_UI_SECTIONWIDTH "sectionwidth"
#define AMC_API_KEY_UI_SECTIONHEIGHT "sectionheight"
#define AMC_API_KEY_UI_SECTIONS "sections"
#define AMC_API_KEY_UI_TARGETPAGE "targetpage"
#define AMC_API_KEY_UI_MENUITEMS "menuitems"
#define AMC_API_KEY_UI_TOOLBARITEMS "toolbaritems"
#define AMC_API_KEY_UI_PAGENAME "name"
#define AMC_API_KEY_UI_DIALOGNAME "name"
#define AMC_API_KEY_UI_DIALOGTITLE "title"
#define AMC_API_KEY_UI_PAGES "pages"
#define AMC_API_KEY_UI_DIALOGS "dialogs"
#define AMC_API_KEY_UI_MODULES "modules"
#define AMC_API_KEY_UI_MODULENAME "name"
#define AMC_API_KEY_UI_MODULEUUID "uuid"
#define AMC_API_KEY_UI_MODULETYPE "type"
#define AMC_API_KEY_UI_BUILDUUID "builduuid"
#define AMC_API_KEY_UI_CURRENTLAYER "currentlayer"
#define AMC_API_KEY_UI_SIZEX "sizex"
#define AMC_API_KEY_UI_SIZEY "sizey"
#define AMC_API_KEY_UI_ORIGINX "originx"
#define AMC_API_KEY_UI_ORIGINY "originy"
#define AMC_API_KEY_UI_BASEIMAGERESOURCE "baseimageresource"
#define AMC_API_KEY_UI_HEADLINE "headline"
#define AMC_API_KEY_UI_TITLE "title"
#define AMC_API_KEY_UI_SUBTITLE "subtitle"
#define AMC_API_KEY_UI_TABS "tabs"
#define AMC_API_KEY_UI_ITEMS "items"
#define AMC_API_KEY_UI_BUTTONS "buttons"
#define AMC_API_KEY_UI_COLUMNS "columns"
#define AMC_API_KEY_UI_COLUMNWIDTH "width"
#define AMC_API_KEY_UI_COLUMNWIDTHUNIT "unit"
#define AMC_API_KEY_UI_COLUMNPOSITION "columnposition"
#define AMC_API_KEY_UI_COLUMNSTART "columnstart"
#define AMC_API_KEY_UI_COLUMNEND "columnend"
#define AMC_API_KEY_UI_ROWS "rows"
#define AMC_API_KEY_UI_ROWHEIGHT "height"
#define AMC_API_KEY_UI_ROWHEIGHTUNIT "unit"
#define AMC_API_KEY_UI_ROWPOSITION "rowposition"
#define AMC_API_KEY_UI_ROWSTART "rowstart"
#define AMC_API_KEY_UI_ROWEND "rowend"
#define AMC_API_KEY_UI_ITEMTYPE "type"
#define AMC_API_KEY_UI_ITEMTEXT "text"
#define AMC_API_KEY_UI_ITEMUUID "uuid"
#define AMC_API_KEY_UI_BUTTONDISTRIBUTION "buttondistribution"
#define AMC_API_KEY_UI_ITEMIMAGEUUID "imageuuid"
#define AMC_API_KEY_UI_ITEMBUTTONS "buttons"
#define AMC_API_KEY_UI_ITEMASPECTRATIO "aspectratio"
#define AMC_API_KEY_UI_ITEMLOADINGTEXT "loadingtext"
#define AMC_API_KEY_UI_ITEMMAXWIDTH "maxwidth"
#define AMC_API_KEY_UI_ITEMMAXHEIGHT "maxheight"
#define AMC_API_KEY_UI_ITEMX "x"
#define AMC_API_KEY_UI_ITEMY "y"
#define AMC_API_KEY_UI_ITEMZ "z"
#define AMC_API_KEY_UI_ITEMSCALEX "scalex"
#define AMC_API_KEY_UI_ITEMSCALEY "scaley"
#define AMC_API_KEY_UI_ITEMANGLE "angle"
#define AMC_API_KEY_UI_ITEMDETAILPAGE "detailpage"
#define AMC_API_KEY_UI_ITEMSELECTEVENT "selectevent"
#define AMC_API_KEY_UI_ITEMSELECTIONVALUEUUID "selectionvalueuuid"
#define AMC_API_KEY_UI_ITEMHEADERS "headers"
#define AMC_API_KEY_UI_ITEMENTRIES "entries"
#define AMC_API_KEY_UI_ITEMENTRIESPERPAGE "entriesperpage"
#define AMC_API_KEY_UI_BUTTONUUID "uuid"
#define AMC_API_KEY_UI_BUTTONTARGETPAGE "targetpage"
#define AMC_API_KEY_UI_BUTTONCAPTION "caption"
#define AMC_API_KEY_UI_BUTTONEVENT "event"
#define AMC_API_KEY_UI_BUTTONEVENTFORMVALUES "eventformvalues"
#define AMC_API_KEY_UI_ITEMUPLOADCLASS "uploadclass"
#define AMC_API_KEY_UI_ITEMUPLOADCAPTION "uploadcaption"
#define AMC_API_KEY_UI_ITEMUPLOADISINITIAL "uploadisinitial"
#define AMC_API_KEY_UI_ITEMUPLOADISSAVING "uploadissaving"
#define AMC_API_KEY_UI_ITEMUPLOADFILENAME "uploadfilename"
#define AMC_API_KEY_UI_ITEMUPLOADSUCCESSEVENT "uploadsuccessevent"
#define AMC_API_KEY_UI_ITEMUPLOADFAILUREEVENT "uploadfailureevent"
#define AMC_API_KEY_UI_ITEMVALUE "value"
#define AMC_API_KEY_UI_ITEMPARAMETERDESCRIPTION "paramDescription"
#define AMC_API_KEY_UI_ITEMPARAMETERVALUE "paramValue"
#define AMC_API_KEY_UI_ITEMPARAMETERGROUP "paramGroup"
#define AMC_API_KEY_UI_ITEMPARAMETERSYSTEM "paramSystem"
#define AMC_API_KEY_UI_ITEMBUILDNAME "buildName"
#define AMC_API_KEY_UI_ITEMBUILDLAYERS "buildLayers"
#define AMC_API_KEY_UI_ITEMBUILDUUID "buildUUID"
#define AMC_API_KEY_UI_ITEMBUILDTIMESTAMP "buildTimestamp"
#define AMC_API_KEY_UI_FORMUUID "uuid"
#define AMC_API_KEY_UI_FORMTYPE "type"
#define AMC_API_KEY_UI_FORMDEFAULTVALUE "value"
#define AMC_API_KEY_UI_FORMCHANGEEVENT "changeevent"
#define AMC_API_KEY_UI_FORMDISABLED "disabled"
#define AMC_API_KEY_UI_FORMPREFIX "prefix"
#define AMC_API_KEY_UI_FORMSUFFIX "suffix"
#define AMC_API_KEY_UI_FORMREADONLY "readonly"
#define AMC_API_KEY_UI_FORMCAPTION "caption"
#define AMC_API_KEY_UI_FORMENTITIES "entities"
#define AMC_API_KEY_UI_CONTENT "content"
#define AMC_API_KEY_UI_EVENTNAME "eventname"
#define AMC_API_KEY_UI_EVENTSENDER "senderuuid"
#define AMC_API_KEY_UI_EVENTCONTEXT "contextuuid"
#define AMC_API_KEY_UI_FORMVALUEJSON "formvalues"
#define AMC_API_KEY_UI_EVENTERRORCODE "errorcode"
#define AMC_API_KEY_UI_EVENTERRORMESSAGE "errormessage"
#define AMC_API_KEY_UI_EVENTDIALOGTOSHOW "dialogtoshow"
#define AMC_API_KEY_UI_EVENTCLOSEDIALOGS "closedialogs"
#define AMC_API_KEY_UI_EVENTPAGETOACTIVATE "pagetoactivate"
#define AMC_API_KEY_UI_CONTENTUPDATE "contentupdate"

#define AMC_API_KEY_UI_ITEM_MINENTRIESPERPAGE 4
#define AMC_API_KEY_UI_ITEM_MAXENTRIESPERPAGE 1024
#define AMC_API_KEY_UI_ITEM_DEFAULTENTRIESPERPAGE 16

#define AMC_API_KEY_BUILDUUID "builduuid"
#define AMC_API_KEY_LAYERINDEX "layerindex"
#define AMC_API_KEY_SEGMENTS "segments"
#define AMC_API_KEY_POINTS "points"
#define AMC_API_KEY_TYPE "type"
#define AMC_API_KEY_HATCH "hatch"
#define AMC_API_KEY_LOOP "loop"
#define AMC_API_KEY_POLYLINE "polyline"
#define AMC_API_KEY_X "x"
#define AMC_API_KEY_Y "y"

#define AMC_API_KEY_STATUSINSTANCE_NAME "name"
#define AMC_API_KEY_STATUSINSTANCE_STATE "state"
#define AMC_API_KEY_STATUSPARAMETERGROUP_NAME "name"
#define AMC_API_KEY_STATUSPARAMETER_NAME "name"
#define AMC_API_KEY_STATUSPARAMETER_VALUE "value"
#define AMC_API_KEY_STATUSPARAMETERGROUP_PARAMETERS "parameters"
#define AMC_API_KEY_STATUSPARAMETERGROUPS "parametergroups"
#define AMC_API_KEY_STATUSINSTANCES "instances"

#define AMC_API_KEY_SESSIONUUID "sessionuuid"
#define AMC_API_KEY_SESSIONKEY "sessionkey"

#define AMC_API_KEY_AUTH_SESSIONUUID "sessionuuid"
#define AMC_API_KEY_AUTH_SESSIONKEY "sessionkey"
#define AMC_API_KEY_AUTH_USERNAME "username"
#define AMC_API_KEY_AUTH_SALTEDPASSWORD "password"
#define AMC_API_KEY_AUTH_CLIENTKEY "clientkey"
#define AMC_API_KEY_AUTH_LOGINSALT "loginsalt"
#define AMC_API_KEY_AUTH_TOKEN "token"

#define AMC_API_KEY_TOKEN_SESSION "session"
#define AMC_API_KEY_TOKEN_TOKEN "token"

#endif //__AMC_API_CONSTANTS

