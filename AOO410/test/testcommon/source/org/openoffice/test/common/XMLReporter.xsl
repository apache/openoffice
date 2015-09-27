<?xml version="1.0"?>
<!--***********************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 ***********************************************************-->
 
 
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="html" encoding="utf-8" doctype-public="-//W3C//DTD XHTML 1.0 Strict//EN" doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd" />
<xsl:template match="/">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<style>
body,td,div { -webkit-text-size-adjust:none; font-size: 11px; font-family: verdana, arial, sans-serif; padding: 0; margin: 0}
a.unchecked {text-decoration: none;padding: 0px 4px;color: #000000;font-weight: bold;border-bottom: 2px solid #AAAAAA;}
a.checked {text-decoration: none;border-bottom: 2px solid #E6820E;padding: 0px 4px;color: #000000;font-weight: bold;}
.container {width: 100%;float: left;margin-right: -300px;}
.content {margin-right: 300px;}
.sidebar {width: 300px;float: right;}
.header {font-weight: bold; margin: 4px; border-radius: 3px;-moz-border-radius: 4px;-webkit-border-radius: 4px;background: #61B7CF;padding: 4px;}
table {border-collapse:collapse; border: 1px solid #eeeeee;}
td {border: 1px solid #eeeeee;padding:4px}
th {border: 1px solid #eeeeee;background:#f5f5f5; padding:4px;text-align:left}
tr.Success {color:#118811;}
tr.Failure {color:#F27D15;}
tr.Ignored {color:#F4C700;}
tr.Error {color:#FC130F;}
th.Success {background:#118811;}
th.Failure {background: #FF8888;}
th.Ignored {background:#F4C700;}
th.Error {background:#FF0000;}
	</style>
	<title>JUnit Report</title>
</head>
<body onload="onload()">

<div class="container">
	<div class="content">
		<div class="header">Information</div>
		<div style="padding: 6px">
		<table class="infoTable"  width="100%">
		<tr><th>Test Date</th><td><span id="info.test.date">Unknown</span></td></tr>
		<tr><th>Build ID</th><td><span id="info.app.buildid">Unknown</span> (<span id="info.app.AllLanguages"></span>)</td></tr>
		<tr><th>Revision</th><td id="info.app.Revision">Unknown</td></tr>
		<tr><th>OS</th><td><span id="info.os.name">Unknown</span>-<span id="info.os.version">Unknown</span>-<span id="info.os.arch">Unknown</span></td></tr>
		<tr><th>Host Name</th><td><span id="info.hostname">Unknown</span> (<span id="info.ip"></span>)</td></tr>
		<tr><th>Java</th><td id="java.runtime.version">Unknown</td></tr>
		</table>
		</div>
	</div>
</div>

<div class="sidebar">
	<div class="header">Summary</div>
	<div style="padding: 6px">
	<table class="summaryTable" width="100%">
		<tr><th>All</th><td><a href="#" onclick="showAllRecord()" id="AllNo" class="checked" >0</a></td></tr>
		<tr><th class="Success">Success</th><td><a href="#" onclick="toggleRecord('Success')" class="checked" id="SuccessNo">0</a></td></tr>
		<tr><th class="Failure">Failure</th><td><a href="#" onclick="toggleRecord('Failure')" class="checked" id="FailureNo">0</a></td></tr>
		<tr><th class="Error">Error</th><td><a href="#" onclick="toggleRecord('Error')" class="checked" id="ErrorNo">0</a></td></tr>
		<tr><th class="Ignored">Ignored</th><td><a href="#" onclick="toggleRecord('Ignored')" class="checked" id="IgnoredNo">0</a></td></tr>
	</table>
	</div>
</div>
<div style="clear:both;" class="header">
	<div style="float:left" >Record</div>
	<div style="float:right;">
	</div>
	<div style="clear:both;"></div>
</div>
<div style="padding: 6px" id="recordTableContainer">
</div>


<script>
<xsl:text disable-output-escaping='yes'>
<![CDATA[
function ajax(url, callback) {
	var xmlhttp = window.XMLHttpRequest ? new XMLHttpRequest() : new ActiveXObject("Microsoft.XMLHTTP");
	xmlhttp.onreadystatechange = callback;
	xmlhttp.open("GET", url, true);
	xmlhttp.send();
}

function text(s) {
	return s.replace(/&/g, "&amp;").replace(/</g, "&lt;").replace(/>/g, "&gt;");
}

function $(id) {
	return document.getElementById(id);
}

function $$(e, tag) {
	var els = e.getElementsByTagName(tag);
	return els[0] || null;
}

function $$s(e, tag) {
	return e.getElementsByTagName(tag);
}

function refresh() {
	var data = $('data');
	var testsuiteEl = $$(data, 'testsuite');
	var allNo = testsuiteEl.getAttribute("tests");
	$('AllNo').innerHTML = allNo;
	var failureNo = testsuiteEl.getAttribute("failures");
	$('FailureNo').innerHTML = failureNo;
	var errorNo = testsuiteEl.getAttribute("errors");
	$('ErrorNo').innerHTML = errorNo;
	var ignoredNo = testsuiteEl.getAttribute("ignored");
	$('IgnoredNo').innerHTML = ignoredNo;
	$('SuccessNo').innerHTML = parseInt(allNo) - parseInt(failureNo) - parseInt(errorNo) - parseInt(ignoredNo);
	
	var propEls = $$s(testsuiteEl, "property");
	var props = {};
	for (var i = 0; i < propEls.length; i++) {
		var e = propEls[i];
		props[e.getAttribute("name")] = e.getAttribute("value");
	}
	
	$('info.test.date').innerHTML = props['info.test.date'] || 'Unknown';
	$('info.app.buildid').innerHTML = props['info.app.buildid'] || 'Unknown';
	$('info.app.AllLanguages').innerHTML = props['info.app.AllLanguages'] || '';
	$('info.os.name').innerHTML = props['info.os.name'] || 'Unknown';
	$('info.os.version').innerHTML = props['info.os.version'] || 'Unknown';
	$('info.os.arch').innerHTML = props['info.os.arch'] || 'Unknown';
	if (props['info.app.Revision']) 
		$('info.app.Revision').innerHTML = '<a href="http://svn.apache.org/viewvc?view=revision&revision=' + props['info.app.Revision'] + '">'  + props['info.app.Revision'] + '</a>';
	$('info.hostname').innerHTML = props['info.hostname'] || 'Unknown';
	$('info.ip').innerHTML = props['info.ip'] || '0.0.0.0';
	$('java.runtime.version').innerHTML = props['java.runtime.version'] || 'Unknown';
	
	var testcaseEls = $$s(data, 'testcase');
	var html = '<table class="recordTable"><thead><tr><th>Class</th><th>Method</th><th>Status</th><th width="80%">Message</th><th>Time</th><th>Screenshot</th></tr></thead><tbody id="recordTable">';

	for (var i = 0; i < testcaseEls.length; i++){
		var e = testcaseEls[i], c = null;
		var status = 'Success', message = '', screenshot= '';
		if (c = $$(e, 'failure')) {
			status = 'Failure';
			message = '<div>' + text(c.getAttribute("message")) + '</div>';
			message += '<pre>' + text(c.innerHTML) + '</pre>';
			screenshot = '<a href="screenshot/' + e.getAttribute("name") + '.png">Screenshot</a>';
		} else if (c = $$(e, 'error')) {
			status = 'Error';
			message = '<div>' + text(c.getAttribute("message")) + '</div>';
			message += '<pre>' + text(c.innerHTML) + '</pre>';
			screenshot = '<a href="screenshot/' + e.getAttribute("name") + '.png">Screenshot</a>';
		} else if (c = $$(e, 'ignored')) {
			status = 'Ignored';
			message = text(c.getAttribute("message"));
			var bugPattern = /(.*)#(\d+)(.*)/;
			if( bugPattern.test(message) ) {
				message = '<a href="https://issues.apache.org/ooo/show_bug.cgi?id=' + RegExp.$2 + '">' + RegExp.$1 + RegExp.$2 + '</a>' + RegExp.$3;
			}
		}
		
		html += '<tr class="' + status + '"><td>' 
			+ e.getAttribute("classname") + '</td><td>' 
			+ e.getAttribute("methodname") + '</td><td>' 
			+ status + '</td><td>' 
			+ message + '</td><td>' 
			+ e.getAttribute("time") + '</td><td>' 
			+ screenshot + '</td></tr>';
			
	}
	
	$('recordTableContainer').innerHTML = html + '</tbody></table>';
}

function showAllRecord() {
	var trEls = $$s($('recordTable'), 'tr');
	for (var i = 0; i < trEls.length; i++){
		var e = trEls[i];
		e.style.display = '';
	}
	$("ErrorNo").className = 'checked';
	$("FailureNo").className = 'checked';
	$("IgnoredNo").className = 'checked';
	$("SuccessNo").className = 'checked';
}

function toggleRecord(status) {
	var display = '';
	var el = $(status + "No");
	if (el.className == 'unchecked') {
		el.className = 'checked';
	} else {
		el.className = 'unchecked';
		display = 'none';
	}
	
	var trEls = $$s($('recordTable'), 'tr');
	for (var i = 0; i < trEls.length; i++){
		var e = trEls[i];
		if (e.className == status) {
			e.style.display = display;
		}
	}
}

function onload() {
	refresh();
}


]]>
</xsl:text>
</script>
<div id="data" style="display:none">
	<xsl:copy-of select="*" />
</div>
</body>
</html>
</xsl:template>
</xsl:stylesheet>    
   
