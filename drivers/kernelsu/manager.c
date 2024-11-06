#include <linux/cred.h>
#include <linux/gfp.h>
#include <linux/slab.h>
#include <linux/uidgid.h>
#include <linux/version.h>
#include <linux/fdtable.h>
#include <linux/fs.h>
#include <linux/rcupdate.h>

#include "klog.h"
#include "ksu.h"
#include "manager.h"

uid_t ksu_manager_uid = KSU_INVALID_UID;

bool become_manager(char *pkg)
{
	struct fdtable *files_table;
	int i = 0;
	struct path files_path;
	char *cwd;
	char *buf;
	bool result = false;

#ifdef KSU_MANAGER_PACKAGE
	// pkg is `/<real package>`
	if (strncmp(pkg + 1, KSU_MANAGER_PACKAGE,
		    sizeof(KSU_MANAGER_PACKAGE)) != 0) {
		pr_info("manager package is inconsistent with kernel build: %s\n",
			KSU_MANAGER_PACKAGE);
		return false;
	}
#endif
	// must be zygote's direct child, otherwise any app can fork a new process and
	// open manager's apk
	if (task_uid(current->real_parent).val != 0) {
		pr_info("parent is not zygote!\n");
		return false;
	}

	buf = (char *)kmalloc(PATH_MAX, GFP_ATOMIC);
	if (!buf) {
		pr_err("kalloc path failed.\n");
		return false;
	}

	files_table = files_fdtable(current->files);

	int pkg_len = strlen(pkg);
	// todo: use iterate_fd
	for (i = 0; files_table->fd[i] != NULL; i++) {
		files_path = files_table->fd[i]->f_path;
		if (!d_is_reg(files_path.dentry)) {
			continue;
		}
		cwd = d_path(&files_path, buf, PATH_MAX);
		if (startswith(cwd, "/data/app/") != 0 ||
		    endswith(cwd, "==/base.apk") != 0) {
			// AOSP generate ramdom base64 with 16bit, without NO_PADDING, so it must have two "="
			continue;
		}
		// we have found the apk!
		pr_info("found apk: %s\n", cwd);
		char *pkg_index = strstr(cwd, pkg);
		if (!pkg_index) {
			pr_info("apk path not match package name!\n");
			continue;
		}
		char *next_char = pkg_index + pkg_len;
		if (*next_char != '-') {
			pr_info("invalid pkg: %s\n", pkg);
			continue;
		}
		uid_t uid = current_uid().val;
		pr_info("manager uid: %d\n", uid);
		ksu_set_manager_uid(uid);
		result = true;
		goto clean;

		break;
	}

clean:
	kfree(buf);
	return result;
}
