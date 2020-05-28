/*
 * Copyright (C) 1994-2020 Altair Engineering, Inc.
 * For more information, contact Altair at www.altair.com.
 *
 * This file is part of both the OpenPBS software ("OpenPBS")
 * and the PBS Professional ("PBS Pro") software.
 *
 * Open Source License Information:
 *
 * OpenPBS is free software. You can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * OpenPBS is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Commercial License Information:
 *
 * PBS Pro is commercially licensed software that shares a common core with
 * the OpenPBS software.  For a copy of the commercial license terms and
 * conditions, go to: (http://www.pbspro.com/agreement.html) or contact the
 * Altair Legal Department.
 *
 * Altair's dual-license business model allows companies, individuals, and
 * organizations to create proprietary derivative works of OpenPBS and
 * distribute them - whether embedded or bundled with other software -
 * under a commercial license agreement.
 *
 * Use of Altair's trademarks, including but not limited to "PBS™",
 * "OpenPBS®", "PBS Professional®", and "PBS Pro™" and Altair's logos is
 * subject to Altair's trademark licensing policies.
 */

/**
 * @file	dec_Status.c
 */

#include <pbs_config.h>   /* the master config generated by configure */

#include <sys/types.h>
#include "libpbs.h"
#include "list_link.h"
#include "server_limits.h"
#include "attribute.h"
#include "credential.h"
#include "batch_request.h"
#include "dis.h"

/**
 * @brief
 *	Decode a Status batch request
 *
 * @par
 *	The batch_request structure must already exist (be allocated by the
 *	caller).   It is assumed that the header fields (protocol type,
 *	protocol version, request type, and user name) have already be decoded.
 *
 * @param[in]     sock - socket handle from which to read.
 * @param[in,out] preq - pointer to the batch request structure. The following
 *		elements of the rq_ind.rq_status union are updated:
 *		rq_id     - object id, a variable length string.
 *		rq_status - the linked list of attribute structures
 *
 * @return int
 * @retval 0 - request read and decoded successfully.
 * @retval non-zero - DIS decode error.
 */

int
decode_DIS_Status(int sock, struct batch_request *preq)
{
	int    rc;
	size_t nchars = 0;

	preq->rq_ind.rq_status.rq_id = NULL;

	CLEAR_HEAD(preq->rq_ind.rq_status.rq_attr);

	/*
	 * call the disrcs function to allocate and return a string of all ids
	 * freed in free_br()
	 */
	preq->rq_ind.rq_status.rq_id = disrcs(sock, &nchars, &rc);
	if (rc) return rc;

	rc = decode_DIS_svrattrl(sock, &preq->rq_ind.rq_status.rq_attr);
	return rc;
}
