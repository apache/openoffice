/**************************************************************
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
 *************************************************************/

#define tblselsw_mask_width 16
#define tblselsw_mask_height 16
#define tblselsw_mask_x_hot 1
#define tblselsw_mask_y_hot 14

static const unsigned char tblselsw_mask_bits[] = {
    0x00,0x00,
    0x02,0x00,
    0x06,0x00,
    0x0E,0x00,
    0x1E,0x00,
    0x3E,0x00,
    0x7E,0x00,
    0xFE,0x00,
    0xFE,0x01,
    0xFE,0x03,
    0xFE,0x07,
    0xFE,0x0F,
    0xFE,0x1F,
    0xFE,0x3F,
    0xFE,0x7F,
    0x00,0x00};
