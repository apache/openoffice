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

$(eval $(call gb_Package_Package,gtest_inc,$(EXT_LIBRARIES)/gtest/$(INPATH)/misc/build/gtest-1.7.0))

$(eval $(call gb_Package_add_file,gtest_inc,inc/gtest/gtest.h,include/gtest/gtest.h))

$(eval $(call gb_Package_add_file,gtest_inc,inc/gtest/gtest-death-test.h,include/gtest/gtest-death-test.h))
$(eval $(call gb_Package_add_file,gtest_inc,inc/gtest/gtest-message.h,include/gtest/gtest-message.h))
$(eval $(call gb_Package_add_file,gtest_inc,inc/gtest/gtest-param-test.h,include/gtest/gtest-param-test.h))
$(eval $(call gb_Package_add_file,gtest_inc,inc/gtest/gtest-printers.h,include/gtest/gtest-printers.h))
$(eval $(call gb_Package_add_file,gtest_inc,inc/gtest/gtest-spi.h,include/gtest/gtest-spi.h))
$(eval $(call gb_Package_add_file,gtest_inc,inc/gtest/gtest-test-part.h,include/gtest/gtest-test-part.h))
$(eval $(call gb_Package_add_file,gtest_inc,inc/gtest/gtest-typed-test.h,include/gtest/gtest-typed-test.h))
$(eval $(call gb_Package_add_file,gtest_inc,inc/gtest/gtest_pred_impl.h,include/gtest/gtest_pred_impl.h))
$(eval $(call gb_Package_add_file,gtest_inc,inc/gtest/gtest_prod.h,include/gtest/gtest_prod.h))

$(eval $(call gb_Package_add_file,gtest_inc,inc/gtest/internal/gtest-internal.h,include/gtest/internal/gtest-internal.h))
$(eval $(call gb_Package_add_file,gtest_inc,inc/gtest/internal/gtest-death-test-internal.h,include/gtest/internal/gtest-death-test-internal.h))
$(eval $(call gb_Package_add_file,gtest_inc,inc/gtest/internal/gtest-filepath.h,include/gtest/internal/gtest-filepath.h))
$(eval $(call gb_Package_add_file,gtest_inc,inc/gtest/internal/gtest-linked_ptr.h,include/gtest/internal/gtest-linked_ptr.h))
$(eval $(call gb_Package_add_file,gtest_inc,inc/gtest/internal/gtest-param-util-generated.h,include/gtest/internal/gtest-param-util-generated.h))
$(eval $(call gb_Package_add_file,gtest_inc,inc/gtest/internal/gtest-param-util.h,include/gtest/internal/gtest-param-util.h))
$(eval $(call gb_Package_add_file,gtest_inc,inc/gtest/internal/gtest-port.h,include/gtest/internal/gtest-port.h))
$(eval $(call gb_Package_add_file,gtest_inc,inc/gtest/internal/gtest-string.h,include/gtest/internal/gtest-string.h))
$(eval $(call gb_Package_add_file,gtest_inc,inc/gtest/internal/gtest-tuple.h,include/gtest/internal/gtest-tuple.h))
$(eval $(call gb_Package_add_file,gtest_inc,inc/gtest/internal/gtest-type-util.h,include/gtest/internal/gtest-type-util.h))
