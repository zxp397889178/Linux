/*
 * =====================================================================================
 *
 *       Filename:  reload.h
 *
 *    Description:  调用svrframe的重载配置的功能
 *
 *        Version:  1.0
 *        Created:  2020年04月29日 00时08分35秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#pragma once

#include "BaseCode.h"

/*
 *  ============= <config.xml> 配置信息 ============= 
 *  <main>
 *      <log path="/data/fish/log" module="svrframe" level="3" />
 *      <service type="websocket" header="2">
 *          <extcmd>
 *              <reloadcfg cmd="123456" url="ws://127.0.0.1:18083" />
 *          </extcmd>
 *      </service>
 *  </main>
 */
int32_t reload_config(const CMyString &strXml);


