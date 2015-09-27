<?xml version="1.0" encoding="UTF-8"?>

<!-- 
 **************************************************************
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
 *************************************************************


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
		xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" office:version="1.0">

	<xsl:param name="filename" select="/helpdocument/meta/topic/filename"/>
		
	<xsl:param name="imgmodule">
		<xsl:if test="starts-with($filename,'/text/')">
			<xsl:value-of select="'default_images/'"/>
		</xsl:if>
	</xsl:param>
	
	<xsl:param name="dirlevel">
		<xsl:call-template name="getdepth">
			<xsl:with-param name="filename" select="$filename"/>
		</xsl:call-template>
	</xsl:param>
	
	<xsl:variable name="imgreldir">
		<xsl:if test="starts-with($filename,'/text/')">
			<xsl:call-template name="getreldir">
				<xsl:with-param name="dirlevel" select="$dirlevel"/>
			</xsl:call-template>
		</xsl:if>
	</xsl:variable>
	
	<xsl:variable name="imgroot" select="concat($imgreldir,$imgmodule)"/>

<!--
#############################################################################
 
	OOo 2 Help Format Input Filter Stylesheet
	==================================================================

	attribute values:
	o means no need for support
	+ means is supported
	# means is not supported
#############################################################################
-->


	<xsl:param name="am" select="'&amp;'"/>
	<xsl:param name="sl" select="'/'"/>
	<xsl:param name="qt" select="'&quot;'"/>


	<xsl:variable name="defaultpararoles">
		<xsl:value-of select="'note tip warning paragraph listitem code example tablecontent tablehead heading'"/>
	</xsl:variable>

	<xsl:variable name="defaultcharstyles">
		<xsl:value-of select="'acronym emph keycode literal menuitem path'"/>
	</xsl:variable>

	<xsl:template match="/">
	
		<office:document>
			<office:meta>
				<meta:generator>OpenOffice.org Import Filter</meta:generator>
		
				<dc:title>
					<xsl:value-of select="/helpdocument/meta/topic/title"/>
				</dc:title>
				
				<dc:subject>
					<xsl:value-of select="/helpdocument/meta/topic/filename"/>
				</dc:subject>
				
				<meta:creation-date>					
				<!-- REMOVED DUE TO PROBLEMS WITH CVS MERGE CONFLICTS
					<xsl:value-of select="/helpdocument/meta/history/created/@date"/>
				//-->
				</meta:creation-date>
				<meta:creator>XHP Import Filter</meta:creator>
				<dc:date>
				<!-- REMOVED DUE TO PROBLEMS WITH CVS MERGE CONFLICTS
					<xsl:value-of select="/helpdocument/meta/history/lastedited/@date"/>
				//-->
				</dc:date>
		
				<dc:language>
					<xsl:value-of select="article/@lang"/>
				</dc:language>
		
				<meta:keywords>
					<meta:keyword>
						<xsl:value-of select="/helpdocument/meta/topic/@status"/>
					</meta:keyword>
				</meta:keywords>
		
				<meta:editing-cycles>1</meta:editing-cycles>
				<meta:editing-duration>P0DT0H0M1S</meta:editing-duration>
		
				<meta:user-defined meta:name="Indexer">
					<xsl:value-of select="/helpdocument/meta/topic/@indexer"/>
				</meta:user-defined>
		
				<meta:user-defined meta:name="ID">
					<xsl:value-of select="/helpdocument/meta/topic/@id"/>
				</meta:user-defined>
		
		
		
				<meta:document-statistic meta:table-count="0" meta:image-count="0" meta:object-count="0" meta:page-count="1" meta:paragraph-count="0" meta:word-count="0" meta:character-count="0"/>
			</office:meta>
	
			<office:scripts>
			  <office:event-listeners>
					<xsl:element name="script:event-listener">
						<xsl:attribute name="script:language">ooo:script</xsl:attribute>
						<xsl:attribute name="script:event-name">office:save</xsl:attribute>
						<xsl:attribute name="xlink:href"><xsl:value-of select="concat('vnd.sun.star.script:HelpAuthoring._Main.ValidateOnSave?language=Basic',$am,'location=application')"/></xsl:attribute>
					</xsl:element>
					<xsl:element name="script:event-listener">
						<xsl:attribute name="script:language">ooo:script</xsl:attribute>
						<xsl:attribute name="script:event-name">office:save-as</xsl:attribute>
						<xsl:attribute name="xlink:href"><xsl:value-of select="concat('vnd.sun.star.script:HelpAuthoring._Main.ValidateOnSave?language=Basic',$am,'location=application')"/></xsl:attribute>
					</xsl:element>
					<xsl:element name="script:event-listener">
						<xsl:attribute name="script:language">ooo:script</xsl:attribute>
						<xsl:attribute name="script:event-name">office:load-finished</xsl:attribute>
						<xsl:attribute name="xlink:href"><xsl:value-of select="concat('vnd.sun.star.script:HelpAuthoring._Main.CheckOnLoad?language=Basic',$am,'location=application')"/></xsl:attribute>
					</xsl:element>
				</office:event-listeners>
			</office:scripts>
	
			<office:font-face-decls>
  <style:font-face style:name="Monospace" svg:font-family="Monospace" style:font-adornments="Regular"/>
  <style:font-face style:name="Sans-Serif" svg:font-family="Sans-Serif" style:font-adornments="Regular"/>
  <style:font-face style:name="Tahoma1" svg:font-family="Tahoma"/>
  <style:font-face style:name="monospace" svg:font-family="monospace" style:font-adornments="Regular"/>
  <style:font-face style:name="Andale Sans UI" svg:font-family="&apos;Andale Sans UI&apos;" style:font-pitch="variable"/>
  <style:font-face style:name="Bitstream Vera Serif" svg:font-family="&apos;Bitstream Vera Serif&apos;" style:font-adornments="Regular" style:font-pitch="variable"/>
  <style:font-face style:name="Tahoma" svg:font-family="Tahoma" style:font-pitch="variable"/>
  <style:font-face style:name="Thorndale" svg:font-family="Thorndale" style:font-family-generic="roman" style:font-pitch="variable"/>
 </office:font-face-decls>

 
			<office:styles>
				<xsl:call-template name="createstyles"/>
			</office:styles>

			<office:automatic-styles>
				<xsl:call-template name="createautostyles"/>
			</office:automatic-styles>
			
			<office:master-styles>
			</office:master-styles>
	
			<office:body>
				
				<xsl:call-template name="createvariablesets"/>
				<xsl:apply-templates select="helpdocument/body" />
				
	
			</office:body>
	
		</office:document>
	</xsl:template>

<!-- 
###################################################### 
AHELP 
	+ hid CDATA #REQUIRED
  + visibility (hidden | visible) #IMPLIED
###################################################### 
-->

	<xsl:template match="ahelp">
		<xsl:choose>
			<xsl:when test="@visibility='hidden'">
				<text:span text:style-name="hlp_aux_tag">
					<text:variable-set text:name="AHID_" text:value-type="string">
						<xsl:value-of select="concat('&lt;AHID hid=&quot;',@hid,'&quot;&gt;')"/>
					</text:variable-set>
				</text:span>
				<xsl:apply-templates />
				<text:span text:style-name="hlp_aux_tag">
					<text:variable-set text:name="_AHID" text:value-type="string">
						<xsl:value-of select="'&lt;/AHID&gt;'"/>
					</text:variable-set>
				</text:span>
			</xsl:when>
			<xsl:otherwise>
				<text:span text:style-name="hlp_aux_tag">
					<text:variable-set text:name="AVIS_" text:value-type="string">
						<xsl:value-of select="concat('&lt;AVIS hid=&quot;',@hid,'&quot;&gt;')"/>
					</text:variable-set>
				</text:span>
				<xsl:apply-templates />
				<text:span text:style-name="hlp_aux_tag">
					<text:variable-set text:name="_AVIS" text:value-type="string">
						<xsl:value-of select="'&lt;/AVIS&gt;'"/>
					</text:variable-set>
				</text:span>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>

<!-- 
###################################################### 
ALT
  o xml-lang CDATA #REQUIRED (is always en-US for the source)
  + id CDATA #REQUIRED (is the same as for theimage except for "alt_"
  + localize CDATA #IMPLIED 
###################################################### 
-->

	<xsl:template match="alt">
		<svg:title>  <!-- fixed, changed svg:desc to svg:title -->
			<xsl:choose>
				<xsl:when test="@localize">
					<xsl:value-of select="concat('LOCALIZE=',@localize,'#',.)"/>
				</xsl:when>
				<xsl:otherwise>
					<xsl:value-of select="."/>
				</xsl:otherwise>
			</xsl:choose>
		</svg:title>
	</xsl:template>

<!-- 
###################################################### 
BODY, SEE HEADER
  # localize CDATA #IMPLIED
###################################################### 
-->
<xsl:template match="body">
	<office:text>
		<xsl:apply-templates />
	</office:text>
</xsl:template>

<!-- 
###################################################### 
BOOKMARK
  + branch CDATA #REQUIRED
  o xml-lang CDATA #REQUIRED (is always en-US for the source)
  + id CDATA #REQUIRED
  + localize CDATA #IMPLIED 
###################################################### 
-->
	<xsl:template match="bookmark[starts-with(@branch,'hid')]">
		<xsl:choose>
			<xsl:when test="ancestor::paragraph">
				<text:span text:style-name="hlp_aux_tag">
					<text:variable-set text:name="BOOKMARK" text:value-type="string">
						<xsl:value-of select="concat('&lt;BOOKMARK branch=&quot;',@branch,'&quot; id=&quot;',@id,'&quot; localize=&quot;false&quot;/&gt;')"/>
					</text:variable-set>
				</text:span>
			</xsl:when>
			<xsl:otherwise>
				<text:p text:style-name="hlp_aux_bookmark">
					<text:span text:style-name="hlp_aux_tag">
						<text:variable-set text:name="BOOKMARK" text:value-type="string">
							<xsl:value-of select="concat('&lt;BOOKMARK branch=&quot;',@branch,'&quot; id=&quot;',@id,'&quot; localize=&quot;false&quot;/&gt;')"/>
						</text:variable-set>
					</text:span>
				</text:p>
			</xsl:otherwise>
		</xsl:choose>

	</xsl:template>

	<xsl:template match="bookmark">
		<xsl:choose>
			<xsl:when test="ancestor::paragraph">
				<text:span text:style-name="hlp_aux_tag">
					<text:variable-set text:name="BOOKMARK_" text:value-type="string">
						<xsl:value-of select="concat('&lt;BOOKMARK branch=&quot;',@branch,'&quot; id=&quot;',@id,'&quot; localize=&quot;',@localize,'&quot;&gt;')"/>
					</text:variable-set>
				</text:span>
				<xsl:apply-templates />
				<text:line-break/>
				<text:span text:style-name="hlp_aux_tag">
					<text:variable-set text:name="_BOOKMARK" text:value-type="string">
						<xsl:value-of select="'&lt;/BOOKMARK&gt;'"/>
					</text:variable-set>
				</text:span>
			</xsl:when>
			<xsl:otherwise>
				<text:p text:style-name="hlp_aux_bookmark">
					<text:span text:style-name="hlp_aux_tag">
						<text:variable-set text:name="BOOKMARK_" text:value-type="string">
							<xsl:value-of select="concat('&lt;BOOKMARK branch=&quot;',@branch,'&quot; id=&quot;',@id,'&quot; localize=&quot;',@localize,'&quot;&gt;')"/>
						</text:variable-set>
					</text:span>
					<xsl:apply-templates />
					<text:line-break/>
					<text:span text:style-name="hlp_aux_tag">
						<text:variable-set text:name="_BOOKMARK" text:value-type="string">
							<xsl:value-of select="'&lt;/BOOKMARK&gt;'"/>
						</text:variable-set>
					</text:span>
				</text:p>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>

<!-- 
###################################################### 
BOOKMARK_VALUE 
###################################################### 
-->
	<xsl:template match="bookmark_value">
		<text:line-break />
		<text:span text:style-name="hlp_aux_tag">
			<text:variable-set text:name="BOOKMARKVALUE_" text:value-type="string">
				<xsl:value-of select="'&lt;BOOKMARKVALUE&gt;'"/>
			</text:variable-set>
		</text:span>
		<text:variable-set text:name="BOOKMARKVALUE" text:value-type="string">
			<xsl:apply-templates />
		</text:variable-set>
		<text:span text:style-name="hlp_aux_tag">
			<text:variable-set text:name="_BOOKMARKVALUE" text:value-type="string">
				<xsl:value-of select="'&lt;/BOOKMARKVALUE&gt;'"/>
			</text:variable-set>
		</text:span>
	</xsl:template>

<!-- 
###################################################### 
BR 
###################################################### 
-->
	<xsl:template match="br">
		<text:line-break/>
	</xsl:template>

<!-- 
###################################################### 
CAPTION 
  o xml-lang CDATA #REQUIRED (is always en-US for the source)
  + id CDATA #REQUIRED   (in parent elements)
  + localize CDATA #IMPLIED (in parent elements)
###################################################### 
-->
	<xsl:template match="caption">
		<xsl:apply-templates />
	</xsl:template>

<!-- 
###################################################### 
CASE
  + select CDATA #REQUIRED
###################################################### 
-->
	<xsl:template match="case">
		<text:p text:style-name="hlp_aux_switch">
			<text:span text:style-name="hlp_aux_tag">
				<text:variable-set text:name="CASE_" text:value-type="string">
					<xsl:value-of select="concat('&lt;CASE select=&quot;',@select,'&quot;&gt;')"/>
				</text:variable-set>
			</text:span>
			<xsl:text></xsl:text>
		</text:p>
		<xsl:apply-templates />
		<text:p text:style-name="hlp_aux_switch">
			<text:span text:style-name="hlp_aux_tag">
				<text:variable-set text:name="_CASE" text:value-type="string">
					<xsl:value-of select="'&lt;/CASE&gt;'"/>
				</text:variable-set>
			</text:span>
			<xsl:text></xsl:text>
		</text:p>
	</xsl:template>

<!-- 
###################################################### 
CASEINLINE
  + select CDATA #REQUIRED 
###################################################### 
-->
	<xsl:template match="caseinline">
		<text:span text:style-name="hlp_aux_tag">
			<text:variable-set text:name="CASEINLINE_" text:value-type="string">
				<xsl:value-of select="concat('&lt;CASEINLINE select=&quot;',@select,'&quot;&gt;')"/>
			</text:variable-set>
		</text:span>
		<xsl:apply-templates />
		<text:span text:style-name="hlp_aux_tag">
			<text:variable-set text:name="_CASEINLINE" text:value-type="string">
				<xsl:value-of select="'&lt;/CASEINLINE&gt;'"/>
			</text:variable-set>
		</text:span>
	</xsl:template>

<!-- 
###################################################### 
COMMENT 
###################################################### 
-->
	<xsl:template match="comment">
		<xsl:choose>
			<xsl:when test="ancestor::paragraph">
				<text:span text:style-name="hlp_aux_tag">
					<text:variable-set text:name="COMMENT_" text:value-type="string">
						<xsl:value-of select="'&lt;COMMENT&gt;'"/>
					</text:variable-set>
				</text:span>
				<text:span text:style-name="hlp_aux_comment">
					<text:variable-set text:name="COMMENT" text:value-type="string">
						<xsl:value-of select="."/>
					</text:variable-set>
				</text:span>
				<text:span text:style-name="hlp_aux_tag">
					<text:variable-set text:name="_COMMENT" text:value-type="string">
						<xsl:value-of select="'&lt;/COMMENT&gt;'"/>
					</text:variable-set>
				</text:span>
			</xsl:when>
			<xsl:otherwise>
				<text:p text:style-name="hlp_aux_comment">
					<text:span text:style-name="hlp_aux_tag">
						<text:variable-set text:name="COMMENT_" text:value-type="string">
							<xsl:value-of select="'&lt;COMMENT&gt;'"/>
						</text:variable-set>
					</text:span>
					<text:span text:style-name="hlp_aux_comment">
						<text:variable-set text:name="COMMENT" text:value-type="string">
							<xsl:value-of select="."/>
						</text:variable-set>
					</text:span>
					<text:span text:style-name="hlp_aux_tag">
						<text:variable-set text:name="_COMMENT" text:value-type="string">
							<xsl:value-of select="'&lt;/COMMENT&gt;'"/>
						</text:variable-set>
					</text:span>
					<xsl:text></xsl:text>
				</text:p>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>

<!-- 
###################################################### 
CREATED,  SEE HEADER
  + date CDATA #REQUIRED
###################################################### 
-->
	<xsl:template match="created" />

<!-- 
###################################################### 
DEFAULT 
###################################################### 
-->
	<xsl:template match="default">
		<text:p text:style-name="hlp_aux_switch">
			<text:span text:style-name="hlp_aux_tag">
				<text:variable-set text:name="DEFAULT_" text:value-type="string">
					<xsl:value-of select="'&lt;DEFAULT&gt;'"/>
				</text:variable-set>
			</text:span>
			<xsl:text></xsl:text>
		</text:p>
		<xsl:apply-templates />
		<text:p text:style-name="hlp_aux_switch">
			<text:span text:style-name="hlp_aux_tag">
				<text:variable-set text:name="_DEFAULT" text:value-type="string">
					<xsl:value-of select="'&lt;/DEFAULT&gt;'"/>
				</text:variable-set>
			</text:span>
			<xsl:text></xsl:text>
		</text:p>
	</xsl:template>

<!-- 
###################################################### 
DEFAULTINLINE 
###################################################### 
-->
	<xsl:template match="defaultinline">
		<text:span text:style-name="hlp_aux_tag">
			<text:variable-set text:name="DEFAULTINLINE_" text:value-type="string">
				<xsl:value-of select="'&lt;DEFAULTINLINE&gt;'"/>
			</text:variable-set>
		</text:span>
		<xsl:apply-templates />
		<text:span text:style-name="hlp_aux_tag">
			<text:variable-set text:name="_DEFAULTINLINE" text:value-type="string">
				<xsl:value-of select="'&lt;/DEFAULTINLINE&gt;'"/>
			</text:variable-set>
		</text:span>
	</xsl:template>

<!-- 
###################################################### 
EMBED
	+ href CDATA #REQUIRED
  + role CDATA #IMPLIED
  + level CDATA #IMPLIED
###################################################### 
-->
	<xsl:template match="embed">
		<xsl:variable name="embed" select="concat('embed file=&quot;',@href,'&quot; role=&quot;',@role,'&quot; level=&quot;',@level,'&quot;')" />
		<text:p text:style-name="hlp_aux_embed">
			<xsl:variable name="href" select="substring-before(@href,'#')"/>
			<text:span text:style-name="hlp_aux_tag">
	
				<text:variable-set text:name="EMBED" text:value-type="string">
					<xsl:value-of select="concat('&lt;EMBED href=&quot;',@href,'&quot;')"/>
					<xsl:if test="@role">
						<xsl:value-of select="concat(' role=&quot;',@role,'&quot;')"/>
					</xsl:if>
					<xsl:if test="@level">
						<xsl:value-of select="concat(' level=&quot;',@level,'&quot;')"/>
					</xsl:if>
					<xsl:value-of select="'&gt;'"/>
				</text:variable-set>
	
	
			</text:span>
		</text:p>
	</xsl:template>

<!-- 
###################################################### 
EMBEDVAR
 + href CDATA #REQUIRED
 + markup (keep | ignore) #IMPLIED
###################################################### 
-->
	<xsl:template match="embedvar">
		<xsl:variable name="embed" select="concat('embedvar var=&quot;',@href,'&quot; markup=&quot;',@markup,'&quot;')" />
		<xsl:variable name="href" select="@href"/>
		<text:span text:style-name="hlp_aux_tag">
	
			<text:variable-set text:name="EMBEDVAR" text:value-type="string">
				<xsl:value-of select="concat('&lt;EMBEDVAR var=&quot;',@href,'&quot;')"/>
				<xsl:if test="@markup">
					<xsl:value-of select="concat(' markup=&quot;',@markup,'&quot;')"/>
				</xsl:if>
				<xsl:value-of select="'&gt;'"/>
			</text:variable-set>
		</text:span>
	</xsl:template>

<!-- 
###################################################### 
EMPH 
###################################################### 
-->
	<xsl:template match="emph">
		<text:span text:style-name="hlp_emph">
			<xsl:apply-templates />
		</text:span>
	</xsl:template>

<!-- 
###################################################### 
FILENAME,  SEE HEADER
###################################################### 
-->
	<xsl:template match="filename" />


<!-- 
###################################################### 
HELPDOCUMENT,  SEE HEADER
  + version CDATA #REQUIRED
###################################################### 
-->
<!-- <xsl:template match="helpdocument" /> -->

<!-- 
###################################################### 
HISTORY,  SEE HEADER
###################################################### 
-->
<!-- REMOVED DUE TO PROBLEMS WITH CVS MERGE CONFLICTS
	<xsl:template match="history" />
//-->


<!-- 
###################################################### 
IMAGE 
  + s rc CDATA #REQUIRED
  + width CDATA #IMPLIED
  + height CDATA #IMPLIED
  + id CDATA #REQUIRED
###################################################### 
-->
	<xsl:template match="image">
		<xsl:variable name="src" select="concat($imgroot,@src)" />
		<xsl:choose>
			<xsl:when test="not(parent::paragraph)">
				<text:p text:style-name="hlp_default">
					<text:span text:style-name="hlp_aux_tag">
						<text:variable-set text:name="ID" text:value-type="string">
							<xsl:value-of select="concat('id=&quot;par_id',generate-id(),'&quot; l10n=&quot;F&quot;')"/>
						</text:variable-set>
					</text:span>
					<text:span text:style-name="hlp_aux_tag">
						<text:variable-set text:name="IMG_" text:value-type="string">
							<xsl:value-of select="concat('&lt;IMG ID=&quot;',@id,'&quot;&gt;')"/>
						</text:variable-set>
					</text:span>
					<draw:frame draw:name="{@id}" text:anchor-type="as-char" draw:filter-name="&lt;All formats&gt;">
						<xsl:if test="@height">
							<xsl:attribute name="svg:height">
								<xsl:value-of select="@height"/>
							</xsl:attribute>
						</xsl:if>
						<xsl:if test="@width">
							<xsl:attribute name="svg:width">
								<xsl:value-of select="@width"/>
							</xsl:attribute>
						</xsl:if>
						<draw:image xlink:href="{$src}" xlink:type="simple" xlink:show="embed" xlink:actuate="onLoad" />
						<xsl:apply-templates select="alt"/>
					</draw:frame>
								
					<xsl:if test="caption">
						<text:span text:style-name="hlp_aux_tag">
							<text:variable-set text:name="IMGCAPTION_" text:value-type="string">
								<xsl:value-of select="concat('&lt;IMGCAPTION ID=&quot;',caption/@id,'&quot; LOCALIZE=&quot;',caption/@localize,'&quot;&gt;')"/>
							</text:variable-set>
						</text:span>
									
						<text:span text:style-name="hlp_aux_imgcaption">
							<xsl:apply-templates select="caption"/>
						</text:span>
									
						<text:span text:style-name="hlp_aux_tag">
							<text:variable-set text:name="_IMGCAPTION" text:value-type="string">
								<xsl:value-of select="'&lt;/IMGCAPTION&gt;'"/>
							</text:variable-set>
						</text:span>
					</xsl:if>
								
					<text:span text:style-name="hlp_aux_tag">
						<text:variable-set text:name="_IMG" text:value-type="string">
							<xsl:value-of select="'&lt;/IMG&gt;'"/>
						</text:variable-set>
					</text:span>
				</text:p>
			</xsl:when>
			<xsl:otherwise>
				<text:span text:style-name="hlp_aux_tag">
					<text:variable-set text:name="IMG_" text:value-type="string">
						<xsl:value-of select="concat('&lt;IMG ID=&quot;',@id,'&quot;&gt;')"/>
					</text:variable-set>
				</text:span>
							
				<xsl:variable name="localize_img">
					<xsl:choose>
						<xsl:when test="@localize">
							<xsl:value-of select="concat(' localize=&quot;',@localize,'&quot;')"/>
						</xsl:when>
						<xsl:otherwise>
							<xsl:value-of select="''"/>
						</xsl:otherwise>
					</xsl:choose>
				</xsl:variable>
							
				<draw:frame draw:name="{@id}{$localize_img}" text:anchor-type="as-char" draw:filter-name="&lt;All formats&gt;">
					<xsl:if test="@height">
						<xsl:attribute name="svg:height">
							<xsl:value-of select="@height"/>
						</xsl:attribute>
					</xsl:if>
					<xsl:if test="@width">
						<xsl:attribute name="svg:width">
							<xsl:value-of select="@width"/>
						</xsl:attribute>
					</xsl:if>
					<xsl:if test="$localize_img != ''">
						<xsl:attribute name="draw:style-name">
							<xsl:value-of select="'hlp_img_loc'"/>
						</xsl:attribute>
					</xsl:if>
					<draw:image xlink:href="{$src}" xlink:type="simple" xlink:show="embed" xlink:actuate="onLoad"/>
					<xsl:apply-templates select="alt"/>
				</draw:frame>
								
				<xsl:if test="caption">
					<text:span text:style-name="hlp_aux_tag">
						<text:variable-set text:name="IMGCAPTION_" text:value-type="string">
							<xsl:value-of select="concat('&lt;IMGCAPTION ID=&quot;',caption/@id,'&quot; LOCALIZE=&quot;',caption/@localize,'&quot;&gt;')"/>
						</text:variable-set>
					</text:span>
									
					<text:span text:style-name="hlp_aux_imgcaption">
						<xsl:apply-templates select="caption"/>
					</text:span>
									
					<text:span text:style-name="hlp_aux_tag">
						<text:variable-set text:name="_IMGCAPTION" text:value-type="string">
							<xsl:value-of select="'&lt;/IMGCAPTION&gt;'"/>
						</text:variable-set>
					</text:span>
				</xsl:if>
								
				<text:span text:style-name="hlp_aux_tag">
					<text:variable-set text:name="_IMG" text:value-type="string">
						<xsl:value-of select="'&lt;/IMG&gt;'"/>
					</text:variable-set>
				</text:span>
			</xsl:otherwise>
		</xsl:choose>

	</xsl:template>

<!-- 
###################################################### 
ITEM 
  + type CDATA #REQUIRED
###################################################### 
-->
	<xsl:template match="item">
		<xsl:variable name="style">
			<xsl:value-of select="concat('hlp_',@type)"/>
		</xsl:variable>
		<text:span text:style-name="{$style}">    <!-- style needs to be created, see createstyles template -->
			<xsl:apply-templates />
		</text:span>
	</xsl:template>

<!-- 
###################################################### 
LASTEDITED,  SEE HEADER
  + date CDATA #REQUIRED
###################################################### 
-->
	<xsl:template match="lastedited" />

<!-- 
###################################################### 
LINK
  + href CDATA #REQUIRED
  + name CDATA #REQUIRED
  + type CDATA #IMPLIED
  + target CDATA #IMPLIED
###################################################### 
-->
	<xsl:template match="link">
		<xsl:variable name="href">
			<xsl:choose>
				<xsl:when test="starts-with(@href,'text/')">
					<xsl:value-of select="@href"/>
				</xsl:when>
				<xsl:otherwise>
					<xsl:value-of select="@href"/>
				</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>
		<text:span text:style-name="hlp_aux_tag">
			<text:variable-set text:name="LINK_" text:value-type="string">
				<xsl:value-of select="concat('&lt;LINK href=&quot;',@href,'&quot;')"/>
				<xsl:if test="@name">
					<xsl:value-of select="concat(' name=&quot;',@name,'&quot;')"/>
				</xsl:if>
				<xsl:if test="@type">
					<xsl:value-of select="concat(' type=&quot;',@type,'&quot;')"/>
				</xsl:if>
				<xsl:if test="@target">
					<xsl:value-of select="concat(' target=&quot;',@target,'&quot;')"/>
				</xsl:if>
				<xsl:value-of select="'&gt;'"/>
			</text:variable-set>
		</text:span>
		<xsl:apply-templates />
		<text:span text:style-name="hlp_aux_tag">
			<text:variable-set text:name="_LINK" text:value-type="string">
				<xsl:value-of select="'&lt;/LINK&gt;'"/>
			</text:variable-set>
		</text:span>

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
	<xsl:template match="list">
		<xsl:choose>
			<xsl:when test="@type='ordered'">
				<text:list text:style-name="hlp_orderedlist">
					<xsl:apply-templates />
				</text:list>
			</xsl:when>
			<xsl:when test="@type='unordered'">
				<text:list text:style-name="hlp_unorderedlist">
					<xsl:apply-templates />
				</text:list	>
			</xsl:when>
			<xsl:otherwise>
				<text:list>
					<xsl:apply-templates />
				</text:list	>
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
	<xsl:template match="listitem">
		<text:list-item>
			<xsl:if test="count(preceding-sibling::listitem)=0">
				<xsl:if test="../@startwith">
					<xsl:attribute name="text:start-value"><xsl:value-of select="../@startwith"/></xsl:attribute>
				</xsl:if>
			</xsl:if>
			<xsl:apply-templates />
		</text:list-item>
	</xsl:template>

<!-- 
###################################################### 
META, SEE HEADER
###################################################### 
-->
	<xsl:template match="meta" />

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
	<xsl:template match="object">
		<xsl:choose>
			<xsl:when test="@height">
				<draw:text-box draw:style-name="fr1" draw:name="frame1" text:anchor-type="as-char" svg:width="{@width}" fo:min-height="{@height}" draw:z-index="1">
					<text:p text:style-name="hlp_default">
						<text:span text:style-name="hlp_aux_tag">
							<text:variable-set text:name="OBJECT" text:value-type="string">
								<xsl:value-of select="concat('&lt;OBJECT ID=&quot;',@id,'&quot; TYPE=&quot;',@type,'&quot; DATA=&quot;',@data,'&quot; HEIGHT=&quot;',@height,'&quot; WIDTH=&quot;',@width,'&quot; /&gt;')"/>
							</text:variable-set>
						</text:span>
						<xsl:text></xsl:text>
					</text:p>
				</draw:text-box>
			</xsl:when>
			<xsl:otherwise>
				<text:span text:style-name="hlp_aux_tag">
					<text:variable-set text:name="OBJECT" text:value-type="string">
						<xsl:value-of select="concat('&lt;OBJECT ID=&quot;',@id,'&quot; TYPE=&quot;',@type,'&quot; DATA=&quot;',@data,'&quot; HEIGHT=&quot;',@height,'&quot; WIDTH=&quot;',@width,'&quot; /&gt;')"/>
					</text:variable-set>
				</text:span>
			</xsl:otherwise>
		</xsl:choose>
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
	<xsl:template match="paragraph">
		<xsl:choose>
			<xsl:when test="@role='heading'">
				<xsl:variable name="style" select="concat('hlp_head',@level)" />
				<text:h text:style-name="{$style}" text:outline-level="{@level}"> <!-- fixed text:level bug, must be text:outline-level -->
			
					<xsl:variable name="tagstyle">
						<xsl:choose>
						
							<xsl:when test="@l10n='NEW' or @l10n='CHG'">
								<xsl:value-of select="'hlp_aux_parachanged'"/>
							</xsl:when>
							<xsl:otherwise>
								<xsl:value-of select="'hlp_aux_tag'"/>
							</xsl:otherwise>
						</xsl:choose>
					</xsl:variable>
			
					
					<text:span text:style-name="{$tagstyle}">
						<text:variable-set text:name="ID" text:value-type="string" text:display="none">
							<xsl:choose>
								<xsl:when test="@xml-lang">
									<xsl:value-of select="@id"/>
								</xsl:when>
								<xsl:otherwise>
									<xsl:value-of select="concat(@id,'_NOL10N')"/>
								</xsl:otherwise>
							</xsl:choose>
							<!--
							<xsl:if test="@l10n and not(@l10n='')">
								<xsl:value-of select="concat(' l10n=&quot;',@l10n,'&quot;')"/>
							</xsl:if>
							<xsl:if test="@oldref and not(@oldref='')">
								<xsl:value-of select="concat(' oldref=&quot;',@oldref,'&quot;')"/>
							</xsl:if>
							<xsl:if test="@localize and not(@localize='')">
								<xsl:value-of select="concat(' localize=&quot;',@localize,'&quot;')"/>
							</xsl:if>
							//-->
						</text:variable-set>
					</text:span>
					
					<xsl:apply-templates />
				</text:h>
			</xsl:when>
		
			<xsl:otherwise>
				<xsl:variable name="style" select="concat('hlp_',@role)" />
				<text:p text:style-name="{$style}">
					<xsl:variable name="tagstyle">
						<xsl:choose>
							<xsl:when test="@l10n='NEW' or @l10n='CHG'">
								<xsl:value-of select="'hlp_aux_parachanged'"/>
							</xsl:when>
							<xsl:otherwise>
								<xsl:value-of select="'hlp_aux_tag'"/>
							</xsl:otherwise>
						</xsl:choose>
					</xsl:variable>
					
					<text:span text:style-name="{$tagstyle}">
						<text:variable-set text:name="ID" text:value-type="string" text:display="none">
						<xsl:choose>
								<xsl:when test="@xml-lang">
									<xsl:value-of select="@id"/>
								</xsl:when>
								<xsl:otherwise>
									<xsl:value-of select="concat(@id,'_NOL10N')"/>
								</xsl:otherwise>
							</xsl:choose>
						<!--
							<xsl:if test="@l10n and not(@l10n='')">
								<xsl:value-of select="concat(' l10n=&quot;',@l10n,'&quot;')"/>
							</xsl:if>
							<xsl:if test="@oldref and not(@oldref='')">
								<xsl:value-of select="concat(' oldref=&quot;',@oldref,'&quot;')"/>
							</xsl:if>
							<xsl:if test="@localize and not(@localize='')">
								<xsl:value-of select="concat(' localize=&quot;',@localize,'&quot;')"/>
							</xsl:if>
							//-->
						</text:variable-set>
					</text:span>
					
					<xsl:apply-templates />
				</text:p>
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
	<xsl:template match="section"> <!-- some files contain 2 equal sections cascaded (bug) -->
		<xsl:variable name="id1" select="@id" />
		<xsl:choose>
			<xsl:when test="child::section[@id=$id1]">
				<xsl:apply-templates />
			</xsl:when>
			<xsl:otherwise>
				<xsl:variable name="id">
					<xsl:choose>
						<xsl:when test="@localize='false'">
							<xsl:value-of select="concat(@id,'__NOLOCALIZE')"/>
						</xsl:when>
						<xsl:otherwise>
							<xsl:value-of select="@id"/>
						</xsl:otherwise>
					</xsl:choose>
				</xsl:variable>
				<text:section text:style-name="hlp_aux_section" text:name="{$id}">
					<text:p text:style-name="hlp_aux_section">
						<text:span text:style-name="hlp_aux_tag">
							<text:variable-set text:name="SECTION_" text:value-type="string">
								<xsl:value-of select="concat('&lt;SECTION id=&quot;',@id,'&quot;&gt;')"/>
							</text:variable-set>
						</text:span>
					</text:p>
					<xsl:apply-templates />
					<text:p text:style-name="hlp_aux_section">
						<text:span text:style-name="hlp_aux_tag">
							<text:variable-set text:name="_SECTION" text:value-type="string">
								<xsl:value-of select="'&lt;/SECTION&gt;'"/>
							</text:variable-set>
						</text:span>
					</text:p>
				</text:section>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>

<!-- 
###################################################### 
SORT 
  + order (asc | desc) #IMPLIED
###################################################### 
-->
	<xsl:template match="sort">
		<text:p text:style-name="hlp_aux_sort">
			<text:span text:style-name="hlp_aux_tag">
				<text:variable-set text:name="SORT_" text:value-type="string">
					<xsl:value-of select="concat('&lt;SORT order=&quot;',@order,'&quot;&gt;')"/>
				</text:variable-set>
			</text:span>
			<xsl:text></xsl:text>
		</text:p>
		<xsl:apply-templates />
		<text:p text:style-name="hlp_aux_sort">
			<text:span text:style-name="hlp_aux_tag">
				<text:variable-set text:name="_SORT" text:value-type="string">
					<xsl:value-of select="'&lt;/SORT&gt;'"/>
				</text:variable-set>
			</text:span>
			<xsl:text></xsl:text>
		</text:p>
	</xsl:template>

<!-- 
###################################################### 
SWITCH
  + select (sys | appl | distrib ) #REQUIRED
  + localize CDATA #IMPLIED
###################################################### 
-->
	<xsl:template match="switch">
		<text:p text:style-name="hlp_aux_switch">
			<text:span text:style-name="hlp_aux_tag">
				<text:variable-set text:name="SWITCH_" text:value-type="string">
					<xsl:value-of select="concat('&lt;SWITCH select=&quot;',@select,'&quot; localize=&quot;',@localize,'&quot;&gt;')"/>
				</text:variable-set>
			</text:span>
			<xsl:text></xsl:text>
		</text:p>
		<xsl:apply-templates />
		<text:p text:style-name="hlp_aux_switch">
			<text:span text:style-name="hlp_aux_tag">
				<text:variable-set text:name="_SWITCH" text:value-type="string">
					<xsl:value-of select="'&lt;/SWITCH&gt;'"/>
				</text:variable-set>
			</text:span>
			<xsl:text></xsl:text>
		</text:p>
	</xsl:template>

<!-- 
###################################################### 
SWITCHINLINE
  + select (sys | appl | distrib | target | ver | lang) #REQUIRED
###################################################### 
-->
	<xsl:template match="switchinline">
		<text:span text:style-name="hlp_aux_tag">
			<text:variable-set text:name="SWITCHINLINE_" text:value-type="string">
				<xsl:value-of select="concat('&lt;SWITCHINLINE select=&quot;',@select,'&quot;&gt;')"/>
			</text:variable-set>
		</text:span>
		<xsl:apply-templates />
		<text:span text:style-name="hlp_aux_tag">
			<text:variable-set text:name="_SWITCHINLINE" text:value-type="string">
				<xsl:value-of select="'&lt;/SWITCHINLINE&gt;'"/>
			</text:variable-set>
		</text:span>
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
	<xsl:template match="table">
		<xsl:variable name="maxcols">
			<xsl:value-of select="count(tablerow[1]/tablecell)" />	<!-- colspan for first row unsupported! -->
		</xsl:variable>
	
		<xsl:variable name="id">
			<xsl:value-of select="@id"/>
		</xsl:variable>
	
		<table:table table:name="{$id}" table:style-name="hlp_table">
			<table:table-column table:number-columns-repeated="{$maxcols}"/>
			<xsl:apply-templates />
		</table:table>
		<xsl:if test="caption">
			<text:p text:style-name="hlp_aux_tablecaption">
			
				<text:span text:style-name="hlp_aux_tag">
					<text:variable-set text:name="CAPTION_" text:value-type="string">
						<xsl:value-of select="concat('&lt;CAPTION ID=&quot;',caption/@id,'&quot; LOCALIZE=&quot;',caption/@localize,'&quot;&gt;')"/>
					</text:variable-set>
				</text:span>
			
				<xsl:apply-templates select="caption"/>
				<text:span text:style-name="hlp_aux_tag">
					<text:variable-set text:name="_CAPTION" text:value-type="string">
						<xsl:value-of select="'&lt;/CAPTION&gt;'"/>
					</text:variable-set>
				</text:span>
				<xsl:text></xsl:text>
			</text:p>
		</xsl:if>
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
	<xsl:template match="tablecell">
		<table:table-cell>
			<xsl:if test="@colspan">
				<xsl:attribute name="table:number-columns-spanned">
					<xsl:value-of select="@colspan"/>
				</xsl:attribute>
			</xsl:if>
			<xsl:if test="@rowspan">
				<xsl:attribute name="table:number-rows-spanned">
					<xsl:value-of select="@rowspan"/>
				</xsl:attribute>
			</xsl:if>
			<xsl:apply-templates />
		</table:table-cell>
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
	<xsl:template match="tablerow">
		<table:table-row>
			<xsl:apply-templates />
		</table:table-row>
	</xsl:template>

<!-- 
###################################################### 
TITLE,  SEE HEADER
  o xml-lang CDATA #REQUIRED (is always en-US for the source)
  o id CDATA #REQUIRED (is always tit for the title)
  # localize CDATA #IMPLIED
###################################################### 
-->
	<xsl:template match="title"/>

<!-- 
###################################################### 
TOPIC,  SEE HEADER
  + id CDATA #REQUIRED
  + indexer (exclude | include) #IMPLIED 
###################################################### 
-->
	<xsl:template match="topic"/>

<!-- 
###################################################### 
VARIABLE 
  + id CDATA #REQUIRED
  + visibility (hidden | visible) #IMPLIED
###################################################### 
-->
	<xsl:template match="variable">
		<text:span text:style-name="hlp_aux_tag">
			<text:variable-set text:name="VAR_" text:value-type="string">
				<xsl:value-of select="concat('&lt;VAR ID=&quot;',@id,'&quot; VISIBILITY=&quot;',@visibility,'&quot;&gt;')"/>
			</text:variable-set>
		</text:span>
		<xsl:apply-templates />
		<text:span text:style-name="hlp_aux_tag">
			<text:variable-set text:name="_VAR" text:value-type="string">
				<xsl:value-of select="'&lt;/VAR&gt;'"/>
			</text:variable-set>
		</text:span>
	</xsl:template>


	<xsl:template name="createvariablesets">
		<text:variable-decls>
			<text:variable-decl text:value-type="string" text:name="AHID_"/>
			<text:variable-decl text:value-type="string" text:name="_AHID"/>
			<text:variable-decl text:value-type="string" text:name="AVIS_"/>
			<text:variable-decl text:value-type="string" text:name="_AVIS"/>
			<text:variable-decl text:value-type="string" text:name="AHID_"/>
			<text:variable-decl text:value-type="string" text:name="BOOKMARK"/>
			<text:variable-decl text:value-type="string" text:name="BOOKMARK_"/>
			<text:variable-decl text:value-type="string" text:name="_BOOKMARK"/>
			<text:variable-decl text:value-type="string" text:name="BOOKMARKVALUE_"/>
			<text:variable-decl text:value-type="string" text:name="_BOOKMARKVALUE"/>
			<text:variable-decl text:value-type="string" text:name="BOOKMARKVALUE"/>
			<text:variable-decl text:value-type="string" text:name="CASE_"/>
			<text:variable-decl text:value-type="string" text:name="_CASE"/>
			<text:variable-decl text:value-type="string" text:name="CASEINLINE_"/>
			<text:variable-decl text:value-type="string" text:name="_CASEINLINE"/>
			<text:variable-decl text:value-type="string" text:name="COMMENT_"/>
			<text:variable-decl text:value-type="string" text:name="_COMMENT"/>
			<text:variable-decl text:value-type="string" text:name="COMMENT"/>
			<text:variable-decl text:value-type="string" text:name="DEFAULT_"/>
			<text:variable-decl text:value-type="string" text:name="_DEFAULT"/>
			<text:variable-decl text:value-type="string" text:name="DEFAULTINLINE_"/>
			<text:variable-decl text:value-type="string" text:name="_DEFAULTINLINE"/>
			<text:variable-decl text:value-type="string" text:name="EMBEDVAR"/>
			<text:variable-decl text:value-type="string" text:name="EMBED"/>
			<text:variable-decl text:value-type="string" text:name="IMG_"/>
			<text:variable-decl text:value-type="string" text:name="_IMG"/>
			<text:variable-decl text:value-type="string" text:name="IMGCAPTION_"/>
			<text:variable-decl text:value-type="string" text:name="_IMGCAPTION"/>
			<text:variable-decl text:value-type="string" text:name="LINK_"/>
			<text:variable-decl text:value-type="string" text:name="_LINK"/>
			<text:variable-decl text:value-type="string" text:name="OBJECT"/>
			<text:variable-decl text:value-type="string" text:name="ID"/>
			<text:variable-decl text:value-type="string" text:name="SECTION_"/>
			<text:variable-decl text:value-type="string" text:name="_SECTION"/>
			<text:variable-decl text:value-type="string" text:name="SORT_"/>
			<text:variable-decl text:value-type="string" text:name="_SORT"/>
			<text:variable-decl text:value-type="string" text:name="SWITCH_"/>
			<text:variable-decl text:value-type="string" text:name="_SWITCH"/>
			<text:variable-decl text:value-type="string" text:name="SWITCHINLINE_"/>
			<text:variable-decl text:value-type="string" text:name="_SWITCHINLINE"/>
			<text:variable-decl text:value-type="string" text:name="TABLEATTR"/>
			<text:variable-decl text:value-type="string" text:name="CAPTION_"/>
			<text:variable-decl text:value-type="string" text:name="_CAPTION"/>
			<text:variable-decl text:value-type="string" text:name="_VAR"/>
			<text:variable-decl text:value-type="string" text:name="VAR_"/>
		</text:variable-decls>
	</xsl:template>

<!-- 
###################################################### 
-->

<!-- 
CREATESTYLES
-->
	<xsl:template name="createstyles">

		<style:default-style style:family="paragraph">
			<style:properties fo:language="en" fo:country="US"/>
		</style:default-style>
	
		<xsl:for-each select="//list[@startwith &gt; '1']">
			<xsl:variable name="style">
				<xsl:value-of select="concat('hlp_List',@startwith)"/>
			</xsl:variable>
			<text:list-style style:name="{$style}">
				<text:list-level-style-number text:level="1" text:style-name="Numbering Symbols" style:num-suffix="." style:num-format="1" text:start-value="{@startwith}" />
			</text:list-style>
		</xsl:for-each>
	
		<xsl:for-each select="//item">
			<xsl:if test="not(contains($defaultcharstyles,@type))">
				<xsl:variable name="style">
					<xsl:value-of select="concat('hlp_',@type)"/>
				</xsl:variable>
				<xsl:element name="style:style">
					<xsl:attribute name="style:name">
						<xsl:value-of select="$style"/>
					</xsl:attribute>
					<xsl:attribute name="style:family">
						<xsl:value-of select="'text'"/>
					</xsl:attribute>
				</xsl:element>
			</xsl:if>
		</xsl:for-each>
	
		<xsl:for-each select="//paragraph">
	<!--
		<xsl:if test="not(contains($defaultpararoles,@role))">
	//-->
			<xsl:variable name="style">
				<xsl:value-of select="concat('hlp_',@role)"/>
			</xsl:variable>
			<xsl:element name="style:style">
				<xsl:attribute name="style:name">
					<xsl:value-of select="$style"/>
				</xsl:attribute>
				<xsl:attribute name="style:family">
					<xsl:value-of select="'paragraph'"/>
				</xsl:attribute>
				<xsl:attribute name="style:parent-style-name">
					<xsl:value-of select="'hlp_default'"/>
				</xsl:attribute>
			</xsl:element>
		<!--
		</xsl:if>
		//-->
		</xsl:for-each>
	
		<xsl:for-each select="//paragraph[@role='heading']">
			<xsl:variable name="style">
				<xsl:value-of select="concat('hlp_head',@level)"/>
			</xsl:variable>
			<xsl:element name="style:style">
				<xsl:attribute name="style:name">
					<xsl:value-of select="$style"/>
				</xsl:attribute>
				<xsl:attribute name="style:family">
					<xsl:value-of select="'paragraph'"/>
				</xsl:attribute>
				<xsl:attribute name="style:parent-style-name">
					<xsl:value-of select="'hlp_default'"/>
				</xsl:attribute>
			</xsl:element>
		</xsl:for-each>
	

<!-- include all style definitions (template is not working) -->

  <style:default-style style:family="graphic">
   <style:graphic-properties draw:shadow-offset-x="0.3cm" draw:shadow-offset-y="0.3cm" draw:start-line-spacing-horizontal="0.283cm" draw:start-line-spacing-vertical="0.283cm" draw:end-line-spacing-horizontal="0.283cm" draw:end-line-spacing-vertical="0.283cm" style:flow-with-text="true"/>
   <style:paragraph-properties style:text-autospace="ideograph-alpha" style:line-break="strict" style:writing-mode="lr-tb" style:font-independent-line-spacing="false">
    <style:tab-stops/>
   </style:paragraph-properties>
   <style:text-properties style:use-window-font-color="true" fo:font-size="12pt" fo:language="en" fo:country="US" style:font-size-asian="12pt" style:language-asian="zxx" style:country-asian="none" style:font-size-complex="12pt" style:language-complex="zxx" style:country-complex="none"/>
  </style:default-style>
  <style:default-style style:family="paragraph">
   <style:paragraph-properties fo:hyphenation-ladder-count="no-limit" style:text-autospace="ideograph-alpha" style:punctuation-wrap="hanging" style:line-break="strict" style:tab-stop-distance="2.205cm" style:writing-mode="page"/>
   <style:text-properties style:use-window-font-color="true" style:font-name="Arial" fo:font-size="12pt" fo:language="en" fo:country="US" style:font-name-asian="Andale Sans UI" style:font-size-asian="12pt" style:language-asian="zxx" style:country-asian="none" style:font-name-complex="Tahoma" style:font-size-complex="12pt" style:language-complex="zxx" style:country-complex="none" fo:hyphenate="false" fo:hyphenation-remain-char-count="2" fo:hyphenation-push-char-count="2"/>
  </style:default-style>
  <style:default-style style:family="table">
   <style:table-properties table:border-model="separating"/>
  </style:default-style>
  <style:default-style style:family="table-row">
   <style:table-row-properties fo:keep-together="always"/>
  </style:default-style>
  <style:style style:name="Standard" style:family="paragraph" style:class="text">
   <style:text-properties style:text-line-through-style="solid" style:text-line-through-type="double" style:font-name="Arial" fo:font-size="14pt"/>
  </style:style>
  <style:style style:name="Text_20_body" style:display-name="Text body" style:family="paragraph" style:parent-style-name="Standard" style:class="text">
   <style:paragraph-properties fo:margin-top="0cm" fo:margin-bottom="0.212cm"/>
  </style:style>
  <style:style style:name="Heading" style:family="paragraph" style:parent-style-name="Standard" style:next-style-name="Text_20_body" style:class="text">
   <style:paragraph-properties fo:margin-top="0.423cm" fo:margin-bottom="0.212cm" fo:keep-with-next="always"/>
   <style:text-properties fo:font-size="14pt" style:font-size-asian="14pt" style:font-size-complex="14pt"/>
  </style:style>
  <style:style style:name="Heading_20_1" style:display-name="Heading 1" style:family="paragraph" style:parent-style-name="Heading" style:next-style-name="Text_20_body" style:class="text">
   <style:text-properties fo:font-size="115%" fo:font-weight="bold" style:font-size-asian="115%" style:font-weight-asian="bold" style:font-size-complex="115%" style:font-weight-complex="bold"/>
  </style:style>
  <style:style style:name="Heading_20_2" style:display-name="Heading 2" style:family="paragraph" style:parent-style-name="Heading" style:next-style-name="Text_20_body" style:class="text">
   <style:text-properties fo:font-size="14pt" fo:font-style="italic" fo:font-weight="bold" style:font-size-asian="14pt" style:font-style-asian="italic" style:font-weight-asian="bold" style:font-size-complex="14pt" style:font-style-complex="italic" style:font-weight-complex="bold"/>
  </style:style>
  <style:style style:name="List" style:family="paragraph" style:parent-style-name="Text_20_body" style:class="list">
   <style:text-properties style:font-name-complex="Tahoma1"/>
  </style:style>
  <style:style style:name="Table_20_Contents" style:display-name="Table Contents" style:family="paragraph" style:parent-style-name="Text_20_body" style:class="extra">
   <style:paragraph-properties text:number-lines="false" text:line-number="0"/>
  </style:style>
  <style:style style:name="Table_20_Heading" style:display-name="Table Heading" style:family="paragraph" style:parent-style-name="Table_20_Contents" style:class="extra">
   <style:paragraph-properties fo:text-align="center" style:justify-single-word="false" text:number-lines="false" text:line-number="0"/>
   <style:text-properties fo:font-style="italic" fo:font-weight="bold" style:font-style-asian="italic" style:font-weight-asian="bold" style:font-style-complex="italic" style:font-weight-complex="bold"/>
  </style:style>
  <style:style style:name="Caption" style:family="paragraph" style:parent-style-name="Standard" style:class="extra">
   <style:paragraph-properties fo:margin-top="0.212cm" fo:margin-bottom="0.212cm" text:number-lines="false" text:line-number="0"/>
   <style:text-properties fo:font-size="10pt" fo:font-style="italic" style:font-size-asian="10pt" style:font-style-asian="italic" style:font-size-complex="10pt" style:font-style-complex="italic"/>
  </style:style>
  <style:style style:name="Table" style:family="paragraph" style:parent-style-name="Caption" style:class="extra"/>
  <style:style style:name="Frame_20_contents" style:display-name="Frame contents" style:family="paragraph" style:parent-style-name="Text_20_body" style:class="extra"/>
  <style:style style:name="Footnote" style:family="paragraph" style:parent-style-name="Standard" style:class="extra">
   <style:paragraph-properties fo:margin-left="0.499cm" fo:margin-right="0cm" fo:text-indent="-0.499cm" style:auto-text-indent="false" text:number-lines="false" text:line-number="0"/>
   <style:text-properties fo:font-size="10pt" style:font-size-asian="10pt" style:font-size-complex="10pt"/>
  </style:style>
  <style:style style:name="Index" style:family="paragraph" style:parent-style-name="Standard" style:class="index">
   <style:paragraph-properties text:number-lines="false" text:line-number="0"/>
   <style:text-properties style:font-name-complex="Tahoma1"/>
  </style:style>
  <style:style style:name="Subtitle" style:family="paragraph" style:parent-style-name="Heading" style:next-style-name="Text_20_body" style:class="chapter">
   <style:paragraph-properties fo:text-align="center" style:justify-single-word="false"/>
   <style:text-properties fo:font-size="14pt" fo:font-style="italic" style:font-size-asian="14pt" style:font-style-asian="italic" style:font-size-complex="14pt" style:font-style-complex="italic"/>
  </style:style>
  <style:style style:name="hlp_5f_default" style:display-name="hlp_default" style:family="paragraph" style:parent-style-name="Standard">
   <style:paragraph-properties fo:margin-top="0.199cm" fo:margin-bottom="0.199cm" fo:line-height="100%"/>
   <style:text-properties style:text-line-through-style="none" style:font-name="Arial" fo:font-size="10pt"/>
  </style:style>
  <style:style style:name="hlp_5f_head" style:display-name="hlp_head" style:family="paragraph" style:parent-style-name="hlp_5f_default">
   <style:text-properties style:font-name="Droid Sans"/>
  </style:style>
  <style:style style:name="hlp_5f_paragraph" style:display-name="hlp_paragraph" style:family="paragraph" style:parent-style-name="hlp_5f_default">
   <style:paragraph-properties fo:margin-top="0.199cm" fo:margin-bottom="0.199cm"/>
  </style:style>
  <style:style style:name="hlp_5f_head2" style:display-name="hlp_head2" style:family="paragraph" style:parent-style-name="hlp_5f_head" style:next-style-name="hlp_5f_paragraph" style:default-outline-level="2">
   <style:paragraph-properties fo:margin-top="0.6cm" fo:margin-bottom="0.199cm"/>
   <style:text-properties fo:font-size="16pt" fo:font-weight="bold"/>
  </style:style>
  <style:style style:name="hlp_5f_head4" style:display-name="hlp_head4" style:family="paragraph" style:parent-style-name="hlp_5f_head" style:next-style-name="hlp_5f_paragraph" style:default-outline-level="4">
   <style:paragraph-properties fo:margin-top="0.4cm" fo:margin-bottom="0.199cm"/>
   <style:text-properties fo:font-weight="bold"/>
  </style:style>
  <style:style style:name="hlp_5f_aux_5f_embed" style:display-name="hlp_aux_embed" style:family="paragraph" style:parent-style-name="hlp_5f_aux_5f_default" style:next-style-name="hlp_5f_default">
   <style:paragraph-properties fo:margin-top="0cm" fo:margin-bottom="0cm" fo:background-color="transparent" fo:padding="0.049cm" fo:border-left="0.176cm solid #808080" fo:border-right="none" fo:border-top="none" fo:border-bottom="none">
    <style:background-image/>
   </style:paragraph-properties>
   <style:text-properties style:font-name="monospace" fo:font-size="9pt"/>
  </style:style>
  <style:style style:name="hlp_5f_ul_5f_item" style:display-name="hlp_ul_item" style:family="paragraph" style:parent-style-name="hlp_5f_default" style:list-style-name="List_20_1">
   <style:paragraph-properties fo:margin-top="0.101cm" fo:margin-bottom="0.101cm"/>
  </style:style>
  <style:style style:name="hlp_5f_aux_5f_tablecaption" style:display-name="hlp_aux_tablecaption" style:family="paragraph" style:parent-style-name="hlp_5f_aux_5f_default" style:next-style-name="hlp_5f_default">
   <style:paragraph-properties fo:margin-top="0cm" fo:margin-bottom="0cm" fo:background-color="#e6e6e6" fo:padding="0.101cm" fo:border-left="0.176cm solid #808080" fo:border-right="none" fo:border-top="none" fo:border-bottom="none">
    <style:background-image/>
   </style:paragraph-properties>
  </style:style>
  <style:style style:name="hlp_5f_aux_5f_comment" style:display-name="hlp_aux_comment" style:family="paragraph" style:parent-style-name="hlp_5f_aux_5f_default" style:next-style-name="hlp_5f_default">
   <style:paragraph-properties fo:background-color="#e6ff00" fo:padding="0.049cm" fo:border-left="0.176cm solid #808080" fo:border-right="none" fo:border-top="none" fo:border-bottom="none">
    <style:background-image/>
   </style:paragraph-properties>
   <style:text-properties fo:font-size="8pt"/>
  </style:style>
  <style:style style:name="hlp_5f_aux_5f_tableattributes" style:display-name="hlp_aux_tableattributes" style:family="paragraph" style:parent-style-name="hlp_5f_aux_5f_default" style:next-style-name="hlp_5f_default">
   <style:paragraph-properties fo:margin-top="0cm" fo:margin-bottom="0cm" fo:background-color="#e6e6e6" fo:padding="0.101cm" fo:border-left="0.176cm solid #808080" fo:border-right="none" fo:border-top="none" fo:border-bottom="none">
    <style:background-image/>
   </style:paragraph-properties>
  </style:style>
  <style:style style:name="hlp_5f_aux_5f_imgcaption" style:display-name="hlp_aux_imgcaption" style:family="paragraph" style:parent-style-name="hlp_5f_aux_5f_default" style:next-style-name="hlp_5f_default">
   <style:paragraph-properties fo:margin-top="0cm" fo:margin-bottom="0cm" fo:background-color="#e6e6e6" fo:padding="0cm" fo:border-left="0.176cm solid #808080" fo:border-right="none" fo:border-top="none" fo:border-bottom="none">
    <style:background-image/>
   </style:paragraph-properties>
  </style:style>
  <style:style style:name="hlp_5f_aux_5f_imgcaptionattributes" style:display-name="hlp_aux_imgcaptionattributes" style:family="paragraph" style:parent-style-name="hlp_5f_aux_5f_default" style:next-style-name="hlp_5f_default">
   <style:paragraph-properties fo:margin-top="0cm" fo:margin-bottom="0.499cm" fo:background-color="#e6e6e6" fo:padding="0.101cm" fo:border-left="0.176cm solid #808080" fo:border-right="none" fo:border-top="none" fo:border-bottom="none">
    <style:background-image/>
   </style:paragraph-properties>
  </style:style>
  <style:style style:name="hlp_5f_aux_5f_switch" style:display-name="hlp_aux_switch" style:family="paragraph" style:parent-style-name="hlp_5f_aux_5f_default" style:next-style-name="hlp_5f_default">
   <style:paragraph-properties fo:margin-top="0cm" fo:margin-bottom="0cm" fo:line-height="100%" fo:background-color="transparent" fo:padding="0.049cm" fo:border-left="0.176cm solid #808080" fo:border-right="none" fo:border-top="none" fo:border-bottom="none">
    <style:background-image/>
   </style:paragraph-properties>
  </style:style>
  <style:style style:name="hlp_5f_aux_5f_sort" style:display-name="hlp_aux_sort" style:family="paragraph" style:parent-style-name="hlp_5f_aux_5f_default" style:next-style-name="hlp_5f_default"/>
  <style:style style:name="hlp_5f_tablehead" style:display-name="hlp_tablehead" style:family="paragraph" style:parent-style-name="hlp_5f_default">
   <style:text-properties fo:font-weight="bold"/>
  </style:style>
  <style:style style:name="hlp_5f_tablecontent" style:display-name="hlp_tablecontent" style:family="paragraph" style:parent-style-name="hlp_5f_default"/>
  <style:style style:name="hlp_5f_note" style:display-name="hlp_note" style:family="paragraph" style:parent-style-name="hlp_5f_default" style:next-style-name="hlp_5f_paragraph">
   <style:paragraph-properties fo:padding="0.049cm" fo:border-left="0.176cm solid #0000ff" fo:border-right="none" fo:border-top="none" fo:border-bottom="none"/>
  </style:style>
  <style:style style:name="hlp_5f_warning" style:display-name="hlp_warning" style:family="paragraph" style:parent-style-name="hlp_5f_default" style:next-style-name="hlp_5f_paragraph">
   <style:paragraph-properties fo:padding="0.049cm" fo:border-left="0.176cm solid #ff0000" fo:border-right="none" fo:border-top="none" fo:border-bottom="none"/>
  </style:style>
  <style:style style:name="hlp_5f_tip" style:display-name="hlp_tip" style:family="paragraph" style:parent-style-name="hlp_5f_default" style:next-style-name="hlp_5f_paragraph">
   <style:paragraph-properties fo:padding="0.049cm" fo:border-left="0.176cm solid #cccc00" fo:border-right="none" fo:border-top="none" fo:border-bottom="none"/>
  </style:style>
  <style:style style:name="hlp_5f_aux_5f_section" style:display-name="hlp_aux_section" style:family="paragraph" style:parent-style-name="hlp_5f_aux_5f_default"/>
  <style:style style:name="hlp_5f_aux_5f_default" style:display-name="hlp_aux_default" style:family="paragraph" style:parent-style-name="hlp_5f_default">
   <style:paragraph-properties fo:background-color="transparent" fo:padding="0.049cm" fo:border-left="0.176cm solid #808080" fo:border-right="none" fo:border-top="none" fo:border-bottom="none">
    <style:background-image/>
   </style:paragraph-properties>
  </style:style>
  <style:style style:name="hlp_5f_code" style:display-name="hlp_code" style:family="paragraph" style:parent-style-name="hlp_5f_default">
   <style:paragraph-properties fo:margin-top="0cm" fo:margin-bottom="0cm" fo:line-height="100%" fo:background-color="transparent" fo:padding="0cm" fo:border="none" style:shadow="none" style:join-border="false">
    <style:background-image/>
   </style:paragraph-properties>
   <style:text-properties style:font-name="monospace" fo:font-size="9pt"/>
  </style:style>
  <style:style style:name="hlp_5f_example" style:display-name="hlp_example" style:family="paragraph" style:parent-style-name="hlp_5f_default">
   <style:paragraph-properties fo:padding="0.15cm" fo:border="0.002cm solid #000000"/>
   <style:text-properties style:font-name="Monospace" fo:font-size="9pt"/>
  </style:style>
  <style:style style:name="hlp_5f_head5" style:display-name="hlp_head5" style:family="paragraph" style:parent-style-name="hlp_5f_head" style:next-style-name="hlp_5f_paragraph" style:default-outline-level="5">
   <style:paragraph-properties fo:margin-top="0.4cm" fo:margin-bottom="0.199cm"/>
   <style:text-properties fo:font-size="10pt" fo:font-weight="bold"/>
  </style:style>
  <style:style style:name="hlp_5f_head1" style:display-name="hlp_head1" style:family="paragraph" style:parent-style-name="hlp_5f_head" style:next-style-name="hlp_5f_paragraph" style:default-outline-level="1">
   <style:paragraph-properties fo:margin-top="1cm" fo:margin-bottom="0.199cm"/>
   <style:text-properties fo:font-size="20pt" fo:font-weight="bold"/>
  </style:style>
  <style:style style:name="hlp_5f_head3" style:display-name="hlp_head3" style:family="paragraph" style:parent-style-name="hlp_5f_head" style:next-style-name="hlp_5f_paragraph" style:default-outline-level="3">
   <style:paragraph-properties fo:margin-top="0.4cm" fo:margin-bottom="0.199cm"/>
   <style:text-properties fo:font-size="14pt" fo:font-weight="bold"/>
  </style:style>
  <style:style style:name="hlp_5f_aux_5f_bookmark" style:display-name="hlp_aux_bookmark" style:family="paragraph" style:parent-style-name="hlp_5f_aux_5f_default" style:next-style-name="hlp_5f_aux_5f_default" style:master-page-name="">
   <style:paragraph-properties fo:margin-top="0cm" fo:margin-bottom="0cm" style:page-number="auto" fo:background-color="#e6e6e6">
    <style:background-image/>
   </style:paragraph-properties>
  </style:style>
  <style:style style:name="hlp_5f_ol_5f_item" style:display-name="hlp_ol_item" style:family="paragraph" style:parent-style-name="hlp_5f_default" style:list-style-name="Numbering_20_1">
   <style:paragraph-properties fo:margin-top="0.101cm" fo:margin-bottom="0.101cm"/>
  </style:style>
  <style:style style:name="hlp_5f_codestart" style:display-name="hlp_codestart" style:family="paragraph" style:parent-style-name="hlp_5f_code" style:next-style-name="hlp_5f_code">
   <style:paragraph-properties fo:margin-top="0.499cm" fo:margin-bottom="0cm" fo:padding="0cm" fo:border="none"/>
  </style:style>
  <style:style style:name="hlp_5f_codeend" style:display-name="hlp_codeend" style:family="paragraph" style:parent-style-name="hlp_5f_code" style:next-style-name="hlp_5f_paragraph">
   <style:paragraph-properties fo:margin-top="0cm" fo:margin-bottom="0.499cm" fo:padding="0cm" fo:border="none"/>
  </style:style>
  <style:style style:name="hlp_5f_codeindent1" style:display-name="hlp_codeindent1" style:family="paragraph" style:parent-style-name="hlp_5f_code">
   <style:paragraph-properties fo:margin-left="0.499cm" fo:margin-right="0cm" fo:text-indent="0cm" style:auto-text-indent="false" fo:background-color="transparent" fo:padding="0cm" fo:border="none">
    <style:background-image/>
   </style:paragraph-properties>
  </style:style>
  <style:style style:name="hlp_5f_codeindent2" style:display-name="hlp_codeindent2" style:family="paragraph" style:parent-style-name="hlp_5f_code">
   <style:paragraph-properties fo:margin-left="1cm" fo:margin-right="0cm" fo:text-indent="0cm" style:auto-text-indent="false" fo:padding="0cm" fo:border="none"/>
  </style:style>
  <style:style style:name="hlp_5f_codeindent3" style:display-name="hlp_codeindent3" style:family="paragraph" style:parent-style-name="hlp_5f_code">
   <style:paragraph-properties fo:margin-left="1.499cm" fo:margin-right="0cm" fo:margin-top="0cm" fo:margin-bottom="0cm" fo:text-indent="0cm" style:auto-text-indent="false" fo:padding="0cm" fo:border="none"/>
  </style:style>
  <style:style style:name="hlp_5f_codeindent4" style:display-name="hlp_codeindent4" style:family="paragraph" style:parent-style-name="hlp_5f_code">
   <style:paragraph-properties fo:margin-left="2cm" fo:margin-right="0cm" fo:text-indent="0cm" style:auto-text-indent="false" fo:background-color="#e6e6e6" fo:padding="0cm" fo:border="none">
    <style:background-image/>
   </style:paragraph-properties>
  </style:style>
  <style:style style:name="Footnote_20_Symbol" style:display-name="Footnote Symbol" style:family="text"/>
  <style:style style:name="Numbering_20_Symbols" style:display-name="Numbering Symbols" style:family="text"/>
  <style:style style:name="Bullet_20_Symbols" style:display-name="Bullet Symbols" style:family="text">
   <style:text-properties fo:font-size="9pt" style:font-size-asian="9pt" style:font-size-complex="9pt"/>
  </style:style>
  <style:style style:name="Internet_20_link" style:display-name="Internet link" style:family="text">
   <style:text-properties fo:color="#000080" style:text-underline-style="solid" style:text-underline-width="auto" style:text-underline-color="font-color"/>
  </style:style>
  <style:style style:name="Endnote_20_Symbol" style:display-name="Endnote Symbol" style:family="text"/>
  <style:style style:name="Emphasis" style:family="text">
   <style:text-properties fo:font-style="italic" style:font-style-asian="italic" style:font-style-complex="italic"/>
  </style:style>
  <style:style style:name="Source_20_Text" style:display-name="Source Text" style:family="text"/>
  <style:style style:name="hlp_5f_aux_5f_tag" style:display-name="hlp_aux_tag" style:family="text">
   <style:text-properties style:font-name="Monospace" fo:font-size="6pt" fo:background-color="#ffff99"/>
  </style:style>
  <style:style style:name="hlp_5f_emph" style:display-name="hlp_emph" style:family="text">
   <style:text-properties fo:font-weight="bold"/>
  </style:style>
  <style:style style:name="hlp_5f_aux_5f_comment" style:display-name="hlp_aux_comment" style:family="text">
   <style:text-properties fo:color="#800000" fo:background-color="#e6ff00"/>
  </style:style>
  <style:style style:name="hlp_5f_menuitem" style:display-name="hlp_menuitem" style:family="text">
   <style:text-properties fo:color="#ff00ff" fo:font-weight="bold"/>
  </style:style>
  <style:style style:name="hlp_5f_acronym" style:display-name="hlp_acronym" style:family="text">
   <style:text-properties style:text-underline-style="dotted" style:text-underline-width="auto" style:text-underline-color="font-color"/>
  </style:style>
  <style:style style:name="hlp_5f_literal" style:display-name="hlp_literal" style:family="text">
   <style:text-properties style:font-name="Monospace" fo:font-size="9pt"/>
  </style:style>
  <style:style style:name="hlp_5f_path" style:display-name="hlp_path" style:family="text">
   <style:text-properties style:font-name="Monospace" fo:font-size="9pt"/>
  </style:style>
  <style:style style:name="hlp_5f_aux_5f_parachanged" style:display-name="hlp_aux_parachanged" style:family="text" style:parent-style-name="hlp_5f_aux_5f_tag">
   <style:text-properties fo:background-color="#ffff00"/>
  </style:style>
  <style:style style:name="Frame" style:family="graphic">
   <style:graphic-properties text:anchor-type="paragraph" svg:x="0cm" svg:y="0cm" fo:margin-left="0.201cm" fo:margin-right="0.201cm" fo:margin-top="0.201cm" fo:margin-bottom="0.201cm" style:wrap="parallel" style:number-wrapped-paragraphs="no-limit" style:wrap-contour="false" style:vertical-pos="top" style:vertical-rel="paragraph-content" style:horizontal-pos="center" style:horizontal-rel="paragraph-content" fo:padding="0.15cm" fo:border="0.002cm solid #000000"/>
  </style:style>
  <style:style style:name="Graphics" style:family="graphic">
   <style:graphic-properties text:anchor-type="paragraph" svg:x="0cm" svg:y="0cm" style:wrap="none" style:vertical-pos="top" style:vertical-rel="paragraph" style:horizontal-pos="center" style:horizontal-rel="paragraph"/>
  </style:style>
  <style:style style:name="hlp_5f_img_5f_loc" style:display-name="hlp_img_loc" style:family="graphic" style:parent-style-name="Graphics">
   <style:graphic-properties fo:padding="0.049cm" fo:border="0.088cm solid #e76f00"/>
  </style:style>
  <text:outline-style style:name="Outline">
   <text:outline-level-style text:level="1" style:num-format="">
    <style:list-level-properties/>
   </text:outline-level-style>
   <text:outline-level-style text:level="2" style:num-format="">
    <style:list-level-properties/>
   </text:outline-level-style>
   <text:outline-level-style text:level="3" style:num-format="">
    <style:list-level-properties/>
   </text:outline-level-style>
   <text:outline-level-style text:level="4" style:num-format="">
    <style:list-level-properties/>
   </text:outline-level-style>
   <text:outline-level-style text:level="5" style:num-format="">
    <style:list-level-properties/>
   </text:outline-level-style>
   <text:outline-level-style text:level="6" style:num-format="">
    <style:list-level-properties/>
   </text:outline-level-style>
   <text:outline-level-style text:level="7" style:num-format="">
    <style:list-level-properties/>
   </text:outline-level-style>
   <text:outline-level-style text:level="8" style:num-format="">
    <style:list-level-properties/>
   </text:outline-level-style>
   <text:outline-level-style text:level="9" style:num-format="">
    <style:list-level-properties/>
   </text:outline-level-style>
   <text:outline-level-style text:level="10" style:num-format="">
    <style:list-level-properties/>
   </text:outline-level-style>
  </text:outline-style>
  <text:list-style style:name="Numbering_20_1" style:display-name="Numbering 1">
   <text:list-level-style-number text:level="1" text:style-name="Numbering_20_Symbols" style:num-suffix="." style:num-format="1">
    <style:list-level-properties text:list-level-position-and-space-mode="label-alignment">
     <style:list-level-label-alignment text:label-followed-by="listtab" text:list-tab-stop-position="0.499cm" fo:text-indent="-0.499cm" fo:margin-left="0.499cm"/>
    </style:list-level-properties>
   </text:list-level-style-number>
   <text:list-level-style-number text:level="2" text:style-name="Numbering_20_Symbols" style:num-suffix="." style:num-format="1">
    <style:list-level-properties text:list-level-position-and-space-mode="label-alignment">
     <style:list-level-label-alignment text:label-followed-by="listtab" text:list-tab-stop-position="1cm" fo:text-indent="-0.499cm" fo:margin-left="1cm"/>
    </style:list-level-properties>
   </text:list-level-style-number>
   <text:list-level-style-number text:level="3" text:style-name="Numbering_20_Symbols" style:num-suffix="." style:num-format="1">
    <style:list-level-properties text:list-level-position-and-space-mode="label-alignment">
     <style:list-level-label-alignment text:label-followed-by="listtab" text:list-tab-stop-position="1.499cm" fo:text-indent="-0.499cm" fo:margin-left="1.499cm"/>
    </style:list-level-properties>
   </text:list-level-style-number>
   <text:list-level-style-number text:level="4" text:style-name="Numbering_20_Symbols" style:num-suffix="." style:num-format="1">
    <style:list-level-properties text:list-level-position-and-space-mode="label-alignment">
     <style:list-level-label-alignment text:label-followed-by="listtab" text:list-tab-stop-position="2cm" fo:text-indent="-0.499cm" fo:margin-left="2cm"/>
    </style:list-level-properties>
   </text:list-level-style-number>
   <text:list-level-style-number text:level="5" text:style-name="Numbering_20_Symbols" style:num-suffix="." style:num-format="1">
    <style:list-level-properties text:list-level-position-and-space-mode="label-alignment">
     <style:list-level-label-alignment text:label-followed-by="listtab" text:list-tab-stop-position="2.499cm" fo:text-indent="-0.499cm" fo:margin-left="2.499cm"/>
    </style:list-level-properties>
   </text:list-level-style-number>
   <text:list-level-style-number text:level="6" text:style-name="Numbering_20_Symbols" style:num-suffix="." style:num-format="1">
    <style:list-level-properties text:list-level-position-and-space-mode="label-alignment">
     <style:list-level-label-alignment text:label-followed-by="listtab" text:list-tab-stop-position="3cm" fo:text-indent="-0.499cm" fo:margin-left="3cm"/>
    </style:list-level-properties>
   </text:list-level-style-number>
   <text:list-level-style-number text:level="7" text:style-name="Numbering_20_Symbols" style:num-suffix="." style:num-format="1">
    <style:list-level-properties text:list-level-position-and-space-mode="label-alignment">
     <style:list-level-label-alignment text:label-followed-by="listtab" text:list-tab-stop-position="3.5cm" fo:text-indent="-0.499cm" fo:margin-left="3.5cm"/>
    </style:list-level-properties>
   </text:list-level-style-number>
   <text:list-level-style-number text:level="8" text:style-name="Numbering_20_Symbols" style:num-suffix="." style:num-format="1">
    <style:list-level-properties text:list-level-position-and-space-mode="label-alignment">
     <style:list-level-label-alignment text:label-followed-by="listtab" text:list-tab-stop-position="4.001cm" fo:text-indent="-0.499cm" fo:margin-left="4.001cm"/>
    </style:list-level-properties>
   </text:list-level-style-number>
   <text:list-level-style-number text:level="9" text:style-name="Numbering_20_Symbols" style:num-suffix="." style:num-format="1">
    <style:list-level-properties text:list-level-position-and-space-mode="label-alignment">
     <style:list-level-label-alignment text:label-followed-by="listtab" text:list-tab-stop-position="4.5cm" fo:text-indent="-0.499cm" fo:margin-left="4.5cm"/>
    </style:list-level-properties>
   </text:list-level-style-number>
   <text:list-level-style-number text:level="10" text:style-name="Numbering_20_Symbols" style:num-suffix="." style:num-format="1">
    <style:list-level-properties text:list-level-position-and-space-mode="label-alignment">
     <style:list-level-label-alignment text:label-followed-by="listtab" text:list-tab-stop-position="5.001cm" fo:text-indent="-0.499cm" fo:margin-left="5.001cm"/>
    </style:list-level-properties>
   </text:list-level-style-number>
  </text:list-style>
  <text:list-style style:name="List_20_1" style:display-name="List 1">
   <text:list-level-style-bullet text:level="1" text:style-name="Numbering_20_Symbols" text:bullet-char="•">
    <style:list-level-properties text:list-level-position-and-space-mode="label-alignment">
     <style:list-level-label-alignment text:label-followed-by="listtab" text:list-tab-stop-position="1.035cm" fo:text-indent="-0.4cm" fo:margin-left="1.035cm"/>
    </style:list-level-properties>
    <style:text-properties style:font-name="OpenSymbol"/>
   </text:list-level-style-bullet>
   <text:list-level-style-bullet text:level="2" text:style-name="Numbering_20_Symbols" text:bullet-char="•">
    <style:list-level-properties text:list-level-position-and-space-mode="label-alignment">
     <style:list-level-label-alignment text:label-followed-by="listtab" text:list-tab-stop-position="1.436cm" fo:text-indent="-0.4cm" fo:margin-left="1.436cm"/>
    </style:list-level-properties>
    <style:text-properties style:font-name="OpenSymbol"/>
   </text:list-level-style-bullet>
   <text:list-level-style-bullet text:level="3" text:style-name="Numbering_20_Symbols" text:bullet-char="•">
    <style:list-level-properties text:list-level-position-and-space-mode="label-alignment">
     <style:list-level-label-alignment text:label-followed-by="listtab" text:list-tab-stop-position="1.834cm" fo:text-indent="-0.4cm" fo:margin-left="1.834cm"/>
    </style:list-level-properties>
    <style:text-properties style:font-name="OpenSymbol"/>
   </text:list-level-style-bullet>
   <text:list-level-style-bullet text:level="4" text:style-name="Numbering_20_Symbols" text:bullet-char="•">
    <style:list-level-properties text:list-level-position-and-space-mode="label-alignment">
     <style:list-level-label-alignment text:label-followed-by="listtab" text:list-tab-stop-position="2.235cm" fo:text-indent="-0.4cm" fo:margin-left="2.235cm"/>
    </style:list-level-properties>
    <style:text-properties style:font-name="OpenSymbol"/>
   </text:list-level-style-bullet>
   <text:list-level-style-bullet text:level="5" text:style-name="Numbering_20_Symbols" text:bullet-char="•">
    <style:list-level-properties text:list-level-position-and-space-mode="label-alignment">
     <style:list-level-label-alignment text:label-followed-by="listtab" text:list-tab-stop-position="2.635cm" fo:text-indent="-0.4cm" fo:margin-left="2.635cm"/>
    </style:list-level-properties>
    <style:text-properties style:font-name="OpenSymbol"/>
   </text:list-level-style-bullet>
   <text:list-level-style-bullet text:level="6" text:style-name="Numbering_20_Symbols" text:bullet-char="•">
    <style:list-level-properties text:list-level-position-and-space-mode="label-alignment">
     <style:list-level-label-alignment text:label-followed-by="listtab" text:list-tab-stop-position="3.036cm" fo:text-indent="-0.4cm" fo:margin-left="3.036cm"/>
    </style:list-level-properties>
    <style:text-properties style:font-name="OpenSymbol"/>
   </text:list-level-style-bullet>
   <text:list-level-style-bullet text:level="7" text:style-name="Numbering_20_Symbols" text:bullet-char="•">
    <style:list-level-properties text:list-level-position-and-space-mode="label-alignment">
     <style:list-level-label-alignment text:label-followed-by="listtab" text:list-tab-stop-position="3.434cm" fo:text-indent="-0.4cm" fo:margin-left="3.434cm"/>
    </style:list-level-properties>
    <style:text-properties style:font-name="OpenSymbol"/>
   </text:list-level-style-bullet>
   <text:list-level-style-bullet text:level="8" text:style-name="Numbering_20_Symbols" text:bullet-char="•">
    <style:list-level-properties text:list-level-position-and-space-mode="label-alignment">
     <style:list-level-label-alignment text:label-followed-by="listtab" text:list-tab-stop-position="3.835cm" fo:text-indent="-0.4cm" fo:margin-left="3.835cm"/>
    </style:list-level-properties>
    <style:text-properties style:font-name="OpenSymbol"/>
   </text:list-level-style-bullet>
   <text:list-level-style-bullet text:level="9" text:style-name="Numbering_20_Symbols" text:bullet-char="•">
    <style:list-level-properties text:list-level-position-and-space-mode="label-alignment">
     <style:list-level-label-alignment text:label-followed-by="listtab" text:list-tab-stop-position="4.235cm" fo:text-indent="-0.4cm" fo:margin-left="4.235cm"/>
    </style:list-level-properties>
    <style:text-properties style:font-name="OpenSymbol"/>
   </text:list-level-style-bullet>
   <text:list-level-style-bullet text:level="10" text:style-name="Numbering_20_Symbols" text:bullet-char="•">
    <style:list-level-properties text:list-level-position-and-space-mode="label-alignment">
     <style:list-level-label-alignment text:label-followed-by="listtab" text:list-tab-stop-position="4.636cm" fo:text-indent="-0.4cm" fo:margin-left="4.636cm"/>
    </style:list-level-properties>
    <style:text-properties style:font-name="OpenSymbol"/>
   </text:list-level-style-bullet>
  </text:list-style>
  <text:list-style style:name="hlp_5f_orderedlist" style:display-name="hlp_orderedlist">
   <text:list-level-style-number text:level="1" text:style-name="Numbering_20_Symbols" style:num-suffix="." style:num-format="1">
    <style:list-level-properties text:min-label-width="0.499cm"/>
   </text:list-level-style-number>
   <text:list-level-style-number text:level="2" style:num-suffix="." style:num-format="1">
    <style:list-level-properties text:space-before="0.501cm" text:min-label-width="0.499cm"/>
   </text:list-level-style-number>
   <text:list-level-style-number text:level="3" style:num-suffix="." style:num-format="1">
    <style:list-level-properties text:space-before="1cm" text:min-label-width="0.499cm"/>
   </text:list-level-style-number>
   <text:list-level-style-number text:level="4" style:num-suffix="." style:num-format="1">
    <style:list-level-properties text:space-before="1.501cm" text:min-label-width="0.499cm"/>
   </text:list-level-style-number>
   <text:list-level-style-number text:level="5" style:num-suffix="." style:num-format="1">
    <style:list-level-properties text:space-before="2cm" text:min-label-width="0.499cm"/>
   </text:list-level-style-number>
   <text:list-level-style-number text:level="6" style:num-suffix="." style:num-format="1">
    <style:list-level-properties text:space-before="2.501cm" text:min-label-width="0.499cm"/>
   </text:list-level-style-number>
   <text:list-level-style-number text:level="7" style:num-suffix="." style:num-format="1">
    <style:list-level-properties text:space-before="3.001cm" text:min-label-width="0.499cm"/>
   </text:list-level-style-number>
   <text:list-level-style-number text:level="8" style:num-suffix="." style:num-format="1">
    <style:list-level-properties text:space-before="3.502cm" text:min-label-width="0.499cm"/>
   </text:list-level-style-number>
   <text:list-level-style-number text:level="9" style:num-suffix="." style:num-format="1">
    <style:list-level-properties text:space-before="4.001cm" text:min-label-width="0.499cm"/>
   </text:list-level-style-number>
   <text:list-level-style-number text:level="10" style:num-suffix="." style:num-format="1">
    <style:list-level-properties text:space-before="4.502cm" text:min-label-width="0.499cm"/>
   </text:list-level-style-number>
  </text:list-style>
  <text:list-style style:name="hlp_5f_unorderedlist" style:display-name="hlp_unorderedlist">
   <text:list-level-style-bullet text:level="1" text:style-name="Bullet_20_Symbols" text:bullet-char="•">
    <style:list-level-properties text:min-label-width="0.499cm"/>
    <style:text-properties fo:font-family="StarSymbol" style:font-charset="x-symbol"/>
   </text:list-level-style-bullet>
   <text:list-level-style-bullet text:level="2" text:style-name="Bullet_20_Symbols" text:bullet-char="•">
    <style:list-level-properties text:space-before="0.501cm" text:min-label-width="0.499cm"/>
    <style:text-properties fo:font-family="StarSymbol" style:font-charset="x-symbol"/>
   </text:list-level-style-bullet>
   <text:list-level-style-bullet text:level="3" text:style-name="Bullet_20_Symbols" style:num-suffix="." text:bullet-char="•">
    <style:list-level-properties text:space-before="1cm" text:min-label-width="0.499cm"/>
    <style:text-properties fo:font-family="StarSymbol" style:font-charset="x-symbol"/>
   </text:list-level-style-bullet>
   <text:list-level-style-bullet text:level="4" text:style-name="Bullet_20_Symbols" style:num-suffix="." text:bullet-char="•">
    <style:list-level-properties text:space-before="1.501cm" text:min-label-width="0.499cm"/>
    <style:text-properties fo:font-family="StarSymbol" style:font-charset="x-symbol"/>
   </text:list-level-style-bullet>
   <text:list-level-style-bullet text:level="5" text:style-name="Bullet_20_Symbols" style:num-suffix="." text:bullet-char="•">
    <style:list-level-properties text:space-before="2cm" text:min-label-width="0.499cm"/>
    <style:text-properties fo:font-family="StarSymbol" style:font-charset="x-symbol"/>
   </text:list-level-style-bullet>
   <text:list-level-style-bullet text:level="6" text:style-name="Bullet_20_Symbols" style:num-suffix="." text:bullet-char="•">
    <style:list-level-properties text:space-before="2.501cm" text:min-label-width="0.499cm"/>
    <style:text-properties fo:font-family="StarSymbol" style:font-charset="x-symbol"/>
   </text:list-level-style-bullet>
   <text:list-level-style-bullet text:level="7" text:style-name="Bullet_20_Symbols" style:num-suffix="." text:bullet-char="•">
    <style:list-level-properties text:space-before="3.001cm" text:min-label-width="0.499cm"/>
    <style:text-properties fo:font-family="StarSymbol" style:font-charset="x-symbol"/>
   </text:list-level-style-bullet>
   <text:list-level-style-bullet text:level="8" text:style-name="Bullet_20_Symbols" style:num-suffix="." text:bullet-char="•">
    <style:list-level-properties text:space-before="3.502cm" text:min-label-width="0.499cm"/>
    <style:text-properties fo:font-family="StarSymbol" style:font-charset="x-symbol"/>
   </text:list-level-style-bullet>
   <text:list-level-style-bullet text:level="9" text:style-name="Bullet_20_Symbols" style:num-suffix="." text:bullet-char="•">
    <style:list-level-properties text:space-before="4.001cm" text:min-label-width="0.499cm"/>
    <style:text-properties fo:font-family="StarSymbol" style:font-charset="x-symbol"/>
   </text:list-level-style-bullet>
   <text:list-level-style-bullet text:level="10" text:style-name="Bullet_20_Symbols" style:num-suffix="." text:bullet-char="•">
    <style:list-level-properties text:space-before="4.502cm" text:min-label-width="0.499cm"/>
    <style:text-properties fo:font-family="StarSymbol" style:font-charset="x-symbol"/>
   </text:list-level-style-bullet>
  </text:list-style>
  <text:list-style style:name="Var_20_List" style:display-name="Var List">
   <text:list-level-style-bullet text:level="1" text:style-name="Bullet_20_Symbols" text:bullet-char="•">
    <style:list-level-properties text:min-label-width="0.499cm"/>
    <style:text-properties fo:font-family="StarSymbol" style:font-charset="x-symbol"/>
   </text:list-level-style-bullet>
   <text:list-level-style-bullet text:level="2" text:style-name="Bullet_20_Symbols" style:num-suffix="." text:bullet-char="•">
    <style:list-level-properties text:space-before="0.501cm" text:min-label-width="0.499cm"/>
    <style:text-properties fo:font-family="StarSymbol" style:font-charset="x-symbol"/>
   </text:list-level-style-bullet>
   <text:list-level-style-bullet text:level="3" text:style-name="Bullet_20_Symbols" style:num-suffix="." text:bullet-char="•">
    <style:list-level-properties text:space-before="1cm" text:min-label-width="0.499cm"/>
    <style:text-properties fo:font-family="StarSymbol" style:font-charset="x-symbol"/>
   </text:list-level-style-bullet>
   <text:list-level-style-bullet text:level="4" text:style-name="Bullet_20_Symbols" style:num-suffix="." text:bullet-char="•">
    <style:list-level-properties text:space-before="1.501cm" text:min-label-width="0.499cm"/>
    <style:text-properties fo:font-family="StarSymbol" style:font-charset="x-symbol"/>
   </text:list-level-style-bullet>
   <text:list-level-style-bullet text:level="5" text:style-name="Bullet_20_Symbols" style:num-suffix="." text:bullet-char="•">
    <style:list-level-properties text:space-before="2cm" text:min-label-width="0.499cm"/>
    <style:text-properties fo:font-family="StarSymbol" style:font-charset="x-symbol"/>
   </text:list-level-style-bullet>
   <text:list-level-style-bullet text:level="6" text:style-name="Bullet_20_Symbols" style:num-suffix="." text:bullet-char="•">
    <style:list-level-properties text:space-before="2.501cm" text:min-label-width="0.499cm"/>
    <style:text-properties fo:font-family="StarSymbol" style:font-charset="x-symbol"/>
   </text:list-level-style-bullet>
   <text:list-level-style-bullet text:level="7" text:style-name="Bullet_20_Symbols" style:num-suffix="." text:bullet-char="•">
    <style:list-level-properties text:space-before="3.001cm" text:min-label-width="0.499cm"/>
    <style:text-properties fo:font-family="StarSymbol" style:font-charset="x-symbol"/>
   </text:list-level-style-bullet>
   <text:list-level-style-bullet text:level="8" text:style-name="Bullet_20_Symbols" style:num-suffix="." text:bullet-char="•">
    <style:list-level-properties text:space-before="3.502cm" text:min-label-width="0.499cm"/>
    <style:text-properties fo:font-family="StarSymbol" style:font-charset="x-symbol"/>
   </text:list-level-style-bullet>
   <text:list-level-style-bullet text:level="9" text:style-name="Bullet_20_Symbols" style:num-suffix="." text:bullet-char="•">
    <style:list-level-properties text:space-before="4.001cm" text:min-label-width="0.499cm"/>
    <style:text-properties fo:font-family="StarSymbol" style:font-charset="x-symbol"/>
   </text:list-level-style-bullet>
   <text:list-level-style-bullet text:level="10" text:style-name="Bullet_20_Symbols" style:num-suffix="." text:bullet-char="•">
    <style:list-level-properties text:space-before="4.502cm" text:min-label-width="0.499cm"/>
    <style:text-properties fo:font-family="StarSymbol" style:font-charset="x-symbol"/>
   </text:list-level-style-bullet>
  </text:list-style>
  <text:notes-configuration text:note-class="footnote" text:citation-style-name="Footnote_20_Symbol" style:num-format="1" text:start-value="0" text:footnotes-position="page" text:start-numbering-at="page"/>
  <text:notes-configuration text:note-class="endnote" text:citation-style-name="Endnote_20_Symbol" text:master-page-name="Endnote" style:num-format="1" text:start-value="0"/>
  <text:linenumbering-configuration text:number-lines="false" text:offset="0.499cm" style:num-format="1" text:number-position="left" text:increment="5"/>
<!--  -->

	</xsl:template>

	<xsl:template name="createautostyles">
		<style:style style:name="hlp_table" style:family="table">
			<style:properties fo:margin-top="0.4cm" fo:margin-bottom="0cm" table:align="margins"/>
		</style:style>

	<!--
	<style:style style:name="hlp_aux_section" style:family="section">
		<style:properties fo:background-color="#EEEEEE">
		<style:columns fo:column-count="0" fo:column-gap="0cm"/>
		<style:background-image/>
		</style:properties>
	</style:style>
//-->
	</xsl:template>

<xsl:template name="getreldir">
	<xsl:param name="dirlevel" select="0"/>
	<xsl:param name="reldir" select="'../'"/>
	<xsl:choose>
		<xsl:when test="$dirlevel &gt; 0">
			<xsl:call-template name="getreldir">
				<xsl:with-param name="dirlevel" select="$dirlevel - 1"/>
				<xsl:with-param name="reldir" select="concat($reldir,'../')"/>
			</xsl:call-template>
		</xsl:when>
		<xsl:otherwise>
			<xsl:value-of select="$reldir"/>
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>
	
<xsl:template name="getdepth">
	<xsl:param name="filename" select="''"/>
	<xsl:value-of select="string-length($filename)-string-length(translate($filename,'/',''))"/>
</xsl:template>

	

</xsl:stylesheet>
