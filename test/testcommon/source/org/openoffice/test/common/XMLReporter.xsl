<?xml version="1.0" encoding="UTF-8" ?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="xml" encoding="utf-8" doctype-public="-//W3C//DTD XHTML 1.0 Strict//EN" doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd" />
<xsl:template match="/">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
	<style>
.Success {
	background:#22cc22;
}
.Failure {
	background:#FF4444;
}
.Ignored {
	background:#FFFF44;
}
.Error {
	background:#FF0000;
}
.recordTable {
    border-collapse:collapse;
}
.recordTable thead th{
    padding:5px 10px 5px 10px;
    color:#fff;
    font-size: 14px;
    background-color:#222;
    border-right:1px dotted #666;
}

.recordTable tbody td{
    padding:5px 10px 5px 10px;
    background-color:#f0f0f0;
    border-right:1px dotted #999;
    color:#333;
}

	</style>
	<title>JUnit Report</title>
</head>
<body>
<xml id="data" style="display:none">
	<xsl:copy-of select="*" />
</xml>

<table>
	<thead>
		<tr><th>Class</th><th>Name</th><th>Platform</th><th>Platform</th><th>Platform</th><th>Platform</th></tr>
	</thead>
	<tbody>
	</tbody>
</table>

<table class="recordTable">
	<thead>
		<tr><th>Name</th><th>Status</th><th width="80%">Message</th><th>Time</th><th>Screenshot</th></tr>
	</thead>
	<tbody id="recordTable">
	</tbody>
</table>


<script>
<xsl:text disable-output-escaping='yes'>
<![CDATA[
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
	var testcaseEls = $$s(data, 'testcase');
	var html = '';
	for (var i = 0; i < testcaseEls.length; i++){
		var e = testcaseEls[i], c;
		var status = 'Success', message = '';
		if (c = $$(e, 'failure')) {
			status = 'Failure';
			message = c.getAttribute("message");
		} else if (c = $$(e, 'error')) {
			status = 'Error';
			message = c.getAttribute("message");
		} else if (c = $$(e, 'ignored')) {
			status = 'Ignored';
			message = c.getAttribute("message");
		}
		message = message.replace(/&/g, "&amp;").replace(/</g, "&lt;").replace(/>/g, "&gt;");
		html += '<tr class="' + status + '"><td>' + e.getAttribute("name") + '</td><td>' + status + '</td><td>' + message + '</td><td>' + e.getAttribute("time") + '</td><td>' + '*' + '</td></tr>';
	}
	
	$("recordTable").innerHTML = html;
}

refresh();

]]>
</xsl:text>
</script>
</body>
</html>
</xsl:template>
</xsl:stylesheet>    
   