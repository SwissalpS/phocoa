window.PHOCOA=window.PHOCOA||{};PHOCOA.namespace=function(){var A=arguments,E=null,C,B,D;for(C=0;C<A.length;C=C+1){D=A[C].split(".");E=PHOCOA;for(B=(D[0]=="PHOCOA")?1:0;B<D.length;B=B+1){E[D[B]]=E[D[B]]||{};E=E[D[B]]}}return E};PHOCOA.importJS=function(E,B,A){if(!PHOCOA.importJSCache){PHOCOA.importJSCache={}}if(PHOCOA.importJSCache[E]){return }PHOCOA.importJSCache[E]=true;if(1){var D=new Ajax.Request(E,{asynchronous:false,method:"get"});try{PHOCOA.sandbox(D.transport.responseText,B)}catch(C){if(typeof (console)!="undefined"&&console.warn){console.warn("importJS: "+E+" failed to parse: (errNo: "+C.number+")"+C.message)}}}};PHOCOA.sandbox=function(jsCode,globalNamespace,localNamespace){if(globalNamespace){if(!localNamespace){localNamespace=globalNamespace}eval(jsCode+"\n\nwindow."+globalNamespace+" = "+localNamespace+";")}else{eval(jsCode)}};PHOCOA.importCSS=function(A){var B=document.createElement("link");B.setAttribute("rel","stylesheet");B.setAttribute("type","text/css");B.setAttribute("href",A);document.getElementsByTagName("head")[0].appendChild(B)};PHOCOA.namespace("runtime");PHOCOA.runtime.addObject=function(B,C){PHOCOA.runtime.setupObjectCache();var A=C||B.id;if(!A){throw"No ID could be found."}if(PHOCOA.runtime.objectList[A]){alert("error - cannot add duplicate object: "+A);return }PHOCOA.runtime.objectList[A]=B};PHOCOA.runtime.removeObject=function(A){PHOCOA.runtime.setupObjectCache();delete PHOCOA.runtime.objectList[A]};PHOCOA.runtime.setupObjectCache=function(){if(!PHOCOA.runtime.objectList){PHOCOA.runtime.objectList={}}};PHOCOA.runtime.getObject=function(B){PHOCOA.runtime.setupObjectCache();var A=null;if(PHOCOA.runtime.objectList[B]){A=PHOCOA.runtime.objectList[B]}return A};PHOCOA.namespace("WFRPC");PHOCOA.WFRPC=function(A,C,B){this.target="#page#delegate";this.action=null;this.form=null;this.runsIfInvalid=false;this.invocationPath=null;this.transaction=null;this.isAjax=true;this.submitButton=null;this.callback={success:this.ajaxCallbackSuccess,failure:this.ajaxCallbackFailure};if(A){this.invocationPath=A}if(C){this.target=C}if(B){this.action=B}return this};PHOCOA.WFRPC.prototype={ajaxCallbackSuccess:function(){alert("ajax callback succeeded (not yet implemented).")},ajaxCallbackFailure:function(){alert("ajax callback failed.")},actionURL:function(){return this.invocationPath},actionURLParams:function(D,A){D=D||[];A=A||false;var C=(A?"&":"");C+="__phocoa_rpc_enable=1";C+="&__phocoa_rpc_invocationPath="+escape(this.invocationPath);C+="&__phocoa_rpc_target="+escape(this.target);C+="&__phocoa_rpc_action="+this.action;C+="&__phocoa_rpc_runsIfInvalid="+this.runsIfInvalid;if(D.length){for(var E=0;E<D.length;E++){var B="__phocoa_rpc_argv_"+E;C+="&"+B+"="+D[E]}}C+="&__phocoa_rpc_argc="+D.length;return C},actionAsURL:function(A){return this.actionURL()+"?"+this.actionURLParams(A)},phocoaRPCParameters:function(B){B=B||[];var D={};D.__phocoa_rpc_enable=1;D.__phocoa_rpc_invocationPath=this.invocationPath;D.__phocoa_rpc_target=this.target;D.__phocoa_rpc_action=this.action;D.__phocoa_rpc_runsIfInvalid=this.runsIfInvalid;if(B.length){for(var C=0;C<B.length;C++){var A="__phocoa_rpc_argv_"+C;D[A]=B[C]}}D.__phocoa_rpc_argc=B.length;return D},execute:function(){if(this.form){$$(".phocoaWFFormError").each(function(F){F.update(null)});if(this.isAjax===false){var E=$(this.form);if(this.submitButton){var D='<input type="hidden" name="'+$(this.submitButton).name+'" value="'+$(this.submitButton).value+'" />';Element.insert(E,D)}E.submit()}else{$(this.form).request({method:"GET",parameters:this.phocoaRPCParameters(this.execute.arguments),onSuccess:this.callback.success.bind(this.callback.scope),onFailure:this.callback.failure.bind(this.callback.scope),onException:this.callback.failure.bind(this.callback.scope)})}}else{var A=this.actionAsURL(this.execute.arguments);if(this.isAjax){var C=function(F){F.argument=this.callback.argument;this.callback.success.apply(this.callback.scope,[F])};var B=function(F){F.argument=this.callback.argument;this.callback.failure.apply(this.callback.scope,[F])};this.transaction=new Ajax.Request(A,{method:"get",asynchronous:true,onSuccess:C.bind(this),onFailure:B.bind(this),onException:B.bind(this)})}else{document.location=A}}}};PHOCOA.namespace("WFAction");PHOCOA.WFAction=function(B,A){this.elId=B;this.eventName=A;this.callback=PHOCOA.widgets[this.elId].events[this.eventName].handleEvent;this.rpc=null;this.stopsEvent=true;Event.observe(this.elId,this.eventName,this.yuiTrigger.bindAsEventListener(this));return this};PHOCOA.WFAction.prototype={stopEvent:function(A){Event.stop(A)},yuiTrigger:function(A){if(this.stopsEvent){this.stopEvent(A)}this.execute(A)},execute:function(B){var A=[],C;if(PHOCOA.widgets[this.elId].events[this.eventName].collectArguments){A=PHOCOA.widgets[this.elId].events[this.eventName].collectArguments()}C=A.slice(0);C.splice(0,0,B);if(this.rpc){if(this.callback){this.callback.apply(this.jsCallbackArgs)}A.splice(0,0,Event.element(B).identify(),B.type);this.rpc.callback.argument=C;this.rpc.callback.success=this.rpcCallbackSuccess;this.rpc.callback.scope=this;this.rpc.execute.apply(this.rpc,A)}else{if(this.callback){this.callback.apply(this,C)}else{if(typeof (console)!="undefined"&&console.warn){console.warn("Callback doesn't exist: PHOCOA.widgets."+B.target.identify()+".events."+B.type)}}}},runScriptsInElement:function(el){var scriptEls=el.getElementsByTagName("script");for(idx=0;idx<scriptEls.length;idx++){var node=scriptEls[idx];window.eval(node.innerHTML)}},doPhocoaUIUpdatesJSON:function(updateList){var id,el;if(updateList.update){for(id in updateList.update){el=$(id);el.update(updateList.update[id]);this.runScriptsInElement(el)}}if(updateList.replace){for(id in updateList.replace){el=$(id);el.replace(updateList.replace[id]);this.runScriptsInElement(el)}}if(updateList.run){for(id=0;id<updateList.run.length;id++){window.eval(updateList.run[id])}}},rpcCallbackSuccess:function(o){var theResponse;var contentType=null;if(typeof o.getResponseHeader=="function"){contentType=o.getResponseHeader("Content-Type")}else{contentType=o.getResponseHeader["Content-Type"]}contentType=contentType.strip();switch(contentType){case"application/x-json":theResponse=eval("("+o.responseText+")");break;case"text/xml":theResponse=o.responseXML;break;case"application/x-json-phocoa-ui-updates":theResponse=eval("("+o.responseText+")");this.doPhocoaUIUpdatesJSON(theResponse);return ;break;case"text/plain":theResponse=o.responseText;break;default:theResponse=o.responseText;break}if(PHOCOA.widgets[this.elId].events[this.eventName].ajaxSuccess){var cbArgs=this.rpc.callback.argument.slice(0);cbArgs.splice(0,0,theResponse);PHOCOA.widgets[this.elId].events[this.eventName].ajaxSuccess.apply(null,cbArgs)}}};