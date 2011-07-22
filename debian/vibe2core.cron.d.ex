#
# Regular cron jobs for the vibe2core package
#
0 4	* * *	root	[ -x /usr/bin/vibe2core_maintenance ] && /usr/bin/vibe2core_maintenance
