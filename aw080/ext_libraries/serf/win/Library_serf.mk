#/**************************************************************
# * 
# * Licensed to the Apache Software Foundation (ASF) under one
# * or more contributor license agreements.  See the NOTICE file
# * distributed with this work for additional information
# * regarding copyright ownership.  The ASF licenses this file
# * to you under the Apache License, Version 2.0 (the
# * "License"); you may not use this file except in compliance
# * with the License.  You may obtain a copy of the License at
# * 
# *   http://www.apache.org/licenses/LICENSE-2.0
# * 
# * Unless required by applicable law or agreed to in writing,
# * software distributed under the License is distributed on an
# * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# * KIND, either express or implied.  See the License for the
# * specific language governing permissions and limitations
# * under the License.
# * 
# *************************************************************/

$(eval $(call gb_Library_Library,serf))

$(eval $(call gb_Library_add_package_headers,serf,serf_inc))

$(eval $(call gb_Library_set_include,serf,\
	$$(INCLUDE) \
	-I. \
	-I$(OUTDIR)/inc/apr \
	-I$(OUTDIR)/inc/apr-util \
	-I$(OUTDIR)/inc/external/zlib \
))

$(eval $(call gb_Library_set_defs,serf,\
	$$(DEFS) \
	-DWIN32 -DNDEBUG -D_WINDOWS -D_USRDLL -DWIN32_EXPORTS \
))

ORIGINAL_DEF_FILE=build/serf.def
FIXED_DEF_FILE=build/fixed-serf.def

$(call gb_LinkTarget_get_target,$(call gb_Library__get_linktargetname,serf)) : $(FIXED_DEF_FILE)
$(FIXED_DEF_FILE) : $(ORIGINAL_DEF_FILE)
	cp $< $@
	echo serf_bucket_request_set_CL >> $@

$(eval $(call gb_Library_set_ldflags,serf,\
	$$(LDFLAGS) \
	-DEF:$(FIXED_DEF_FILE)	\
))

$(eval $(call gb_Library_add_linked_libs,serf,\
	apr \
	apr-util \
	libeay32 \
	ssleay32 \
	zlib \
    $(gb_STDLIBS) \
))

$(eval $(call gb_Library_add_cobjects,serf,\
	serf/$(INPATH)/misc/build/serf-1.1.0/buckets/aggregate_buckets \
	serf/$(INPATH)/misc/build/serf-1.1.0/buckets/request_buckets \
	serf/$(INPATH)/misc/build/serf-1.1.0/buckets/buckets \
	serf/$(INPATH)/misc/build/serf-1.1.0/buckets/simple_buckets \
	serf/$(INPATH)/misc/build/serf-1.1.0/buckets/file_buckets \
	serf/$(INPATH)/misc/build/serf-1.1.0/buckets/mmap_buckets \
	serf/$(INPATH)/misc/build/serf-1.1.0/buckets/socket_buckets \
	serf/$(INPATH)/misc/build/serf-1.1.0/buckets/response_buckets \
	serf/$(INPATH)/misc/build/serf-1.1.0/buckets/headers_buckets \
	serf/$(INPATH)/misc/build/serf-1.1.0/buckets/allocator \
	serf/$(INPATH)/misc/build/serf-1.1.0/buckets/dechunk_buckets \
	serf/$(INPATH)/misc/build/serf-1.1.0/buckets/deflate_buckets \
	serf/$(INPATH)/misc/build/serf-1.1.0/buckets/limit_buckets \
	serf/$(INPATH)/misc/build/serf-1.1.0/buckets/ssl_buckets \
	serf/$(INPATH)/misc/build/serf-1.1.0/buckets/barrier_buckets \
	serf/$(INPATH)/misc/build/serf-1.1.0/buckets/chunk_buckets \
	serf/$(INPATH)/misc/build/serf-1.1.0/buckets/bwtp_buckets \
	serf/$(INPATH)/misc/build/serf-1.1.0/buckets/iovec_buckets \
	serf/$(INPATH)/misc/build/serf-1.1.0/auth/auth \
	serf/$(INPATH)/misc/build/serf-1.1.0/auth/auth_basic \
	serf/$(INPATH)/misc/build/serf-1.1.0/auth/auth_digest \
	serf/$(INPATH)/misc/build/serf-1.1.0/auth/auth_kerb \
	serf/$(INPATH)/misc/build/serf-1.1.0/auth/auth_kerb_gss \
	serf/$(INPATH)/misc/build/serf-1.1.0/context \
	serf/$(INPATH)/misc/build/serf-1.1.0/incoming \
	serf/$(INPATH)/misc/build/serf-1.1.0/outgoing \
	serf/$(INPATH)/misc/build/serf-1.1.0/ssltunnel \
))
