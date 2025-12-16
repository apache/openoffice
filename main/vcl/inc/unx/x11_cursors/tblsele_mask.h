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

#define tblsele_mask_width 24
#define tblsele_mask_height 24
#define tblsele_mask_x_hot 22
#define tblsele_mask_y_hot 11

static const unsigned char tblsele_mask_bits[] = {
	0x00,0x00,0x00,		// 0000 0000 0000 0000 0000 0000
	0x00,0x00,0x00,		// 0000 0000 0000 0000 0000 0000
	0x00,0x02,0x00,		// 0000 0000 0000 0100 0000 0000
	0x00,0x06,0x00,		// 0000 0000 0000 0110 0000 0000
	0x00,0x0E,0x00,		// 0000 0000 0000 0111 0000 0000
	0x00,0x0E,0x10,		// 0000 0000 0000 0111 1000 0000
	0x00,0x0E,0x30,		// 0000 0000 0000 0111 1100 0000
	0x00,0x0E,0x70,		// 0000 0000 0000 0111 1110 0000
	0x00,0x0E,0xF0,		// 0000 0000 0000 0111 1111 0000
	0x00,0x0E,0xF1,		// 0000 0000 0000 0111 1111 1000
	0x00,0x0E,0xF3,		// 0000 0000 0000 0111 1111 1100
	0x00,0x0E,0xF7,		// 0000 0000 0000 0111 1111 1110
	0x00,0x0E,0xF3,		// 0000 0000 0000 0111 1111 1100
	0x00,0x0E,0xF1,		// 0000 0000 0000 0111 1111 1000
	0x00,0x0E,0xF0,		// 0000 0000 0000 0111 1111 0000
	0x00,0x0E,0x70,		// 0000 0000 0000 0111 1110 0000
	0x00,0x0E,0x30,		// 0000 0000 0000 0111 1100 0000
	0x00,0x0E,0x10,		// 0000 0000 0000 0111 1000 0000
	0x00,0x0E,0x00,		// 0000 0000 0000 0111 0000 0000
	0x00,0x06,0x00,		// 0000 0000 0000 0110 0000 0000
	0x00,0x02,0x00,		// 0000 0000 0000 0100 0000 0000
	0x00,0x00,0x00,		// 0000 0000 0000 0000 0000 0000
	0x00,0x00,0x00,		// 0000 0000 0000 0000 0000 0000
	0x00,0x00,0x00};	// 0000 0000 0000 0000 0000 0000
