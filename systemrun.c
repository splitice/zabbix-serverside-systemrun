/*
** Zabbix
** Copyright (C) 2001-2014 Zabbix SIA
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
**/

#include "sysinc.h"
#include "module.h"
#include "common.h"
#include "sysinfo.h"

#include "log.h"


/* the variable keeps timeout setting for item processing */
static int	item_timeout = 60;//Default to 60s

int	zbx_module_system_run(AGENT_REQUEST *request, AGENT_RESULT *result);

static ZBX_METRIC keys[] =
/*      KEY                     FLAG		FUNCTION        	TEST PARAMETERS */
{
	{"system.simple_run",		CF_HAVEPARAMS,		zbx_module_system_run,	"echo test"},
	{NULL}
};

/******************************************************************************
 *                                                                            *
 * Function: zbx_module_api_version                                           *
 *                                                                            *
 * Purpose: returns version number of the module interface                    *
 *                                                                            *
 * Return value: ZBX_MODULE_API_VERSION_ONE - the only version supported by   *
 *               Zabbix currently                                             *
 *                                                                            *
 ******************************************************************************/
int	zbx_module_api_version()
{
	return ZBX_MODULE_API_VERSION_ONE;
}

/******************************************************************************
 *                                                                            *
 * Function: zbx_module_item_timeout                                          *
 *                                                                            *
 * Purpose: set timeout value for processing of items                         *
 *                                                                            *
 * Parameters: timeout - timeout in seconds, 0 - no timeout set               *
 *                                                                            *
 ******************************************************************************/
void	zbx_module_item_timeout(int timeout)
{
	item_timeout = timeout;
}

/******************************************************************************
 *                                                                            *
 * Function: zbx_module_item_list                                             *
 *                                                                            *
 * Purpose: returns list of item keys supported by the module                 *
 *                                                                            *
 * Return value: list of item keys                                            *
 *                                                                            *
 ******************************************************************************/
ZBX_METRIC	*zbx_module_item_list()
{
	return keys;
}

int	execute_str_mod(const char *command, AGENT_RESULT *result)
{
	int	ret = SYSINFO_RET_FAIL;
	char	*cmd_result = NULL, error[MAX_STRING_LEN];

	assert(result);

	init_result(result);

	if (SUCCEED != zbx_execute(command, &cmd_result, error, sizeof(error), item_timeout))
	{
		SET_MSG_RESULT(result, zbx_strdup(NULL, error));
		goto lbl_exit;
	}

	zbx_rtrim(cmd_result, ZBX_WHITESPACE);

	zabbix_log(LOG_LEVEL_DEBUG, "Run remote command [%s] Result [%d] [%.20s]...",
			command, strlen(cmd_result), cmd_result);

	if ('\0' == *cmd_result)	/* we got whitespace only */
		goto lbl_exit;

	SET_TEXT_RESULT(result, zbx_strdup(NULL, cmd_result));

	ret = SYSINFO_RET_OK;
lbl_exit:
	zbx_free(cmd_result);

	return ret;
}

int	zbx_module_system_run(AGENT_REQUEST *request, AGENT_RESULT *result)
{
	char	*command, *flag;

	if (2 < request->nparam){
		SET_MSG_RESULT(result, strdup("Incorrect number of parameters given"));
		return SYSINFO_RET_FAIL;
	}

	command = get_rparam(request, 0);
	flag = get_rparam(request, 1);

	if (NULL == command || '\0' == *command){
		SET_MSG_RESULT(result, strdup("Incorrect command given"));
		return SYSINFO_RET_FAIL;
	}

	//zabbix_log(LOG_LEVEL_WARNING, "Executing command '%s'", command);
	zabbix_log(LOG_LEVEL_DEBUG, "Executing command '%s'", command);

	if (NULL == flag || '\0' == *flag || 0 == strcmp(flag, "wait"))	/* default parameter */
		return execute_str_mod(command, result);
	else if (0 != strcmp(flag, "nowait") || SUCCEED != zbx_execute_nowait(command)){
		SET_MSG_RESULT(result, strdup("Execution Failed"));
		return SYSINFO_RET_FAIL;
	}

	SET_UI64_RESULT(result, 1);

	return SYSINFO_RET_OK;
}

/******************************************************************************
 *                                                                            *
 * Function: zbx_module_init                                                  *
 *                                                                            *
 * Purpose: the function is called on agent startup                           *
 *          It should be used to call any initialization routines             *
 *                                                                            *
 * Return value: ZBX_MODULE_OK - success                                      *
 *               ZBX_MODULE_FAIL - module initialization failed               *
 *                                                                            *
 * Comment: the module won't be loaded in case of ZBX_MODULE_FAIL             *
 *                                                                            *
 ******************************************************************************/
int	zbx_module_init()
{
	return ZBX_MODULE_OK;
}

/******************************************************************************
 *                                                                            *
 * Function: zbx_module_uninit                                                *
 *                                                                            *
 * Purpose: the function is called on agent shutdown                          *
 *          It should be used to cleanup used resources if there are any      *
 *                                                                            *
 * Return value: ZBX_MODULE_OK - success                                      *
 *               ZBX_MODULE_FAIL - function failed                            *
 *                                                                            *
 ******************************************************************************/
int	zbx_module_uninit()
{
	return ZBX_MODULE_OK;
}
