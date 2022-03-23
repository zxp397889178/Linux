/*
 * =====================================================================================
 *
 *       Filename:  Command.cpp
 *
 *    Description:  wrapper of keyboard command
 *
 *        Version:  1.0
 *        Created:  2014年12月13日 11时32分39秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.gamefunx.com(IGG)
 *	 URL http://blog.csdn.net/lxh1230119/article/details/7784914
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include "CommandBase.h"



///////////////////////////////////////////////////////////////////////////
int _kbhit(void)
{
	struct termios oldt, newt;  
	int ch;  
	int oldf;  
	tcgetattr(STDIN_FILENO, &oldt);  
	newt = oldt;  
	newt.c_lflag &= ~(ICANON | ECHO);  
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);  
	oldf = fcntl(STDIN_FILENO, F_GETFL, 0);  
	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);  
	ch = getchar();  
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);  
	fcntl(STDIN_FILENO, F_SETFL, oldf);  
	if(ch != EOF)  
	{  
		ungetc(ch, stdin);  
		return 1;  
	}  
	return 0;  
}

int arrow_filter(int key)
{
	if (key == VK_ARROW_FILTER)
	{
		key = getchar();
	}
	return key;
}
 

////////////////////////////////////////////////////////////////////////////
//
CCommandBase::CMD_VECTOR	CCommandBase::s_vecCommand;

CCommandBase::CCommandBase()
	: m_strCmdBuffer()
{
	this->Register("help", CCommandBase::cmdHelp, "display help message.");
}

CCommandBase::~CCommandBase()
{

}

int32_t CCommandBase::Process(void)
{
	if (!_kbhit())
	{
		return 0;
	}

	int key = getchar();
	key = arrow_filter(key);
	switch(key)
	{
	case VK_RETURN: //execute command
		printf("\n");
		//printf("cmd=>[%s]\n", m_strCmdBuffer.c_str());
		if (m_strCmdBuffer.length()>0)
		{
			int ret = this->ParseCommand(m_strCmdBuffer.c_str());
			if (-1 == ret)
			{
				m_strCmdBuffer.clear();
				return -1;
			}
			this->StoreCmd(m_strCmdBuffer.c_str());
			m_strCmdBuffer.clear();
		}
		printf(">");
		break;
	case VK_BACK:	// erase one char
		if (m_strCmdBuffer.length()>0)
		{
			//TODO: 此处存在显示异常
			printf("\b\b\b");
			std::string::iterator it(m_strCmdBuffer.end());
			m_strCmdBuffer.erase(--it);
		}
		break;
	case VK_ESCAPE:	// cancel command
		m_strCmdBuffer.clear();
		printf("...[Cancel]\n>");
		break;
	case VK_UP:		// previous command
		if (this->LookupCmd())
		{
			printf("%s", m_strCmdBuffer.c_str());
		}
		break;
	case VK_DOWN:	// next command
		if (this->LookdownCmd())
		{
			printf("%s", m_strCmdBuffer.c_str());
		}
		break;
	case VK_LEFT:
	case VK_RIGHT:
	default:
		if ((key>=0x20 && key<=0x7E))
		{
			m_strCmdBuffer.append(1, (char)key);
			//printf("%c", key);
		}
		break;
	}

	return 0;
}

int32_t CCommandBase::cmdHelp(const char* text)
{
	for(CCommandBase::CMD_VECTOR::iterator 
			it(CCommandBase::s_vecCommand.begin());
			it != CCommandBase::s_vecCommand.end(); ++it)
	{
		CCommandBase::ConsoleCommand &cmd = *it;
		printf("%-28s %-64s\n", cmd.szCmd, cmd.szNote);
	}

	return 0;
}

int32_t CCommandBase::ParseCommand(const char *cmd_buffer)
{
	if (NULL == cmd_buffer || '\0'==cmd_buffer[0])
	{
		return 0;
	}

	char szCmd[1024] = "";
	strcpy(szCmd, cmd_buffer);

	//别命命令处理
	if (!strncmp(cmd_buffer, "?", 1) || !strncmp(cmd_buffer, "? ", 2))
	{
		strcpy(szCmd, "help");
	}

	for(CMD_VECTOR::iterator it(s_vecCommand.begin()); 
			it != s_vecCommand.end(); ++it)
	{
		ConsoleCommand &cmd = *it;
		size_t cmdLen = strlen(cmd.szCmd);
		if (!strncasecmp(cmd.szCmd, szCmd, cmdLen ))
		{
			if ((strlen(szCmd)==cmdLen) || isspace(szCmd[cmdLen]))
			{
				int32_t ret = cmd.pFunc(&szCmd[cmdLen+1]);
				return ret;
			}
		}
	}
	printf("\033[0;40;31mUnknown Command.\033[0m\n");
	return 0;
}

void CCommandBase::StoreCmd(const char *cmd_buffer)
{
	//TODO: store commmand
}

bool CCommandBase::LookupCmd(void)
{
	//TODO: look previous command
	return false;
}

bool CCommandBase::LookdownCmd(void)
{
	//TODO: look next command
	return false;
}

bool CCommandBase::Register(const char *pszCmd, GPFN_CONSOLECMD pFunc, const char *pszNote)
{
	if (NULL == pszCmd || '\0' == pszCmd[0])
	{
		return false;
	}

	struct ConsoleCommand cmd;
	strncpy(cmd.szCmd, pszCmd, std::min(sizeof(cmd.szCmd)-1, strlen(pszCmd)) );
	cmd.pFunc = pFunc;
	if (NULL != pszNote)
	{
		strncpy(cmd.szNote, pszNote, std::min(sizeof(cmd.szNote)-1, strlen(pszNote)) );
	}
	return this->Register(cmd);
}

bool CCommandBase::Register(struct ConsoleCommand &cmd)
{
	for(CMD_VECTOR::iterator it(s_vecCommand.begin()); 
			it != s_vecCommand.end(); ++it)
	{
		ConsoleCommand &rCmd = *it;
		if (!strcmp(rCmd.szCmd, cmd.szCmd))
		{
			return false;
		}
	}

	s_vecCommand.push_back(cmd);
	//printf("push one command [%s]\n", cmd.szCmd);

	return true;
}

