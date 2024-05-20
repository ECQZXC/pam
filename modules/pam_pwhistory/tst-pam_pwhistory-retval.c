/*
 * Check pam_pwhistory return values.
 *
 * Copyright (c) 2023 Stefan Schubert <schubi@suse.de>
 */

#include "test_assert.h"

#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <security/pam_appl.h>

#define MODULE_NAME "pam_pwhistory"
#define TEST_NAME "tst-" MODULE_NAME "-retval"

static const char service_file[] = TEST_NAME ".service";
static struct pam_conv conv;

int
main(void)
{
	pam_handle_t *pamh = NULL;
	FILE *fp;
	char cwd[PATH_MAX];

	ASSERT_NE(NULL, getcwd(cwd, sizeof(cwd)));

	/* PAM_USER_UNKNOWN */
	ASSERT_NE(NULL, fp = fopen(service_file, "w"));
	ASSERT_LT(0,
		  fprintf(fp, "#%%PAM-1.0\n"
			      "auth required %s/.libs/%s.so\n"
			      "account required %s/.libs/%s.so\n"
			      "password required %s/.libs/%s.so\n"
			      "session required %s/.libs/%s.so\n",
			  cwd, MODULE_NAME,
			  cwd, MODULE_NAME,
			  cwd, MODULE_NAME,
			  cwd, MODULE_NAME));
	ASSERT_EQ(0, fclose(fp));

	ASSERT_EQ(PAM_SUCCESS,
		  pam_start_confdir(service_file, "", &conv, ".", &pamh));
	ASSERT_NE(NULL, pamh);
	ASSERT_EQ(PAM_MODULE_UNKNOWN, pam_authenticate(pamh, 0));
	ASSERT_EQ(PAM_MODULE_UNKNOWN, pam_setcred(pamh, 0));
	ASSERT_EQ(PAM_MODULE_UNKNOWN, pam_acct_mgmt(pamh, 0));
	ASSERT_EQ(PAM_USER_UNKNOWN, pam_chauthtok(pamh, 0));
	ASSERT_EQ(PAM_MODULE_UNKNOWN, pam_open_session(pamh, 0));
	ASSERT_EQ(PAM_MODULE_UNKNOWN, pam_close_session(pamh, 0));
	ASSERT_EQ(PAM_SUCCESS, pam_end(pamh, 0));
	pamh = NULL;

	/* cleanup */
	ASSERT_EQ(0, unlink(service_file));

	return 0;
}
