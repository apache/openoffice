<?xml version="1.0" encoding="UTF-8" ?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="html" encoding="utf-8" doctype-public="-//W3C//DTD XHTML 1.0 Strict//EN" doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd" />
<xsl:template match="/">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<style>
body {
	font: 90% Arial, sans-serif;
}

tr.Success {
	color:#118811;
}

tr.Failure {
	color:#F27D15;
}

tr.Ignored {
	color:#F4C700;
}

tr.Error {
	color:#FC130F;
}

th.Success {
	background:#118811;
}

th.Failure {
	background: #F27D15;
}

th.Ignored {
	background:#F4C700;
}

th.Error {
	background:#FC130F;
}

a.unchecked {
	text-decoration: none;
	padding: 0px 4px;
	color: #000000;
	font-weight: bold;
	border-bottom: 2px solid #AAAAAA;
}

a.checked {
	text-decoration: none;
	border-bottom: 2px solid #E6820E;
	padding: 0px 4px;
	color: #000000;
	font-weight: bold;
}

.container {
	width: 100%;
	float: left;
	margin-right: -300px;
}

.content {
	margin-right: 300px;
}

.sidebar {
	width: 300px;
	float: right;
}

table {
	font-size: 90%;
	border-collapse:collapse;
    width:100%;
    background: white;
	border: 1px solid #666;
}

td {
	border: 1px solid #BBB;
	padding: 4px;
	text-align: left;
	vertical-align: top;
}

th {
	background: #DDD;
	border: 1px solid #BBB;
	padding: 4px;
	width: 80px;
	font-weight: bold;
	text-align: left;
}

.pieContainer {
     height: 100px;
}
.pieBackground {
     background-color: grey;
     position: absolute;
     width: 100px;
     height: 100px;
     -moz-border-radius: 50px;
     -webkit-border-radius: 50px;
     -o-border-radius: 50px;
     border-radius: 50px;
     -moz-box-shadow: -1px 1px 3px #000;
     -webkit-box-shadow: -1px 1px 3px #000;
     -o-box-shadow: -1px 1px 3px #000;
     box-shadow: -1px 1px 3px #000;
} 

.pie {
         position: absolute;
         width: 100px;
         height: 100px;
         -moz-border-radius: 50px;
         -webkit-border-radius: 50px;
         -o-border-radius: 50px;
         border-radius: 50px;
         clip: rect(0px, 50px, 100px, 0px);
    }
.hold {
     position: absolute;
     width: 100px;
     height: 100px;
     -moz-border-radius: 50px;
     -webkit-border-radius: 50px;
     -o-border-radius: 50px;
     border-radius: 50px;
     clip: rect(0px, 100px, 100px, 50px);
}
#pieSlice1 .pie {
     background-color: #1b458b;
     -webkit-transform:rotate(50deg);
     -moz-transform:rotate(50deg);
     -o-transform:rotate(50deg);
     transform:rotate(50deg);
}

	</style>
	<title>JUnit Report</title>
</head>
<body onload="onload()">

<div class="container">
	<div class="content">
		<div style="font-weight: bold;padding: 6px">Information</div>
		<div style="padding: 6px">
		<table class="infoTable">
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
	<div style="font-weight: bold;padding: 6px">Summary</div>
	<div style="padding: 6px">
	<table class="summaryTable">
		<tr><th>All</th><td><a href="#" onclick="showAllRecord()" id="AllNo" class="checked" >0</a></td></tr>
		<tr><th class="Success">Success</th><td><a href="#" onclick="toggleRecord('Success')" class="checked" id="SuccessNo">0</a></td></tr>
		<tr><th class="Failure">Failure</th><td><a href="#" onclick="toggleRecord('Failure')" class="checked" id="FailureNo">0</a></td></tr>
		<tr><th class="Error">Error</th><td><a href="#" onclick="toggleRecord('Error')" class="checked" id="ErrorNo">0</a></td></tr>
		<tr><th class="Ignored">Ignored</th><td><a href="#" onclick="toggleRecord('Ignored')" class="checked" id="IgnoredNo">0</a></td></tr>
	</table>
	</div>
</div>
<div style="clear:both; padding: 6px;font-weight: bold">
	<div style="float:left">Record</div>
	<div style="float:right;">
	View:
	</div>
</div>
<div style="clear:both;padding: 6px">
	<table class="recordTable">
		<thead>
			<tr><th>Class</th><th>Method</th><th>Status</th><th width="80%">Message</th><th>Time</th><th>Screenshot</th></tr>
		</thead>
		<tbody id="recordTable">
		</tbody>
	</table>
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
	
	$('info.app.buildid').innerHTML = props['info.app.buildid'] || 'Unknown';
	$('info.app.AllLanguages').innerHTML = props['info.app.AllLanguages'] || '';
	$('info.os.name').innerHTML = props['info.os.name'] || 'Unknown';
	$('info.os.version').innerHTML = props['info.os.version'] || 'Unknown';
	$('info.os.arch').innerHTML = props['info.os.arch'] || 'Unknown';
	if (props['info.app.Revision']) 
		$('info.app.Revision').innerHTML = '<a href="https://fisheye6.atlassian.com/changelog/~br=trunk/ooo/?showid=' + props['info.app.Revision'] + '">'  + props['info.app.Revision'] + '</a>';
	$('info.hostname').innerHTML = props['info.hostname'] || 'Unknown';
	$('info.ip').innerHTML = props['info.ip'] || '0.0.0.0';
	$('java.runtime.version').innerHTML = props['java.runtime.version'] || 'Unknown';
	
	var testcaseEls = $$s(data, 'testcase');
	var html = '';
	for (var i = 0; i < testcaseEls.length; i++){
		var e = testcaseEls[i], c = null;
		var status = 'Success', message = '', screenshot= '';
		if (c = $$(e, 'failure')) {
			status = 'Failure';
			message = text(c.getAttribute("message"));
			screenshot = '<a href="../screenshot/' + e.getAttribute("name") + '.png">Screenshot</a>';
		} else if (c = $$(e, 'error')) {
			status = 'Error';
			message = text(c.getAttribute("message"));
			screenshot = '<a href="../screenshot/' + e.getAttribute("name") + '.png">Screenshot</a>';
		} else if (c = $$(e, 'ignored')) {
			status = 'Ignored';
			message = text(c.getAttribute("message"));
		    var bugPattern = /(.*)#(\d+)(.*)/;
		    bugPattern.test(message);
		    message = '<a href="https://issues.apache.org/ooo/show_bug.cgi?id=' + RegExp.$2 + '">' + RegExp.$1 + RegExp.$2 + '</a>' + RegExp.$3;
		}
		
		html += '<tr class="' + status + '"><td>' 
			+ e.getAttribute("classname") + '</td><td>' 
			+ e.getAttribute("methodname") + '</td><td>' 
			+ status + '</td><td>' 
			+ message + '</td><td>' 
			+ e.getAttribute("time") + '</td><td>' 
			+ screenshot + '</td></tr>';
	}
	
	$('recordTable').innerHTML = html;
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
<xml id="data" style="display:none">
	<xsl:copy-of select="*" />
</xml>
</body>
</html>
</xsl:template>
</xsl:stylesheet>    
   