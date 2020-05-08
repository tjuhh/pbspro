/*
 * Copyright (C) 1994-2020 Altair Engineering, Inc.
 * For more information, contact Altair at www.altair.com.
 *
 * This file is part of the PBS Professional ("PBS Pro") software.
 *
 * Open Source License Information:
 *
 * PBS Pro is free software. You can redistribute it and/or modify it under the
 * terms of the GNU Affero General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * PBS Pro is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.
 * See the GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Commercial License Information:
 *
 * For a copy of the commercial license terms and conditions,
 * go to: (http://www.pbspro.com/UserArea/agreement.html)
 * or contact the Altair Legal Department.
 *
 * Altair’s dual-license business model allows companies, individuals, and
 * organizations to create proprietary derivative works of PBS Pro and
 * distribute them - whether embedded or bundled with other software -
 * under a commercial license agreement.
 *
 * Use of Altair’s trademarks, including but not limited to "PBS™",
 * "PBS Professional®", and "PBS Pro™" and Altair’s logos is subject to Altair's
 * trademark licensing policies.
 *
 */
/**
 * @file	int_ucred.c
 * @brief
 * send user credentials to the server.
 *
 * migrate users info on the current server to a destination server.
 *
 * @note
 * This code is not mean to be used by a persistent process.
 * If an error occurs, not all the allocated structures are freed.
 */

#include <pbs_config.h>   /* the master config generated by configure */

#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "libpbs.h"
#include "dis.h"
#include "ticket.h"
#include "pbs_ifl.h"
#include "pbs_ecl.h"


/**
 * @brief
 *	send user credentials to the server.
 *
 * @param[in] c - connection handler
 * @param[in] user - username
 * @param[in] type - cred type
 * @param[in] buf - credentials
 * @param[in] len - length of buffer
 *
 * @return      int
 * @retval      0  		success
 * @retval      error code      error
 *
 */

int
PBSD_ucred(int c, char *user, int type, char *buf, int len)
{
	int			rc;
	struct batch_reply	*reply = NULL;

	/* initialize the thread context data, if not already initialized */
	if (pbs_client_thread_init_thread_context() != 0)
		return pbs_errno;

	/* lock pthread mutex here for this connection */
	/* blocking call, waits for mutex release */
	if (pbs_client_thread_lock_connection(c) != 0)
		return pbs_errno;

	DIS_tcp_funcs();

	if ((rc =encode_DIS_ReqHdr(c, PBS_BATCH_UserCred, pbs_current_user)) ||
		(rc = encode_DIS_UserCred(c, user, type, buf, len)) ||
		(rc = encode_DIS_ReqExtend(c, NULL))) {
		if (set_conn_errtxt(c, dis_emsg[rc]) != 0) {
			pbs_errno = PBSE_SYSTEM;
		} else {
			pbs_errno = PBSE_PROTOCOL;
		}
		(void)pbs_client_thread_unlock_connection(c);
		return pbs_errno;
	}
	if (dis_flush(c)) {
		pbs_errno = PBSE_PROTOCOL;
		(void)pbs_client_thread_unlock_connection(c);
		return pbs_errno;
	}

	reply = PBSD_rdrpy(c);

	PBSD_FreeReply(reply);

	rc = get_conn_errno(c);

	/* unlock the thread lock and update the thread context data */
	if (pbs_client_thread_unlock_connection(c) != 0)
		return pbs_errno;

	return rc;
}


