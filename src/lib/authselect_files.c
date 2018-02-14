/*
    Authors:
        Pavel Březina <pbrezina@redhat.com>

    Copyright (C) 2017 Red Hat

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "authselect.h"
#include "lib/authselect_private.h"
#include "lib/util/template.h"

errno_t
authselect_files_generate(struct authselect_profile *profile,
                          const char **features,
                          struct authselect_files **_files)
{
    struct authselect_files *files;
    errno_t ret;
    int i;

    if (profile == NULL) {
        return EINVAL;
    }

    files = malloc_zero(struct authselect_files);
    if (files == NULL) {
        return ENOMEM;
    }

    struct {
        const char *template;
        char **_storage;
    } tpls[] = {
        {profile->files.systemauth,      &files->systemauth},
        {profile->files.passwordauth,    &files->passwordauth},
        {profile->files.smartcardauth,   &files->smartcardauth},
        {profile->files.fingerprintauth, &files->fingerprintauth},
        {profile->files.postlogin,       &files->postlogin},
        {profile->files.nsswitch,        &files->nsswitch},
        {profile->files.dconfdb,         &files->dconfdb},
        {profile->files.dconflock,       &files->dconflock},
        {NULL, NULL},
    };

    /* Template may be NULL so we must compare against destination. */
    for (i = 0; tpls[i]._storage != NULL; i++) {
        if (tpls[i].template == NULL) {
            *tpls[i]._storage = NULL;
            continue;
        }

        *tpls[i]._storage = template_generate(tpls[i].template, features);
        if (tpls[i]._storage == NULL) {
            ret = ENOMEM;
            goto done;
        }
    }

    *_files = files;

    ret = EOK;

done:
    if (ret != EOK) {
        ERROR("Unable to generate files [%d]: %s", ret, strerror(ret));
        authselect_files_free(files);
    }

    return ret;
}

_PUBLIC_ const char *
authselect_files_nsswitch(const struct authselect_files *files)
{
    if (files == NULL) {
        return NULL;
    }

    return files->nsswitch;
}

_PUBLIC_ const char *
authselect_files_systemauth(const struct authselect_files *files)
{
    if (files == NULL) {
        return NULL;
    }

    return files->systemauth;
}

_PUBLIC_ const char *
authselect_files_passwordauth(const struct authselect_files *files)
{
    if (files == NULL) {
        return NULL;
    }

    return files->passwordauth;
}

_PUBLIC_ const char *
authselect_files_smartcardauth(const struct authselect_files *files)
{
    if (files == NULL) {
        return NULL;
    }

    return files->smartcardauth;
}

_PUBLIC_ const char *
authselect_files_fingerprintauth(const struct authselect_files *files)
{
    if (files == NULL) {
        return NULL;
    }

    return files->fingerprintauth;
}

_PUBLIC_ const char *
authselect_files_postlogin(const struct authselect_files *files)
{
    if (files == NULL) {
        return NULL;
    }

    return files->postlogin;
}

_PUBLIC_ const char *
authselect_files_dconf_db(const struct authselect_files *files)
{
    if (files == NULL) {
        return NULL;
    }

    return files->dconfdb;
}

_PUBLIC_ const char *
authselect_files_dconf_lock(const struct authselect_files *files)
{
    if (files == NULL) {
        return NULL;
    }

    return files->dconflock;
}

void
authselect_files_free_content(struct authselect_files *files)
{
    if (files == NULL) {
        return;
    }

    if (files->systemauth != NULL) {
        free(files->systemauth);
    }

    if (files->passwordauth != NULL) {
        free(files->passwordauth);
    }

    if (files->smartcardauth != NULL) {
        free(files->smartcardauth);
    }

    if (files->fingerprintauth != NULL) {
        free(files->fingerprintauth);
    }

    if (files->postlogin != NULL) {
        free(files->postlogin);
    }

    if (files->nsswitch != NULL) {
        free(files->nsswitch);
    }

    if (files->dconfdb != NULL) {
        free(files->dconfdb);
    }

    if (files->dconflock != NULL) {
        free(files->dconflock);
    }
}

_PUBLIC_ void
authselect_files_free(struct authselect_files *files)
{
    if (files == NULL) {
        return;
    }

    authselect_files_free_content(files);

    free(files);
}
