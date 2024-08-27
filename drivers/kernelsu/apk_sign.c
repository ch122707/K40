#include "linux/err.h"
#include "linux/fs.h"
#include "linux/gfp.h"
#include "linux/kernel.h"
#include "linux/moduleparam.h"

#include "klog.h" // IWYU pragma: keep
#include "kernel_compat.h"
#include "crypto/hash.h"
#include "linux/slab.h"
#include "linux/version.h"

struct zip_entry_header {
	uint32_t signature;
	uint16_t version;
	uint16_t flags;
	uint16_t compression;
	uint16_t mod_time;
	uint16_t mod_date;
	uint32_t crc32;
	uint32_t compressed_size;
	uint32_t uncompressed_size;
	uint16_t file_name_length;
	uint16_t extra_field_length;
} __attribute__((packed));
