#ifndef KFS_CONFIG_H
# define KFS_CONFIG_H

/* Resolution screen size */
# define SCREEN_WIDTH			1280
# define SCREEN_HEIGHT			1024

/* Number of virtual terminals: only between 1 and 9 are supported */
# define VIRTUAL_DESKTOP_COUNT	5

#if VIRTUAL_DESKTOP_COUNT < 1 || VIRTUAL_DESKTOP_COUNT > 9
# error "KFS only supports between 1 and 9 terminals"
#endif

#endif
