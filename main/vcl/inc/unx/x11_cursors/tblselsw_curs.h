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

#define tblselsw_curs_width 16
#define tblselsw_curs_height 16
#define tblselsw_curs_x_hot 1
#define tblselsw_curs_y_hot 14

static const unsigned char tblselsw_curs_bits[] = {
	0x00,0x00,	// 0000 0000 0000 0000
	0x00,0x00,	// 0000 0000 0000 0000
	0x00,0x00,	// 0000 0000 0000 0000
	0x40,0x00,	// 0010 0000 0000 0000
	0xC0,0x00,	// 0011 0000 0000 0000
	0xC1,0x00,	// 0011 1000 0000 0000
	0xC3,0x00,	// 0011 1100 0000 0000
	0xC7,0x00,	// 0011 1110 0000 0000
	0xCF,0x00,	// 0011 1111 0000 0000
	0xCF,0x10,	// 0011 1111 1000 0000
	0xCF,0x30,	// 0011 1111 1100 0000
	0xCF,0x70,	// 0011 1111 1110 0000
	0xCF,0xF0,	// 0011 1111 1111 0000
	0xCF,0xF1,	// 0011 1111 1111 1000
	0x00,0x00,	// 0000 0000 0000 0000
	0x00,0x00};	// 0000 0000 0000 0000
