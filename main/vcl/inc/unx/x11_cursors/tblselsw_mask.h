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

#define tblselsw_mask_width 24
#define tblselsw_mask_height 24
#define tblselsw_mask_x_hot 1
#define tblselsw_mask_y_hot 11

static const unsigned char tblselsw_mask_bits[] = {
	0x00,0x00,0x00,		// 0000 0000 0000 0000 0000 0000
	0x00,0x00,0x00,		// 0000 0000 0000 0000 0000 0000
	0x00,0x40,0x00,		// 0000 0000 0010 0000 0000 0000
	0x00,0x60,0x00,		// 0000 0000 0110 0000 0000 0000
	0x00,0x70,0x00,		// 0000 0000 1110 0000 0000 0000
	0x00,0x70,0x00,		// 0000 0001 1110 0000 0000 0000
	0x0C,0x70,0x00,		// 0000 0011 1110 0000 0000 0000
	0x0E,0x70,0x00,		// 0000 0111 1110 0000 0000 0000
	0x0F,0x70,0x00,		// 0000 1111 1110 0000 0000 0000
	0x8F,0x70,0x00,		// 0001 1111 1110 0000 0000 0000
	0xCF,0x70,0x00,		// 0011 1111 1110 0000 0000 0000
	0xEF,0x70,0x00,		// 0111 1111 1110 0000 0000 0000
	0xCF,0x70,0x00,		// 0011 1111 1110 0000 0000 0000
	0x8F,0x70,0x00,		// 0001 1111 1110 0000 0000 0000
	0x0F,0x70,0x00,		// 0000 1111 1110 0000 0000 0000
	0x0E,0x70,0x00,		// 0000 0111 1110 0000 0000 0000
	0x0C,0x70,0x00,		// 0000 0011 1110 0000 0000 0000
	0x00,0x70,0x00,		// 0000 0001 1110 0000 0000 0000
	0x00,0x70,0x00,		// 0000 0000 1110 0000 0000 0000
	0x00,0x60,0x00,		// 0000 0000 0110 0000 0000 0000
	0x00,0x40,0x00,		// 0000 0000 0010 0000 0000 0000
	0x00,0x00,0x00,		// 0000 0000 0000 0000 0000 0000
	0x00,0x00,0x00,		// 0000 0000 0000 0000 0000 0000
	0x00,0x00,0x00};	// 0000 0000 0000 0000 0000 0000
