/*
 * =====================================================================================
 *
 *       Filename:  color.h
 *
 *    Description:  定义颜色的宏
 *
 *        Version:  1.0
 *        Created:  2015年07月30日 16时04分13秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.net(IGG)
 *
 * =====================================================================================
 */

#ifndef _QW_BASELIB_COLOR_H_
#define _QW_BASELIB_COLOR_H_

// console color defined
// http://www.linuxidc.com/Linux/2008-09/16184.htm
#ifndef CONSOLE_COLOR_SET
#define CONSOLE_COLOR_SET
// 前景颜色
#	define	CON_FG_BLACK				30	
#	define  CON_FG_RED					31	
#	define  CON_FG_GREEN				32
#	define  CON_FG_YELLOW				33
#	define  CON_FG_BLUE					34
#	define  CON_FG_FUCHSIA				35	// 紫红色
#	define  CON_FG_CYANIC				36	// 青蓝色
#	define  CON_FG_WHITE				37
// 背景颜色
#	define	CON_BG_BLACK				40
#	define  CON_BG_RED					41	
#	define  CON_BG_GREEN				42
#	define  CON_BG_YELLOW				43
#	define  CON_BG_BLUE					44
#	define  CON_BG_FUCHSIA				45	// 紫红色
#	define  CON_BG_CYANIC				46	// 青蓝色
#	define  CON_BG_WHITE				47
// 其他定义
#	define  CON_ATTR_DEFAULT			0	//终端默认设置
#	define  CON_ATTR_HIGHLIGHT			1	//高亮显示
#	define  CON_ATTR_UNDERLINE			4	//使用下划线
#	define  CON_ATTR_FLICKER			5	//闪烁
#	define  CON_ATTR_REVERSEDISPLAY		7	//反白显示
#	define  CON_ATTR_INVISIBLE			8	//不可见
#endif	//CONSOLE_COLOR_SET

#endif //  _QW_BASELIB_COLOR_H_

