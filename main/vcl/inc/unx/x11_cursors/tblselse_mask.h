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

#define tblselse_mask_width 16
#define tblselse_mask_height 16
#define tblselse_mask_x_hot 14
#define tblselse_mask_y_hot 14

static const unsigned char tblselse_mask_bits[] = {
	0x00,0x00,	// 0000 0000 0000 0000
	0x00,0x04,	// 0000 0000 0000 0010
	0x00,0x06,	// 0000 0000 0000 0110
	0x00,0x07,	// 0000 0000 0000 1110
	0x00,0x87,	// 0000 0000 0001 1110
	0x00,0xC7,	// 0000 0000 0011 1110
	0x00,0xE7,	// 0000 0000 0111 1110
	0x00,0xF7,	// 0000 0000 1111 1110
	0x08,0xF7,	// 0000 0001 1111 1110
	0x0C,0xF7,	// 0000 0011 1111 1110
	0x0E,0xF7,	// 0000 0111 1111 1110
	0x0F,0xF7,	// 0000 1111 1111 1110
	0x8F,0xF7,	// 0001 1111 1111 1110
	0xCF,0xF7,	// 0011 1111 1111 1110
	0xEF,0xF7,	// 0111 1111 1111 1110
	0x00,0x00};	// 0000 0000 0000 0000
