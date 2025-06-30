<%
    Option Explicit
    Response.Expires = 0
%>
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

<!-- #include file = "common.inc" -->

<%
    Session( "GIFID" ) = File_getDataVirtual( csFileCurrent, ".", ";" )( 0 )
%>

<html>

<head>
</head>

<frameset rows="*,2" border="0" frameborder="0" framespacing="0">
	<frame src="./show.asp" name="frame1" scrolling="yes"  resize marginwidth="0" marginheight="0" frameborder="0">
	<frame src="./poll.asp" name="frame2" scrolling="no" noresize marginwidth="0" marginheight="0" frameborder="0">
</frameset>

<noframes>

<body bgcolor="white">
    <meta http-equiv="refresh" content="1;URL=./show.asp">
</body>

</html>
