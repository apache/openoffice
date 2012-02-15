<?xml version='1.0' encoding="UTF-8"?>

<!-- 
  FILTER FOR OPENOFFICE.ORG 2+ 
  v2.0BETA7 - Sep 07 2009
-->

<xsl:stylesheet version="1.0" 
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		xmlns:office="urn:oasis:names:tc:opendocument:xmlns:office:1.0"
		xmlns:style="urn:oasis:names:tc:opendocument:xmlns:style:1.0"
		xmlns:text="urn:oasis:names:tc:opendocument:xmlns:text:1.0"
		xmlns:table="urn:oasis:names:tc:opendocument:xmlns:table:1.0"
		xmlns:draw="urn:oasis:names:tc:opendocument:xmlns:drawing:1.0"
		xmlns:fo="urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0"
		xmlns:xlink="http://www.w3.org/1999/xlink"
		xmlns:dc="http://purl.org/dc/elements/1.1/"
		xmlns:meta="urn:oasis:names:tc:opendocument:xmlns:meta:1.0"
		xmlns:number="urn:oasis:names:tc:opendocument:xmlns:datastyle:1.0"
		xmlns:svg="urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0"
		xmlns:chart="urn:oasis:names:tc:opendocument:xmlns:chart:1.0"
		xmlns:dr3d="urn:oasis:names:tc:opendocument:xmlns:dr3d:1.0"
		xmlns:math="http://www.w3.org/1998/Math/MathML"
		xmlns:form="urn:oasis:names:tc:opendocument:xmlns:form:1.0"
		xmlns:script="urn:oasis:names:tc:opendocument:xmlns:script:1.0"
		xmlns:ooo="http://openoffice.org/2004/office"
		xmlns:ooow="http://openoffice.org/2004/writer"
		xmlns:oooc="http://openoffice.org/2004/calc"
		xmlns:dom="http://www.w3.org/2001/xml-events"
		xmlns:xforms="http://www.w3.org/2002/xforms"
		xmlns:xsd="http://www.w3.org/2001/XMLSchema"
		xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" office:version="1.0" 
		exclude-result-prefixes="office meta table number dc fo xlink chart math script xsl draw svg dr3d form text style xsi xsd xforms dom oooc ooow ooo">

<xsl:output method="xml" indent="no" omit-xml-declaration="no"  />
<!--
<xsl:output doctype-public="-//OpenOffice.org//Help Document//EN" />
<xsl:output doctype-system="http://documentation.openoffice.org/xmlhelp.dtd" />
//-->
<xsl:strip-space elements="*" />
<xsl:param name="imgroot" select="'default_images/'"/>

<!--
#############################################################################
 
	StarOffice Help Format Output Filter Stylesheet - Jan 26, 2006
	==================================================================
	(c) 2004, Sun Microsystems, Inc. - FPE

#############################################################################
-->

<!-- 
###################################################### 
META DATA
###################################################### 
-->
<xsl:variable name="title" select="/office:document/office:meta/dc:title"/>
<xsl:variable name="filename" select="/office:document/office:meta/dc:subject" />
<xsl:variable name="path">
	<xsl:call-template name="getpath">
		<xsl:with-param name="file"><xsl:value-of select="$filename" /></xsl:with-param>
	</xsl:call-template>
</xsl:variable>
<xsl:variable name="fileonly" select="substring-after($filename,$path)"/>
<xsl:variable name="topic_id" select="/office:document/office:meta/meta:user-defined[@meta:name='ID']"/>
<xsl:variable name="topic_indexer" select="/office:document/office:meta/meta:user-defined[@meta:name='Indexer']"/>
<xsl:variable name="kw" select="/office:document/office:meta/meta:keywords/meta:keyword"/>

<xsl:variable name="topic_status">
	<xsl:choose>
		<xsl:when test="contains('DRAFT FINAL PUBLISH STALLED DEPRECATED', $kw)">
			<xsl:value-of select="$kw"/>
		</xsl:when>
		<xsl:otherwise>
			<xsl:value-of select="''"/>
		</xsl:otherwise>
	</xsl:choose>
</xsl:variable>

<!-- REMOVED DUE TO PROBLEMS WITH CVS MERGE CONFLICTS
<xsl:variable name="history_created" select="/office:document/office:meta/meta:user-defined[@meta:name='Created']"/>
<xsl:variable name="history_created_date" select="/office:document/office:meta/meta:creation-date"/>
<xsl:variable name="history_lastedited" select="/office:document/office:meta/meta:user-defined[@meta:name='Lastedited']"/>
<xsl:variable name="history_lastedited_date" select="/office:document/office:meta/dc:date"/>
//-->

<!-- 
###################################################### 
DOCUMENT SKELETON
###################################################### 
-->
<xsl:template match="/">
	
	<helpdocument version="1.0">
	<xsl:call-template name="lf"/>
	<xsl:call-template name="licheader"/> <!-- inserts the license header -->
	<xsl:call-template name="lf"/>
	<meta>
	<xsl:call-template name="lf"/>		
			<topic id="{$topic_id}">
				<xsl:if test="not($topic_indexer = '')">
					<xsl:attribute name="indexer"><xsl:value-of select="$topic_indexer"/></xsl:attribute>
				</xsl:if>
				<xsl:if test="not($topic_status = '')">
					<xsl:attribute name="status"><xsl:value-of select="$topic_status"/></xsl:attribute>
				</xsl:if>
				<xsl:call-template name="lf"/>
				<title xml-lang="en-US" id="tit"><xsl:value-of select="$title"/></title>
				<xsl:call-template name="lf"/>
				<filename><xsl:value-of select="$filename"/></filename>
			<xsl:call-template name="lf"/>
			</topic>
			<!-- REMOVED DUE TO PROBLEMS WITH CVS MERGE CONFLICTS
			<history>
				<created date="{$history_created_date}"><xsl:value-of select="$history_created"/></created>
				<lastedited date="{$history_lastedited_date}"><xsl:value-of select="$history_lastedited"/></lastedited>
			</history>
			//-->
		<xsl:call-template name="lf"/>
		</meta>
		<xsl:call-template name="lf"/>
		<body>
			<xsl:apply-templates select="/office:document/office:body/office:text" />
		<xsl:call-template name="lf"/>
		</body>
	<xsl:call-template name="lf"/>
	</helpdocument>
</xsl:template>


<!-- 
###################################################### 
AHELP 
	 hid CDATA #REQUIRED
   visibility (hidden | visible) #IMPLIED
###################################################### 
-->

<xsl:template match="text:variable-set[@text:name='AHID_']">
	<xsl:variable name="hid"><xsl:value-of select="substring-before(substring-after(.,'hid=&quot;'),'&quot;&gt;')"/></xsl:variable>
	<xsl:text disable-output-escaping="yes">&lt;ahelp hid=&quot;</xsl:text>
	<xsl:value-of select="$hid"/>
	<xsl:text disable-output-escaping="yes">&quot; visibility=&quot;hidden&quot;&gt;</xsl:text>
</xsl:template>

<xsl:template match="text:variable-set[@text:name='AVIS_']">
	<xsl:variable name="hid"><xsl:value-of select="substring-before(substring-after(.,'hid=&quot;'),'&quot;&gt;')"/></xsl:variable>
	<xsl:text disable-output-escaping="yes">&lt;ahelp hid=&quot;</xsl:text>
	<xsl:value-of select="$hid"/>
	<xsl:text disable-output-escaping="yes">&quot;&gt;</xsl:text>
</xsl:template>

<xsl:template match="text:variable-set[@text:name='_AHID']">
	<xsl:text disable-output-escaping="yes">&lt;/ahelp&gt;</xsl:text>
</xsl:template>

<xsl:template match="text:variable-set[@text:name='_AVIS']">
	<xsl:text disable-output-escaping="yes">&lt;/ahelp&gt;</xsl:text>
</xsl:template>


<!-- 
###################################################### 
ALT, SEE IMAGE
  xml-lang CDATA #REQUIRED (is always en-US for the source)
  id CDATA #REQUIRED
  localize CDATA #IMPLIED 
###################################################### 
-->

<!-- 
###################################################### 
BODY, SEE HEADER
  # localize CDATA #IMPLIED
###################################################### 
-->
<xsl:template match="office:body">
	<xsl:apply-templates />
</xsl:template>

<xsl:template match="office:text">
	<xsl:apply-templates />
</xsl:template>


<!-- 
######################################################
BOOKMARK
  branch CDATA #REQUIRED
  xml-lang CDATA #REQUIRED (is always en-US for the source)
  id CDATA #REQUIRED
  localize CDATA #IMPLIED 
###################################################### 
-->

<xsl:template match="text:variable-set[@text:name='BOOKMARK_']">
	<xsl:variable name="branch"><xsl:value-of select="substring-before(substring-after(.,'branch=&quot;'),'&quot;')"/></xsl:variable>
	<xsl:variable name="id"><xsl:value-of select="substring-before(substring-after(.,'id=&quot;'),'&quot;')"/></xsl:variable>
	<xsl:variable name="localize"><xsl:value-of select="substring-before(substring-after(.,'localize=&quot;'),'&quot;')"/></xsl:variable>
	<xsl:text disable-output-escaping="yes">
&lt;bookmark xml-lang="en-US" branch=&quot;</xsl:text><xsl:value-of select="$branch"/>
	<xsl:text disable-output-escaping="yes">&quot; id=&quot;</xsl:text>
	<xsl:value-of select="$id"/>
	<xsl:if test="not($localize='')">
	<xsl:text disable-output-escaping="yes">&quot; localize=&quot;</xsl:text>
	<xsl:value-of select="$localize"/>
	</xsl:if>
	<xsl:text disable-output-escaping="yes">&quot;&gt;</xsl:text>
</xsl:template>

<xsl:template match="text:variable-set[@text:name='BOOKMARK']">
	<xsl:variable name="branch"><xsl:value-of select="substring-before(substring-after(.,'branch=&quot;'),'&quot;')"/></xsl:variable>
	<xsl:variable name="id"><xsl:value-of select="substring-before(substring-after(.,'id=&quot;'),'&quot;')"/></xsl:variable>
	<xsl:variable name="localize"><xsl:value-of select="substring-before(substring-after(.,'localize=&quot;'),'&quot;')"/></xsl:variable>
	<xsl:text disable-output-escaping="yes">
&lt;bookmark xml-lang="en-US" branch=&quot;</xsl:text><xsl:value-of select="$branch"/>
	<xsl:text disable-output-escaping="yes">&quot; id=&quot;</xsl:text>
	<xsl:value-of select="$id"/>
	<xsl:if test="not($localize='')">
	<xsl:text disable-output-escaping="yes">&quot; localize=&quot;</xsl:text>
	<xsl:value-of select="$localize"/>
	</xsl:if>
	<xsl:text disable-output-escaping="yes">&quot;/&gt;</xsl:text>
</xsl:template>


<xsl:template match="text:variable-set[@text:name='_BOOKMARK']">
	<xsl:text disable-output-escaping="yes">
&lt;/bookmark&gt;</xsl:text>
</xsl:template>

<!-- 
###################################################### 
BOOKMARK_VALUE 
###################################################### 
-->
<xsl:template match="text:variable-set[@text:name='BOOKMARKVALUE']">
	<xsl:call-template name="lf" />
	<bookmark_value><xsl:apply-templates />	</bookmark_value>
</xsl:template>

<!-- 
###################################################### 
BR 
###################################################### 
-->
<xsl:template match="text:line-break">
	<xsl:variable name="style">
		<xsl:call-template name="getmasterstyle">
			<xsl:with-param name="style">
				<xsl:call-template name="getdisplayname">
					<xsl:with-param name="style" select="ancestor::text:p/@text:style-name"/>
				</xsl:call-template>
			</xsl:with-param>
		</xsl:call-template>
	</xsl:variable>
	<xsl:if test="not(starts-with($style,'hlp_aux_'))">
		<br />
	</xsl:if>
</xsl:template>

<!-- 
###################################################### 
CAPTION 
  o xml-lang CDATA #REQUIRED (is always en-US for the source)
  + id CDATA #REQUIRED   (in parent elements)
  + localize CDATA #IMPLIED (in parent elements)
###################################################### 
-->

<!-- distinguish between table and image caption -->

<xsl:template match="text:variable-set[@text:name='IMGCAPTION_']">
	<xsl:variable name="id"><xsl:value-of select="substring-before(substring-after(.,'ID=&quot;'),'&quot;')"/></xsl:variable>
	<xsl:variable name="localize"><xsl:value-of select="substring-before(substring-after(.,'LOCALIZE=&quot;'),'&quot;')"/></xsl:variable>
	<xsl:text disable-output-escaping="yes">
&lt;caption xml-lang="en-US" id=&quot;</xsl:text>
	<xsl:value-of select="$id"/>
	<xsl:if test="not($localize='')">
	<xsl:text disable-output-escaping="yes">&quot; localize=&quot;</xsl:text>
	<xsl:value-of select="$localize"/>
	</xsl:if>
	<xsl:text disable-output-escaping="yes">&quot;&gt;</xsl:text>
</xsl:template>

<xsl:template match="text:variable-set[@text:name='_IMGCAPTION']">
	<xsl:text disable-output-escaping="yes">&lt;/caption&gt;</xsl:text>
</xsl:template>

<!-- 
###################################################### 
CASE
  select CDATA #REQUIRED
###################################################### 
-->

<xsl:template match="text:variable-set[@text:name='CASE_']">
	<xsl:variable name="select"><xsl:value-of select="substring-before(substring-after(.,'select=&quot;'),'&quot;')"/></xsl:variable>
	<xsl:text disable-output-escaping="yes">
&lt;case select=&quot;</xsl:text>
	<xsl:value-of select="$select"/>
	<xsl:text disable-output-escaping="yes">&quot;&gt;</xsl:text>
</xsl:template>

<xsl:template match="text:variable-set[@text:name='_CASE']">
<xsl:text disable-output-escaping="yes">
&lt;/case&gt;</xsl:text>
</xsl:template>


<!-- 
###################################################### 
CASEINLINE
  select CDATA #REQUIRED 
###################################################### 
-->

<xsl:template match="text:variable-set[@text:name='CASEINLINE_']">
	<xsl:variable name="select"><xsl:value-of select="substring-before(substring-after(.,'select=&quot;'),'&quot;')"/></xsl:variable>
	<xsl:text disable-output-escaping="yes">&lt;caseinline select=&quot;</xsl:text>
	<xsl:value-of select="$select"/>
	<xsl:text disable-output-escaping="yes">&quot;&gt;</xsl:text>
</xsl:template>

<xsl:template match="text:variable-set[@text:name='_CASEINLINE']">
<xsl:text disable-output-escaping="yes">
&lt;/caseinline&gt;</xsl:text>
</xsl:template>

<!-- 
###################################################### 
COMMENT 
###################################################### 
-->

<xsl:template match="text:variable-set[@text:name='COMMENT_']">
<xsl:text disable-output-escaping="yes">&lt;comment&gt;</xsl:text>
</xsl:template>

<xsl:template match="text:variable-set[@text:name='COMMENT']">
<xsl:apply-templates />
</xsl:template>

<xsl:template match="text:variable-set[@text:name='_COMMENT']">
<xsl:text disable-output-escaping="yes">&lt;/comment&gt;</xsl:text>
</xsl:template>

<!-- 
###################################################### 
CREATED,  SEE HEADER
###################################################### 
-->


<!-- 
###################################################### 
DEFAULT 
###################################################### 
-->

<xsl:template match="text:variable-set[@text:name='DEFAULT_']">
<xsl:text disable-output-escaping="yes">
&lt;default&gt;</xsl:text>
</xsl:template>

<xsl:template match="text:variable-set[@text:name='_DEFAULT']">
<xsl:text disable-output-escaping="yes">
&lt;/default&gt;</xsl:text>
</xsl:template>

<!-- 
###################################################### 
DEFAULTINLINE 
###################################################### 
-->

<xsl:template match="text:variable-set[@text:name='DEFAULTINLINE_']">
<xsl:text disable-output-escaping="yes">&lt;defaultinline&gt;</xsl:text>
</xsl:template>

<xsl:template match="text:variable-set[@text:name='_DEFAULTINLINE']">
<xsl:text disable-output-escaping="yes">&lt;/defaultinline&gt;</xsl:text>
</xsl:template>

<!-- 
###################################################### 
EMBED
	href CDATA #REQUIRED
  role CDATA #IMPLIED
  level CDATA #IMPLIED
###################################################### 
-->

<xsl:template match="text:variable-set[@text:name='EMBED']">
	<xsl:variable name="href"><xsl:value-of select="substring-before(substring-after(.,'href=&quot;'),'&quot;')"/></xsl:variable>
	<xsl:variable name="role"><xsl:value-of select="substring-before(substring-after(.,'role=&quot;'),'&quot;')"/></xsl:variable>
	<xsl:variable name="level"><xsl:value-of select="substring-before(substring-after(.,'level=&quot;'),'&quot;')"/></xsl:variable>

	<embed href="{$href}">
		<xsl:if test="not($role = '')"><xsl:attribute name="role"><xsl:value-of select="$role"/></xsl:attribute></xsl:if>
		<xsl:if test="not($level = '')"><xsl:attribute name="level"><xsl:value-of select="$level"/></xsl:attribute></xsl:if>
	</embed>
</xsl:template>

<!-- 
###################################################### 
EMBEDVAR
 href CDATA #REQUIRED
 markup (keep | ignore) #IMPLIED
###################################################### 
-->

<xsl:template match="text:variable-set[@text:name='EMBEDVAR']">
	<xsl:variable name="href"><xsl:value-of select="substring-before(substring-after(.,'var=&quot;'),'&quot;')"/></xsl:variable>
	<xsl:variable name="markup"><xsl:value-of select="substring-before(substring-after(.,'markup=&quot;'),'&quot;')"/></xsl:variable>

	<embedvar href="{$href}">
		<xsl:if test="not($markup = '')"><xsl:attribute name="markup"><xsl:value-of select="$markup"/></xsl:attribute></xsl:if>
	</embedvar>
</xsl:template>

<!-- 
###################################################### 
EMPH -> SEE TEXT:SPAN 
###################################################### 
-->

<!-- 
###################################################### 
FILENAME,  SEE HEADER
###################################################### 
-->

<!-- 
###################################################### 
HELPDOCUMENT,  SEE HEADER
  + version CDATA #REQUIRED
###################################################### 
-->

<!-- 
###################################################### 
HISTORY,  SEE HEADER
###################################################### 
-->

<!-- 
###################################################### 
IMAGE 
  src CDATA #REQUIRED
  width CDATA #IMPLIED
  height CDATA #IMPLIED
  id CDATA #REQUIRED
###################################################### 
-->

<xsl:template match="text:variable-set[@text:name='IMG_']">
	<xsl:variable name="id">
		<xsl:choose>
			<xsl:when test="contains(following::draw:frame/@draw:name,' localize')">
				<xsl:value-of select="substring-before(following::draw:frame/@draw:name,' ')"/>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="following::draw:frame/@draw:name"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	
	<xsl:variable name="width"><xsl:value-of select="following::draw:frame/@svg:width"/></xsl:variable>
	<xsl:variable name="height"><xsl:value-of select="following::draw:frame/@svg:height"/></xsl:variable>
	
	
	<xsl:variable name="localize">
		<xsl:choose>
			<xsl:when test="contains(following::draw:frame/@draw:name,' localize')">
				<xsl:value-of select="substring-before(substring-after(following::draw:frame/@draw:name,'localize=&quot;'),'&quot;')"/>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="''"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	
	<xsl:variable name="src">
		<xsl:choose>
			<xsl:when test="contains(following::draw:frame/draw:image/@xlink:href,$imgroot)">
				<xsl:value-of select="substring-after(following::draw:frame/draw:image/@xlink:href,$imgroot)"/>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="following::draw:frame/draw:image/@xlink:href"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	
	
	<xsl:text disable-output-escaping="yes">&lt;image id=&quot;</xsl:text><xsl:value-of select="$id"/><xsl:text disable-output-escaping="yes">&quot; src=&quot;</xsl:text><xsl:value-of select="$src"/>
	<!--
	<xsl:text disable-output-escaping="yes">&quot; width=&quot;</xsl:text>
	<xsl:value-of select="$width"/>
	<xsl:text disable-output-escaping="yes">&quot; height=&quot;</xsl:text>
	<xsl:value-of select="$height"/>
	//-->
	<xsl:if test="$localize != ''">
		<xsl:text disable-output-escaping="yes">&quot; localize=&quot;</xsl:text><xsl:value-of select="$localize"/>
	</xsl:if>
	<xsl:text disable-output-escaping="yes">&quot;&gt;</xsl:text>

	<xsl:variable name="altid"><xsl:value-of select="concat('alt_',substring-after($id,'img_'))"/></xsl:variable>
		<xsl:choose>
			<xsl:when test="following::draw:frame/svg:title">  <!-- fixed, changed svg:desc to svg:title -->
			
				<alt xml-lang="en-US" id="{$altid}">
					<xsl:choose>
						<xsl:when test="starts-with(following::draw:frame/svg:title,'LOCALIZE=')">
							<xsl:attribute name="localize">
								<xsl:value-of select="substring-after(substring-before(following::draw:frame/svg:title,'#'),'LOCALIZE=')"/>
							</xsl:attribute>
							<xsl:value-of select="substring-after(following::draw:frame/svg:title,'#')"/>
						</xsl:when>
						<xsl:otherwise>
							<xsl:value-of select="following::draw:frame/svg:title"/>
						</xsl:otherwise>
					</xsl:choose>
				</alt>
				
			</xsl:when>
			<xsl:otherwise>
				<alt xml-lang="en-US" id="{$altid}">
					<xsl:value-of select="'{ENTER ALTERNATE DESCRIPTION HERE}'"/>
				</alt>
			</xsl:otherwise>
		</xsl:choose>

</xsl:template>

<xsl:template match="text:variable-set[@text:name='_IMG']">
	<xsl:text disable-output-escaping="yes">&lt;/image&gt;</xsl:text>
</xsl:template>
<!-- 
###################################################### 
ITEM -> SEE TEXT:SPAN
  + type CDATA #REQUIRED
###################################################### 
-->

<!-- 
###################################################### 
LASTEDITED,  SEE HEADER
  + date CDATA #REQUIRED
###################################################### 
-->

<!-- 
###################################################### 
LINK
  + href CDATA #REQUIRED
  + name CDATA #REQUIRED
  + type CDATA #IMPLIED
  + target CDATA #IMPLIED
###################################################### 
-->

<xsl:template match="text:variable-set[@text:name='LINK_']">
	
	<xsl:variable name="href">
		<xsl:value-of select="substring-before(substring-after(.,'href=&quot;'),'&quot;')"/>
	</xsl:variable>
	
	<xsl:variable name="type">
		<xsl:value-of select="substring-before(substring-after(.,'type=&quot;'),'&quot;')"/>
	</xsl:variable>
	
	<xsl:variable name="target">
		<xsl:value-of select="substring-before(substring-after(.,'target=&quot;'),'&quot;')"/>
	</xsl:variable>
	
	<xsl:variable name="name">
		<xsl:value-of select="substring-before(substring-after(.,'name=&quot;'),'&quot;')"/>
	</xsl:variable>
	
	<xsl:text disable-output-escaping="yes">&lt;link</xsl:text>
	<xsl:value-of select="concat(' href=&quot;',$href,'&quot;')"/>
	<xsl:if test="not($name = '')">
		<xsl:value-of select="concat(' name=&quot;',$name,'&quot;')"/>
	</xsl:if>
	<xsl:if test="not($type = '')">
			<xsl:value-of select="concat(' type=&quot;',$type,'&quot;')"/>
	</xsl:if>
	<xsl:if test="not($target = '')">
			<xsl:value-of select="concat(' target=&quot;',$target,'&quot;')"/>
	</xsl:if>
	<xsl:text disable-output-escaping="yes">&gt;</xsl:text>
</xsl:template>

<xsl:template match="text:variable-set[@text:name='_LINK']">
	<xsl:text disable-output-escaping="yes">&lt;/link&gt;</xsl:text>
</xsl:template>


<!-- 
###################################################### 
LIST
  + type CDATA #REQUIRED
  + startwith CDATA #IMPLIED
  # format (1 | i | I | a | A) #IMPLIED
  # bullet (disc | circle | square) #IMPLIED
  # localize CDATA #IMPLIED
  # sorted (asc | desc) #IMPLIED
###################################################### 
-->
<xsl:template match="text:list">
<xsl:param name="stylename"><xsl:value-of select="@text:style-name"/></xsl:param>

	<xsl:choose>
	<!-- ORDERED LISTS -->
	<xsl:when test="//text:list-style[@style:name=$stylename]/text:list-level-style-number[@text:level='1']"> <!-- fixed list bug --> 
	<xsl:text>
</xsl:text>
	<list type="ordered">
	
		<xsl:variable name="liststyle">
			<xsl:call-template name="getdisplayname">
				<xsl:with-param name="style" select="@text:style-name"/>
			</xsl:call-template>
		</xsl:variable>
	
		<xsl:variable name="startvalue">
	
			<xsl:choose>
			
				<xsl:when test="text:list-item[1]/@text:start-value">
					<xsl:value-of select="text:list-item[1]/@text:start-value"/>
				</xsl:when>
			
				<xsl:when test="//text:list-style[@style:name=$liststyle]">
					<xsl:value-of select="//text:list-style[@style:name=$liststyle]/text:list-level-style-number[@text:level='1']/@text:start-value"/>
				</xsl:when>
				
				<xsl:when test="@text:continue-numbering='true'">
					<xsl:call-template name="getprevol">
						<xsl:with-param name="node" select="."/>
						<xsl:with-param name="nodepos" select="1"/>
						<xsl:with-param name="ct" select="1"/>
					</xsl:call-template>
				</xsl:when>
				
				<xsl:otherwise>
					<xsl:value-of select="'1'"/>
				</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>
		
	<xsl:if test="not($startvalue='1') and not($startvalue='')">
			<xsl:attribute name="startwith">
				<xsl:value-of select="$startvalue"/>
			</xsl:attribute>
		</xsl:if>
		
		<xsl:apply-templates />
	<xsl:text>
</xsl:text>
	</list>
	
	</xsl:when>	
	
	<!-- UNORDERED LISTS -->
	<xsl:when test="//text:list-style[@style:name=$stylename]/text:list-level-style-bullet[@text:level='1']"> <!-- fixed list bug --> 

	<xsl:call-template name="lf"/>
	<list type="unordered">
		<xsl:variable name="masterstyle">
			<xsl:call-template name="getmasterstyle">
				<xsl:with-param name="style"><xsl:value-of select="@style:name"/></xsl:with-param>
			</xsl:call-template>
		</xsl:variable>
		<xsl:apply-templates />
	</list>
	
	</xsl:when>
	<xsl:otherwise>
	<list type="UNKNOWN TYPE">
		<xsl:apply-templates />
	</list>
	
	</xsl:otherwise>
	</xsl:choose>

</xsl:template>
	
	
<!-- 
###################################################### 
LISTITEM 
  # format (1 | i | I | a | A) #IMPLIED
  # bullet (disc | circle | square) #IMPLIED
  # localize CDATA #IMPLIED
  # class CDATA #IMPLIED
###################################################### 
-->
<xsl:template match="text:list-item">
	<xsl:text>
</xsl:text>
	<listitem>
		<xsl:apply-templates />
	<xsl:text>
</xsl:text>
	</listitem>
</xsl:template>

<!-- 
###################################################### 
META, SEE HEADER
###################################################### 
-->

<!-- 
###################################################### 
OBJECT
  + type CDATA #REQUIRED
  + id CDATA #REQUIRED
  + data CDATA #REQUIRED
  + height CDATA #IMPLIED
  + width CDATA #IMPLIED
###################################################### 
-->

<xsl:template match="text:variable-set[@text:name='OBJECT']">
	<xsl:variable name="type">
		<xsl:value-of select="substring-before(substring-after(.,'TYPE=&quot;'),'&quot;')"/>
	</xsl:variable>
	<xsl:variable name="id">
		<xsl:value-of select="substring-before(substring-after(.,'ID=&quot;'),'&quot;')"/>
	</xsl:variable>
	<xsl:variable name="data">
		<xsl:value-of select="substring-before(substring-after(.,'DATA=&quot;'),'&quot;')"/>
	</xsl:variable>
	<xsl:variable name="height">
		<xsl:value-of select="substring-before(substring-after(.,'HEIGHT=&quot;'),'&quot;')"/>
	</xsl:variable>
	<xsl:variable name="width">
		<xsl:value-of select="substring-before(substring-after(.,'WIDTH=&quot;'),'&quot;')"/>
	</xsl:variable>

	<object id="{$id}" type="{$type}" data="{$data}">
		<xsl:if test="not($height = '')"><xsl:attribute name="height"><xsl:value-of select="$height" /></xsl:attribute></xsl:if>
		<xsl:if test="not($width = '')"><xsl:attribute name="width"><xsl:value-of select="$width" /></xsl:attribute></xsl:if>
	</object>
	
</xsl:template>


<!-- 
###################################################### 
PARAGRAPH 
  + role CDATA #REQUIRED
  + level CDATA #IMPLIED
  + id CDATA #REQUIRED
  + l10n CDATA #REQUIRED
  o xml-lang CDATA #REQUIRED (is always en-US for the source)
  + oldref CDATA #IMPLIED
  + localize CDATA #IMPLIED
###################################################### 
-->

<xsl:template match="text:h | text:p">

	<xsl:variable name="masterstyle">
		<xsl:call-template name="getmasterstyle">
			<xsl:with-param name="style">
				<xsl:call-template name="getdisplayname">
					<xsl:with-param name="style" select="@text:style-name"/>
				</xsl:call-template>
			</xsl:with-param>
		</xsl:call-template>
	</xsl:variable>


	<xsl:variable name="role">
		<xsl:choose>
			<xsl:when test="name(.)='text:h'">
				<xsl:value-of select="'heading'"/>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="substring-after($masterstyle,'hlp_')"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	
	<xsl:variable name="level">
		<xsl:choose>
			<xsl:when test="name(.)='text:h'">
				<xsl:value-of select="@text:outline-level"/>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="''"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>

	<xsl:choose>
	<xsl:when test="not(starts-with($masterstyle,'hlp_'))"/>
	<xsl:when test="not(starts-with($masterstyle,'hlp_aux_') or ancestor::office:annotation or . = '')">
	
	
		<xsl:variable name="id">
			<xsl:choose>
				<xsl:when test="(descendant::text:variable-set[@text:name='ID']/@text:display='none')">
					 	<xsl:value-of select="descendant::text:variable-set[@text:name='ID']/@office:string-value"/>
				</xsl:when>
				<xsl:otherwise>
					<xsl:value-of select="descendant::text:variable-set[@text:name='ID']"/>
				</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>

			
		<xsl:variable name="localize">
			<xsl:choose>
				<xsl:when test="substring($id,string-length($id)-6)='_NOL10N'">
					<xsl:value-of select="'FALSE'"/></xsl:when>
				<xsl:otherwise><xsl:value-of select="'TRUE'"/></xsl:otherwise>
			</xsl:choose>
		</xsl:variable>
		
		<xsl:variable name="real_id">
			<xsl:choose>
				<xsl:when test="$localize='TRUE'"><xsl:value-of select="$id"/></xsl:when>
				<xsl:otherwise><xsl:value-of select="substring-before($id,'_NOL10N')"/></xsl:otherwise>
			</xsl:choose>
		</xsl:variable>
		
		
		<!-- testing only, basic would need to be adjusted and also @xml:id is not visible in the UI
		<xsl:variable name="id" select="@xml:id"/>
		//-->
		
		<!-- remove these, these are no longer needed. no l10n paragraphs should rather get no @xml:lang
		<xsl:variable name="l10n">
			<xsl:choose>
				<xsl:when test="contains(descendant::text:variable-set[@text:name='ID'],'l10n')">
					<xsl:value-of select="substring-before(substring-after(descendant::text:variable-set[@text:name='ID'],'l10n=&quot;'),'&quot;')"/>
				</xsl:when>
				<xsl:when test="(descendant::text:variable-set[@text:name='ID']='') or not(descendant::text:variable-set[@text:name='ID'])">
					<xsl:value-of select="'NEW'"/>
				</xsl:when>
			</xsl:choose>
		</xsl:variable>
			
		<xsl:variable name="oldref">
			<xsl:choose>
				<xsl:when test="contains(descendant::text:variable-set[@text:name='ID'],'oldref')">
					<xsl:value-of select="substring-before(substring-after(descendant::text:variable-set[@text:name='ID'],'oldref=&quot;'),'&quot;')"/>
				</xsl:when>
			</xsl:choose>
		</xsl:variable>
		
	//-->		
		
		<xsl:text>
</xsl:text>
		<paragraph id="{$real_id}" role="{$role}" xml-lang="en-US"><xsl:if test="$localize='FALSE'">
				<xsl:attribute name="localize"><xsl:value-of select="'false'"/></xsl:attribute>
			</xsl:if>
			<xsl:if test="not($level = '')"><xsl:attribute name="level"><xsl:value-of select="$level" /></xsl:attribute></xsl:if><xsl:apply-templates /></paragraph>
	</xsl:when>
	<xsl:when test="$masterstyle='hlp_aux_tablecaption'"/>
	<xsl:otherwise>
		<xsl:apply-templates select="child::*"/>
	</xsl:otherwise>
	</xsl:choose>  
</xsl:template>


<!-- 
###################################################### 
SECTION
  + id CDATA #REQUIRED
  + localize CDATA #IMPLIED 
###################################################### 
-->
<xsl:template match="text:section">
	<xsl:variable name="id">
		<xsl:choose>
			<xsl:when test="contains(@text:name,'__NOLOCALIZE')">
				<xsl:value-of select="substring-before(@text:name,'__NOLOCALIZE')"/>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="@text:name"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	<xsl:variable name="localize">
		<xsl:if test="contains(@text:name,'__NOLOCALIZE')">
			<xsl:value-of select="'false'"/>
		</xsl:if>
	</xsl:variable>
	<xsl:text>
</xsl:text>
	<section id="{$id}">
		<xsl:if test="not($localize='')">
			<xsl:attribute name="localize">
				<xsl:value-of select="$localize"/>
			</xsl:attribute>
		</xsl:if>
		<xsl:apply-templates />
		<xsl:text>
</xsl:text>
	</section>
	
</xsl:template>


<!-- 
###################################################### 
SORT 
  + order (asc | desc) #IMPLIED
###################################################### 
-->
<xsl:template match="text:variable-set[@text:name='SORT_']">
	<xsl:variable name="order">
		<xsl:value-of select="substring-before(substring-after(.,'order=&quot;'),'&quot;')"/>
	</xsl:variable>
	<xsl:text disable-output-escaping="yes">
&lt;sort</xsl:text>
<xsl:if test="not($order = '')">
<xsl:text disable-output-escaping="yes"> order=&quot;</xsl:text><xsl:value-of select="$order"/><xsl:text disable-output-escaping="yes">&quot;</xsl:text>
</xsl:if>
<xsl:text disable-output-escaping="yes">&gt;</xsl:text>
</xsl:template>

<xsl:template match="text:variable-set[@text:name='_SORT']">
	<xsl:text disable-output-escaping="yes">
&lt;/sort&gt;</xsl:text>
</xsl:template>
	
<!-- 
###################################################### 
SWITCH
  + select (sys | appl | distrib | target | lang | ver) #REQUIRED
  + localize CDATA #IMPLIED
###################################################### 
-->

<xsl:template match="text:variable-set[@text:name='SWITCH_']">
	<xsl:variable name="select">
		<xsl:value-of select="substring-before(substring-after(.,'select=&quot;'),'&quot;')"/>
	</xsl:variable>
	<xsl:variable name="localize">
		<xsl:value-of select="substring-before(substring-after(.,'localize=&quot;'),'&quot;')"/>
	</xsl:variable>
	<xsl:text disable-output-escaping="yes">
&lt;switch select=&quot;</xsl:text><xsl:value-of select="$select"/><xsl:text disable-output-escaping="yes">&quot;</xsl:text>
	<xsl:if test="not($localize = '')">
<xsl:text disable-output-escaping="yes"> localize=&quot;</xsl:text><xsl:value-of select="$localize"/><xsl:text disable-output-escaping="yes">&quot;</xsl:text>
</xsl:if>
<xsl:text disable-output-escaping="yes">&gt;</xsl:text>
</xsl:template>

<xsl:template match="text:variable-set[@text:name='_SWITCH']">
<xsl:text disable-output-escaping="yes">
&lt;/switch&gt;</xsl:text>
</xsl:template>

<!-- 
###################################################### 
SWITCHINLINE
  + select (sys | appl | distrib | target | ver | lang) #REQUIRED
###################################################### 
-->

<xsl:template match="text:variable-set[@text:name='SWITCHINLINE_']">
	<xsl:variable name="select">
		<xsl:value-of select="substring-before(substring-after(.,'select=&quot;'),'&quot;')"/>
	</xsl:variable>
	<xsl:text disable-output-escaping="yes">&lt;switchinline select=&quot;</xsl:text><xsl:value-of select="$select"/><xsl:text disable-output-escaping="yes">&quot;&gt;</xsl:text>
</xsl:template>

<xsl:template match="text:variable-set[@text:name='_SWITCHINLINE']">
<xsl:text disable-output-escaping="yes">&lt;/switchinline&gt;</xsl:text>
</xsl:template>

<!-- 
###################################################### 
TABLE
  + name CDATA #IMPLIED
  + width CDATA #IMPLIED
  + height CDATA #IMPLIED
  + unit CDATA #IMPLIED
  + class CDATA #IMPLIED
  + id CDATA #REQUIRED
  + localize CDATA #IMPLIED
###################################################### 
-->
<xsl:template match="table:table">
	<xsl:variable name="id">
		<xsl:value-of select="@table:name"/>
	</xsl:variable>
	<xsl:text>
</xsl:text>

	<table id="{$id}">

		<xsl:if test="following::text:p[1]//text:variable-set[@text:name='CAPTION_']">
			<xsl:variable name="capattr">
				<xsl:value-of select="following::text:p[2]//text:variable-set[@text:name='CAPTION_']"/>
			</xsl:variable>
			<xsl:variable name="capid">
				<xsl:value-of select="substring-before(substring-after($capattr,'ID=&quot;'),'&quot;')"/>
			</xsl:variable>
			<xsl:variable name="caplocalize">
				<xsl:value-of select="substring-before(substring-after($capattr,'LOCALIZE=&quot;'),'&quot;')"/>
			</xsl:variable>
			
		</xsl:if>

		<xsl:apply-templates />
	<xsl:text>
</xsl:text>
	</table>
	
	<xsl:text>
</xsl:text>
</xsl:template>

<!-- 
###################################################### 
TABLECELL
  + colspan CDATA #IMPLIED
  + rowspan CDATA #IMPLIED
  # width CDATA #IMPLIED
  # class CDATA #IMPLIED
  # unit CDATA #IMPLIED
  # localize CDATA #IMPLIED
###################################################### 
-->
<xsl:template match="table:table-cell">
	<xsl:variable name="colspan"><xsl:value-of select="@table:number-columns-spanned"/></xsl:variable>
	<xsl:variable name="rowspan"><xsl:value-of select="@table:number-rows-spanned"/></xsl:variable>
	<xsl:text>
</xsl:text>
	<tablecell>
		<xsl:if test="not($colspan='')"><xsl:attribute name="colspan"><xsl:value-of select="$colspan"/></xsl:attribute></xsl:if>
		<xsl:if test="not($rowspan='')"><xsl:attribute name="rowspan"><xsl:value-of select="$rowspan"/></xsl:attribute></xsl:if>
		<xsl:apply-templates />
		<xsl:text>
</xsl:text>
	</tablecell>
</xsl:template>

<!-- 
###################################################### 
TABLEROW
  # height CDATA #IMPLIED
  # class CDATA #IMPLIED
  # unit CDATA #IMPLIED
  # localize CDATA #IMPLIED
###################################################### 
-->
<xsl:template match="table:table-row">
	<xsl:text>
</xsl:text>
	<tablerow>
		<xsl:apply-templates />
	<xsl:text>
</xsl:text>		
	</tablerow>
</xsl:template>

<!-- 
###################################################### 
TITLE,  SEE HEADER
  o xml-lang CDATA #REQUIRED (is always en-US for the source)
  o id CDATA #REQUIRED (is always tit for the title)
  # localize CDATA #IMPLIED
###################################################### 
-->

<!-- 
###################################################### 
TOPIC,  SEE HEADER
  + id CDATA #REQUIRED
  + indexer (exclude | include) #IMPLIED 
###################################################### 
-->

<!-- 
###################################################### 
VARIABLE 
  + id CDATA #REQUIRED
  + visibility (hidden | visible) #IMPLIED
###################################################### 
-->
<xsl:template match="text:variable-set[@text:name='VAR_']">
	<xsl:variable name="id">
		<xsl:value-of select="substring-before(substring-after(.,'ID=&quot;'),'&quot;')"/>
	</xsl:variable>
	<xsl:variable name="visibility">
		<xsl:value-of select="substring-before(substring-after(.,'VISIBILITY=&quot;'),'&quot;')"/>
	</xsl:variable>
	
	<xsl:text disable-output-escaping="yes">&lt;variable id=&quot;</xsl:text><xsl:value-of select="$id"/>
<xsl:text disable-output-escaping="yes">&quot;</xsl:text>
<xsl:if test="not($visibility='')">
<xsl:text disable-output-escaping="yes"> visibility=&quot;</xsl:text>
<xsl:value-of select="$visibility"/>
<xsl:text disable-output-escaping="yes">&quot;</xsl:text>
</xsl:if>
<xsl:text disable-output-escaping="yes">&gt;</xsl:text>
</xsl:template>

<xsl:template match="text:variable-set[@text:name='_VAR']">
<xsl:text disable-output-escaping="yes">&lt;/variable&gt;</xsl:text></xsl:template>

<!-- 
###################################################### 
text:span, covers EMPH and ITEM
###################################################### 
-->

<xsl:template match="text:span">

	<!-- no multiple spans allowed, only use the outmost span -->
	<xsl:choose>
		<xsl:when test="ancestor::text:span" >
			<xsl:apply-templates />
		</xsl:when>
		<xsl:otherwise>
		
		<xsl:variable name="masterstyle">	<!-- this is to resolve automatic styles by the writer -->
			<xsl:call-template name="getmasterstyle">
				<xsl:with-param name="style">
					<xsl:call-template name="getdisplayname">
						<xsl:with-param name="style" select="@text:style-name"/>
					</xsl:call-template>
				</xsl:with-param>
			</xsl:call-template>
		</xsl:variable>

		<xsl:choose>
			<xsl:when test="starts-with($masterstyle,'hlp_aux_')">
				<xsl:apply-templates />
			</xsl:when>
			<xsl:when test="not(starts-with($masterstyle,'hlp_'))"><emph><xsl:apply-templates/></emph></xsl:when>
			<xsl:when test="$masterstyle = 'hlp_emph'"><emph><xsl:apply-templates/></emph></xsl:when>
			<xsl:otherwise>
				<xsl:variable name="spanstyle"><xsl:value-of select="substring-after($masterstyle,'hlp_')"/></xsl:variable><item type="{$spanstyle}"><xsl:apply-templates /></item></xsl:otherwise>
		</xsl:choose>

		</xsl:otherwise>
	</xsl:choose>

	
</xsl:template>



<!-- the rest is removed --> 
<xsl:template match="*" />

<!-- 
###################################################### 
LICENSE HEADER
###################################################### 
-->

<xsl:template name="licheader">
<xsl:text disable-output-escaping="yes">&lt;!--***********************************************************
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
 ***********************************************************--&gt;
 </xsl:text>

	</xsl:template>


<xsl:template name="getmasterstyle">
	<xsl:param name="style"/>
	<xsl:choose>
		<xsl:when test="$style='hlp_default'">
			<xsl:value-of select="'hlp_paragraph'"/>
		</xsl:when>
		<xsl:when test="starts-with($style,'hlp_')">
			<xsl:value-of select="$style"/>
		</xsl:when>
		<xsl:otherwise>
			<xsl:variable name="masterstyle">
				<xsl:call-template name="getdisplayname">
					<xsl:with-param name="style">
						<xsl:value-of select="/office:document/office:automatic-styles/style:style[@style:name=$style]/@style:parent-style-name"/>
					</xsl:with-param>
				</xsl:call-template>
			</xsl:variable>
			<xsl:choose>
				<xsl:when test="$masterstyle=''">
				<xsl:choose>
					<xsl:when test="$style='hlp_default'">
						<xsl:value-of select="'hlp_paragraph'"/>
					</xsl:when>
					<xsl:when test="/office:document/office:styles/style:style[@style:name=$style]">
						<xsl:value-of select="$style"/>
					</xsl:when>
					<xsl:otherwise>
						<xsl:value-of select="''"/>
					</xsl:otherwise>
				</xsl:choose>
			</xsl:when>
			<xsl:otherwise>
				<xsl:choose>
					<xsl:when test="starts-with($masterstyle,'hlp_default')">
						<xsl:value-of select="'hlp_paragraph'"/>
					</xsl:when>
					<xsl:when test="starts-with($masterstyle,'hlp_')">
						<xsl:value-of select="$masterstyle"/>
					</xsl:when>
					<xsl:otherwise>
						<xsl:value-of select="'UNKNOWN'"/>		<!-- fallback -->
					</xsl:otherwise>
				</xsl:choose>
			</xsl:otherwise>
		</xsl:choose>
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>
	
<xsl:template name="string-replace">
	<xsl:param name="string"/>
	<xsl:param name="from"/>
	<xsl:param name="to"/>
	<xsl:choose>
		<xsl:when test="contains($string,$from)">
			<xsl:value-of select="substring-before($string,$from)"/>
			<xsl:value-of select="$to"/>
			<xsl:call-template name="string-replace">
				<xsl:with-param name="string" select="substring-after($string,$from)"/>
				<xsl:with-param name="from" select="$from"/>
				<xsl:with-param name="to" select="$to"/>
			</xsl:call-template>
		</xsl:when>
		<xsl:otherwise>
			<xsl:value-of select="$string"/>
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>

<xsl:template name="getpath">
	<xsl:param name="file" />
	<xsl:param name="path" />
	<xsl:choose>
		<xsl:when test="contains($file,'/')">
			<xsl:call-template name="getpath">
				<xsl:with-param name="file">
					<xsl:value-of select="substring-after($file,'/')"/>
				</xsl:with-param>
				<xsl:with-param name="path">
					<xsl:value-of select="concat($path,substring-before($file,'/'),'/')"/>
				</xsl:with-param>
			</xsl:call-template>
		</xsl:when>
		<xsl:otherwise>
			<xsl:value-of select="$path"/>
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>

<xsl:template name="getprevol">
		<xsl:param name="node"/>
		<xsl:param name="nodepos"/>
		<xsl:param name="ct"/>
		<xsl:choose>
			<xsl:when test="$node/preceding-sibling::text:list[$nodepos]/@text:continue-numbering">
				<xsl:call-template name="getprevol">
					<xsl:with-param name="node" select="$node"/>
					<xsl:with-param name="nodepos" select="$nodepos+1"/>
					<xsl:with-param name="ct" select="$ct + count($node/preceding-sibling::text:list[$nodepos]/text:list-item) + count($node/preceding-sibling::text:list[$nodepos]/text:list-header)"/>
				</xsl:call-template>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="$ct + count($node/preceding-sibling::text:list[$nodepos]/text:list-item) + count($node/preceding-sibling::text:list[$nodepos]/text:list-header)"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>

<xsl:template name="getdisplayname">
	<xsl:param name="style"/>
	<xsl:choose>
		<xsl:when test="/office:document/office:styles/style:style[@style:name=$style]/@style:display-name">
			<xsl:value-of select="/office:document/office:styles/style:style[@style:name=$style]/@style:display-name"/>
		</xsl:when>
		<xsl:otherwise>
			<xsl:value-of select="$style"/>
		</xsl:otherwise>
	</xsl:choose>
	
</xsl:template>
	
<xsl:template name="lf">
<xsl:text disable-output-escaping="yes">
</xsl:text>
</xsl:template>

</xsl:stylesheet>
