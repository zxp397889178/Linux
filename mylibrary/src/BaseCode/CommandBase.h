/*
 * =====================================================================================
 *
 *       Filename:  Command.h
 *
 *    Description:  wrapper of keyboard command
 *
 *        Version:  1.0
 *        Created:  2014年12月13日 11时32分21秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.gamefunx.com(IGG)
 *
 * =====================================================================================
 */

#ifndef _MYLIB_P2PWRAPPER_COMMAND_H_
#define _MYLIB_P2PWRAPPER_COMMAND_H_

#include <sys/types.h>
#include <stdint.h>
#include <stdio.h>
#include <vector>
#include <string>

#ifndef OS_WIN32
#define VK_RETURN		0x0A
#define	VK_BACK			0x08
#define VK_ESCAPE		0x1B
#define VK_ARROW_FILTER	0x4B
#define VK_UP			0x41
#define VK_DOWN			0x42
#define	VK_RIGHT		0x43
#define VK_LEFT			0x44
#endif


typedef int32_t (*GPFN_CONSOLECMD)(const char*);
class CCommandBase
{
	struct ConsoleCommand
	{
		char				szCmd[64];		//command
		GPFN_CONSOLECMD		pFunc;			//function
		char				szNote[128];	//command descript

		ConsoleCommand()
		{
			szCmd[0]='\0';
			pFunc = NULL;
			szNote[0] = '\0';
		}
	};
	typedef std::vector<ConsoleCommand>		CMD_VECTOR;
public:
	CCommandBase();
	~CCommandBase();

	int32_t Process(void);

protected:
	int32_t ParseCommand(const char *cmd_buffer);

	bool Register(const char *pszCmd, GPFN_CONSOLECMD pFunc, const char *pszNote);
	bool Register(struct ConsoleCommand &cmd);

private:
	static int32_t cmdHelp(const char* text);

	void StoreCmd(const char *cmd_buffer);
	bool LookupCmd(void);
	bool LookdownCmd(void);
	
private:
	static CMD_VECTOR		s_vecCommand;
	std::string				m_strCmdBuffer;
};


#endif //_MYLIB_P2PWRAPPER_COMMAND_H_ 
